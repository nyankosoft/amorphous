#ifndef  __3DMATH_VECTOR3_H__
#define  __3DMATH_VECTOR3_H__


#include "3DMathLibBase.h"


#ifdef USE_D3DXVECTOR

// use D3DXVECTOR3 class as Vector3
#include "D3DX_Adapter.hpp"

#else /* USE_D3DXVECTOR */

// use the original Vector3
#include "Vector3_Gen.hpp"

#endif /* USE_D3DXVECTOR */


inline Scalar Vec3GetAngleBetween( const Vector3& a, const Vector3& b )
{
	const Scalar margin = 0.000001f;

	const float length_a = Vec3Length(a);
	const float length_b = Vec3Length(b);

	if( fabs(length_a) < margin || fabs(length_b) < margin )
		return 0;

	Scalar dot = Vec3Dot( a, b );

	Scalar acos_arg = dot / ( length_a * length_b );

//	if( acos_arg < -1.0f + margin )     acos_arg = -1.0f + margin;
//	else if( 1.0f - margin < acos_arg ) acos_arg =  1.0f - margin;
	if( acos_arg < -1.0f )     acos_arg = -1.0f;
	else if( 1.0f < acos_arg ) acos_arg =  1.0f;

	return acos( acos_arg );
}


#endif  /*  __3DMATH_VECTOR3_H__  */
