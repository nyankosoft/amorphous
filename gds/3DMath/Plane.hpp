#ifndef __3DMath_Plane_HPP__
#define __3DMath_Plane_HPP__


#include "Vector3.hpp"
#include <float.h>


template<typename T>
class tPlane
{
public:

	tVector3<T> normal;

	T dist;

	int type;  // for fast side tests - 0:vertical to x-axis, 1:vertical to y-axis, 2:vertical to z-axis, 3:non-vertical to any axis  

public:

	/// The ctor creates an invalid plane by default
	tPlane()
		:
	normal( tVector3<T>(0,0,0) ),
	dist(0),
	type(3)
	{}

	tPlane( const tVector3<T>& _normal, T _dist ) : normal(_normal), dist(_dist) { type = 3; }

	bool operator<(tPlane<T>& plane) const { return (dist < plane.dist); }

	// bool operator==(tPlane<T>& plane);

	inline void Flip() { normal *= (-1); dist *= (-1); }

	/// calculate the signed distance between the plane and a given position
	/// returns negative value when the point is behind the plane (in a negative half-space)
	inline T GetDistanceFromPoint( const tVector3<T>& pos ) const;
};


template<typename T>
inline T tPlane<T>::GetDistanceFromPoint( const tVector3<T>& pos ) const
{
/*
#ifdef _DEBUG
	if( rvPoint.x < -FLT_MAX || FLT_MAX < rvPoint.x ||
		rvPoint.y < -FLT_MAX || FLT_MAX < rvPoint.y ||
		rvPoint.z < -FLT_MAX || FLT_MAX < rvPoint.z )
		return 0.0f;
#endif*/

		return Vec3Dot( normal, pos ) - dist;
}


/*	
inline float SPlane::GetDistanceFromPoint( tVector3<T>& rvPoint )
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


template<typename T>
inline int ClassifyPoint( const tPlane<T>& plane, const tVector3<T>& point )
{
	float pdist = Vec3Dot( point, plane.normal ) - plane.dist;

	if( pdist > DIST_EPSILON )   // point is in front of the plane
		return PNT_FRONT;
	if( pdist < -DIST_EPSILON )  // point is behind the plane
		return PNT_BACK;
	return PNT_ONPLANE;          // point is on the plane
}


template<typename T>
inline bool AlmostSamePlanes( const tPlane<T>& plane0, const tPlane<T>& plane1, const T dist_error = (T)0.001, const T normal_error = (T)0.001 )
{
	if(	fabs( plane0.dist - plane1.dist ) < dist_error
	 && fabs( plane0.normal.x - plane1.normal.x ) < normal_error 
	 && fabs( plane0.normal.y - plane1.normal.y ) < normal_error 
	 && fabs( plane0.normal.z - plane1.normal.z ) < normal_error )
	{
		return true;
	}
	else
		return false;
}


typedef tPlane<float> Plane;
typedef tPlane<double> dPlane;

typedef tPlane<float> SPlane; // deprecated



#endif  /*  __3DMath_Plane_HPP__  */
