//=====================================================================
//  tVector3_Gen.hpp
//     - generic 3-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __VECTOR3_GEN_H__
#define  __VECTOR3_GEN_H__

#include "precision.h"


namespace amorphous
{

template<typename T>
class tVector3
{
public:

	T x, y, z;

	// Constructor specializations for float and double follow after the class definition.
	inline tVector3() : x(0), y(0), z(0) {}

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

// Specialize the init-by-zero constructors so that each type get the right notations (0.0f for float, and 0.0 for double)
// Compilers might do all the necessary conversions at compile time, so this might be unnecessary to avoid performance drop.
template<> inline tVector3<float>::tVector3() : x(0.0f), y(0.0f), z(0.0f) {}
template<> inline tVector3<double>::tVector3() : x(0.0), y(0.0), z(0.0) {}


typedef tVector3<float> Vector3;
typedef tVector3<double> dVector3;

} // namespace amorphous


#include "Vector3_Gen.inl"



#endif		/*  __VECTOR3_GEN_H__  */
