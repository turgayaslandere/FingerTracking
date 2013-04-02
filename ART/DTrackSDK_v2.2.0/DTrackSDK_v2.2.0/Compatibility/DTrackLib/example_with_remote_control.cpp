/* DTrackSDK: C++ example, A.R.T. GmbH 16.12.05-13.5.11
 *
 * example_with_remote_control:
 *    C++ example using dtracklib with DTrack remote control
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
 *  - example with DTrack remote commands:
 *    starts DTrack, collects frames and stops DTrack again
 *  - using DTrackSDK v2.0.2
 *  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
 *
 * $Id: example_with_remote_control.cpp,v 1.2 2011/07/06 08:52:15 sebastianz Exp $
 */
#include "DTracklib.hpp"

#include <iostream>
#include <sstream>

using namespace std;

// global DTrack
static DTracklib* dt = NULL;


/**
 * 	\brief Prints current tracking data to console.
 */
static void output_to_console()
{
	printf("\nframe %lu ts %.3f nbodcal %d nbod %d nfly %d nmea %d nmar %d ngl %d\n",
		dt->get_framenr(), dt->get_timestamp(), dt->get_nbodycal(), dt->get_nbody(),
		dt->get_nflystick(), dt->get_nmeatool(), dt->get_nmarker(), dt->get_nglove());

	// bodies:
	dtracklib_body_type body;
	for(int i=0; i<dt->get_nbody(); i++){
		body = dt->get_body(i);

		printf("bod %lu qu %.3f loc %.2f %.2f %.2f ang %.2f %.2f %.2f rot %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
			body.id, body.quality,
			body.loc[0], body.loc[1], body.loc[2],
			body.ang[0], body.ang[1], body.ang[2],
			body.rot[0], body.rot[1], body.rot[2], body.rot[3], body.rot[4], body.rot[5],
			body.rot[6], body.rot[7], body.rot[8]);
	}

	// A.R.T. FlySticks:
	dtracklib_flystick_type flystick;
	for(int i=0; i<dt->get_nflystick(); i++){
		flystick = dt->get_flystick(i);

		printf("fly %lu qu %.3f bt %lx loc %.2f %.2f %.2f ang %.2f %.2f %.2f rot %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
			flystick.id, flystick.quality, flystick.bt,
			flystick.loc[0], flystick.loc[1], flystick.loc[2],
			flystick.ang[0], flystick.ang[1], flystick.ang[2],
			flystick.rot[0], flystick.rot[1], flystick.rot[2], flystick.rot[3], flystick.rot[4], flystick.rot[5],
			flystick.rot[6], flystick.rot[7], flystick.rot[8]);
	}

	// measurement tools:
	dtracklib_meatool_type meatool;
	for(int i=0; i<dt->get_nmeatool(); i++){
		meatool = dt->get_meatool(i);

		printf("mea %lu qu %.3f bt %lx loc %.2f %.2f %.2f rot %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
			meatool.id, meatool.quality, meatool.bt,
			meatool.loc[0], meatool.loc[1], meatool.loc[2],
			meatool.rot[0], meatool.rot[1], meatool.rot[2], meatool.rot[3], meatool.rot[4], meatool.rot[5],
			meatool.rot[6], meatool.rot[7], meatool.rot[8]);
	}

	// markers:
	dtracklib_marker_type marker;
	for(int i=0; i<dt->get_nmarker(); i++){
		marker = dt->get_marker(i);

		printf("mar %d qu %.3f loc %.2f %.2f %.2f\n",
			marker.id, marker.quality,
			marker.loc[0], marker.loc[1], marker.loc[2]);
	}

	// A.R.T. Fingertracking hands:
	dtracklib_glove_type glove;
	for(int i=0; i<dt->get_nglove(); i++){
		glove = dt->get_glove(i);

		printf("gl %d qu %.3f lr %s nf %d loc %.2f %.2f %.2f rot %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
			glove.id, glove.quality, (glove.lr == 0) ? "left" : "right", glove.nfinger,
			glove.loc[0], glove.loc[1], glove.loc[2],
			glove.rot[0], glove.rot[1], glove.rot[2], glove.rot[3], glove.rot[4], glove.rot[5],
			glove.rot[6], glove.rot[7], glove.rot[8]);

		for(int j=0; j<glove.nfinger; j++){
			printf("  fi %d loc %.1f %.1f %.1f rot %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
				j, glove.finger[j].loc[0], glove.finger[j].loc[1], glove.finger[j].loc[2],
				glove.finger[j].rot[0], glove.finger[j].rot[1], glove.finger[j].rot[2],
				glove.finger[j].rot[3], glove.finger[j].rot[4], glove.finger[j].rot[5],
				glove.finger[j].rot[6], glove.finger[j].rot[7], glove.finger[j].rot[8]);
			printf("  fi %d tip %.1f pha %.1f %.1f %.1f ang %.1f %.1f\n",
				j, glove.finger[j].radiustip,
				glove.finger[j].lengthphalanx[0], glove.finger[j].lengthphalanx[1], glove.finger[j].lengthphalanx[2],
				glove.finger[j].anglephalanx[0], glove.finger[j].anglephalanx[1]);
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
	if(argc != 4){
		cout << "Usage: " << argv[0] << " <data port> <remote host> <remote port>" << endl;
		return -1;
	}

	istringstream portstream(argv[1]);
	int port, rport;

	portstream >> port;  // data port
	if(portstream.fail() || port <=0 || port >= 65536){
		cout << "invalid port '" << argv[1] << "'" << endl;
		return -2;
	}
	
	portstream.clear();
	portstream.str(argv[3]);
	portstream >> rport;  // remote port

	if(portstream.fail() || rport <=0 || rport >= 65536){
		cout << "invalid remote port '" << argv[3] << "'" << endl;
		return -2;
	}

	// init library:
	dt = new DTracklib(port, argv[2], rport);

	if(!dt->valid()){
		cout << "DTrack init error" << endl;
		return -3;
	}

	// start measurement and call for data:
	if((!dt->send(DTRACKLIB_CMD_CAMERAS_AND_CALC_ON)) || (!dt->send(DTRACKLIB_CMD_SEND_DATA))){
		cout << "DTrack send command error" << endl;
		return -4;
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

	// stop data transmission / measurement / cameras
	dt->send(DTRACKLIB_CMD_CAMERAS_OFF);
	dt->send(DTRACKLIB_CMD_STOP_DATA);
	delete dt;

	return 0;
}
