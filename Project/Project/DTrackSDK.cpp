/* DTrackSDK: C++ source file, A.R.T. GmbH 3.5.07-27.01.12
 *
 * DTrackSDK: functions to receive and process DTrack UDP packets (ASCII protocol), as
 * well as to exchange DTrack2 TCP command strings.
 * Copyright (C) 2007-2012, Advanced Realtime Tracking GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Version v2.2.0
 *
 * Purpose:
 *  - receives DTrack UDP packets (ASCII protocol) and converts them into easier to handle data
 *  - sends and receives DTrack2 commands (TCP)
 *  - DTrack2 network protocol due to: 'Technical Appendix DTrack v2.0'
 *  - for ARTtrack Controller versions v0.2 (and compatible versions)
 *  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
 *
 * $Id: DTrackSDK.cpp,v 1.41 2012/01/27 08:50:41 christoph Exp $
 */

#include "DTrackSDK.hpp"

#include <iostream>
#include <sstream>

using namespace DTrackSDK_Net;
using namespace DTrackSDK_Parse;


/**
 * 	\brief	Constructor. Use for listening mode.
 *
 *	@param[in]	data_port		port number to receive tracking data from DTrack
 */
DTrackSDK::DTrackSDK(unsigned short data_port) {
	init("", 0, data_port, SYS_DTRACK_UNKNOWN);
}


/**
 * 	\brief	Constructor. Use for DTrack2.
 *
 *	@param[in]	server_host		hostname or IP address of ARTtrack Controller (empty string if not used)
 *	@param[in]	data_port		port number to receive tracking data from DTrack
 */
DTrackSDK::DTrackSDK(const std::string server_host, unsigned short data_port)
{
	init(server_host, 50105, data_port, SYS_DTRACK_2);
}


/**
 * 	\brief	Constructor. Use for DTrack.
 *
 * 	This constructor can also be used for DTrack2. In this case server_port must be 50105.
 *
 *	@param[in]	server_host		hostname or IP address of ARTtrack Controller (empty string if not used)
 *	@param[in]	server_port		port number of DTrack
 *	@param[in]	data_port		port number to receive tracking data from DTrack
 */
DTrackSDK::DTrackSDK(const std::string server_host, unsigned short server_port, unsigned short data_port)
{
	init(server_host, server_port, data_port, SYS_DTRACK_UNKNOWN);
}


/**	std::vector<DTrack_Monitor2D_Type> act_2d;        //!< array with marker data for each camera
 * 	\brief	General constructor.
 *
 *	@param[in]	server_host		hostname or IP address of ARTtrack Controller (empty string if not used)
 *	@param[in]	server_port		port number of ARTtrack Controller
 *	@param[in]	data_port		port number to receive tracking data from ARTtrack Controller (0 if to be chosen)
 *	@param[in]	remote_type		type of system to connect to
 *	@param[in]	data_bufsize	size of buffer for UDP packets in bytes; default is 32kb (32768 bytes)
 *	@param[in]	data_timeout_us	timeout for receiving tracking data in us; default is 1s (1,000,000 us)
 *	@param[in]	srv_timeout_us	timeout for reply of ARTtrack Controller in us; default is 10s (10,000,000 us)
 */
DTrackSDK::DTrackSDK(const std::string server_host,	unsigned short server_port,	unsigned short data_port,
	RemoteSystemType remote_type, int data_bufsize,	int data_timeout_us, int srv_timeout_us)
{
	init(server_host, server_port, data_port, remote_type, data_bufsize, data_timeout_us, srv_timeout_us);
}


/**
 * 	\brief	Private init called by constructor.
 *
 *	@param[in]	server_host		hostname or IP address of ARTtrack Controller (empty string if not used)
 *	@param[in]	server_port		port number of ARTtrack Controller
 *	@param[in]	data_port		port number to receive tracking data from ARTtrack Controller (0 if to be chosen)
 *	@param[in]	remote_type		type of system to connect to
 *	@param[in]	data_bufsize	size of buffer for UDP packets in bytes; default is 32kb (32768 bytes)
 *	@param[in]	data_timeout_us	timeout for receiving tracking data in us; default is 1s (1,000,000 us)
 *	@param[in]	srv_timeout_us	timeout for reply of ARTtrack Controller in us; default is 10s (10,000,000 us)
 */
