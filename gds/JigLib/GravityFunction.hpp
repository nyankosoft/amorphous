#ifndef __GRAVITYFUNCTION_H__
#define __GRAVITYFUNCTION_H__


#include "3DMath/Vector3.hpp"


//extern float g_fGFreeMaxRadius;
//extern float g_fOmegaSq;


class CGravityFunction
{
public:
	static float ms_fGFreeMaxRadius;
	static float ms_fOmegaSq;
};


inline Vector3 GravityAccel_CylindricalColony( Vector3 vPosition )
{
	return GravityAccel_CylindricalColony( vPosition );
}

extern Vector3 GravityAccel_CylindricalColony( Vector3& rvPosition );





#endif  /*  __GRAVITYFUNCTION_H__  */
