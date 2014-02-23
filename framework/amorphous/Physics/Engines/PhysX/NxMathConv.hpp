#ifndef __NxMathConv_H__
#define __NxMathConv_H__


#include "3DMath/Matrix34.hpp"
#include "3DMath/AABB3.hpp"
#include "NxPhysics.h"


namespace amorphous
{


namespace physics
{

// PhysX to Framework conversions

inline Vector3 ToVector3( const NxVec3& src )
{
	return Vector3( src.x, src.y, src.z );
}


inline Matrix33 ToMatrix33( const NxMat33& src )
{
	return Matrix33(
		ToVector3( src.getColumn(0) ),
		ToVector3( src.getColumn(1) ),
		ToVector3( src.getColumn(2) )
		);
}


inline Matrix34 ToMatrix34( const NxMat34& src )
{
	return Matrix34( ToVector3(src.t), ToMatrix33(src.M) );
}


inline AABB3 ToAABB3( const NxBounds3& src )
{
	return AABB3( ToVector3(src.min), ToVector3(src.max) );
}



// Framework to PhysX conversions

inline NxVec3 ToNxVec3( const Vector3& src )
{
	return NxVec3( src.x, src.y, src.z );
}

inline NxMat33 ToNxMat33( const Matrix33& src )
{
	NxMat33 dest;
	dest.setColumn( 0, ToNxVec3(src.GetColumn(0)) );
	dest.setColumn( 1, ToNxVec3(src.GetColumn(1)) );
	dest.setColumn( 2, ToNxVec3(src.GetColumn(2)) );
	return dest;
}

inline NxMat34 ToNxMat34( const Matrix34& src )
{
	NxMat34 dest;
	dest.t = ToNxVec3( src.vPosition );
	dest.M = ToNxMat33( src.matOrient );
	return dest;
}

inline NxBounds3 ToNxBounds3( const AABB3& src )
{
	NxBounds3 dest;
	dest.set( ToNxVec3(src.vMin), ToNxVec3(src.vMax) );
	return dest;
}


} // namespace physics

} // namespace amorphous



#endif /* __NxMathConv_H__ */