void DTrackSDK::init(const std::string server_host,	unsigned short server_port,	unsigned short data_port,
	RemoteSystemType remote_type, int data_bufsize,	int data_timeout_us, int srv_timeout_us)
{
	rsType = remote_type;
	int err;

	d_udpsock = NULL;
	d_tcpsock = NULL;
	d_udpbuf = NULL;

	lastDataError = ERR_NONE;
	lastServerError = ERR_NONE;
	setLastDTrackError();

	d_udptimeout_us = data_timeout_us;
	d_tcptimeout_us = srv_timeout_us;
	d_remoteport = server_port;

	net_init();

	// parse remote address if available
	d_remote_ip = 0;
	if (!server_host.empty()) {
		d_remote_ip = ip_name2ip(server_host.c_str());
	}

	// create UDP socket:
	d_udpport = data_port;

	if ((d_remote_ip != 0) && (server_port == 0)) { // listen to multicast case
		err = udp_init(&d_udpsock, &d_udpport, d_remote_ip);
	} else { // normal case
		err = udp_init(&d_udpsock, &d_udpport);
	}
	if (err) {
		d_udpsock = NULL;
		d_udpport = 0;
		return;
	}

	// create UDP buffer:
	d_udpbufsize = data_bufsize;
	d_udpbuf = (char *)malloc(data_bufsize);
	if (!d_udpbuf) {
		udp_exit(d_udpsock);
		d_udpsock = NULL;
		d_udpport = 0;
		return;
	}

	if (d_remote_ip) {
		if (server_port == 0) { // multicast
			d_remoteport = 0;
		} else {
			if (rsType != SYS_DTRACK) {
				err = tcp_client_init(&d_tcpsock, d_remote_ip, server_port);
				if (err) {  // no connection to DTrack2 server
					// on error assuming DTrack if system is unknown
					if (rsType == SYS_DTRACK_UNKNOWN)
					{
						rsType = SYS_DTRACK;
						// DTrack will not listen to tcp port 50105 -> ignore tcp connection
					}
				} else {
					// TCP connection up, should be DTrack2
					rsType = SYS_DTRACK_2;
				}
			}
		}
	}
	// reset actual DTrack data:
	act_framecounter = 0;
	act_timestamp = -1;

	act_num_body = act_num_flystick = act_num_meatool = act_num_mearef = act_num_hand = act_num_human = 0;
	act_num_marker = 0;

	d_message_origin = "";
	d_message_status = "";
	d_message_framenr = 0;
	d_message_errorid = 0;
	d_message_msg = "";
}


/**
 * 	\brief Destructor.
 */
DTrackSDK::~DTrackSDK()
{
	// release buffer
	free(d_udpbuf);

	// release sockets & net
	if ((d_remote_ip != 0) && (d_remoteport == 0)) {
		udp_exit(d_udpsock, d_remote_ip);
	} else {
		udp_exit(d_udpsock);
	}
	tcp_exit(d_tcpsock);
	net_exit();
}


/**
 * 	\brief	Set timeout for receiving tracking data.
 *
 * 	@param[in]	timeout		timeout for receiving tracking data in us; default is 1s (1,000,000 us)
 * 	@return		Success? (i.e. valid timeout)
 */
bool DTrackSDK::setDataTimeoutUS(int timeout) {
	if (timeout < 1)
		return false;
	d_udptimeout_us = timeout;
	return true;
}


/**
 * 	\brief	Set timeout for reply of ARTtrack Controller.
 *
 * 	@param[in]	timeout		timeout for reply of ARTtrack Controller in us; default is 1s (1,000,000 us)
 * 	@return		Success? (i.e. valid timeout)
 */
bool DTrackSDK::setControllerTimeoutUS(int timeout) {
	if (timeout < 1)
		return false;
	d_tcptimeout_us = timeout;
	return true;
}


/**
 * 	\brief	Get current remote system type (e.g. DTrack, DTrack2).
 *
 * 	@return	type of remote system
 */
DTrackSDK::RemoteSystemType DTrackSDK::getRemoteSystemType()
{
	return rsType;
}

/**
 * 	\brief	Get last error as error code (data transmission).
 *
 * 	@return error code (success = 0)
 */
DTrackSDK::Errors DTrackSDK::getLastDataError()
{
	return lastDataError;
}


/**
 * 	\brief	Get last error as error code (command transmission).
 *
 * 	@return error code (success = 0)
 */
DTrackSDK::Errors DTrackSDK::getLastServerError()
{
	return lastServerError;
}


/**
 * 	\brief Set last dtrack error.
 *
 * 	@param[in]	newError		new error code for last operation; default is 0 (success)
 * 	@param[in]	newErrorString	corresponding error string if exists (optional)
 */
void DTrackSDK::setLastDTrackError(int newError, std::string newErrorString)
{
	lastDTrackError = newError;
	lastDTrackErrorString = newErrorString;
}


/**
 * 	\brief	Get last DTrack error code.
 *
 * 	@return Error code.
 */
int DTrackSDK::getLastDTrackError()
{
	return lastDTrackError;
}


/**
 * 	\brief	Get last DTrack error description.
 *
 * 	@return Error description.
 */
std::string DTrackSDK::getLastDTrackErrorDescription()
{
	return lastDTrackErrorString;
}


/**
 *	\brief Is UDP socket open to receive tracking data on local machine?
 *	An open socket is needed to receive data, but does not guarantee this.
 *	Especially in case no data is sent to this port.
 *
 *	Replaces valid() in older SDKs.
 *	@return	socket open?
 */
bool DTrackSDK::isLocalDataPortValid()
{
	return (d_udpsock != NULL);
}


/**
 * 	\brief Is TCP connection for DTrack2 commands active?
 *
 *	On DTrack systems this function returns always false.
 *	@return	command interface active?
 */
