/* DTrack2SDK: C++ header file, A.R.T. GmbH 3.5.07-13.5.11
 *
 * DTrack2: functions to receive and process DTrack UDP packets (ASCII protocol), as
 * well as to exchange DTrack2 TCP command strings.
 * Copyright (C) 2007-2008, Advanced Realtime Tracking GmbH
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
 *  - sends and receives DTrack2 commands (TCP)
 *  - DTrack2 network protocol due to: 'Technical Appendix DTrack v2.0'
 *  - for ARTtrack Controller versions v0.2 (and compatible versions)
 *  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
 *
 * Usage:
 *  - for Linux, Unix:
 *    - comment '#define OS_WIN', uncomment '#define OS_UNIX' in file 'DTrack2.cpp'
 *  - for MS Windows:
 *    - comment '#define OS_UNIX', uncomment '#define OS_WIN' in file 'DTrack2.cpp'
 *    - link with 'ws2_32.lib'
 *
 * $Id: DTrack2.hpp,v 1.4 2011/07/06 08:52:15 sebastianz Exp $
 */

#ifndef _ART_DTRACK2_H
#define _ART_DTRACK2_H

#include <string>
#include <vector>

#include "../../DTrackSDK.hpp"

// Typedef aliases
typedef DTrack_Body_Type      dtrack2_body_type;
typedef DTrack_FlyStick_Type  dtrack2_flystick_type;
typedef DTrack_MeaTool_Type   dtrack2_meatool_type;
typedef DTrack_Hand_Type      dtrack2_hand_type;
typedef DTrack_Marker_Type    dtrack2_marker_type;

/**
 * 	\brief Wrapper class for DTrack 2 SDK.
 */
class DTrack2
{
public:

	/**
	 * 	\brief	Constructor.
	 *
	 * 	@param[in] server_host 			TCP access to DTrack2 server: hostname or IP address of ARTtrack Controller, empty string if not used
	 *	@param[in] server_port			TCP access to DTrack2 server: port number of ARTtrack Controller, default 50105
	 *	@param[in] data_port			UDP port number to receive tracking data from ARTtrack Controller (0 if to be chosen)
	 *	@param[in] data_bufsize			size of buffer for UDP packets (in bytes)
	 *	@param[in] data_timeout_us		UDP timeout (receiving) in us (in micro second)
	 *	@param[in] server_timeout_us	TCP timeout for access to DTrack2 server (in micro second; receiving and sending)
	 */
	DTrack2(
		const std::string& server_host = "", unsigned short server_port = 50105, unsigned short data_port = 0,
		int data_bufsize = 20000, int data_timeout_us = 1000000, int server_timeout_us = 10000000
	);

	/**
	 * 	\brief Destructor.
	 */
	~DTrack2();

	/**
	 * 	\brief	Check if initialization was successfull.
	 *
	 *	@return Valid?
	 */
	bool valid();

	/**
	 * 	\brief Get used UDP port number.
	 *
	 *	@return	local udp data port used for receiving tracking data
	 */
	unsigned short get_data_port();

	/**
	 * 	\brief Check last data receive error (timeout)
	 *
	 *	@return	timeout occured?
	 */
	bool data_timeout();

	/**
	 * 	\brief Check last data receive error (net error)
	 *
	 * 	@return	net error occured?
	 */
	bool data_neterror();

	/**
	 * 	\brief Check last data receive error (parser)
	 *
	 *	@return	error occured while parsing tracking data?
	 */
	bool data_parseerror();

	/**
	 * 	\brief Check if connection to DTrack2 server is completely lost.
	 *
	 * 	@return connection lost?
	 */
	bool server_noconnection();

	/**
	 * 	\brief	Check last command receive/send error (timeout)
	 *
	 *	@return	timeout occured?
	 */
	bool server_timeout();

	/**
	 * 	\brief	Check last command receive/send error (network)
	 *
	 *	@return net error occured?
	 */
	bool server_neterror();

