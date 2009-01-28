#ifndef __STRUCT_PLANE_H__
#define __STRUCT_PLANE_H__


#include "Vector3.h"
#include <float.h>


struct SPlane
{
	Vector3	normal;

	Scalar dist;

	int type;  // for fast side tests - 0:vertical to x-axis, 1:vertical to y-axis, 2:vertical to z-axis, 3:non-vertical to any axis  


	SPlane() { memset(this, 0, sizeof(SPlane)); type = 3;}

	SPlane( const Vector3& _normal, Scalar _dist ) : normal(_normal), dist(_dist) { type = 3; }

	bool operator<(SPlane& plane) { return (dist < plane.dist); }

	// bool operator==(SPlane& plane);

	inline void Flip() { normal *= (-1); dist *= (-1); }

	/// calculate the signed distance between the plane and a given position
	/// returns negative value when the point is behind the plane (in a negative half-space)
	inline float GetDistanceFromPoint( const Vector3& rvPoint ) const;
};


inline float SPlane::GetDistanceFromPoint( const Vector3& rvPoint ) const
{
/*
#ifdef _DEBUG
	if( rvPoint.x < -FLT_MAX || FLT_MAX < rvPoint.x ||
		rvPoint.y < -FLT_MAX || FLT_MAX < rvPoint.y ||
		rvPoint.z < -FLT_MAX || FLT_MAX < rvPoint.z )
		return 0.0f;
#endif*/

		return Vec3Dot( normal, rvPoint ) - dist;
}


/*	
inline float SPlane::GetDistanceFromPoint( Vector3& rvPoint )
{
	switch( type )
	{
	case 0: return rPoint.x - dist;	//plane is perpendicular to x-axis
	case 1: return rPoint.y - dist;	//plane is perpendicular to y-axis
	case 2: return rPoint.z - dist;	//plane is perpendicular to z-axis
	default:
		return Vec3Dot( &normal, &rvPoint ) - dist;
	}
}*/


enum ePointStatus { PNT_ONPLANE, PNT_FRONT, PNT_BACK };

#define DIST_EPSILON	0.001


inline int ClassifyPoint( const SPlane& plane, const Vector3& point )
{
	float pdist = Vec3Dot( point, plane.normal ) - plane.dist;

	if( pdist > DIST_EPSILON )   // point is in front of the plane
		return PNT_FRONT;
	if( pdist < -DIST_EPSILON )  // point is behind the plane
		return PNT_BACK;
	return PNT_ONPLANE;          // point is on the plane
}

#endif  /*  __STRUCT_PLANE_H__  */
