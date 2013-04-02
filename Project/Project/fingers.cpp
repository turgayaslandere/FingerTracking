
#include "fingers.hpp"

fingers::fingers(int handid)
{
	
ID=handid;
}


 double fingers::GetFingerTip(DTrack_Hand_Type_d hand)
{


	return 1.0f;


}



//void fingers:: trafo_loc2coo( double locres[3], const double loccoo[3], const double rotcoo[9], const double loc[3] )
//{
//
//
//	for(int i=0; i<3; i++)
//	{
//		tmploc[i] = rotcoo[i+0*3] * loc[0] + rotcoo[i+1*3] * loc[1] + rotcoo[i+2*3] * loc[2];
//	}
//
//	for(int i=0; i<3; i++)
//	{
//		locres[i] = tmploc[i] + loccoo[i];
//	}
//
//}