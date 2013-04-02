#ifndef _FINGERS_H_
#define _FINGERS_H_
#include <iostream>
#include <sstream>
#include <cmath>
#include "DTrackSDK.hpp"

#define M_PI 3.14
using namespace std;



class fingers{

public:



fingers(int handid);
//double lochand[3], locroom[3];
//double tmploc[3];

int ID;
double GetFingerTip(DTrack_Hand_Type_d hand);
double GetFirstJoint();

//private:

//void trafo_loc2coo( double locres[3], const double loccoo[3], const double rotcoo[9], const double loc[3] );



};


#endif