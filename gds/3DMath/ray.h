#ifndef __3DMATH_RAY_H__
#define __3DMATH_RAY_H__

#include "3DMath/Vector3.h"

struct CRayHit
{
public:


};


class CRay
{
	/// in
	Vector3 vDirection;
	Vector3 vOrigin;

	/// contact position
	Vector3 vEnd;

	float fFraction;

	int iSurfaceIndex;

	/// polygon hit by the ray
	int iPolygonIndex;

public:

	CRay() { vDirection = vOrigin = vEnd = Vector3(0,0,0); fFraction = 1.0f; iSurfaceIndex = -1; }

};


class CLineSegmentHit
{
public:

	/// contact position
	Vector3 vEnd;

	float fFraction;

	int iSurfaceIndex;

	/// polygon hit by the ray
//	int iPolygonIndex;

public:

	CLineSegmentHit()
		:
	vEnd(Vector3(0,0,0)),
	fFraction(1.0f),
	iSurfaceIndex(-1)
//	iPolygonIndex(0)
	{}
};


class CLineSegment
{
public:
	/// in
	Vector3 vStart;
	Vector3 vGoal;

public:

	CLineSegment()
		:
	vStart(Vector3(0,0,0)),
	vGoal(Vector3(0,0,0))
	{}
};


#endif  /*  __3DMATH_RAY_H__  */
