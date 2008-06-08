//=====================================================================
//  TVector2_Gen.h
//     - generic 2-dimensional vector class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __VECTOR2_GEN_H__
#define  __VECTOR2_GEN_H__

#include "precision.h"


template<class TScalar>
class TVector2
{
public:

	TScalar x, y;

	inline TVector2() {}
	inline TVector2( TScalar _x, TScalar _y );

	//	~TVector2();

	/// vector - vector operators

	inline TVector2 operator=( const TVector2& v );

	inline TVector2 operator+( const TVector2& v ) const;
	inline TVector2 operator-( const TVector2& v ) const;

	inline void operator+=( const TVector2& v );
	inline void operator-=( const TVector2& v );


	/// vector - scalar operators

	inline TVector2 operator*( const TScalar f ) const;
	inline TVector2 operator/( const TScalar f ) const;

	inline void operator*=( const TScalar f );
	inline void operator/=( const TScalar f );

	inline TScalar& operator[]( int i );


	inline void Normalize();

	inline TScalar GetLength() const;

	inline TScalar GetLengthSq() const;

};


#include "Vector2_Gen.inl"


#endif		/*  __VECTOR2_GEN_H__  */
