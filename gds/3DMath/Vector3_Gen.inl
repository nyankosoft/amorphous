//=====================================================================
//  Vector3_Gen.inl
//     - inline implementations of generic 3-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#include <math.h>
#include "misc.hpp"


// global operators

inline Vector3 operator*( const Scalar f, const Vector3& v )
{
	Vector3 vOut;
	vOut.x = f * v.x;
	vOut.y = f * v.y;
	vOut.z = f * v.z;
	return vOut;
}


// global functions

inline Scalar Vec3Dot( const Vector3& v0, const Vector3& v1 )
{
	return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
}


inline void Vec3Cross( Vector3& vOut, const Vector3& v0, const Vector3& v1 )
{
	vOut.x = v0.y * v1.z - v0.z * v1.y;
	vOut.y = v0.z * v1.x - v0.x * v1.z;
	vOut.z = v0.x * v1.y - v0.y * v1.x;
}


inline Vector3 Vec3Cross( const Vector3& v0, const Vector3& v1 )
{
	Vector3 vOut;
	Vec3Cross( vOut, v0, v1 );
	return vOut;
}


inline Scalar Vec3LengthSq( const Vector3& v )
{
	return ( v.x * v.x + v.y * v.y + v.z * v.z );
}

inline Scalar Vec3Length( const Vector3& v )
{
	return (Scalar)sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
}

inline void Vec3Normalize( Vector3& v )
{
	v /= Vec3Length(v);
}


inline void Vec3Normalize( Vector3& dest, const Vector3& src )
{
	dest = src / Vec3Length(src);
}


inline Vector3 Vec3GetNormalized( const Vector3& v )
{
	return v / Vec3Length(v);
}


/// Projects v2 onto v1 and reurns the projected vector
/// \param v2 vector which is projected to v1
static inline Vector3 Vec3Project(const Vector3 & v1, const Vector3 & v2)
{
  return Vec3Dot(v1, v2) * v1 / Vec3LengthSq( v1 );
}


bool inline IsSensible( const Vector3& v )
{
	if( !is_finite(v.x) || !is_finite(v.y) || !is_finite(v.z) )
		return false;
	else
		return true;
}



// member functions & operators

inline Vector3::Vector3( Scalar _x, Scalar _y, Scalar _z )
: x(_x), y(_y), z(_z)
{}

inline Scalar& Vector3::operator[]( const int i )
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


inline const Scalar& Vector3::operator[]( const int i ) const
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

inline Vector3 Vector3::operator=( const Vector3& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}


inline bool Vector3::operator==( const Vector3& v ) const
{
	if( x == v.x && y == v.y && z == v.z )
		return true;
	else
		return false;
}


inline Vector3 Vector3::operator+( const Vector3& v ) const
{
	Vector3 vOut;
	vOut.x = x + v.x;
	vOut.y = y + v.y;
	vOut.z = z + v.z;
	return vOut;
}


inline Vector3 Vector3::operator-( const Vector3& v ) const
{
	Vector3 vOut;
	vOut.x = x - v.x;
	vOut.y = y - v.y;
	vOut.z = z - v.z;
	return vOut;
}


inline void Vector3::operator+=( const Vector3& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
}


inline void Vector3::operator-=( const Vector3& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}


inline Vector3 Vector3::operator*( const Scalar f ) const
{
	Vector3 vOut;
	vOut.x = x * f;
	vOut.y = y * f;
	vOut.z = z * f;
	return vOut;
}


inline Vector3 Vector3::operator/( const Scalar f ) const
{
	Vector3 vOut;
	vOut.x = x / f;
	vOut.y = y / f;
	vOut.z = z / f;
	return vOut;
}


inline void Vector3::operator*=( const Scalar f )
{
	x *= f;
	y *= f;
	z *= f;
}


inline void Vector3::operator/=( const Scalar f )
{
	x /= f;
	y /= f;
	z /= f;
}


inline void Vector3::Normalize()
{
	Vec3Normalize(*this);
}


inline Scalar Vector3::GetLength() const
{
	return Vec3Length(*this);
}


inline Scalar Vector3::GetLengthSq() const
{
	return Vec3LengthSq(*this);
}