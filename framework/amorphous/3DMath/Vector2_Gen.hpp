//=====================================================================
//  Vector2_Gen.hpp
//     - generic 2-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __VECTOR2_GEN_H__
#define  __VECTOR2_GEN_H__

#include "precision.h"


namespace amorphous
{


template<class Scalar>
class tVector2
{
public:

	Scalar x, y;

	// Constructor specializations for float and double follow after the class definition.
	inline tVector2() : x(0), y(0) {}

	inline tVector2( Scalar _x, Scalar _y );

	//	~tVector2();

	/// vector - vector operators

	inline bool operator==( const tVector2& v ) const;
	inline bool operator!=( const tVector2& v ) const { return !((*this) == v); }

	inline tVector2 operator=( const tVector2& v );

	inline tVector2 operator+( const tVector2& v ) const;
	inline tVector2 operator-( const tVector2& v ) const;

	inline void operator+=( const tVector2& v );
	inline void operator-=( const tVector2& v );


	/// vector - scalar operators

	inline tVector2 operator*( const Scalar f ) const;
	inline tVector2 operator/( const Scalar f ) const;

	inline void operator*=( const Scalar f );
	inline void operator/=( const Scalar f );

	inline Scalar& operator[]( int i );

	inline const Scalar& operator[]( int i ) const;


	inline void Normalize();

	inline Scalar GetLength() const;

	inline Scalar GetLengthSq() const;

};

template<> inline tVector2<float>::tVector2() : x(0.0f), y(0.0f) {}
template<> inline tVector2<double>::tVector2() : x(0.0), y(0.0) {}


} // namespace amorphous


#include "Vector2_Gen.inl"

#endif		/*  __VECTOR2_GEN_H__  */
