/* DTrackSDK: C++ header file, A.R.T. GmbH 21.4.05-13.5.11
 *
 * DTrack: functions to receive and process DTrack UDP packets (ASCII protocol)
 * Copyright (C) 2005-2011, Advanced Realtime Tracking GmbH
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
 * Version v2.0.2
 *
 * Purpose:
 *  - receives DTrack UDP packets (ASCII protocol) and converts them into easier to handle data
 *  - sends DTrack remote commands (UDP)
 *  - DTrack network protocol due to: 'Technical Appendix DTrack v1.24 (December 19, 2006)'
 *  - for DTrack versions v1.16 - v1.24 (and compatible versions)
 *  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
 *
 * Usage:
 *  - for Linux, Unix:
 *    - comment '#define OS_WIN', uncomment '#define OS_UNIX' in file 'DTrack.cpp'
 *  - for MS Windows:
 *    - comment '#define OS_UNIX', uncomment '#define OS_WIN' in file 'DTrack.cpp'
 *    - link with 'ws2_32.lib'
 *
 * $Id: DTrack.hpp,v 1.6 2011/07/06 08:52:15 sebastianz Exp $
 */

#ifndef _ART_DTRACK_H
#define _ART_DTRACK_H

#include "../../DTrackSDK.hpp"

// Typedef aliases
typedef DTrack_Body_Type      dtrack_body_type;
typedef DTrack_FlyStick_Type  dtrack_flystick_type;
typedef DTrack_MeaTool_Type   dtrack_meatool_type;
typedef DTrack_Hand_Type      dtrack_hand_type;
typedef DTrack_Marker_Type    dtrack_marker_type;

#define DTRACK_CMD_CAMERAS_OFF			1
#define DTRACK_CMD_CAMERAS_ON			2
#define DTRACK_CMD_CAMERAS_AND_CALC_ON	3

#define DTRACK_CMD_SEND_DATA			11
#define DTRACK_CMD_STOP_DATA			12
#define DTRACK_CMD_SEND_N_DATA			13

/**
 * 	Wrapper class for DTrack SDK.
 */
class DTrack
{
public:
	/**
	 * 	\brief	Constructor.
	 *
	 * 	@param[in] data_port		UDP port number to receive data from DTrack
	 *	@param[in] remote_host		DTrack remote control: hostname or IP address of DTrack PC (NULL if not used)
	 *	@param[in] remote_port		port number of DTrack remote control (0 if not used)
	 *	@param[in] data_bufsize		size of buffer for UDP packets (in bytes)
	 *	@param[in] data_timeout_us	UDP timeout (receiving and sending) in us (micro second)
	 */
	DTrack(
		int data_port = 5000, const char* remote_host = NULL, int remote_port = 0,
		int data_bufsize = 20000, int data_timeout_us = 1000000
	);

	/**
	 * 	\brief	Destructor.
	 */
	~DTrack(void);

	/**
	 * 	\brief	Check if initialization was successfull.
	 *
	 *	@return Valid?
	 */
	bool valid(void);

	/**
	 * 	\brief Check last receive/send error (timeout)
	 *
	 *	@return	timeout occured?
	 */
	bool timeout();

	/**
	 * 	\brief Check last receive/send error (udp error)
	 *
	 * 	@return	udp error occured?
	 */
	bool udperror();

	/**
	 * 	\brief Check last receive/send error (parser)
	 *
	 *	@return	error occured while parsing tracking data?
	 */
	bool parseerror();

	/**
	 *	\brief	Receive and process one DTrack data packet (UDP; ASCII protocol)
	 *
	 *	@return	successful?
	 */
	bool receive();

	/**
	 * 	\brief	Get frame counter.
	 *
	 *	Refers to last received frame.
	 *	@return	frame counter
	 */
	unsigned int get_framecounter();

	/**
	 * 	\brief	Get timestamp.
	 *
	 *	Refers to last received frame.
	 *	@return	timestamp (-1 if information not available)
	 */
	double get_timestamp();

	/**
	 * 	\brief	Get number of standard bodies.
	 *
	 *	Refers to last received frame.
	 *	@return	number of standard bodies
	 */
	int get_num_body();

	/**
	 * 	\brief	Get 6d data.
	 *
	 *	Refers to last received frame.
	 *	@param[in]	id	id, range 0 .. num_body - 1
	 *	@return		id-th standard body data.
	 */
	dtrack_body_type get_body(int id);