bool DTrackSDK::isCommandInterfaceValid()
{
	return (d_tcpsock != NULL);
}


/**
 *	\brief	Receive and process one tracking data packet.
 *
 *	Updates internal data structures.
 *	@return	receive succeeded?
 */
bool DTrackSDK::receive()
{
	char* s;
	int i, j, k, l, n, len, id;
	char sfmt[20];
	int iarr[5];
	double d, darr[6];
	int loc_num_bodycal, loc_num_handcal, loc_num_flystick1, loc_num_meatool;

	lastDataError = ERR_NONE;
	lastServerError = ERR_NONE;

	if (!isLocalDataPortValid()) {
		lastDataError = ERR_NET;
		return false;
	}
	// defaults:
	act_framecounter = 0;
	act_timestamp = -1;   // i.e. not available
	loc_num_bodycal = loc_num_handcal = -1;  // i.e. not available
	loc_num_flystick1 = loc_num_meatool = 0;

	// receive UDP packet:
	len = udp_receive(d_udpsock, d_udpbuf, d_udpbufsize-1, d_udptimeout_us);
	if (len == -1) {
		lastDataError = ERR_TIMEOUT;
		return false;
	}

	if (len <= 0) {
		lastDataError = ERR_NET;
		return false;
	}

	s = d_udpbuf;
	s[len] = '\0';

	// process lines:
	lastDataError = ERR_PARSE;

	do {
		// line for frame counter:
		if (!strncmp(s, "fr ", 3)) {
			s += 3;
			if (!(s = string_get_ui(s, &act_framecounter))) {
				act_framecounter = 0;
				return false;
			}
			continue;
		}
		// line for timestamp:
		if (!strncmp(s, "ts ", 3)) {
			s += 3;
			if (!(s = string_get_d(s, &act_timestamp)))	{
				act_timestamp = -1;
				return false;
			}
			continue;
		}
		// line for additional information about number of calibrated bodies:
		if (!strncmp(s, "6dcal ", 6)) {
			s += 6;
			if (!(s = string_get_i(s, &loc_num_bodycal))) {
				return false;
			}
			continue;
		}
		// line for standard body data:
		if (!strncmp(s, "6d ", 3)) {
			s += 3;
			// disable all existing dataargc != 2
			for (i=0; i<act_num_body; i++) {
				memset(&act_body[i], 0, sizeof(DTrack_Body_Type));
				act_body[i].id = i;
				act_body[i].quality = -1;
			}
			// get number of standard bodies (in line)
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			// get data of standard bodies
			for (i=0; i<n; i++) {
				if (!(s = string_get_block(s, "id", &id, NULL, &d))) {
					return false;
				}
				// adjust length of vector
				if (id >= act_num_body) {
					act_body.resize(id + 1);
					for (j = act_num_body; j<=id; j++) {
						memset(&act_body[j], 0, sizeof(DTrack_Body_Type));
						act_body[j].id = j;
						act_body[j].quality = -1;
					}
					act_num_body = id + 1;
				}
				act_body[id].id = id;
				act_body[id].quality = d;
				if (!(s = string_get_block(s, "ddd", NULL, NULL, act_body[id].loc))) {
					return false;
				}
				if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_body[id].rot))) {
					return false;
				}
			}
			continue;
		}

		// line for Flystick data (older format):
		if (!strncmp(s, "6df ", 4)) {
			s += 4;
			// get number of calibrated Flysticks
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			loc_num_flystick1 = n;
			// adjust length of vector
			if (n != act_num_flystick) {
				act_flystick.resize(n);
				act_num_flystick = n;
			}
			// get data of Flysticks
			for (i=0; i<n; i++) {
				if (!(s = string_get_block(s, "idi", iarr, NULL, &d))) {
					return false;
				}
				if (iarr[0] != i) {	// not expected
					return false;
				}
				act_flystick[i].id = iarr[0];
				act_flystick[i].quality = d;
				act_flystick[i].num_button = 8;
				k = iarr[1];
				for (j=0; j<8; j++) {
					act_flystick[i].button[j] = k & 0x01;
					k >>= 1;
				}
				act_flystick[i].num_joystick = 2;  // additionally to buttons 5-8
				if (iarr[1] & 0x20) {
					act_flystick[i].joystick[0] = -1;
				} else
				if (iarr[1] & 0x80) {
					act_flystick[i].joystick[0] = 1;
				} else {
					act_flystick[i].joystick[0] = 0;
				}
				if(iarr[1] & 0x10){
					act_flystick[i].joystick[1] = -1;
				}else if(iarr[1] & 0x40){
					act_flystick[i].joystick[1] = 1;
				}else{
					act_flystick[i].joystick[1] = 0;
				}
				if (!(s = string_get_block(s, "ddd", NULL, NULL, act_flystick[i].loc))) {
					return false;
				}
				if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_flystick[i].rot))) {
					return false;
				}
			}
			continue;
		}

		// line for Flystick data (newer format):
		if (!strncmp(s, "6df2 ", 5)) {
			s += 5;
			// get number of calibrated Flysticks
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			// adjust length of vector
			if (n != act_num_flystick) {
				act_flystick.resize(n);
				act_num_flystick = n;
			}
			// get number of Flysticks
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			// get data of Flysticks
			for (i=0; i<n; i++) {
				if (!(s = string_get_block(s, "idii", iarr, NULL, &d))) {
					return false;
				}
				if (iarr[0] != i) {  // not expected
					return false;
				}
				act_flystick[i].id = iarr[0];
				act_flystick[i].quality = d;
				if ((iarr[1] > DTRACK_FLYSTICK_MAX_BUTTON)||(iarr[1] > DTRACK_FLYSTICK_MAX_JOYSTICK)) {
					return false;
				}
				act_flystick[i].num_button = iarr[1];
				act_flystick[i].num_joystick = iarr[2];
				if (!(s = string_get_block(s, "ddd", NULL, NULL, act_flystick[i].loc))){
					return false;
				}
				if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_flystick[i].rot))){
					return false;
				}
				strcpy(sfmt, "");
				j = 0;
				while (j < act_flystick[i].num_button) {
					strcat(sfmt, "i");
					j += 32;
				}
				j = 0;
				while (j < act_flystick[i].num_joystick) {
					strcat(sfmt, "d");
					j++;
				}
				if (!(s = string_get_block(s, sfmt, iarr, NULL, act_flystick[i].joystick))) {
					return false;
				}
				k = l = 0;
				for (j=0; j<act_flystick[i].num_button; j++) {
					act_flystick[i].button[j] = iarr[k] & 0x01;
					iarr[k] >>= 1;
					l++;
					if (l == 32) {
						k++;
						l = 0;
					}
				}
			}
			continue;
		}

		// line for measurement tool data:
		if (!strncmp(s, "6dmt ", 5)) {
			s += 5;
			// get number of calibrated measurement tools
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			loc_num_meatool = n;
			// adjust length of vector
			if (n != act_num_meatool) {
				act_meatool.resize(n);
				act_num_meatool = n;
			}
			// get data of measurement tools
			for (i=0; i<n; i++) {
				if (!(s = string_get_block(s, "idi", iarr, NULL, &d))) {
					return false;
				}
				if (iarr[0] != i) {  // not expected
					return false;
				}
				act_meatool[i].id = iarr[0];
				act_meatool[i].quality = d;
				act_meatool[i].num_button = 1;
				act_meatool[i].button[0] = iarr[1] & 0x01;
				if (!(s = string_get_block(s, "ddd", NULL, NULL, act_meatool[i].loc))) {
					return false;
				}
				if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_meatool[i].rot))) {
					return false;
				}
			}
			continue;
		}

		// line for measurement reference data:
		if (!strncmp(s, "6dmtr ", 6)) {
			s += 6;
			// get number of measurement references
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			// get number of calibrated measurement references
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			// adjust length of vector
			if (n != act_num_mearef) {
				act_mearef.resize(n);
				act_num_mearef = n;
			}

			// get data of measurement references
			for (i=0; i<n; i++) {
				if (!(s = string_get_block(s, "id", &id, NULL, &d))) {
					return false;
				}
				act_mearef[i].id = id;
				act_mearef[i].quality = d;
				if (!(s = string_get_block(s, "ddd", NULL, NULL, act_mearef[i].loc))) {
					return false;
				}
				if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_mearef[i].rot))) {
					return false;
				}
			}
			continue;
		}

		// line for additional information about number of calibrated Fingertracking hands:
		if (!strncmp(s, "glcal ", 6)) {
			s += 6;
			if (!(s = string_get_i(s, &loc_num_handcal))) {	// get number of calibrated hands
				return false;
			}
		}

		// line for A.R.T. Fingertracking hand data:
		if (!strncmp(s, "gl ", 3)) {
			s += 3;
			// disable all existing data
			for (i=0; i<act_num_hand; i++) {
				memset(&act_hand[i], 0, sizeof(DTrack_Hand_Type));
				act_hand[i].id = i;
				act_hand[i].quality = -1;
			}
			// get number of hands (in line)
			if (!(s = string_get_i(s, &n))) {
				return false;
			}
			// get data of hands
			for (i=0; i<n; i++) {
				if (!(s = string_get_block(s, "idii", iarr, NULL, &d))){
					return false;
				}
				id = iarr[0];
				if (id >= act_num_hand) {  // adjust length of vector
					act_hand.resize(id + 1);
					for (j=act_num_hand; j<=id; j++) {
						memset(&act_hand[j], 0, sizeof(DTrack_Hand_Type));
						act_hand[j].id = j;
						act_hand[j].quality = -1;
					}
					act_num_hand = id + 1;
				}
				act_hand[id].id = iarr[0];
				act_hand[id].lr = iarr[1];
				act_hand[id].quality = d;
				if (iarr[2] > DTRACK_HAND_MAX_FINGER) {
					return false;
				}
				act_hand[id].nfinger = iarr[2];
				if (!(s = string_get_block(s, "ddd", NULL, NULL, act_hand[id].loc))) {
					return false;

				}
				if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_hand[id].rot))){
					return false;
				}
				// get data of fingers
				for (j = 0; j < act_hand[id].nfinger; j++) {
					if (!(s = string_get_block(s, "ddd", NULL, NULL, act_hand[id].finger[j].loc))) {
						return false;
					}
					if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_hand[id].finger[j].rot))){
						return false;
					}
					if (!(s = string_get_block(s, "dddddd", NULL, NULL, darr))){
						return false;
					}
					act_hand[id].finger[j].radiustip = darr[0];
					act_hand[id].finger[j].lengthphalanx[0] = darr[1];
					act_hand[id].finger[j].anglephalanx[0] = darr[2];
					act_hand[id].finger[j].lengthphalanx[1] = darr[3];
					act_hand[id].finger[j].anglephalanx[1] = darr[4];
					act_hand[id].finger[j].lengthphalanx[2] = darr[5];
				}
			}
			continue;
		}

		// line for 6dj human model data
		if (!strncmp(s, "6dj ", 4)) {
			s += 4;

			// get number of calibrated human models
			if (!(s = string_get_i(s, &n))) { 
				return false;	 
			}
			// adjust length of vector
			if(n != act_num_human){
				act_human.resize(n);
				act_num_human = n;
			}
			for(i=0; i<act_num_human; i++){
				memset(&act_human[i], 0, sizeof(DTrack_Human_Type));
				act_human[i].id = i;
				act_human[i].num_joints = 0;
			}
			
			// get number of human models
			if (!(s = string_get_i(s, &n))) { 
				return false;
			}
			int id_human;
			for (i=0; i<n; i++) {
				if (!(s = string_get_block(s, "ii", iarr, NULL,NULL))){
					return false;
				}
				if (iarr[0] > act_num_human - 1) // not expected
					return false;

				id_human = iarr[0];
				act_human[id_human].id = iarr[0];
				act_human[id_human].num_joints = iarr[1];

				for (j = 0; j < iarr[1]; j++){
					if (!(s = string_get_block(s, "id", &id, NULL, &d))){
						return false;
					}
					act_human[id_human].joint[j].id = id;
					act_human[id_human].joint[j].quality = d;

					if (!(s = string_get_block(s, "dddddd", NULL, NULL, darr))){
						return false;
					}
					memcpy(act_human[id_human].joint[j].loc, &darr,  3*sizeof(double));
					memcpy(act_human[id_human].joint[j].ang, &darr[3],  3*sizeof(double));

					if (!(s = string_get_block(s, "ddddddddd", NULL, NULL, act_human[id_human].joint[j].rot))){
						return false;
					}
				}
			}
			continue;
		}

		// line for single marker data:
		if (!strncmp(s, "3d ", 3)) {
			s += 3;
			// get number of markers
			if (!(s = string_get_i(s, &act_num_marker))) {
				act_num_marker = 0;
				return false;
			}
			if (act_num_marker > (int )act_marker.size()) {
				act_marker.resize(act_num_marker);
			}
			// get data of single markers
			for (i=0; i<act_num_marker; i++) {
				if (!(s = string_get_block(s, "id", &act_marker[i].id, NULL, &act_marker[i].quality))) {
					return false;
				}
				if (!(s = string_get_block(s, "ddd", NULL, NULL, act_marker[i].loc))) {
					return false;
				}
			}
			continue;
		}

		// ignore unknown line identifiers (could be valid in future DTracks)
	} while((s = string_nextline(d_udpbuf, s, d_udpbufsize)));

	// set number of calibrated standard bodies, if necessary:
	if (loc_num_bodycal >= 0) {	// '6dcal' information was available
		n = loc_num_bodycal - loc_num_flystick1 - loc_num_meatool;
		if (n > act_num_body) {  // adjust length of vector
			act_body.resize(n);
			for (j=act_num_body; j<n; j++) {
				memset(&act_body[j], 0, sizeof(DTrack_Body_Type));
				act_body[j].id = j;
				act_body[j].quality = -1;
			}
		}
		act_num_body = n;
	}

	// set number of calibrated Fingertracking hands, if necessary:
	if (loc_num_handcal >= 0) {  // 'glcal' information was available
		if (loc_num_handcal > act_num_hand) {  // adjust length of vector
			act_hand.resize(loc_num_handcal);
			for (j=act_num_hand; j<loc_num_handcal; j++) {
				memset(&act_hand[j], 0, sizeof(DTrack_Hand_Type));
				act_hand[j].id = j;
				act_hand[j].quality = -1;
			}
		}
		act_num_hand = loc_num_handcal;
	}

	lastDataError = ERR_NONE;
	return true;
}


