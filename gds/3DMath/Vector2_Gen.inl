//=====================================================================
//  TVector2_Gen.inl
//     - inline implementations of generic 2-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#include <math.h>


// global operators

template<class TScalar>
inline TVector2<TScalar> operator*( const TScalar f, const TVector2<TScalar>& v )
{
	TVector2 vOut;
	vOut.x = f * v.x;
	vOut.y = f * v.y;
	return vOut;
}


// global functions

template<class TScalar>
inline TScalar Vec2Dot( TVector2<TScalar>& v0, TVector2<TScalar>& v1 )
{
	return (v0.x * v1.x + v0.y * v1.y);
}


//inline void Vec2Cross()

template<class TScalar>
inline TScalar Vec2LengthSq( const TVector2<TScalar>& v )
{
	return ( v.x * v.x + v.y * v.y );
}

template<class TScalar>
inline TScalar Vec2Length( const TVector2<TScalar>& v )
{
	return (TScalar)sqrt( v.x * v.x + v.y * v.y );
}

template<class TScalar>
inline void Vec2Normalize( TVector2<TScalar>& v )
{
	v /= Vec2Length(v);
}

template<class TScalar>
inline TVector2<TScalar> Vec2GetNormalized( const TVector2<TScalar>& v )
{
	return v / Vec2Length(v);
}



// member functions & operators

template<class TScalar>
inline TVector2<TScalar>::TVector2( TScalar _x, TScalar _y )
: x(_x), y(_y)
{}

template<class TScalar>
inline TScalar& TVector2<TScalar>::operator[]( int i )
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


template<class TScalar>
inline TVector2<TScalar> TVector2<TScalar>::operator=( const TVector2<TScalar>& v )
{
	x = v.x;
	y = v.y;
	return *this;
}


template<class TScalar>
inline TVector2<TScalar> TVector2<TScalar>::operator+( const TVector2<TScalar>& v ) const
{
	TVector2 vOut;
	vOut.x = x + v.x;
	vOut.y = y + v.y;
	return vOut;
}


template<class TScalar>
inline TVector2<TScalar> TVector2<TScalar>::operator-( const TVector2<TScalar>& v ) const
{
	TVector2 vOut;
	vOut.x = x - v.x;
	vOut.y = y - v.y;
	return vOut;
}


template<class TScalar>
inline void TVector2<TScalar>::operator+=( const TVector2<TScalar>& v )
{
	x += v.x;
	y += v.y;
}


template<class TScalar>
inline void TVector2<TScalar>::operator-=( const TVector2<TScalar>& v )
{
	x -= v.x;
	y -= v.y;
}


template<class TScalar>
inline TVector2<TScalar> TVector2<TScalar>::operator*( const TScalar f ) const
{
	TVector2 vOut;
	vOut.x = x * f;
	vOut.y = y * f;
	return vOut;
}


template<class TScalar>
inline TVector2<TScalar> TVector2<TScalar>::operator/( const TScalar f ) const
{
	TVector2 vOut;
	vOut.x = x / f;
	vOut.y = y / f;
	return vOut;
}


template<class TScalar>
inline void TVector2<TScalar>::operator*=( const TScalar f )
{
	x *= f;
	y *= f;
}


template<class TScalar>
inline void TVector2<TScalar>::operator/=( const TScalar f )
{
	x /= f;
	y /= f;
}


template<class TScalar>
inline void TVector2<TScalar>::Normalize()
{
	Vec2Normalize(*this);
}


template<class TScalar>
inline TScalar TVector2<TScalar>::GetLength() const
{
	return Vec2Length(*this);
}


template<class TScalar>
inline TScalar TVector2<TScalar>::GetLengthSq() const
{
	return Vec2LengthSq(*this);
}
