#ifndef  __3DMATH_VECTOR2_H__
#define  __3DMATH_VECTOR2_H__


// use the original Vector2 class
#include "Vector2_Gen.hpp"


namespace amorphous
{


typedef TVector2<float> Vector2;
typedef TVector2<double> dVector2;


template<class TScalar>
inline TScalar Vec2GetAngleBetween( const TVector2<TScalar>& a, const TVector2<TScalar>& b )
{
	const TScalar margin = 0.000001f;

	const TScalar length_a = Vec2Length(a);
	const TScalar length_b = Vec2Length(b);

	if( abs(length_a) < margin || abs(length_b) < margin )
		return 0;

	TScalar dot = Vec2Dot( a, b );

	TScalar acos_arg = dot / ( length_a * length_b );

//	if( acos_arg < -1.0f + margin )     acos_arg = -1.0f + margin;
//	else if( 1.0f - margin < acos_arg ) acos_arg =  1.0f - margin;
	if( acos_arg < -1.0f )     acos_arg = -1.0f;
	else if( 1.0f < acos_arg ) acos_arg =  1.0f;

	return acos( acos_arg );
}


} // namespace amorphous



#endif  /*  __3DMATH_VECTOR2_H__  */
