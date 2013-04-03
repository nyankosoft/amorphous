//=====================================================================
//  Vector4.inl
//     - inline implementations of generic 4-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#include <math.h>
#include "misc.hpp"


namespace amorphous
{

// global operators

inline Vector4 operator*( const Scalar f, const Vector4& v )
{
	Vector4 vOut;
	vOut.x = f * v.x;
	vOut.y = f * v.y;
	vOut.z = f * v.z;
	return vOut;
}


// global functions
/*
inline Scalar Vec3Dot( const Vector4& v0, const Vector4& v1 )
{
	return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
}

inline void Vec3Cross( Vector4& vOut, const Vector4& v0, const Vector4& v1 )
{
	vOut.x = v0.y * v1.z - v0.z * v1.y;
	vOut.y = v0.z * v1.x - v0.x * v1.z;
	vOut.z = v0.x * v1.y - v0.y * v1.x;
}

inline Vector4 Vec3Cross( const Vector4& v0, const Vector4& v1 )
{
	Vector4 vOut;
	Vec3Cross( vOut, v0, v1 );
	return vOut;
}

inline Scalar Vec3LengthSq( const Vector4& v )
{
	return ( v.x * v.x + v.y * v.y + v.z * v.z );
}

inline Scalar Vec3Length( const Vector4& v )
{
	return (Scalar)sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
}

inline void Vec3Normalize( Vector4& v )
{
	v /= Vec3Length(v);
}


inline void Vec3Normalize( Vector4& dest, const Vector4& src )
{
	dest = src / Vec3Length(src);
}


inline Vector4 Vec3GetNormalized( const Vector4& v )
{
	return v / Vec3Length(v);
}
*/


bool inline IsSensible( const Vector4& v )
{
	if( !is_finite(v.x) || !is_finite(v.y) || !is_finite(v.z)|| !is_finite(v.w) )
		return false;
	else
		return true;
}



// member functions & operators

inline Vector4::Vector4( Scalar _x, Scalar _y, Scalar _z, Scalar _w )
: x(_x), y(_y), z(_z), w(_w)
{}

inline Scalar& Vector4::operator[]( const int i )
{
	switch( i )
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		return w;
	}
}


inline const Scalar& Vector4::operator[]( const int i ) const
{
	switch( i )
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		return w;
	}
}

inline Vector4 Vector4::operator=( const Vector4& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}


inline bool Vector4::operator==( const Vector4& v ) const
{
	if( x == v.x && y == v.y && z == v.z && w == v.w )
		return true;
	else
		return false;
}


inline Vector4 Vector4::operator+( const Vector4& v ) const
{
	Vector4 vOut;
	vOut.x = x + v.x;
	vOut.y = y + v.y;
	vOut.z = z + v.z;
	vOut.w = w + v.w;
	return vOut;
}


inline Vector4 Vector4::operator-( const Vector4& v ) const
{
	Vector4 vOut;
	vOut.x = x - v.x;
	vOut.y = y - v.y;
	vOut.z = z - v.z;
	vOut.w = w - v.w;
	return vOut;
}


inline void Vector4::operator+=( const Vector4& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}


inline void Vector4::operator-=( const Vector4& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}


inline Vector4 Vector4::operator*( const Scalar f ) const
{
	Vector4 vOut;
	vOut.x = x * f;
	vOut.y = y * f;
	vOut.z = z * f;
	vOut.w = w * f;
	return vOut;
}


inline Vector4 Vector4::operator/( const Scalar f ) const
{
	Vector4 vOut;
	vOut.x = x / f;
	vOut.y = y / f;
	vOut.z = z / f;
	vOut.w = w / f;
	return vOut;
}


inline void Vector4::operator*=( const Scalar f )
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
}


inline void Vector4::operator/=( const Scalar f )
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;
}

/*
inline void Vector4::Normalize()
{
	Vec3Normalize(*this);
}


inline Scalar Vector4::GetLength() const
{
	return Vec3Length(*this);
}


inline Scalar Vector4::GetLengthSq() const
{
	return Vec3LengthSq(*this);
}
*/

} // namespace amorphous