	/**
	 * 	\brief	Check last command receive/send error (parsing)
	 *
	 *	@return	error occured while parsing command data?
	 */
	bool server_parseerror();

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
	dtrack2_body_type get_body(int id);

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
	dtrack2_flystick_type get_flystick(int id);

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
	dtrack2_meatool_type get_meatool(int id);

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
	dtrack2_hand_type get_hand(int id);

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
	dtrack2_marker_type get_marker(int index);

	/**
	 * 	\brief	Set DTrack2 parameter.
	 *
	 *	@param[in] 	category	parameter category
	 *	@param[in] 	name		parameter name
	 *	@param[in] 	value		parameter value
	 *	@return 	setting was successful (if not, a DTrack2 error message is available)
	 */
	bool set_parameter(const std::string category, const std::string name, const std::string value);

	/**
	 * 	\brief	Set DTrack2 parameter.
	 *
	 * 	@param[in]	parameter	total parameter (category, name and value; without starting "dtrack2 set ")
	 *	@return		setting was successful (if not, a DTrack2 error message is available)
	 */
	bool set_parameter(const std::string parameter);

	/**
	 * 	\brief	Get DTrack2 parameter.
	 *
	 *	@param[in] 	category	parameter category
	 *	@param[in] 	name		parameter name
	 *	@param[out]	value		parameter value
	 *	@return		getting was successful (if not, a DTrack2 error message is available)
	 */
	bool get_parameter(const std::string category, const std::string name, std::string& value);

	/**
	 * 	\brief	Get DTrack2 parameter.
	 *
	 *	@param[in] 	parameter	total parameter (category and name; without starting "dtrack2 get ")
	 *	@param[out]	value		parameter value
	 *	@return		getting was successful (if not, a DTrack2 error message is available)
	 */
	bool get_parameter(const std::string parameter, std::string& value);

	/**
	 *	\brief	Send DTrack2 command.
	 *
	 *	@param[in]	command	command (without starting "dtrack2 ")
	 *	@return 	command was successful and "dtrack2 ok" is received (if not, a DTrack2 error message is available)
	 */
	bool send_command(const std::string command);

	/**
	 * 	\brief	Get last DTrack2 error code.
	 *
	 *	@param[out]	errorcode	error as code number
	 * 	@return 	error code was available, otherwise last command was successful
	 */
	bool get_lasterror(int& errorcode);

	/**
	 * 	\brief	Get last DTrack2 error code.
	 *
	 *	@param[out]	errorstring	error as string
	 *	@return 	error code was available, otherwise last command was successful
	 */
	bool get_lasterror(std::string& errorstring);

	/**
	 * 	\brief	Get DTrack2 message.
	 *
	 *	@return message was available
	 */
	bool get_message();
	
	/**
	 * 	\brief Get origin of last DTrack2 message.
	 *
	 *	@return origin
	 */
	std::string get_message_origin();

	/**
	 * 	\brief Get status of last DTrack2 message.
	 *
	 *	@return status
	 */
	std::string get_message_status();

	/**
	 * 	\brief Get frame counter of last DTrack2 message.
	 *
	 *	@return frame counter
	 */
	unsigned int get_message_framenr();

	/**
	 * 	\brief Get error id of last DTrack2 message.
	 *
	 *	@return error id
	 */
	unsigned int get_message_errorid();

	/**
	 * 	\brief Get message string of last DTrack2 message.
	 *
	 *	@return mesage string
	 */
	std::string get_message_msg();
private:
	DTrackSDK *sdk;                                     //!< unified sdk
	int act_num_body;									//!< number of standard bodies
	std::vector<dtrack2_body_type> act_body;			//!< array containing 6df data
	int act_num_flystick;								//!< number of flysticks
	std::vector<dtrack2_flystick_type> act_flystick;	//!< array containing flystick data
	int act_num_meatool;								//!< number of measurement tools
	std::vector<dtrack2_meatool_type> act_meatool;		//!< array containing 6dmt data
	int act_num_marker;									//!< number of single markers
	std::vector<dtrack2_marker_type> act_marker;		//!< array containing 3d data
	int act_num_hand;									//!< number of fingertracking hands
	std::vector<dtrack2_hand_type> act_hand;			//!< array containing gl data
};

#endif // _ART_DTRACK2_H


