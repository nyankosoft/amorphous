//=====================================================================
//  Vector2_Gen.h
//     - generic 2-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __VECTOR2_GEN_H__
#define  __VECTOR2_GEN_H__

#include "precision.h"


class Vector2
{
public:

	Scalar x, y;

	inline Vector2() {}
	inline Vector2( Scalar _x, Scalar _y );

	//	~Vector2();

	/// vector - vector operators

	inline Vector2 operator=( const Vector2& v );

	inline Vector2 operator+( const Vector2& v ) const;
	inline Vector2 operator-( const Vector2& v ) const;

	inline void operator+=( const Vector2& v );
	inline void operator-=( const Vector2& v );


	/// vector - scalar operators

	inline Vector2 operator*( const Scalar f ) const;
	inline Vector2 operator/( const Scalar f ) const;

	inline void operator*=( const Scalar f );
	inline void operator/=( const Scalar f );

	inline Scalar& operator[]( int i );


	inline void Normalize();

	inline Scalar GetLength() const;

	inline Scalar GetLengthSq() const;

};


#include "Vector2_Gen.inl"


#endif		/*  __VECTOR2_GEN_H__  */