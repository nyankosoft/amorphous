#ifndef  __D3DX_ADAPTER_H__
#define  __D3DX_ADAPTER_H__

// define general Vec3... functions for D3DXVec3... functions
// users do not have to include this file by themselves.

#include <d3dx9.h>

#include "3DMath/precision.h"
#include "3DMath/misc.hpp"


typedef D3DXVECTOR2 Vector2;
typedef D3DXVECTOR3 Vector3;


#define Vec3TransformCoord	D3DXVec3TransformCoord

/*

global Normalize() function with one argument is not used
since it is ambiguous whether the handed vector is normalized
or the normalized vector is given as the return value

inline void Vec3Normalize( D3DXVECTOR3* pv ) { Vec3Normalize(pv,pv); }


inline Vector3 Vec3Normalize( Vector3& v )
{
	Vector3 v_out;
	Vec3Normalize(&v_out,&v);
	return v_out;
}*/


inline void Vec3Normalize( Vector3& vDest, const Vector3& vSrc )
{
	D3DXVec3Normalize( &vDest, &vSrc );
}


inline Vector3 Vec3Zero() { return Vector3(0,0,0); }
inline Vector3 Vec3Scalar3( float f ) { return Vector3(f,f,f); }


inline Vector3 Vec3Cross(const Vector3& v0, const Vector3& v1)
{
	Vector3 v;
	D3DXVec3Cross( &v, &v0, &v1 );
	return v;
}

inline void Vec3Cross( Vector3& vOut, const Vector3& v0, const Vector3& v1)
{
	D3DXVec3Cross( &vOut, &v0, &v1 );
}

inline float Vec3Dot(const Vector3& v0, const Vector3& v1)
{
	return D3DXVec3Dot( &v0, &v1 );
}

inline float Vec3Length( const Vector3& v )
{
	return D3DXVec3Length( &v );
}

inline float Vec3LengthSq( const Vector3& v )
{
	return D3DXVec3LengthSq( &v );
}

inline Vector3 Vec3GetNormalized( const Vector3& v )
{
	return v / Vec3Length(v);
}


// Helper for orthonormalise - projection of v2 onto v1
static inline Vector3 Vec3Project(const Vector3 & v1, const Vector3 & v2)
{
  return Vec3Dot(v1, v2) * v1 / Vec3LengthSq( v1 );
}


inline bool IsSensible( const Vector3& v )
{
	if( !is_finite(v.x) || !is_finite(v.y) || !is_finite(v.z) )
		return false;
	else
		return true;
}


inline Vector3 Vec3NormalizeSafe( const Vector3& vSrc, const Vector3& safe = Vector3(0,1,0) )
{
	const float lSq = Vec3LengthSq( vSrc );
	if (lSq > SCALAR_TINY)
	{
//		return vSrc * (1.0f / Sqrt(lSq));
		return vSrc * (1.0f / sqrtf(lSq));
	}
	else
	{
		return safe;
	}
}




// Vector2 -> D3DXVECTOR2


inline float Vec2Dot(const Vector2& v0, const Vector2& v1)
{
	return D3DXVec2Dot( &v0, &v1 );
}

inline float Vec2Length( const Vector2& v )
{
	return D3DXVec2Length( &v );
}

inline float Vec2LengthSq( const Vector2& v )
{
	return D3DXVec2LengthSq( &v );
}

inline Vector2 Vec2GetNormalized( const Vector2& v )
{
	return v / Vec2Length(v);
}


#endif  /*  __D3DX_ADAPTER_H__  */