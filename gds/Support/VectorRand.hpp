#ifndef  __VectorRand_H__
#define  __VectorRand_H__


#include "3DMath/Vector2.hpp"
#include "3DMath/Vector3.hpp"
#include "Support/MTRand.hpp"


/// used by Vec3GetRandDir() & Vec3GetRandDir()
inline float GetSignedRandNonZeroToOne()
{
	return RangedRand( 0.1f, 1.0f ) * ( 500 < RangedRand(1000) ? 1.0f : -1.0f );
}

/*
/// returns normalized random direction of Vector3
inline Vector2 Vec2RandDir()
{
	Vector2 v(
		GetSignedRandNonZeroToOne(),
		GetSignedRandNonZeroToOne()
		);

	Vec2Normalize( v, v );

	return v;
}
*/

/// returns normalized random direction of Vector3
inline Vector3 Vec3RandDir()
{
	Vector3 v(
		GetSignedRandNonZeroToOne(),
		GetSignedRandNonZeroToOne(),
		GetSignedRandNonZeroToOne()
		);

	Vec3Normalize( v, v );

	return v;
}


#endif		/*  __VectorRand_H__  */
