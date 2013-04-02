/* DTrackSDK: C++ example, A.R.T. GmbH 16.12.05-13.5.11
 *
 * example_without_remote_control:
 *    C++ example using DTrack2 with DTrack2 remote control
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
 * Purpose:
 *  - example with DTrack2 remote commands: collects DTrack data
 *  - using DTrackSDK v2.0.2
 *  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
 *
 * $Id: example_with_remote_control.cpp,v 1.7 2011/07/06 08:52:15 sebastianz Exp $
 */
#include "DTrack2.hpp"

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

// global DTrack2
static DTrack2* dt = NULL;


/**
 * 	\brief Prints current tracking data to console.
 */
static void output_to_console()
{
	cout.precision(3);
	cout.setf(ios::fixed, ios::floatfield);

	cout << endl << "frame " << dt->get_framecounter() << " ts " << dt->get_timestamp()
	     << " nbod " << dt->get_num_body() << " nfly " << dt->get_num_flystick()
	     << " nmea " << dt->get_num_meatool() << " nhand " << dt->get_num_hand()
	     << " nmar " << dt->get_num_marker() << endl;

	// bodies:
	dtrack2_body_type body;
	for(int i=0; i<dt->get_num_body(); i++){
		body = dt->get_body(i);

		if(body.quality < 0){
			cout << "bod " << body.id << " not tracked" << endl;
		}else{
			cout << "bod " << body.id << " qu " << body.quality
			     << " loc " << body.loc[0] << " " << body.loc[1] << " " << body.loc[2]
			     << " rot " << body.rot[0] << " " << body.rot[1] << " " << body.rot[2]
			     << " " << body.rot[3] << " " << body.rot[4] << " " << body.rot[5]
			     << " " << body.rot[6] << " " << body.rot[7] << " " << body.rot[8] << endl;
		}
	}

	// A.R.T. Flysticks:
	dtrack2_flystick_type flystick;
	for(int i=0; i<dt->get_num_flystick(); i++){
		flystick = dt->get_flystick(i);

		if(flystick.quality < 0){
			cout << "fly " << flystick.id << " not tracked" << endl;
		}else{
			cout << "flystick " << flystick.id << " qu " << flystick.quality
			     << " loc " << flystick.loc[0] << " " << flystick.loc[1] << " " << flystick.loc[2]
			     << " rot " << flystick.rot[0] << " " << flystick.rot[1] << " " << flystick.rot[2]
			     << " " << flystick.rot[3] << " " << flystick.rot[4] << " " << flystick.rot[5]
			     << " " << flystick.rot[6] << " " << flystick.rot[7] << " " << flystick.rot[8] << endl;
		}

		cout << "      btn";
		for(int j=0; j<flystick.num_button; j++){
			cout << " " << flystick.button[j];
		}

		cout << " joy";
		for(int j=0; j<flystick.num_joystick; j++){
			cout << " " << flystick.joystick[j];
		}

		cout << endl;
	}

	// measurement tools:
	dtrack2_meatool_type meatool;
	for(int i=0; i<dt->get_num_meatool(); i++){
		meatool = dt->get_meatool(i);

		if(meatool.quality < 0){
			cout << "mea " << meatool.id << " not tracked" << endl;
		}else{
			cout << "mea " << meatool.id << " qu " << meatool.quality
			     << " loc " << meatool.loc[0] << " " << meatool.loc[1] << " " << meatool.loc[2]
			     << " rot " << meatool.rot[0] << " " << meatool.rot[1] << " " << meatool.rot[2]
			     << " " << meatool.rot[3] << " " << meatool.rot[4] << " " << meatool.rot[5]
			     << " " << meatool.rot[6] << " " << meatool.rot[7] << " " << meatool.rot[8] << endl;
		}

		if (meatool.num_button) {
			cout << "      btn";
			for(int j=0; j<meatool.num_button; j++){
				cout << " " << meatool.button[j];
			}
			cout << endl;
		}
	}

	// markers:
	dtrack2_marker_type marker;
	for(int i=0; i<dt->get_num_marker(); i++){
		marker = dt->get_marker(i);

		cout << "mar " << marker.id << " qu " << marker.quality
		     << " loc " << marker.loc[0] << " " << marker.loc[1] << " " << marker.loc[2] << endl;
	}

	// A.R.T. Fingertracking hands:
	dtrack2_hand_type hand;
	for(int i=0; i<dt->get_num_hand(); i++){
		hand = dt->get_hand(i);

		if(hand.quality < 0){
			cout << "hand " << hand.id << " not tracked" << endl;
		}else{
			cout << "hand " << hand.id << " qu " << hand.quality
			     << " lr " << ((hand.lr == 0) ? "left" : "right") << " nf " << hand.nfinger
			     << " loc " << hand.loc[0] << " " << hand.loc[1] << " " << hand.loc[2]
			     << " rot " << hand.rot[0] << " " << hand.rot[1] << " " << hand.rot[2]
			     << " " << hand.rot[3] << " " << hand.rot[4] << " " << hand.rot[5]
			     << " " << hand.rot[6] << " " << hand.rot[7] << " " << hand.rot[8] << endl;

			for(int j=0; j<hand.nfinger; j++){
				cout << "       fi " << j
				     << " loc " << hand.finger[j].loc[0] << " " << hand.finger[j].loc[1] << " " << hand.finger[j].loc[2]
				     << " rot " << hand.finger[j].rot[0] << " " << hand.finger[j].rot[1] << " " << hand.finger[j].rot[2]
				     << " " << hand.finger[j].rot[3] << " " << hand.finger[j].rot[4] << " " << hand.finger[j].rot[5]
				     << " " << hand.finger[j].rot[6] << " " << hand.finger[j].rot[7] << " " << hand.finger[j].rot[8] << endl;
				cout << "       fi " << j
				     << " tip " << hand.finger[j].radiustip
				     << " pha " << hand.finger[j].lengthphalanx[0] << " " << hand.finger[j].lengthphalanx[1]
				     << " " << hand.finger[j].lengthphalanx[2]
				     << " ang " << hand.finger[j].anglephalanx[0] << " " << hand.finger[j].anglephalanx[1] << endl;
			}
		}
	}
}


