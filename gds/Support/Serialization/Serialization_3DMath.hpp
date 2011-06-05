#ifndef __GAMELIB1_3DMATH_SERIALIZATION_H__
#define __GAMELIB1_3DMATH_SERIALIZATION_H__

#include "../../3DMath/Matrix34.hpp"
#include "../../3DMath/Matrix44.hpp"
#include "../../3DMath/Transform.hpp"
#include "../../3DMath/AABB3.hpp"
#include "../../3DMath/OBB3.hpp"
#include "../../3DMath/Capsule.hpp"
#include "../../3DMath/Matrix22.hpp"
#include "../../3DMath/AABB2.hpp"

#include "Archive.hpp"


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


inline IArchive& operator & ( IArchive& ar, Transform& pose )
{
	ar & pose.qRotation & pose.vTranslation;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, Matrix44& mat )
{
	ar & mat(0,0) & mat(0,1) & mat(0,2) & mat(0,3);
	ar & mat(1,0) & mat(1,1) & mat(1,2) & mat(1,3);
	ar & mat(2,0) & mat(2,1) & mat(2,2) & mat(1,3);
	ar & mat(3,0) & mat(3,1) & mat(3,2) & mat(3,3);

	return ar;
}


inline IArchive& operator & ( IArchive& ar, AABB3& aabb )
{
	ar & aabb.vMin;
	ar & aabb.vMax;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, OBB3& obb )
{
	ar & obb.center;
	ar & obb.radii;

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
	ar & sphere.center;
	ar & sphere.radius;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, Capsule& capsule )
{
	ar & capsule.p0;
	ar & capsule.p1;
	ar & capsule.radius;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, Vector2& v )
{
	ar & v.x & v.y;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, Matrix22& mat )
{
	ar & mat(0,0) & mat(0,1);
	ar & mat(1,0) & mat(1,1);

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
