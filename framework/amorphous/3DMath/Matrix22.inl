/*!
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
  
	\file matrix_vector3.inl
*/

#include "misc.hpp"


namespace amorphous
{

//########################################################################
// 
//                       tMatrix22<T>
//data: [0][2]
//      [1][3]
//########################################################################
template<typename T> inline tMatrix22<T>::tMatrix22() {};
template<typename T> inline tMatrix22<T>::~tMatrix22() {};

template<typename T>
inline void tMatrix22<T>::SetTo(T val)
{
  data[0] = val;
  data[1] = val;
  data[2] = val;
  data[3] = val;
}

template<typename T>
inline tMatrix22<T>::tMatrix22(T val) {SetTo(val);}

template<typename T>
inline tMatrix22<T>::tMatrix22(T v11, T v21,	// first column
                          T v12, T v22 )	// 2nd column
{
  data[0] = v11;
  data[1] = v21;

  data[2] = v12;
  data[3] = v22;
}


template<typename T>
inline tMatrix22<T>::tMatrix22( const tVector2<T> & v1, // first column
                           const tVector2<T> & v2 )
{
  data[0] = v1[0];
  data[1] = v1[1];
  
  data[2] = v2[0];
  data[3] = v2[1];
}


template<typename T>
inline void tMatrix22<T>::SetData(const T * d)
{
  memcpy(data, d, 4*sizeof(T));
}


template<typename T>
inline tVector2<T> tMatrix22<T>::GetColumn(uint i) const
{
  const uint o = i*2; 
  return tVector2<T>(data[o], data[o+1]);
}


template<typename T>
inline void tMatrix22<T>::SetColumn(uint i, const tVector2<T> & col)
{
  const uint o = i*2; 
  data[o]   = col[0];
  data[o+1] = col[1];
}


template<typename T>
inline bool tMatrix22<T>::IsSensible() const
{
  for (unsigned i = 0 ; i < 4 ; ++i)
  {
		if (!is_finite(data[i]))
	    return false;
  }
  return true;
}

/*
inline void tMatrix22<T>::show(const char * str) const
{
  uint i, j;
  TRACE("%s tMatrix22<T>::this = 0x%x \n", str, (int) this);
  for (i = 0 ; i < 3 ; i++)
  {
    for (j = 0 ; j < 3 ; j++)
    {
      TRACE("%4f ", operator()(i, j));
    }
    TRACE("\n");
  }
}*/


template<typename T>
inline tMatrix22<T> & tMatrix22<T>::operator+=(const tMatrix22<T> & rhs)
{
  for (uint i = 4 ; i-- != 0 ;)
    data[i] += rhs.data[i];
  return *this;
}


template<typename T>
inline tMatrix22<T> & tMatrix22<T>::operator-=(const tMatrix22<T> & rhs)
{
  for (uint i = 4 ; i-- != 0 ;)
    data[i] -= rhs.data[i];
  return *this;
}


template<typename T>
inline tMatrix22<T> & tMatrix22<T>::operator*=(const T rhs)
{
  for (uint i = 4 ; i-- != 0 ;)
		data[i] *= rhs;
  return *this;
}


template<typename T>
inline tMatrix22<T> & tMatrix22<T>::operator/=(const T rhs)
{
  const T inv_rhs = 1.0f/rhs;
  for (uint i = 4 ; i-- != 0 ;)
		data[i] *= inv_rhs;
  return *this;
}


template<typename T>
inline tMatrix22<T> tMatrix22<T>::operator+(const tMatrix22<T> & rhs) const
{
  return tMatrix22<T>(*this) += rhs;
}


template<typename T>
inline tMatrix22<T> tMatrix22<T>::operator-(const tMatrix22<T> & rhs) const
{
  return tMatrix22<T>(*this) -= rhs;
}


template<typename T>
inline bool tMatrix22<T>::operator==(const tMatrix22<T> & rhs) const
{
	for( int i=0; i<4; i++ )
	{
		if( data[i] != rhs.data[i] )
			return false;
	}

	return true;
}


template<typename T>
inline void tMatrix22<T>::SetIdentity()
{
	*this = tMatrix22<T>Identity();
}


template<typename T>
inline void tMatrix22<T>::SetRotation( const T angle )
{
	*this = tMatrix22<T>Rotation( angle );
}


/*
inline void tMatrix22<T>::Orthonormalize()
{
	Vector2 u1( operator()(0, 0), operator()(1, 0) );
	Vector2 u2( operator()(0, 1), operator()(1, 1) );

	Vector2 w1, w2;
	Vec3Normalize( w1, u1 );
	Vec3Normalize( w2, (u2 - Vec3Project(w1, u2)) );
	Vec3Normalize( w3, (u3 - Vec3Project(w1, u3) - Vec3Project(w2, u3)) );

	//  Vector2 w1 = u1.normalise();

	//  Vector2 w2 = (u2 - proj(w1, u2)).normalise();
	//  Vector2 w3 = (u3 - proj(w1, u3) - proj(w2, u3)).normalise();

	operator()(0, 0) = w1[0];
	operator()(1, 0) = w1[1];
	operator()(2, 0) = w1[2];

	operator()(0, 1) = w2[0];
	operator()(1, 1) = w2[1];
	operator()(2, 1) = w2[2];

	operator()(0, 2) = w3[0];
	operator()(1, 2) = w3[1];
	operator()(2, 2) = w3[2];

	if (IsSensible() == false)
	{
		//    TRACE("orthonormalize() resulted in bad matrix\n");
		*this = tMatrix22<T>(Vector2(1, 0, 0), Vector2(0, 1, 0), Vector2(0, 0, 1));
	}
}
*/

template<typename T>
inline void tMatrix22<T>::TransformByTranspose( tVector2<T> & dest, const tVector2<T> & src ) const
{
	dest[0] = operator()(0,0) * src[0] + operator()(1,0) * src[1];
	dest[1] = operator()(0,1) * src[0] + operator()(1,1) * src[1];
}

/*
inline void tMatrix22<T>::CopyRowMajorMatrix4( const T *pSrcData )
{
	operator()(0, 0) = pSrcData[0];
	operator()(1, 0) = pSrcData[1];

	operator()(0, 1) = pSrcData[2];
	operator()(1, 1) = pSrcData[3];

}


inline void tMatrix22<T>::GetRowMajorMatrix44( T *pDest ) const
{
	// first row
	pDest[4 * 0 + 0] = operator()(0, 0);
	pDest[4 * 0 + 1] = operator()(1, 0);
	pDest[4 * 0 + 2] = operator()(2, 0);
	pDest[4 * 0 + 3] = 0;

	// second row
	pDest[4 * 1 + 0] = operator()(0, 1);
	pDest[4 * 1 + 1] = operator()(1, 1);
	pDest[4 * 1 + 2] = operator()(2, 1);
	pDest[4 * 1 + 3] = 0;

	// third row
	pDest[4 * 2 + 0] = operator()(0, 2);
	pDest[4 * 2 + 1] = operator()(1, 2);
	pDest[4 * 2 + 2] = operator()(2, 2);
	pDest[4 * 2 + 3] = 0;

	// fourth row
	pDest[4 * 3 + 0] = 0;
	pDest[4 * 3 + 1] = 0;
	pDest[4 * 3 + 2] = 0;
	pDest[4 * 3 + 3] = 1;
}*/



//=============================================================================
// global operators
//=============================================================================

template<typename T>
inline tMatrix22<T> operator*(const tMatrix22<T> & lhs, const T rhs)
{
  tMatrix22<T> result;
  
  for (uint i = 4 ; i-- != 0 ; )
    result.data[i] = rhs * lhs.data[i];
  return result;
}


template<typename T>
inline tMatrix22<T> operator*(const tMatrix22<T> & lhs, const tMatrix22<T> & rhs)
{
  static tMatrix22<T> out; // avoid ctor/dtor
  
  for (uint oj = 2 ; oj-- != 0 ;)
  {
    for (uint oi = 2 ; oi-- != 0 ;)
    {
      out(oi, oj) =
        lhs(oi, 0)*rhs(0, oj) +
        lhs(oi, 1)*rhs(1, oj);
    }
  }
  return out;
}


// matrix * vector
template<typename T>
inline Vector2 operator*(const tMatrix22<T> & lhs, const Vector2 & rhs)
{
  return Vector2(
    lhs(0,0) * rhs[0] +
    lhs(0,1) * rhs[1],
    lhs(1,0) * rhs[0] +
    lhs(1,1) * rhs[1]);
}


template<typename T>
inline tMatrix22<T> Matrix22Transpose(const tMatrix22<T> & rhs)
{
  return tMatrix22<T>(rhs(0, 0), rhs(0, 1),
                  rhs(1, 0), rhs(1, 1) );
}


//inline tMatrix22<T> m3gamma(T gamma)
template<typename T>
inline tMatrix22<T> Matrix22Rotation( T theta )
{
  tMatrix22<T> result(0);
  T s = (T) sin(theta);
  T c = (T) cos(theta);
  
  result(0,0) = c;
  result(1,1) = c;
  result(1,0) = s;
  result(0,1) = -s;
  
  return result;
}


/*
inline tMatrix22<T> GetSkewSymmetricMatrix( const Vector2 & rhs )
{
	return tMatrix22<T>(     0, -rhs.z,  rhs.y,
		             rhs.z,      0, -rhs.x,
					-rhs.y,  rhs.x,      0 );
}


inline T trace(const tMatrix22<T> & rhs)
{
  return rhs(0,0) + rhs(1,1) + rhs(2,2);
}
*/

} // namespace amorphous