/**
 * 	\brief	Get number of calibrated standard bodies (as far as known).
 *
 *	Refers to last received frame.
 *	@return		number of calibrated standard bodies
 */
int DTrackSDK::getNumBody()
{
	return act_num_body;
}


/**
 * 	\brief	Get standard body data
 *
 *	Refers to last received frame. Currently not tracked bodies get a quality of -1.
 *	@param[in]	id	id, range 0 .. (max standard body id - 1)
 *	@return		id-th standard body data
 */
DTrack_Body_Type_d* DTrackSDK::getBody(int id)
{
	if ((id >= 0) && (id < act_num_body))
		return &act_body.at(id);
	return NULL;
}


/**
 * 	\brief	Get number of calibrated Flysticks.
 *
 *	Refers to last received frame.
 *	@return		number of calibrated Flysticks
 */
int DTrackSDK::getNumFlyStick()
{
	return act_num_flystick;
}


/**
 * 	\brief	Get Flystick data.
 *
 *	Refers to last received frame. Currently not tracked bodies get a quality of -1.
 *	@param[in]	id	id, range 0 .. (max flystick id - 1)
 *	@return		id-th Flystick data.
 */
DTrack_FlyStick_Type_d* DTrackSDK::getFlyStick(int id)
{
	if ((id >= 0) && (id < act_num_flystick))
		return &act_flystick.at(id);
	return NULL;
}


