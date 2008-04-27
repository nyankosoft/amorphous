//=====================================================================
//  Vector2_Gen.inl
//     - inline implementations of generic 2-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#include <math.h>


// global operators

inline Vector2 operator*( const Scalar f, const Vector2& v )
{
	Vector2 vOut;
	vOut.x = f * v.x;
	vOut.y = f * v.y;
	return vOut;
}


// global functions

inline Scalar Vec2Dot( Vector2& v0, Vector2& v1 )
{
	return (v0.x * v1.x + v0.y * v1.y);
}


//inline void Vec2Cross()

inline Scalar Vec2LengthSq( const Vector2& v )
{
	return ( v.x * v.x + v.y * v.y );
}

inline Scalar Vec2Length( const Vector2& v )
{
	return (Scalar)sqrt( v.x * v.x + v.y * v.y );
}

inline void Vec2Normalize( Vector2& v )
{
	v /= Vec2Length(v);
}

inline Vector2 Vec2GetNormalized( const Vector2& v )
{
	return v / Vec2Length(v);
}



// member functions & operators

inline Vector2::Vector2( Scalar _x, Scalar _y )
: x(_x), y(_y)
{}

inline Scalar& Vector2::operator[]( int i )
{
	switch( i )
	{
	case 0:
		return x;
	case 1:
		return y;
	default:
		return y;
	}
}


inline Vector2 Vector2::operator=( const Vector2& v )
{
	x = v.x;
	y = v.y;
	return *this;
}


inline Vector2 Vector2::operator+( const Vector2& v ) const
{
	Vector2 vOut;
	vOut.x = x + v.x;
	vOut.y = y + v.y;
	return vOut;
}


inline Vector2 Vector2::operator-( const Vector2& v ) const
{
	Vector2 vOut;
	vOut.x = x - v.x;
	vOut.y = y - v.y;
	return vOut;
}


inline void Vector2::operator+=( const Vector2& v )
{
	x += v.x;
	y += v.y;
}


inline void Vector2::operator-=( const Vector2& v )
{
	x -= v.x;
	y -= v.y;
}


inline Vector2 Vector2::operator*( const Scalar f ) const
{
	Vector2 vOut;
	vOut.x = x * f;
	vOut.y = y * f;
	return vOut;
}


inline Vector2 Vector2::operator/( const Scalar f ) const
{
	Vector2 vOut;
	vOut.x = x / f;
	vOut.y = y / f;
	return vOut;
}


inline void Vector2::operator*=( const Scalar f )
{
	x *= f;
	y *= f;
}


inline void Vector2::operator/=( const Scalar f )
{
	x /= f;
	y /= f;
}


inline void Vector2::Normalize()
{
	Vec2Normalize(*this);
}


inline Scalar Vector2::GetLength() const
{
	return Vec2Length(*this);
}


inline Scalar Vector2::GetLengthSq() const
{
	return Vec2LengthSq(*this);
}