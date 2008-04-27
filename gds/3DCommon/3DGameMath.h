#ifndef __3DGAMEMATH_H__
#define __3DGAMEMATH_H__


#include "3DMath/Vector3.h"
#include "3DMath/Matrix33.h"

#include <float.h>


//bounce factor = 0		no bounce
//bounce factor = 1		complete bounce
inline void ClipVelocity(Vector3& rvOut,
						 Vector3& rvIn,
						 Vector3& rvNormal,
						 float fBounceFactor = 0)
{
	float f = Vec3Dot( rvNormal, rvIn );
	f *= ( 1.0f + fBounceFactor );
	rvOut = rvIn + (-f) * rvNormal; 
}


inline void GetSkewSymmetricMatrix( D3DXMATRIX* mat, D3DXVECTOR3& v )
{
	mat->_11 = 0.0f; mat->_12 = -v.z; mat->_13 =  v.y;
	mat->_21 =  v.z; mat->_22 = 0.0f; mat->_23 = -v.x;
	mat->_31 = -v.y; mat->_32 =  v.x; mat->_33 = 0.0f;

	mat->_14 = mat->_24 = mat->_34 =
	mat->_41 = mat->_42 = mat->_43 = mat->_44 = 0.0f;
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
