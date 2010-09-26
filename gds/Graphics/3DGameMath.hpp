#ifndef __3DGAMEMATH_H__
#define __3DGAMEMATH_H__


#include "3DMath/Vector3.hpp"
#include "3DMath/Matrix33.hpp"

#include <float.h>


/// \param fBounceFactor [0,1] 0: no bounce / 1: complete bounce
inline void ClipVelocity(Vector3& rvOut,
						 const Vector3& rvIn,
						 const Vector3& rvNormal,
						 float fBounceFactor = 0)
{
	float f = Vec3Dot( rvNormal, rvIn );
	f *= ( 1.0f + fBounceFactor );
	rvOut = rvIn + (-f) * rvNormal; 
}


/*
inline bool IsSensible(Vector3& v)
{
	if( _finite(v.x) != 0 && _finite(v.y) != 0 && _finite(v.z) != 0 )
		return true;
	else
		return false;
}


inline bool IsSensible( D3DXMATRIX& mat )
{
	float *paf = (float *)mat;
	int i;
	for( i=0; i<16; i++ )
	{
		if( !_finite(paf[i]) )
			return false;
	}
	return true;
}*/


// calc local coord from the normal vector info
inline void CreateSpaceFromNormal( const Vector3& vNormal, Vector3& vRight, Vector3& vUp )
{
	// calculate right(x) and up(y) directions
	if( vNormal.y == 0 )
	{
		// vertical surface - up vector is (0,1,0)
		vUp = Vector3(0,1,0);
		Vec3Cross( vRight, vNormal, vUp );

	}
	else if( 0.999f < vNormal.y )
	{
		// horizontal floor - up vector is (0,0,1)
		vRight = Vector3(1,0,0);
		vUp = Vector3(0,0,1);
	}
	else if( vNormal.y < -0.999f )
	{
		// horizontal ceiling - up vector is (0,0,-1)
		vRight = Vector3(-1,0,0);
		vUp = Vector3(0,0,1);
	}
	else
	{
		// slope
		vUp = Vector3(0,1,0);
		Vec3Cross( vRight, vNormal, vUp );
		Vec3Normalize( vRight, vRight );
		Vec3Cross( vUp, vRight, vNormal );
		Vec3Normalize( vUp, vUp );
	}
}


inline Matrix33 CreateOrientFromNormal( Vector3 vNormal )
{
	Vector3 vRight = Vector3(1,0,0), vUp = Vector3(0,0,1);
	CreateSpaceFromNormal( vNormal, vRight, vUp );
	Matrix33 matOrient;
	matOrient.SetColumn( 0, vRight );
	matOrient.SetColumn( 1, vNormal );
	matOrient.SetColumn( 2, vUp );
	return matOrient;
}


/**
 * create a 3x3 matrix that represents coordinates space with 3 orthogonal unit vectors
 * NOTE: vFwd must be normalized in advance
 */
inline Matrix33 CreateOrientFromFwdDir( const Vector3& vFwd )
{
	Vector3 vSeedUpDir = Vector3(0,1,0);
	Vector3 vRight, vUp;

	if( 1.0f - fabsf(vFwd.y) < 0.0001f )
	{
		// foward direction is almost vertical
		// - need to change the seed vector for up direction
		vSeedUpDir = Vector3(0,0,-1);
	}

	vRight = Vec3Cross( vSeedUpDir, vFwd );
	Vec3Normalize( vRight, vRight );

	vUp = Vec3Cross( vFwd, vRight );
	Vec3Normalize( vUp, vUp );

	return Matrix33( vRight, vUp, vFwd );
}


#endif	/*  __3DGAMEMATH_H__  */
