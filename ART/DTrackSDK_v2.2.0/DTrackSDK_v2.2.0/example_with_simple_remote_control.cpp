/* DTrackSDK: C++ example, A.R.T. GmbH 16.12.05-27.1.12
*
* example_with_simple_remote_control:
*    C++ example using DTrackSDK with simple DTrack/DTrack2 remote control
* Copyright (C) 2005-2012, Advanced Realtime Tracking GmbH
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
*  - example with DTrack/DTrack2 remote commands:
*    starts measurement, collects frames and stops measurement again
*  - using DTrackSDK v2.2.0
*  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
*
* $Id: example_with_simple_remote_control.cpp,v 1.7 2012/01/27 08:50:41 christoph Exp $
*/
#include "DTrackSDK.hpp"

#include <iostream>
#include <sstream>

using namespace std;

// global DTrackSDK
static DTrackSDK* dt = NULL;


/**
* 	\brief Prints current tracking data to console.
*/
static void output_to_console()
{
	cout.precision(3);
	cout.setf(ios::fixed, ios::floatfield);

	cout << endl << "frame " << dt->getFrameCounter() << " ts " << dt->getTimeStamp()
		<< " nbod " << dt->getNumBody() << " nfly " << dt->getNumFlyStick()
		<< " nmea " << dt->getNumMeaTool() << " nmearef " << dt->getNumMeaRef() 
		<< " nhand " << dt->getNumHand() << " nmar " << dt->getNumMarker() 
		<< " nhuman " << dt->getNumHuman() 
		<< endl;

	// bodies:
	DTrack_Body_Type_d body;
	for(int i=0; i<dt->getNumBody(); i++){
		body = *dt->getBody(i);

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
	DTrack_FlyStick_Type_d flystick;
	for(int i=0; i<dt->getNumFlyStick(); i++){
		flystick = *dt->getFlyStick(i);

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
	DTrack_MeaTool_Type_d meatool;
	for(int i=0; i<dt->getNumMeaTool(); i++){
		meatool = *dt->getMeaTool(i);

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

	// measurement references:
	DTrack_MeaRef_Type_d mearef;
	for(int i=0; i<dt->getNumMeaRef(); i++){
		mearef = *dt->getMeaRef(i);

		if(mearef.quality < 0){
			cout << "mearef " << mearef.id << " not tracked" << endl;
		}else{
			cout << "mearef " << mearef.id << " qu " << mearef.quality
				<< " loc " << mearef.loc[0] << " " << mearef.loc[1] << " " << mearef.loc[2]
			<< " rot " << mearef.rot[0] << " " << mearef.rot[1] << " " << mearef.rot[2]
			<< " " << mearef.rot[3] << " " << mearef.rot[4] << " " << mearef.rot[5]
			<< " " << mearef.rot[6] << " " << mearef.rot[7] << " " << mearef.rot[8] << endl;
		}
	}

	// markers:
	DTrack_Marker_Type_d marker;
	for(int i=0; i<dt->getNumMarker(); i++){
		marker = *dt->getMarker(i);

		cout << "mar " << marker.id << " qu " << marker.quality
			<< " loc " << marker.loc[0] << " " << marker.loc[1] << " " << marker.loc[2] << endl;
	}

	// A.R.T. Fingertracking hands:
	DTrack_Hand_Type_d hand;
	for(int i=0; i<dt->getNumHand(); i++){
		hand = *dt->getHand(i);

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

	// A.R.T human model (6dj)
	DTrack_Human_Type human;

	if (dt->getNumHuman() < 1) {
		cout << "no human model data" << endl;
	}

	for(int i=0; i<dt->getNumHuman(); i++){
		human = *dt->getHuman(i);
		cout << "human " << human.id << " num joints " << human.num_joints << endl;
		for (int j=0; j<human.num_joints; j++){
			if(human.joint[j].quality<0){
				cout << "joint " << human.joint[j].id << " not tracked" << endl;
			}else{
				cout << "joint " << human.joint[j].id << " qu " << human.joint[j].quality
					<< " loc "  << human.joint[j].loc[0] << " " << human.joint[j].loc[1] << " " << human.joint[j].loc[2]
				<< " ang "  << human.joint[j].ang[0] << " " << human.joint[j].ang[1] << " " << human.joint[j].ang[2]
				<< " rot "  << human.joint[j].rot[0] << " " << human.joint[j].rot[1] << " " << human.joint[j].rot[2]
				<< " "      << human.joint[j].rot[3] << " " << human.joint[j].rot[4] << " " << human.joint[j].rot[5]
				<< " "      << human.joint[j].rot[6] << " " << human.joint[j].rot[7] << " " << human.joint[j].rot[8]
				<< endl;
			}
		}
		cout << endl;
	}
}


/**
* 	\brief Prints error messages to console
*
* 	@return No error occured?
*/
static bool data_error_to_console()
{
	if (dt->getLastDataError() == DTrackSDK::ERR_TIMEOUT) {
		cout << "--- timeout while waiting for tracking data" << endl;
		return false;
	}

	if (dt->getLastDataError() == DTrackSDK::ERR_NET) {
		cout << "--- error while receiving tracking data" << endl;
		return false;
	}

	if (dt->getLastDataError() == DTrackSDK::ERR_PARSE){
		cout << "--- error while parsing tracking data" << endl;
		return false;
	}

	return true;
}


/**
* 	\brief	Prints ATC messages to console.
*/
static void messages_to_console()
{
	while(dt->getMessage()){
		cout << "ATC message: \"" << dt->getMessageStatus() << "\" \"" << dt->getMessageMsg() << "\"" << endl;
	}
}


/**
* 	\brief Main.
*/
int main(int argc, char** argv)
{
	if (argc != 4) {
		cout << "Usage: " << argv[0] << " <server host> <server port> <data port>" << endl;
		return -1;
	}

	int port = 0, rport = 0;
	istringstream portstream(argv[2]);
	portstream >> rport;  // remote port
	if(portstream.fail() || rport <=0 || rport >= 65536){
		cout << "invalid remote port '" << argv[3] << "'" << endl;
		return -2;
	}

	portstream.clear();
	portstream.str(argv[3]);
	portstream >> port;  // data port
	if(portstream.fail() || port <=0 || port >= 65536){
		cout << "invalid port '" << argv[2] << "'" << endl;
		return -2;
	}

	// init library:
	dt = new DTrackSDK((const char *)argv[1], rport, port);

	if(!dt->isLocalDataPortValid()){
		cout << "DTrackSDK init error" << endl;
		return -3;
	}

	cout << "connected to ATC '" << argv[1] <<"' (local data port " << dt->getDataPort() << ")" << endl;

	// start measurement:
	if (!dt->startMeasurement()) {
		cout << "Start measurement failed!" << endl;
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
		}
		messages_to_console();

	}

	// stop measurement & clean up:
	dt->stopMeasurement();
	messages_to_console();
	delete dt;

	return 0;
}
