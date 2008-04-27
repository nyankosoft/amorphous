
#ifndef __GAMELIB1_3DMATH_SERIALIZATION_H__
#define __GAMELIB1_3DMATH_SERIALIZATION_H__

#include "3DMath/Vector3.h"
#include "3DMath/Matrix33.h"
#include "3DMath/Matrix34.h"
#include "3DMath/Quaternion.h"
#include "3DMath/aabb3.h"

#include "3DMath/Vector2.h"
#include "3DMath/aabb2.h"

#include "Archive.h"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, Vector3& v )
{
	ar & v.x & v.y & v.z;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, Matrix33& mat )
{
	ar & mat(0,0) & mat(0,1) & mat(0,2);
	ar & mat(1,0) & mat(1,1) & mat(1,2);
	ar & mat(2,0) & mat(2,1) & mat(2,2);

	return ar;
}


inline IArchive& operator & ( IArchive& ar, Matrix34& mat )
{
	ar & mat.vPosition;
	ar & mat.matOrient;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, Quaternion& q )
{
	ar & q.x & q.y & q.z & q.w;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, AABB3& aabb )
{
	ar & aabb.vMin;
	ar & aabb.vMax;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, SPlane& plane )
{
	ar & plane.normal;
	ar & plane.dist;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, Sphere& sphere )
{
	ar & sphere.vCenter;
	ar & sphere.radius;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, Vector2& v )
{
	ar & v.x & v.y;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, AABB2& aabb )
{
	ar & aabb.vMin;
	ar & aabb.vMax;

	return ar;
}


}	  /*  Serialization  */


}	  /*  GameLib1  */


#endif  /*  __GAMELIB1_3DMATH_SERIALIZATION_H__  */
