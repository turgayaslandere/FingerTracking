/* DTrackSDK: C++ example, ART GmbH 16.12.05-21.6.12
 *
 * example_fingertracking:
 *    C++ example using DTrackSDK Fingertracking data
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
 *  - example without DTrack2 remote commands: collects DTrack data
 *  - using DTrackSDK v2.2.0
 *  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
 */
#include "DTrackSDK.hpp"

#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

static void trafo_loc2coo( double locres[3], const double loccoo[3], const double rotcoo[9], const double loc[3] );

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
			<< " nhand " << dt->getNumHand() 
			<< endl;

	// ART Fingertracking hands:
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
				double lochand[3], locroom[3];

				// calculating the position for tip and all joints of the fingers (in hand and room coordinate system):

				for(int k=0; k<3; k++){
					lochand[k] = hand.finger[j].loc[k];  // finger tip (already in hand coordinate system)
				}

				trafo_loc2coo(locroom, hand.loc, hand.rot, lochand);  // finger tip (in room coordinate system)

				cout << "   finger " << j
						<< " tip (hand) " << lochand[0] << " " << lochand[1] << " " << lochand[2]
						<< " tip (room) " << locroom[0] << " " << locroom[1] << " " << locroom[2]
						<< endl;

				lochand[0] = -hand.finger[j].lengthphalanx[0];  // first joint (in finger coordinate system)
				lochand[1] = lochand[2] = 0;

				trafo_loc2coo(lochand, hand.finger[j].loc, hand.finger[j].rot, lochand);  // first joint (in hand coordinate system)
				trafo_loc2coo(locroom, hand.loc, hand.rot, lochand);  // first joint (in room coordinate system)

				cout << "           "
						<< " joint 1 (hand) " << lochand[0] << " " << lochand[1] << " " << lochand[2]
						<< " joint 1 (room) " << locroom[0] << " " << locroom[1] << " " << locroom[2]
						<< endl;

				lochand[0] = -hand.finger[j].lengthphalanx[0]  // second joint (in finger coordinate system)
					- hand.finger[j].lengthphalanx[1] * cos(hand.finger[j].anglephalanx[0] * M_PI / 180);
				lochand[1] = 0;
				lochand[2] = hand.finger[j].lengthphalanx[1] * sin(hand.finger[j].anglephalanx[0] * M_PI / 180);

				trafo_loc2coo(lochand, hand.finger[j].loc, hand.finger[j].rot, lochand);  // second joint (in hand coordinate system)
				trafo_loc2coo(locroom, hand.loc, hand.rot, lochand);  // second joint (in room coordinate system)

				cout << "           "
						<< " joint 2 (hand) " << lochand[0] << " " << lochand[1] << " " << lochand[2]
						<< " joint 2 (room) " << locroom[0] << " " << locroom[1] << " " << locroom[2]
						<< endl;

				lochand[0] = -hand.finger[j].lengthphalanx[0]  // third joint (in finger coordinate system)
					- hand.finger[j].lengthphalanx[1] * cos(hand.finger[j].anglephalanx[0] * M_PI / 180)
					- hand.finger[j].lengthphalanx[2] * cos( (hand.finger[j].anglephalanx[0] + hand.finger[j].anglephalanx[1]) * M_PI / 180 );
				lochand[1] = 0;
				lochand[2] = hand.finger[j].lengthphalanx[1] * sin(hand.finger[j].anglephalanx[0] * M_PI / 180)
					+ hand.finger[j].lengthphalanx[2] * sin( (hand.finger[j].anglephalanx[0] + hand.finger[j].anglephalanx[1]) * M_PI / 180 );

				trafo_loc2coo(lochand, hand.finger[j].loc, hand.finger[j].rot, lochand);  // third joint (in hand coordinate system)
				trafo_loc2coo(locroom, hand.loc, hand.rot, lochand);  // third joint (in room coordinate system)

				cout << "           "
						<< " joint 3 (hand) " << lochand[0] << " " << lochand[1] << " " << lochand[2]
						<< " joint 3 (room) " << locroom[0] << " " << locroom[1] << " " << locroom[2]
						<< endl;
			}
		}
	}
}


/**
 * 	\brief Transforms position into another coordinate system.
 */
static void trafo_loc2coo( double locres[3], const double loccoo[3], const double rotcoo[9], const double loc[3] )
{
	double tmploc[3];

	for(int i=0; i<3; i++){
		tmploc[i] = rotcoo[i+0*3] * loc[0] + rotcoo[i+1*3] * loc[1] + rotcoo[i+2*3] * loc[2];
	}

	for(int i=0; i<3; i++){
		locres[i] = tmploc[i] + loccoo[i];
	}
}


/**
 * 	\brief Prints error messages to console
 *
 * 	@return No error occured?
 */
static bool error_to_console()
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
	dt = new DTrackSDK(port);

	if(!dt->isLocalDataPortValid()){
		cout << "DTrack init error" << endl;
		return -3;
	}

	// receiving:
	while( 1 ){

		if(dt->receive()){
			output_to_console();
		} else {
			error_to_console();
		}

	}

	// clean up:
	delete dt;

	return 0;
}
