#ifndef  __3DMATH_VECTOR3_H__
#define  __3DMATH_VECTOR3_H__

#include "3DMathLibBase.h"


#ifdef USE_D3DXVECTOR

// use D3DXVECTOR3 class as Vector3
#include "D3DX_Adapter.h"

#else /* USE_D3DXVECTOR */

// use the original Vector3
#include "Vector3_Gen.h"

#endif /* USE_D3DXVECTOR */


inline Scalar Vec3GetAngleBetween( const Vector3& a, const Vector3& b )
{
	const float length_a = Vec3Length(a);
	const float length_b = Vec3Length(b);

	if( length_a == 0 || length_b == 0 )
		return 0;

	const Scalar margin = 0.00001f;
	Scalar dot = Vec3Dot( a, b );

	if( dot < -1.0f + margin )     dot = -1.0f + margin;
	else if( 1.0f - margin < dot ) dot =  1.0f - margin;

	return acos( dot / ( length_a * length_b ) );
}


#endif  /*  __3DMATH_VECTOR3_H__  */