/**
 * 	\brief	Get number of calibrated measurement tools.
 *
 *	Refers to last received frame.
 *	@return		number of calibrated measurement tools
 */
int DTrackSDK::getNumMeaTool()
{
	return act_num_meatool;
}


/**
 * 	\brief	Get measurement tool data.
 *
 *	Refers to last received frame. Currently not tracked bodies get a quality of -1.
 *	@param[in]	id	id, range 0 .. (max tool id - 1)
 *	@return		id-th measurement tool data.
 */
DTrack_MeaTool_Type_d* DTrackSDK::getMeaTool(int id)
{
	if ((id >= 0) && (id < act_num_meatool))
		return &act_meatool.at(id);
	return NULL;
}


/**
 * 	\brief	Get number of calibrated measurement references.
 *
 *	Refers to last received frame.
 *	@return		number of calibrated measurement references
 */
int DTrackSDK::getNumMeaRef()
{
	return act_num_mearef;
}


/**
 * 	\brief	Get measurement reference data.
 *
 *	Refers to last received frame. Currently not tracked bodies get a quality of -1.
 *	@param[in]	id	id, range 0 .. (max measurement reference id - 1)
 *	@return		id-th measurement reference data.
 */
DTrack_MeaRef_Type_d* DTrackSDK::getMeaRef(int id)
{
	if ((id >= 0) && (id < act_num_mearef))
		return &act_mearef.at(id);
	return NULL;
}


