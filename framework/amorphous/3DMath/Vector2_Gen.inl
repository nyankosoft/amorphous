//=====================================================================
//  tVector2_Gen.inl
//     - inline implementations of generic 2-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#include <math.h>


namespace amorphous
{

// global operators

template<class Scalar>
inline tVector2<Scalar> operator*( const Scalar f, const tVector2<Scalar>& v )
{
	tVector2<Scalar> vOut;
	vOut.x = f * v.x;
	vOut.y = f * v.y;
	return vOut;
}

template<class Scalar>
inline tVector2<Scalar> operator-( const tVector2<Scalar>& v )
{
	tVector2<Scalar> vOut;
	vOut.x = (-1) * v.x;
	vOut.y = (-1) * v.y;
	return vOut;
}


// global functions

template<class Scalar>
inline Scalar Vec2Dot( const tVector2<Scalar>& v0, const tVector2<Scalar>& v1 )
{
	return (v0.x * v1.x + v0.y * v1.y);
}


//inline void Vec2Cross()

template<class Scalar>
inline Scalar Vec2LengthSq( const tVector2<Scalar>& v )
{
	return ( v.x * v.x + v.y * v.y );
}

template<class Scalar>
inline Scalar Vec2Length( const tVector2<Scalar>& v )
{
	return (Scalar)sqrt( v.x * v.x + v.y * v.y );
}

template<class Scalar>
inline void Vec2Normalize( tVector2<Scalar>& v )
{
	v /= Vec2Length(v);
}

template<class Scalar>
inline tVector2<Scalar> Vec2GetNormalized( const tVector2<Scalar>& v )
{
	return v / Vec2Length(v);
}



// member functions & operators

template<class Scalar>
inline tVector2<Scalar>::tVector2( Scalar _x, Scalar _y )
: x(_x), y(_y)
{}

template<class Scalar>
inline Scalar& tVector2<Scalar>::operator[]( int i )
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

template<class Scalar>
inline const Scalar& tVector2<Scalar>::operator[]( int i ) const
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


template<class Scalar>
inline bool tVector2<Scalar>::operator==( const tVector2& v ) const
{
	if( x == v.x && y == v.y )
		return true;
	else
		return false;
}

template<class Scalar>
inline tVector2<Scalar> tVector2<Scalar>::operator=( const tVector2<Scalar>& v )
{
	x = v.x;
	y = v.y;
	return *this;
}


template<class Scalar>
inline tVector2<Scalar> tVector2<Scalar>::operator+( const tVector2<Scalar>& v ) const
{
	tVector2<Scalar> vOut;
	vOut.x = x + v.x;
	vOut.y = y + v.y;
	return vOut;
}


template<class Scalar>
inline tVector2<Scalar> tVector2<Scalar>::operator-( const tVector2<Scalar>& v ) const
{
	tVector2 vOut;
	vOut.x = x - v.x;
	vOut.y = y - v.y;
	return vOut;
}


template<class Scalar>
inline void tVector2<Scalar>::operator+=( const tVector2<Scalar>& v )
{
	x += v.x;
	y += v.y;
}


template<class Scalar>
inline void tVector2<Scalar>::operator-=( const tVector2<Scalar>& v )
{
	x -= v.x;
	y -= v.y;
}


template<class Scalar>
inline tVector2<Scalar> tVector2<Scalar>::operator*( const Scalar f ) const
{
	tVector2<Scalar> vOut;
	vOut.x = x * f;
	vOut.y = y * f;
	return vOut;
}


template<class Scalar>
inline tVector2<Scalar> tVector2<Scalar>::operator/( const Scalar f ) const
{
	tVector2<Scalar> vOut;
	vOut.x = x / f;
	vOut.y = y / f;
	return vOut;
}


template<class Scalar>
inline void tVector2<Scalar>::operator*=( const Scalar f )
{
	x *= f;
	y *= f;
}


template<class Scalar>
inline void tVector2<Scalar>::operator/=( const Scalar f )
{
	x /= f;
	y /= f;
}


template<class Scalar>
inline void tVector2<Scalar>::Normalize()
{
	Vec2Normalize(*this);
}


template<class Scalar>
inline Scalar tVector2<Scalar>::GetLength() const
{
	return Vec2Length(*this);
}


template<class Scalar>
inline Scalar tVector2<Scalar>::GetLengthSq() const
{
	return Vec2LengthSq(*this);
}

} // namespace amorphous
