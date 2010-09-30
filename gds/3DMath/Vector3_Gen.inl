//=====================================================================
//  tVector3_Gen.inl
//     - inline implementations of generic 3-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#include <math.h>
#include "misc.hpp"


// global operators

template<typename T>
inline tVector3<T> operator*( const T f, const tVector3<T>& v )
{
	tVector3<T> vOut;
	vOut.x = f * v.x;
	vOut.y = f * v.y;
	vOut.z = f * v.z;
	return vOut;
}


// global functions

template<typename T>
inline T Vec3Dot( const tVector3<T>& v0, const tVector3<T>& v1 )
{
	return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
}


template<typename T>
inline void Vec3Cross( tVector3<T>& vOut, const tVector3<T>& v0, const tVector3<T>& v1 )
{
	vOut.x = v0.y * v1.z - v0.z * v1.y;
	vOut.y = v0.z * v1.x - v0.x * v1.z;
	vOut.z = v0.x * v1.y - v0.y * v1.x;
}


template<typename T>
inline tVector3<T> Vec3Cross( const tVector3<T>& v0, const tVector3<T>& v1 )
{
	tVector3<T> vOut;
	Vec3Cross( vOut, v0, v1 );
	return vOut;
}


template<typename T>
inline T Vec3LengthSq( const tVector3<T>& v )
{
	return ( v.x * v.x + v.y * v.y + v.z * v.z );
}

template<typename T>
inline T Vec3Length( const tVector3<T>& v )
{
	return (T)sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
}

template<typename T>
inline void Vec3Normalize( tVector3<T>& v )
{
	v /= Vec3Length(v);
}


template<typename T>
inline void Vec3Normalize( tVector3<T>& dest, const tVector3<T>& src )
{
	dest = src / Vec3Length(src);
}


template<typename T>
inline tVector3<T> Vec3GetNormalized( const tVector3<T>& v )
{
	return v / Vec3Length(v);
}


/// Projects v2 onto v1 and reurns the projected vector
/// \param v2 vector which is projected to v1
template<typename T>
static inline tVector3<T> Vec3Project(const tVector3<T> & v1, const tVector3<T> & v2)
{
  return Vec3Dot(v1, v2) * v1 / Vec3LengthSq( v1 );
}


template<typename T>
bool inline IsSensible( const tVector3<T>& v )
{
	if( !is_finite(v.x) || !is_finite(v.y) || !is_finite(v.z) )
		return false;
	else
		return true;
}



// member functions & operators

template<typename T>
inline tVector3<T>::tVector3( T _x, T _y, T _z )
: x(_x), y(_y), z(_z)
{}

template<typename T>
inline T& tVector3<T>::operator[]( const int i )
{
	switch( i )
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		return z;
	}
}


template<typename T>
inline const T& tVector3<T>::operator[]( const int i ) const
{
	switch( i )
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		return z;
	}
}

template<typename T>
inline tVector3<T> tVector3<T>::operator=( const tVector3<T>& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}


template<typename T>
inline bool tVector3<T>::operator==( const tVector3<T>& v ) const
{
	if( x == v.x && y == v.y && z == v.z )
		return true;
	else
		return false;
}


template<typename T>
inline tVector3<T> tVector3<T>::operator+( const tVector3<T>& v ) const
{
	tVector3<T> vOut;
	vOut.x = x + v.x;
	vOut.y = y + v.y;
	vOut.z = z + v.z;
	return vOut;
}


template<typename T>
inline tVector3<T> tVector3<T>::operator-( const tVector3<T>& v ) const
{
	tVector3<T> vOut;
	vOut.x = x - v.x;
	vOut.y = y - v.y;
	vOut.z = z - v.z;
	return vOut;
}


template<typename T>
inline void tVector3<T>::operator+=( const tVector3<T>& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
}


template<typename T>
inline void tVector3<T>::operator-=( const tVector3& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}


template<typename T>
inline tVector3<T> tVector3<T>::operator*( const T f ) const
{
	tVector3<T> vOut;
	vOut.x = x * f;
	vOut.y = y * f;
	vOut.z = z * f;
	return vOut;
}


template<typename T>
inline tVector3<T> tVector3<T>::operator/( const T f ) const
{
	tVector3<T> vOut;
	vOut.x = x / f;
	vOut.y = y / f;
	vOut.z = z / f;
	return vOut;
}


template<typename T>
inline void tVector3<T>::operator*=( const T f )
{
	x *= f;
	y *= f;
	z *= f;
}


template<typename T>
inline void tVector3<T>::operator/=( const T f )
{
	x /= f;
	y /= f;
	z /= f;
}


template<typename T>
inline void tVector3<T>::Normalize()
{
	Vec3Normalize(*this);
}


template<typename T>
inline T tVector3<T>::GetLength() const
{
	return Vec3Length(*this);
}


template<typename T>
inline T tVector3<T>::GetLengthSq() const
{
	return Vec3LengthSq(*this);
}