/**
 * 	\brief Prints error messages to console
 *
 * 	@return No error occured?
 */
static bool data_error_to_console()
{
	if(dt->data_timeout()){
		cout << "--- timeout while waiting for udp data" << endl;
		return false;
	}

	if(dt->data_neterror()){
		cout << "--- error while receiving udp data" << endl;
		return false;
	}

	if(dt->data_parseerror()){
		cout << "--- error while parsing udp data" << endl;
		return false;
	}

	return true;
}


/**
 * 	\brief	Prints ARTtrack Controller command errors to console
 *
 * 	@return No error occured?
 */
static bool command_error_to_console()
{
	// SDK errors:
	if(dt->server_noconnection()){
		cout << "--- no connection to ARTtrack Controller" << endl;
		return false;
	}

	if(dt->server_timeout()){
		cout << "--- timeout while waiting for server answer" << endl;
		return false;
	}

	if(dt->server_neterror()){
		cout << "--- error while waiting for server answer" << endl;
		return false;
	}

	if(dt->server_parseerror()){
		cout << "--- error while parsing server answer" << endl;
		return false;
	}

	// ARTtrack Controller errors:
	int errcode;
	if(!dt->get_lasterror(errcode)){
		return true;
	}

	string errstring;
	dt->get_lasterror(errstring);
	cout << "ATC command error: " << errcode << " \"" << errstring << "\"" << endl;

	return false;
}


/**
 * 	\brief	Prints ATC messages to console.
 */
static void messages_to_console()
{
	while(dt->get_message()){
		cout << "ATC message: \"" << dt->get_message_status() << "\" \"" << dt->get_message_msg() << "\"" << endl;
	}
}


/**
 * 	\brief Main.
 */
int main(int argc, char** argv)
{
	if(argc != 3){
		cout << "Usage: " << argv[0] << " <server host> <data port>" << endl;
		return -1;
	}

	int port = 0;
	istringstream portstream(argv[2]);
	portstream >> port;  // data port
	if(portstream.fail() || port <=0 || port >= 65536){
		cout << "invalid port '" << argv[2] << "'" << endl;
		return -2;
	}

	// initialize library:
	dt = new DTrack2((const char *)argv[1], 50105, port);

	if(!dt->valid()){
		cout << "DTrack2 init error" << endl;
		return -3;
	}

	cout << "connected to ATC '" << argv[1] <<"' (local data port " << dt->get_data_port() << ")" << endl;

	// start measurement:

	if(!dt->send_command("tracking start")){  // command: start measurement
		command_error_to_console();
		messages_to_console();
		delete dt;
		return -4;
	}

	// receiving:
	int loop_count = 0;
	while(loop_count++ < 100){
		if(dt->receive()){
			output_to_console();
		} else {
			data_error_to_console();
			command_error_to_console();
		}
		messages_to_console();
	}

	// stop measurement & clean up:
	dt->send_command("tracking stop");
	command_error_to_console();
	messages_to_console();
	delete dt;
	cout << "disconnected from ATC" << endl;

	return 0;
}
