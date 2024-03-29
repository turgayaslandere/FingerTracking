/* DTrackSDK: C++ example, A.R.T. GmbH 16.12.05-13.5.11
 *
 * example_without_remote_control:
 *    C++ example using DTrack without DTrack remote control
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
 *  - example without DTrack remote commands: collects DTrack data
 *  - using DTrackSDK v2.0.2
 *  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
 *
 * $Id: example_without_remote_control.cpp,v 1.5 2011/07/06 08:52:15 sebastianz Exp $
 */
#include "DTrack.hpp"

#include <iostream>
#include <sstream>

using namespace std;

// global DTrack
static DTrack* dt = NULL;


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
	dtrack_body_type body;
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

	// A.R.T. FlySticks:
	dtrack_flystick_type flystick;
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
	dtrack_meatool_type meatool;
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
	dtrack_marker_type marker;
	for(int i=0; i<dt->get_num_marker(); i++){
		marker = dt->get_marker(i);

		cout << "mar " << marker.id << " qu " << marker.quality
		     << " loc " << marker.loc[0] << " " << marker.loc[1] << " " << marker.loc[2] << endl;
	}

	// A.R.T. Fingertracking hands:
	dtrack_hand_type hand;
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
static bool error_to_console()
{
	if(dt->timeout()){
		cout << "--- timeout while waiting for udp data" << endl;
		return false;
	}

	if(dt->udperror()){
		cout << "--- error while receiving udp data" << endl;
		return false;
	}

	if(dt->parseerror()){
		cout << "--- error while parsing udp data" << endl;
		return false;
	}

	return true;
}


/**
 * 	\brief Main.
 */
int main(int argc, char** argv)
{
	if(argc != 2){
		cout << "Usage: " << argv[0] << " <data port>" << endl;
		return -1;
	}

	istringstream portstream(argv[1]);
	int port;
	portstream >> port;  // data port


	if(portstream.fail() || port <=0 || port >= 65536){
		cout << "invalid port '" << argv[1] << "'" << endl;
		return -2;
	}

	// init library:
	dt = new DTrack(port);

	if(!dt->valid()){
		cout << "DTrack init error" << endl;
		return -3;
	}

	// receiving:
	int loop_count = 0;
	while(loop_count++ < 100){

		if(dt->receive()){
			output_to_console();
		} else {
			error_to_console();
		}

	}

	//clean up:
	delete dt;

	return 0;
}
