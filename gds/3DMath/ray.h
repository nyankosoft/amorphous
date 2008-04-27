#ifndef __3DMATH_RAY_H__
#define __3DMATH_RAY_H__

#include "3DMath/Vector3.h"

struct SRay
{
	/// in
	Vector3 vStart;
	Vector3 vGoal;

	/// out ///

	/// contact position
	Vector3 vEnd;

	float fFraction;

	int iSurfaceIndex;

	/// polygon hit by the ray
	int iPolygonIndex;

	SRay() { vStart = vGoal = vEnd = Vector3(0,0,0); fFraction = 1.0f; iSurfaceIndex = -1; }

};


#endif  /*  __3DMATH_RAY_H__  */
