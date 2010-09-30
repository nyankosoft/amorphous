//=====================================================================
//  tVector3_Gen.hpp
//     - generic 3-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __VECTOR3_GEN_H__
#define  __VECTOR3_GEN_H__

#include "precision.h"


template<typename T>
class tVector3
{
public:

	T x, y, z;

	inline tVector3() {}
	inline tVector3( T _x, T _y, T _z );

	//	~tVector3();

	/// vector - vector operators

	inline bool operator==( const tVector3& v ) const;
	inline bool operator!=( const tVector3& v ) const { return !((*this) == v); }

	inline tVector3 operator=( const tVector3& v );

	inline tVector3 operator+( const tVector3& v ) const;
	inline tVector3 operator-( const tVector3& v ) const;

	inline void operator+=( const tVector3& v );
	inline void operator-=( const tVector3& v );

	inline tVector3 operator-() const { return tVector3(-x,-y,-z); }

	/// vector - scalar operators

	inline tVector3 operator*( const T f ) const;
	inline tVector3 operator/( const T f ) const;

	inline void operator*=( const T f );
	inline void operator/=( const T f );

	inline T& operator[]( const int i );

	inline const T& operator[]( const int i ) const;

	inline void Normalize();

	inline T GetLength() const;

	inline T GetLengthSq() const;

};


#include "Vector3_Gen.inl"


typedef tVector3<float> Vector3;
typedef tVector3<double> dVector3;


#endif		/*  __VECTOR3_GEN_H__  */