/**
 * 	\brief	Get number of calibrated Fingertracking hands (as far as known).
 *
 *	Refers to last received frame.
 *	@return		number of calibrated fingertracking hands
 */
int DTrackSDK::getNumHand()
{
	return act_num_hand;
}


/**
 * 	\brief	Get Fingertracking hand data.
 *
 *	Refers to last received frame. Currently not tracked bodies get a quality of -1.
 *	@param[in]	id	id, range 0 .. (max hand id - 1)
 *	@return		id-th Fingertracking hand data
 */
DTrack_Hand_Type_d* DTrackSDK::getHand(int id)
{
	if ((id >= 0) && (id < act_num_hand))
		return &act_hand.at(id);
	return NULL;
}


/**
* 	\brief	Get human data
*
*	Refers to last received frame. Currently not tracked human models get a num_joints 0
*	@return		  id-th human model data
*/
int DTrackSDK::getNumHuman()
{
	return act_num_human;
}


/**
* 	\brief	Get human data
*
*	Refers to last received frame. Currently not tracked human models get a num_joints 0
*	@param[in]	id	id, range 0 .. (max standard body id - 1)
*	@return		id-th human model data
*/
DTrack_Human_Type* DTrackSDK::getHuman(int id)
{
	if ((id >= 0) && (id < act_num_human))
		return &act_human.at(id);
	return NULL;
}


/**
 * 	\brief	Get number of tracked single markers.
 *
 *	Refers to last received frame.
 *	@return	number of tracked single markers
 */
int DTrackSDK::getNumMarker()
{
	return act_num_marker;
}


/**
 * 	\brief	Get single marker data.
 *
 *	Refers to last received frame. Currently not tracked bodies get a quality of -1.
 *	@param[in]	index	index, range 0 .. (max marker id - 1)
 *	@return		i-th single marker data
 */
DTrack_Marker_Type_d* DTrackSDK::getMarker(int index)
{
	if ((index >= 0) && (index < act_num_marker))
		return &act_marker.at(index);
	return NULL;
}


/**
 * 	\brief	Get frame counter.
 *
 *	Refers to last received frame.
 *	@return		frame counter
 */
unsigned int DTrackSDK::getFrameCounter()
{
	return act_framecounter;
}


/**
 * 	\brief	Get timestamp.
 *
 *	Refers to last received frame.
 *	@return		timestamp (-1 if information not available)
 */
double DTrackSDK::getTimeStamp()
{
	return act_timestamp;
}


/**
 *	\brief	Send DTrack command via UDP.
 *
 *	Answer is not received and therefore not processed.
 *	@param[in]	command		command string
 *	@return		sending command succeeded? if not, a DTrack error is available
 */
