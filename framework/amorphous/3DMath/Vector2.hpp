#ifndef  __3DMATH_VECTOR2_H__
#define  __3DMATH_VECTOR2_H__


// use the original Vector2 class
#include "Vector2_Gen.hpp"


namespace amorphous
{


typedef tVector2<float> Vector2;
typedef tVector2<double> dVector2;


template<class Scalar>
inline Scalar Vec2GetAngleBetween( const tVector2<Scalar>& a, const tVector2<Scalar>& b )
{
	const Scalar margin = 0.000001f;

	const Scalar length_a = Vec2Length(a);
	const Scalar length_b = Vec2Length(b);

	if( abs(length_a) < margin || abs(length_b) < margin )
		return 0;

	Scalar dot = Vec2Dot( a, b );

	Scalar acos_arg = dot / ( length_a * length_b );

//	if( acos_arg < -1.0f + margin )     acos_arg = -1.0f + margin;
//	else if( 1.0f - margin < acos_arg ) acos_arg =  1.0f - margin;
	if( acos_arg < -1.0f )     acos_arg = -1.0f;
	else if( 1.0f < acos_arg ) acos_arg =  1.0f;

	return acos( acos_arg );
}


} // namespace amorphous



#endif  /*  __3DMATH_VECTOR2_H__  */
