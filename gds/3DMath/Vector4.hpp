//=====================================================================
//  Vector4.hpp
//     - generic 4-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __Vector4_HPP__
#define  __Vector4_HPP__

#include "precision.h"


class Vector4
{
public:

	Scalar x, y, z, w;

	inline Vector4() {}
	inline Vector4( Scalar _x, Scalar _y, Scalar _z, Scalar _w );

	//	~Vector4();

	/// vector - vector operators

	inline bool operator==( const Vector4& v ) const;

	inline Vector4 operator=( const Vector4& v );

	inline Vector4 operator+( const Vector4& v ) const;
	inline Vector4 operator-( const Vector4& v ) const;

	inline void operator+=( const Vector4& v );
	inline void operator-=( const Vector4& v );

	inline Vector4 operator-() { return Vector4(-x,-y,-z,-w); }

	/// vector - scalar operators

	inline Vector4 operator*( const Scalar f ) const;
	inline Vector4 operator/( const Scalar f ) const;

	inline void operator*=( const Scalar f );
	inline void operator/=( const Scalar f );

	inline Scalar& operator[]( const int i );

	inline const Scalar& operator[]( const int i ) const;

//	inline void Normalize();

//	inline Scalar GetLength() const;

//	inline Scalar GetLengthSq() const;
};


#include "Vector4.inl"


#endif		/*  __Vector4_HPP__  */