bool DTrackSDK::sendCommand(const std::string command)
{
	if (!isLocalDataPortValid())
		return false;
	lastDataError = ERR_NONE;
	// dest is dtrack2
	if (rsType == SYS_DTRACK_2)	{
		// command style is dtrack?
		if (0 == strncmp(command.c_str(), "dtrack ", 7)) {
			std::string c = command.substr(7);
			// start measurement
			if (0 == strncmp(c.c_str(), "10 3",4)) {
				return startMeasurement();
			}
			// stop measurement
			if (	(0 == strncmp(c.c_str(), "10 0",4))
				||	(0 == strncmp(c.c_str(), "10 1",4)))
			{
				return stopMeasurement();
			}
			// simulate success of other old commands
			return true;
		}
	}
	if (udp_send(d_udpsock, (void*)command.c_str(), (unsigned int)command.length() + 1, d_remote_ip, d_remoteport, d_udptimeout_us))
	{
		lastDataError = ERR_NET;
		return false;
	}
	if (strcmp(command.c_str(), "dtrack 10 3") == 0) {
	#ifdef OS_UNIX
		sleep(1);     // some delay (actually only necessary for older DTrack versions...)
	#endif
	#ifdef OS_WIN
		Sleep(1000);  // some delay (actually only necessary for older DTrack versions...)
	#endif
	}
	return true;
}


/**
 * 	\brief Send DTrack2 command to DTrack and receive answer (TCP command interface).
 *
 *	Answers like "dtrack2 ok" and "dtrack2 err .." are processed. Both cases are reflected in
 *	the return value. getLastDTrackError() and getLastDTrackErrorDescription() will return more information.
 *
 * 	@param[in]	command	DTrack2 command string
 * 	@param[out]	answer	buffer for answer; NULL if specific answer is not needed
 * 	@return	0	specific answer, needs to be parsed
 *  @return 1   answer is "dtrack2 ok"
 *  @return 2   answer is "dtrack2 err ..". Refer to getLastDTrackError() and getLastDTrackErrorDescription().
 * 	@return <0 if error occured (-1 receive timeout, -2 wrong system type, -3 command too long,
 *  -9 broken tcp connection, -10 tcp connection invalid, -11 send command failed)
 */
int DTrackSDK::sendDTrack2Command(const std::string command, std::string *answer)
{
	// Params via TCP are not supported in DTrack
	if (rsType != SYS_DTRACK_2)
		return -2;

	// reset dtrack error
	setLastDTrackError();

	// command too long?
	if (command.length() > DTRACK_PROT_MAXLEN) {
		lastServerError = ERR_NET;
		return -3;
	}

	// connection invalid
	if (!isCommandInterfaceValid()) {
		lastServerError = ERR_NET;
		return -10;
	}

	// send TCP command string:
	if ((tcp_send(d_tcpsock, command.c_str(), command.length() + 1, d_tcptimeout_us))) {
		lastServerError = ERR_NET;
		return -11;
	}

	// receive TCP response string:
	char ans[DTRACK_PROT_MAXLEN];
	int err;
	if ((err = tcp_receive(d_tcpsock, ans, DTRACK_PROT_MAXLEN, d_tcptimeout_us)) < 0) {

		if (err == -1) {	// timeout
			lastServerError = ERR_TIMEOUT;
		}
		else
		if (err == -9) {	// broken connection
			tcp_exit(d_tcpsock);
			d_tcpsock = NULL;
		}
		else
			lastServerError = ERR_NET;	// network error

		if (answer)
			*answer = "";

		return err;
	}

	// parse answer:

	// check for "dtrack2 ok" / no error
	if (0 == strcmp(ans, "dtrack2 ok"))
		return 1;

	// got error msg?
	if (0 == strncmp(ans, "dtrack2 err ", 12)) {
		char *s = ans + 12;
		int i;

		// parse error code
		if (!(s = string_get_i((char *)s, &i))) {
			setLastDTrackError(-1100, "SDK error -1100");
			lastServerError = ERR_PARSE;
			return -1100;
		}
		lastDTrackError = i;

		// parse error string
		if (!(s = string_get_quoted_text((char *)s, lastDTrackErrorString))) {
			setLastDTrackError(-1100, "SDK error -1100");
			lastServerError = ERR_PARSE;
			return -1101;
		}

		return 2;
	}

	// not 'dtrack2 ok'/'dtrack2 err ..' -> return msg
	if (answer)
		*answer = ans;

	lastServerError = ERR_NONE;
	return 0;
}


/**
 * 	\brief	Set DTrack2 parameter.
 *
 *	@param[in] 	category	parameter category
 *	@param[in] 	name		parameter name
 *	@param[in] 	value		parameter value
 *	@return		success? (if not, a DTrack error message is available)
 */
bool DTrackSDK::setParam(const std::string category, const std::string name, const std::string value)
{
	return setParam(category + " " + name + " " + value);
}


/**
 * 	\brief	Set DTrack2 parameter.
 *
 * 	@param[in]	parameter	 complete parameter string without starting "dtrack set "
 *	@return		success? (if not, a DTrack error message is available)
 */