	/**
	 * 	\brief	Get number of flysticks.
	 *
	 *	Refers to last received frame.
	 *	@return	number of flysticks
	 */
	int get_num_flystick();

	/**
	 * 	\brief	Get 6df data.
	 *
	 *	Refers to last received frame.
	 *	@param[in]	id	id, range 0 .. num_flystick - 1
	 *	@return		id-th flystick data
	 */
	dtrack_flystick_type get_flystick(int id);

	/**
	 * 	\brief	Get number of measurement tools.
	 *
	 *	Refers to last received frame.
	 *	@return	number of measurement tools
	 */
	int get_num_meatool();

	/**
	 * 	\brief	Get 6dmt data.
	 *
	 *	Refers to last received frame.
	 *	@param[in]	id	id, range 0 .. num_meatool - 1
	 *	@return		id-th measurement tool data
	 */
	dtrack_meatool_type get_meatool(int id);

	/**
	 * 	\brief	Get number of fingertracking hands.
	 *
	 *	Refers to last received frame.
	 *	@return	number of fingertracking hands
	 */
	int get_num_hand();

	/**
	 * 	\brief	Get gl data.
	 *
	 *	Refers to last received frame.
	 *	@param[in]	id	id, range 0 .. num_hand - 1
	 *	@return		id-th Fingertracking hand data.
	 */
	dtrack_hand_type get_hand(int id);

	/**
	 * 	\brief	Get number of single markers
	 *
	 *	Refers to last received frame.
	 *	@return	number of single markers
	 */
	int get_num_marker();

	/**
	 * 	\brief	Get 3d data.
	 *
	 *	Refers to last received frame.
	 *	@param[in]	index	index, range 0 .. num_marker - 1
	 *	@return		id-th single marker data
	 */
	dtrack_marker_type get_marker(int index);

	/**
	 * 	\brief	Control cameras by remote commands to DTrack (UDP; ASCII protocol; Default off).
	 *
	 *	@param[in]	onoff	switch on/off
	 *	@return		sending of remote commands was successfull
	 */
	bool cmd_cameras(bool onoff);

	/**
	 * 	\brief	Control tracking calculation by remote commands to DTrack (UDP, ASCII protocol; Default: on).
	 *
	 *	@param[in]	onoff	Switch function (1 on, 0 off)
	 *	@return 	sending of remote commands was successfull (1 yes, 0 no)
	 */
	bool cmd_tracking(bool onoff);

	/**
	 * 	\brief	Control sending of UDP output data by remote commands to DTrack (UDP, ASCII protocol; Default: on).
	 *
	 *	@param[in]	onoff	Switch function (1 on, 0 off)
	 *	@return 	sending of remote commands was successfull (1 yes, 0 no)
	 */
	bool cmd_sending_data(bool onoff);

	/**
	 * 	\brief	Control sending of a fixed number of UDP output data frames by remote commands to DTrack (UDP, ASCII protocol).
	 *
	 *	@param[in]	frames	Number of frames
	 *	@return 	sending of remote commands was successfull (1 yes, 0 no)
	 */
	bool cmd_sending_fixed_data(int frames);  // start sending of a fixed number of UDP output frames

private:
	DTrackSDK *sdk;         //!< unified sdk
	bool remoteCameras;		//!< DTrack status: cameras on/off
	bool remoteTracking;	//!< DTrack status: tracking on/off
	bool remoteSending;		//!< DTrack status: sending of UDP output data on/off
	int act_num_body;								//!< number of standard bodies
	std::vector<dtrack_body_type> act_body;			//!< array containing 6d data
	int act_num_flystick;							//!< number of flysticks
	std::vector<dtrack_flystick_type> act_flystick;	//!< array containing 6df data
	int act_num_meatool;							//!< number of measurement tools
	std::vector<dtrack_meatool_type> act_meatool;	//!< array containing 6dmt data
	int act_num_marker;								//!< number of single markers
	std::vector<dtrack_marker_type> act_marker;		//!< array containing 3d data
	int act_num_hand;								//!< number of fingertracking hands
	std::vector<dtrack_hand_type> act_hand;			//!< array containing gl data
};

#endif // _ART_DTRACK_H
