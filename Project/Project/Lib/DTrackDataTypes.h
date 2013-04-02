/* DTrackTypes: C header file, A.R.T. GmbH 3.5.07-16.1.12
 *
 * Type definitions used in DTrackSDK
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
 * $Id: DTrackDataTypes.h,v 1.15 2012/01/19 10:36:57 christoph Exp $
 */

#ifndef _ART_DTRACK_DATATYPES_H_
#define _ART_DTRACK_DATATYPES_H_

#include <vector>

namespace DTrackSDK_Datatypes {

/**
 * 	\brief	Single marker data (3DOF, float)
 */
typedef struct{
	int id;          //!< id number (starting with 1)
	float quality;   //!< quality (0.0 <= qu <= 1.0; -1 not tracked)
	float loc[3];    //!< location (in mm)
} DTrack_Marker_Type_f;

/**
 * 	\brief	Single marker data (3DOF, double)
 */
typedef struct{
	int id;          //!< id number (starting with 1)
	double quality;  //!< quality (0.0 <= qu <= 1.0; -1 not tracked)
	double loc[3];   //!< location (in mm)
} DTrack_Marker_Type_d;

/**
 * 	\brief	DTrack_Marker_Type definition for older SDKs
 */
typedef DTrack_Marker_Type_f DTrack_Marker_Type;

// -----------------------------------------------------------------------------------------------------

/**
 * 	\brief	Standard body data (6DOF, float)
 *
 *	Currently not tracked bodies get a quality of -1.
 */
typedef struct{
	int id;          //!< id number (starting with 0)
	float quality;   //!< quality (0 <= qu <= 1, no tracking if -1)
	float loc[3];    //!< location (in mm)
	float rot[9];    //!< rotation matrix (column-wise)
} DTrack_Body_Type_f;

/**
 * 	\brief	Standard body data (6DOF, double)
 *
 *	Currently not tracked bodies get a quality of -1.
 */
typedef struct{
	int id;          //!< id number (starting with 0)
	double quality;  //!< quality (0 <= qu <= 1, no tracking if -1)
	double loc[3];   //!< location (in mm)
	double rot[9];   //!< rotation matrix (column-wise)
} DTrack_Body_Type_d;

/**
 * 	\brief	DTrack_Body_Type definition for older SDKs
 */
typedef DTrack_Body_Type_f DTrack_Body_Type;

// -----------------------------------------------------------------------------------------------------

#define DTRACK_FLYSTICK_MAX_BUTTON    16	//!< FlyStick data: maximum number of buttons
#define DTRACK_FLYSTICK_MAX_JOYSTICK   8	//!< FlyStick data: maximum number of joystick values

/**
 * 	\brief	A.R.T.Flystick data (6DOF + buttons, float)
 *
 * 	Currently not tracked bodies get a quality of -1.
 *	Note the maximum number of buttons and joystick values.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	float quality;  //!< quality (0 <= qu <= 1, no tracking if -1)
	int num_button; //!< number of buttons
	int button[DTRACK_FLYSTICK_MAX_BUTTON];  //!< button state (1 pressed, 0 not pressed); 0 front, 1..n-1 right to left
	int num_joystick;  //!< number of joystick values
	float joystick[DTRACK_FLYSTICK_MAX_JOYSTICK];  //!< joystick value (-1 <= joystick <= 1); 0 horizontal, 1 vertical
	float loc[3];   //!< location (in mm)
	float rot[9];   //!< rotation matrix (column-wise)
} DTrack_FlyStick_Type_f;

/**
 * 	\brief	A.R.T.Flystick data (6DOF + buttons, double)
 *
 * 	Currently not tracked bodies get a quality of -1.
 *	Note the maximum number of buttons and joystick values.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	double quality; //!< quality (0 <= qu <= 1, no tracking if -1)
	int num_button; //!< number of buttons
	int button[DTRACK_FLYSTICK_MAX_BUTTON];  //!< button state (1 pressed, 0 not pressed): 0 front, 1..n-1 right to left
	int num_joystick;  //!< number of joystick values
	double joystick[DTRACK_FLYSTICK_MAX_JOYSTICK];  //!< joystick value (-1 <= joystick <= 1); 0 horizontal, 1 vertical
	double loc[3];  //!< location (in mm)
	double rot[9];  //!< rotation matrix (column-wise)
} DTrack_FlyStick_Type_d;

/**
 * 	\brief	DTrack_FlyStick_Type definition for older SDKs
 */
typedef DTrack_FlyStick_Type_f DTrack_FlyStick_Type;

// -----------------------------------------------------------------------------------------------------

#define DTRACK_MEATOOL_MAX_BUTTON    1  //!< Measurement tool data: maximum number of buttons

/**
 * 	\brief	Measurement tool data (6DOF + buttons, float)
 *
 * 	Currently not tracked bodies get a quality of -1.
 * 	Note the maximum number of buttons.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	float quality;  //!< quality (0 <= qu <= 1, no tracking if -1)
	int num_button; //!< number of buttons
	int button[DTRACK_MEATOOL_MAX_BUTTON];  //!< button state (1 pressed, 0 not pressed)
	float loc[3];   //!< location (in mm)
	float rot[9];   //!< rotation matrix (column-wise)
} DTrack_MeaTool_Type_f;

/**
 * 	\brief	Measurement tool data (6DOF + buttons, double)
 *
 * 	Currently not tracked bodies get a quality of -1.
 * 	Note the maximum number of buttons.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	double quality; //!< quality (0 <= qu <= 1, no tracking if -1)
	int num_button; //!< number of buttons
	int button[DTRACK_MEATOOL_MAX_BUTTON];  //!< button state (1 pressed, 0 not pressed)
	double loc[3];  //!< location (in mm)
	double rot[9];  //!< rotation matrix (column-wise)
} DTrack_MeaTool_Type_d;

/**
 * 	\brief	DTrack_MeaTool_Type definition for older SDKs
 */
typedef DTrack_MeaTool_Type_f DTrack_MeaTool_Type;

// -----------------------------------------------------------------------------------------------------

/**
 * 	\brief	Measurement reference data (6DOF, float)
 *
 * 	Currently not tracked bodies get a quality of -1.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	float quality;  //!< quality (0 <= qu <= 1, no tracking if -1)
	float loc[3];   //!< location (in mm)
	float rot[9];   //!< rotation matrix (column-wise)
} DTrack_MeaRef_Type_f;

/**
 * 	\brief	Measurement reference data (6DOF, double)
 *
 * 	Currently not tracked bodies get a quality of -1.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	double quality; //!< quality (0 <= qu <= 1, no tracking if -1)
	double loc[3];  //!< location (in mm)
	double rot[9];  //!< rotation matrix (column-wise)
} DTrack_MeaRef_Type_d;

/**
 * 	\brief	DTrack_MeaRef_Type definition for older SDKs
 */
typedef DTrack_MeaRef_Type_f DTrack_MeaRef_Type;

// -----------------------------------------------------------------------------------------------------

#define DTRACK_HAND_MAX_FINGER    5  //!< Fingertracking hand data: maximum number of fingers

/**
 *	\brief	A.R.T.Fingertracking hand data (6DOF + fingers, float)
 *
 *	Currently not tracked bodies get a quality of -1.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	float quality;  //!< quality (0 <= qu <= 1, no tracking if -1)
	int lr;         //!< left (0) or right (1) hand
	int nfinger;    //!< number of fingers (maximum 5)
	float loc[3];   //!< back of the hand: location (in mm)
	float rot[9];   //!< back of the hand: rotation matrix (column-wise)
	struct{
		float loc[3];            //!< location (in mm)
		float rot[9];            //!< rotation matrix (column-wise)
		float radiustip;         //!< radius of tip
		float lengthphalanx[3];  //!< length of phalanxes; order: outermost, middle, innermost
		float anglephalanx[2];   //!< angle between phalanxes
	} finger[DTRACK_HAND_MAX_FINGER];	//!< order: thumb, index finger, middle finger, ...
} DTrack_Hand_Type_f;

/**
 *	\brief	A.R.T.Fingertracking hand data (6DOF + fingers, double)
 *
 *	Currently not tracked bodies get a quality of -1.
 */
typedef struct{
	int id;         //!< id number (starting with 0)
	double quality; //!< quality (0 <= qu <= 1, no tracking if -1)
	int lr;         //!< left (0) or right (1) hand
	int nfinger;    //!< number of fingers (maximum 5)
	double loc[3];  //!< back of the hand: location (in mm)
	double rot[9];  //!< back of the hand: rotation matrix (column-wise)
	struct{
		double loc[3];           //!< location (in mm)
		double rot[9];           //!< rotation matrix (column-wise)
		double radiustip;        //!< radius of tip
		double lengthphalanx[3]; //!< length of phalanxes; order: outermost, middle, innermost
		double anglephalanx[2];  //!< angle between phalanxes
	} finger[DTRACK_HAND_MAX_FINGER];	//!< order: thumb, index finger, middle finger, ...
} DTrack_Hand_Type_d;

/**
 * 	\brief	DTrack_Hand_Type definition for older SDKs
 */
typedef DTrack_Hand_Type_f DTrack_Hand_Type;


// A.R.T human model type
#define DTRACK_HUMAN_MAX_JOINTS 20

/**
 * 	\brief	A.R.T human model (max 30 joints (6DOF + angles + rotation, float))
 *
 * 	Currently not tracked bodies get a quality of -1.
 * 	Note the maximum number of buttons.
 */
typedef struct{
	int id;         //!< id of the human model (starting with 0)
	int num_joints; //!< number of joints of the human model
	struct {
		int id;          //!< id of the joint (starting with 0)
		float quality;   //!< quality of the joint (0 <= qu <= 1, no tracking if -1)
		float loc[3];    //!< location of the joint (in mm)
		float ang[3];    //!< angles in relation to the joint coordinate system
		float rot[9];    //!< rotation matrix  of the joint (column-wise) in relation to room coordinaten system
	} joint[DTRACK_HUMAN_MAX_JOINTS]; //!< location and orientation of the joint
} DTrack_Human_Type_f;

/**
 * 	\brief	A.R.T human model (max 30 joints (6DOF + angles + rotation, double))
 *
 * 	Currently not tracked bodies get a quality of -1.
 * 	Note the maximum number of buttons.
 */
typedef struct{
	int id;         //!< id of the human model (starting with 0)
	int num_joints; //!< number of joints of the human model
	struct {
		int id;           //!< id of the joint (starting with 0)
		double quality;   //!< quality of the joint (0 <= qu <= 1, no tracking if -1)
		double loc[3];    //!< location of the joint (in mm)
		double ang[3];    //!< angles in relation to the joint coordinate system
		double rot[9];    //!< rotation matrix of the joint (column-wise) in relation to room coordinaten system
	} joint[DTRACK_HUMAN_MAX_JOINTS]; //!< location and orientation of the joint
} DTrack_Human_Type_d;

typedef DTrack_Human_Type_d DTrack_Human_Type;

}

#endif /* ART_DTRACK_DATATYPES_H_ */