bool DTrackSDK::setParam(const std::string parameter)
{
	// send command, 1 means answer "dtrack2 ok"
	return (1 == sendDTrack2Command("dtrack2 set " + parameter));
}


/**
 * 	\brief	Get DTrack2 parameter.
 *
 *	@param[in] 	category	parameter category
 *	@param[in] 	name		parameter name
 *	@param[out]	value		parameter value
 *	@return		success? (if not, a DTrack error message is available)
 */
bool DTrackSDK::getParam(const std::string category, const std::string name, std::string& value)
{
	return getParam(category + " " + name, value);
}


/**
 * 	\brief	Get DTrack2 parameter.
 *
 *	@param[in] 	parameter	complete parameter string without starting "dtrack get "
 *	@param[out]	value		parameter value
 *	@return		success? (if not, a DTrack error message is available)
 */
bool DTrackSDK::getParam(const std::string parameter, std::string& value)
{
	// Params via TCP are not supported in DTrack
	if (rsType != SYS_DTRACK_2)
		return false;

	std::string res;
	// expected answer is "dtrack2 set" -> return value 0
	if (0 != sendDTrack2Command("dtrack2 get " + parameter, &res))
		return false;

	// parse parameter from answer
	if (0 == strncmp(res.c_str(), "dtrack2 set ", 12)) {
		char *str = strdup(res.c_str() + 12);
		char *s = str;
		if (!(s = string_cmp_parameter(s, parameter.c_str()))) {
			free(str);
			lastServerError = ERR_PARSE;
			return false;
		}

		// assign result
		value = s;
		free(str);
		return true;
	}

	return false;
}


/**
 *	\brief	Get DTrack2 message.
 *
 *	Updates internal message structures
 *	@return message available?
 */
bool DTrackSDK::getMessage()
{
	// Messages via TCP are not supported in DTrack
	if (rsType != SYS_DTRACK_2)
		return false;

	// send request
	std::string res;
	if (0 != sendDTrack2Command("dtrack2 getmsg", &res))
		return false;

	// check answer
	if (0 != strncmp(res.c_str(), "dtrack2 msg ", 12))
		return false;

	// reset values
	d_message_origin = d_message_msg = d_message_status = "";
	d_message_framenr = d_message_errorid = 0;

	// parse message
	const char* s = res.c_str() + 12;
	// get 'origin'
	if (!(s = string_get_word((char *)s, d_message_origin)))
		return false;

	// get 'status'
	if (!(s = string_get_word((char *)s, d_message_status)))
			return false;

	unsigned int ui;
	// get 'frame counter'
	if(!(s = string_get_ui((char *)s, &ui)))
		return false;
	d_message_framenr = ui;

	// get 'error id'
	if(!(s = string_get_ui((char *)s, &ui)))
		return false;
	d_message_errorid = ui;

	// get 'message'
	if(!(s = string_get_quoted_text((char *)s, d_message_msg)))
		return false;

	return true;
}


/**
 * 	\brief Get data port where tracking data is received.
 *
 *	@return Data port.
 */
unsigned short DTrackSDK::getDataPort()
{
	return d_udpport;
}


/**
 *	\brief Get origin of last DTrack2 message.
 *
 *	@return origin
 */
std::string DTrackSDK::getMessageOrigin()
{
	return d_message_origin;
}

/**
 *	\brief Get status of last DTrack2 message.
 *
 *	@return status
 */
std::string DTrackSDK::getMessageStatus()
{
	return d_message_status;
}


/**
 * 	\brief Get frame counter of last DTrack2 message.
 *
 *	@return frame counter
 */
unsigned int DTrackSDK::getMessageFrameNr()
{
	return d_message_framenr;
}


/**
 * 	\brief Get error id of last DTrack2 message.
 *
 *	@return error id
 */
unsigned int DTrackSDK::getMessageErrorId()
{
	return d_message_errorid;
}


/**
 * 	\brief Get message string of last DTrack2 message.
 *
 *	@return mesage string
 */
std::string DTrackSDK::getMessageMsg()
{
	return d_message_msg;
}


/**
 * 	\brief Start measurement.
 *
 *	Ensure via DTrack frontend that data is sent to the local data port.
 *	@return 	Is command successful? If measurement is already running the return value is false.
 */
bool DTrackSDK::startMeasurement()
{
	// Check for special DTrack handling
	if (rsType == SYS_DTRACK) {
		return (sendCommand("dtrack 10 3")) && (sendCommand("dtrack 31"));
	}

	// start tracking, 1 means answer "dtrack2 ok"
	return (1 == sendDTrack2Command("dtrack2 tracking start"));
}


/**
 * 	\brief Stop measurement.
 *
 * 	@return 	Is command successful? If measurement is not running return value is true.
 */
bool DTrackSDK::stopMeasurement()
{
	// Check for special DTrack handling
	if (rsType == SYS_DTRACK) {
		return (sendCommand("dtrack 32")) && (sendCommand("dtrack 10 0"));
	}

	// stop tracking, 1 means answer "dtrack2 ok"
	return (1 == sendDTrack2Command("dtrack2 tracking stop"));
}
