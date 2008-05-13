//=====================================================================
//  Vector3_Gen.h
//     - generic 3-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __VECTOR3_GEN_H__
#define  __VECTOR3_GEN_H__

#include "precision.h"


class Vector3
{
public:

	Scalar x, y, z;

	inline Vector3() {}
	inline Vector3( Scalar _x, Scalar _y, Scalar _z );

	//	~Vector3();

	/// vector - vector operators

	inline bool operator==( const Vector3& v ) const;

	inline Vector3 operator=( const Vector3& v );

	inline Vector3 operator+( const Vector3& v ) const;
	inline Vector3 operator-( const Vector3& v ) const;

	inline void operator+=( const Vector3& v );
	inline void operator-=( const Vector3& v );

	inline Vector3 operator-() { return Vector3(-x,-y,-z); }

	/// vector - scalar operators

	inline Vector3 operator*( const Scalar f ) const;
	inline Vector3 operator/( const Scalar f ) const;

	inline void operator*=( const Scalar f );
	inline void operator/=( const Scalar f );

	inline Scalar& operator[]( const int i );

	inline const Scalar& operator[]( const int i ) const;

	inline void Normalize();

	inline Scalar GetLength() const;

	inline Scalar GetLengthSq() const;

};


#include "Vector3_Gen.inl"


#endif		/*  __VECTOR3_GEN_H__  */
