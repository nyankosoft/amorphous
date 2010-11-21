/*!
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
  
	\file matrix_vector3.inl
*/

#include "misc.hpp"
#include <math.h>
//#include <stdio.h>
#include <string.h> // for memcpy


//########################################################################
// 
//                       Matrix3
//
//########################################################################

template<typename T>
inline tMatrix33<T>::tMatrix33() {}

template<typename T>
inline tMatrix33<T>::~tMatrix33() {}

template<typename T>
inline void tMatrix33<T>::SetTo(T val)
{
  data[0] = val;
  data[1] = val;
  data[2] = val;
  data[3] = val;
  data[4] = val;
  data[5] = val;
  data[6] = val;
  data[7] = val;
  data[8] = val;
}

template<typename T>
inline tMatrix33<T>::tMatrix33(T val) {SetTo(val);}

template<typename T>
inline tMatrix33<T>::tMatrix33(T v11, T v21, T v31, // first column
                          T v12, T v22, T v32, // 2nd column
                          T v13, T v23, T v33  )
{
  data[0] = v11;
  data[1] = v21;
  data[2] = v31;
  
  data[3] = v12;
  data[4] = v22;
  data[5] = v32;
  
  data[6] = v13;
  data[7] = v23;
  data[8] = v33;
}


template<typename T>
inline tMatrix33<T>::tMatrix33(const tVector3<T> & v1, // first column
                        const tVector3<T> & v2, 
                        const tVector3<T> & v3)
{
  data[0] = v1[0];
  data[1] = v1[1];
  data[2] = v1[2];
  
  data[3] = v2[0];
  data[4] = v2[1];
  data[5] = v2[2];
  
  data[6] = v3[0];
  data[7] = v3[1];
  data[8] = v3[2];
}


template<typename T>
inline void tMatrix33<T>::SetData(const T * d)
{
  memcpy(data, d, 9*sizeof(T));
}


template<typename T>
inline tVector3<T> tMatrix33<T>::GetColumn(uint i) const
{
  const uint o = i*3; 
  return tVector3<T>(data[o], data[o+1], data[o+2]);
}


template<typename T>
inline void tMatrix33<T>::SetColumn(uint i, const tVector3<T> & col)
{
  const uint o = i*3; 
  data[o]   = col[0];
  data[o+1] = col[1];
  data[o+2] = col[2];
}


template<typename T>
inline bool tMatrix33<T>::IsSensible() const
{
  for (unsigned i = 0 ; i < 9 ; ++i)
  {
		if (!is_finite(data[i]))
	    return false;
  }
  return true;
}

/*
template<typename T>
inline void tMatrix33<T>::show(const char * str) const
{
  uint i, j;
  TRACE("%s tMatrix33<T>::this = 0x%x \n", str, (int) this);
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
inline tMatrix33<T> & tMatrix33<T>::operator+=(const tMatrix33<T> & rhs)
{
  for (uint i = 9 ; i-- != 0 ;)
    data[i] += rhs.data[i];
  return *this;
}


template<typename T>
inline tMatrix33<T> & tMatrix33<T>::operator-=(const tMatrix33<T> & rhs)
{
  for (uint i = 9 ; i-- != 0 ;)
    data[i] -= rhs.data[i];
  return *this;
}


template<typename T>
inline tMatrix33<T> & tMatrix33<T>::operator*=(const T rhs)
{
  for (uint i = 9 ; i-- != 0 ;)
		data[i] *= rhs;
  return *this;
}


template<typename T>
inline tMatrix33<T> & tMatrix33<T>::operator/=(const T rhs)
{
  const T inv_rhs = 1.0f/rhs;
  for (uint i = 9 ; i-- != 0 ;)
		data[i] *= inv_rhs;
  return *this;
}


template<typename T>
inline tMatrix33<T> tMatrix33<T>::operator+(const tMatrix33<T> & rhs) const
{
  return tMatrix33<T>(*this) += rhs;
}


template<typename T>
inline tMatrix33<T> tMatrix33<T>::operator-(const tMatrix33<T> & rhs) const
{
  return tMatrix33<T>(*this) -= rhs;
}


template<typename T>
inline void tMatrix33<T>::SetIdentity()
{
	*this = tMatrix33Identity<T>();
}


template<typename T>
inline void tMatrix33<T>::SetRotationX( const T angle )
{
	*this = Matrix33RotationX<T>( angle );
}


template<typename T>
inline void tMatrix33<T>::SetRotationY( const T angle )
{
	*this = Matrix33RotationY<T>( angle );
}


template<typename T>
inline void tMatrix33<T>::SetRotationZ( const T angle )
{
	*this = Matrix33RotationZ<T>( angle );
}


template<typename T>
inline bool tMatrix33<T>::operator==( const tMatrix33<T>& rhs ) const
{
	for (uint i = 9 ; i-- != 0 ;)
	{
		if( data[i] != rhs.data[i] )
			return false;
	}

	return true;
}


template<typename T>
inline void tMatrix33<T>::Orthonormalize()
{
	tVector3<T> u1(operator()(0, 0), operator()(1, 0), operator()(2, 0));
	tVector3<T> u2(operator()(0, 1), operator()(1, 1), operator()(2, 1));
	tVector3<T> u3(operator()(0, 2), operator()(1, 2), operator()(2, 2));

	tVector3<T> w1, w2, w3;
	Vec3Normalize( w1, u1 );
	Vec3Normalize( w2, (u2 - Vec3Project(w1, u2)) );
	Vec3Normalize( w3, (u3 - Vec3Project(w1, u3) - Vec3Project(w2, u3)) );

	//  tVector3<T> w1 = u1.normalise();

	//  tVector3<T> w2 = (u2 - proj(w1, u2)).normalise();
	//  tVector3<T> w3 = (u3 - proj(w1, u3) - proj(w2, u3)).normalise();

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
		*this = tMatrix33<T>(tVector3<T>(1, 0, 0), tVector3<T>(0, 1, 0), tVector3<T>(0, 0, 1));
	}
}


template<typename T>
inline void tMatrix33<T>::TransformByTranspose( tVector3<T> & dest, const tVector3<T> & src ) const
{
	dest[0] = operator()(0,0) * src[0] + operator()(1,0) * src[1] + operator()(2,0) * src[2];
	dest[1] = operator()(0,1) * src[0] + operator()(1,1) * src[1] + operator()(2,1) * src[2];
	dest[2] = operator()(0,2) * src[0] + operator()(1,2) * src[1] + operator()(2,2) * src[2];
}


template<typename T>
inline void tMatrix33<T>::CopyRowMajorMatrix4( const T *pSrcData )
{
	operator()(0, 0) = pSrcData[0];
	operator()(1, 0) = pSrcData[1];
	operator()(2, 0) = pSrcData[2];

	operator()(0, 1) = pSrcData[4];
	operator()(1, 1) = pSrcData[5];
	operator()(2, 1) = pSrcData[6];

	operator()(0, 2) = pSrcData[8];
	operator()(1, 2) = pSrcData[9];
	operator()(2, 2) = pSrcData[10];
}


template<typename T>
inline void tMatrix33<T>::GetRowMajorMatrix44( T *pDest ) const
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
}



//=============================================================================
// global operators
//=============================================================================

template<typename T>
inline tMatrix33<T> operator*(const tMatrix33<T> & lhs, const T rhs)
{
  tMatrix33<T> result;
  
  for (uint i = 9 ; i-- != 0 ; )
    result.data[i] = rhs * lhs.data[i];
  return result;
}


template<typename T>
inline tMatrix33<T> operator*(const tMatrix33<T> & lhs, const tMatrix33<T> & rhs)
{
  static tMatrix33<T> out; // avoid ctor/dtor
  
  for (uint oj = 3 ; oj-- != 0 ;)
  {
    for (uint oi = 3 ; oi-- != 0 ;)
    {
      out(oi, oj) =
        lhs(oi, 0)*rhs(0, oj) +
        lhs(oi, 1)*rhs(1, oj) +
        lhs(oi, 2)*rhs(2, oj);
    }
  }
  return out;
}


// matrix * vector
template<typename T>
inline tVector3<T> operator*(const tMatrix33<T> & lhs, const tVector3<T> & rhs)
{
  return tVector3<T>(
    lhs(0,0) * rhs[0] +
    lhs(0,1) * rhs[1] +
    lhs(0,2) * rhs[2],
    lhs(1,0) * rhs[0] +
    lhs(1,1) * rhs[1] +
    lhs(1,2) * rhs[2],
    lhs(2,0) * rhs[0] +
    lhs(2,1) * rhs[1] +
    lhs(2,2) * rhs[2]);
}


template<typename T>
inline tMatrix33<T> Matrix33Transpose(const tMatrix33<T> & rhs)
{
  return tMatrix33<T>(rhs(0, 0), rhs(0, 1), rhs(0, 2),
                 rhs(1, 0), rhs(1, 1), rhs(1, 2),
                 rhs(2, 0), rhs(2, 1), rhs(2, 2) );
}


template<typename T>
inline tMatrix33<T> GetSkewSymmetricMatrix( const tVector3<T> & rhs )
{
	return tMatrix33<T>(     0, -rhs.z,  rhs.y,
		             rhs.z,      0, -rhs.x,
					-rhs.y,  rhs.x,      0 );
}


template<typename T>
inline T trace(const tMatrix33<T> & rhs)
{
  return rhs(0,0) + rhs(1,1) + rhs(2,2);
}


// Some useful rotation Matrix3's
//inline tMatrix33<T> m3alpha(T alpha)
template<typename T>
inline tMatrix33<T> Matrix33RotationX( T alpha )
{
  tMatrix33<T> result(0);
  T s = (T) sin(alpha);
  T c = (T) cos(alpha);
  
  result(0,0) = 1;
  result(1,1) = c;
  result(2,2) = c;
  result(2,1) = s;
  result(1,2) = -s;
  
  return result;
}

//inline tMatrix33<T> m3beta(T beta)
template<typename T>
inline tMatrix33<T> Matrix33RotationY( T beta )
{
  tMatrix33<T> result(0);
  T s = (T) sin(beta);
  T c = (T) cos(beta);
  
  result(1,1) = 1;
  result(2,2) = c;
  result(0,0) = c;
  result(0,2) = s;
  result(2,0) = -s;
  
  return result;
}

//inline tMatrix33<T> m3gamma(T gamma)
template<typename T>
inline tMatrix33<T> Matrix33RotationZ( T gamma )
{
  tMatrix33<T> result(0);
  T s = (T) sin(gamma);
  T c = (T) cos(gamma);
  
  result(2,2) = 1;
  result(0,0) = c;
  result(1,1) = c;
  result(1,0) = s;
  result(0,1) = -s;
  
  return result;
}


template<typename T>
inline tMatrix33<T> Matrix33RotationAxis( T angle, const tVector3<T>& axis )
{
	tMatrix33<T> s(       0,  axis.z, -axis.y,
		        -axis.z,       0,  axis.x,
				 axis.y, -axis.x,       0 );

	return tMatrix33Identity<T>() + (T)sin(angle) * s + (1.0f - (T)cos(angle)) * s * s;
}


template<typename T>
inline tMatrix33<T> Matrix33RotationHPR( T heading, T pitch, T roll )
{
	tMatrix33<T> rotation = tMatrix33Identity<T>();

	if( heading != 0 )
		rotation = rotation * Matrix33RotationY( heading ); // the last rotation to be applied
	if( pitch != 0 )
		rotation = rotation * Matrix33RotationX( pitch );
	if( roll != 0 )
		rotation = rotation * Matrix33RotationZ( roll ); // the first rotation to be applied

	return rotation;
}


template<typename T> inline tMatrix33<T> Matrix33RotationHPR_deg( T heading, T pitch, T roll )
{
	return Matrix33RotationHPR( (T)rad_to_deg(heading), (T)rad_to_deg(pitch), (T)rad_to_deg(roll) );
}


/*
inline tMatrix33<T> rotation_matrix(T ang, const tVector3<T> & dir)
{
  // from page 32(45) of glspec.dvi
  tMatrix33<T> uut(dir[0]*dir[0], dir[1]*dir[0], dir[2]*dir[0],
              dir[0]*dir[1], dir[1]*dir[1], dir[2]*dir[1],
              dir[0]*dir[2], dir[1]*dir[2], dir[2]*dir[2]);
  
//    uut.set(0,0, dir[0]*dir[0]);
//    uut.set(0,1, dir[0]*dir[1]);
//    uut.set(0,2, dir[0]*dir[2]);
  
//    uut.set(1,0, dir[1]*dir[0]);
//    uut.set(1,1, dir[1]*dir[1]);
//    uut.set(1,2, dir[1]*dir[2]);
  
//    uut.set(2,0, dir[2]*dir[0]);
//    uut.set(2,1, dir[2]*dir[1]);
//    uut.set(2,2, dir[2]*dir[2]);
  
  tMatrix33<T> s(0, dir[2], -dir[1],
            -dir[2], 0, dir[0],
            dir[1], -dir[0], 0);
  
//    s.set(0,1, -dir[2]);
//    s.set(0,2,  dir[1]);
  
//    s.set(1,0,  dir[2]);
//    s.set(1,2, -dir[0]);
  
//    s.set(2,0, -dir[1]);
//    s.set(2,1,  dir[0]);
  
  return (uut + (T) cos_deg(ang) * 
          (matrix3_identity() - uut) + (T) sin_deg(ang) * s);
}

// converts a rotation matrix into a rotation of degrees about axis
void calculate_rot_from_matrix(const tMatrix33<T> & matrix, tVector3<T> & axis, T & degrees)
{
  T factor = (trace(matrix) - 1.0f) * 0.5f;
  if (factor > 1.0f)
    factor = 1.0f;
  else if (factor < -1.0f)
    factor = -1.0f;
  degrees = acos_deg(factor);

  if (degrees == 0.0f)
  {
    axis = tVector3<T>(1.0f, 0.0f, 0.0f);
    return;
  }
  else if (degrees == 180.0f)
  {
    if ( (matrix(0, 0) > matrix(1, 1)) && (matrix(0, 0) > matrix(2, 2)) )
    {
      axis[0] = 0.5f * sqrt(matrix(0, 0) - matrix(1, 1) - matrix(2, 2) + 1.0f);
      axis[1] = matrix(0, 1) / (2.0f * axis[0]);
      axis[2] = matrix(0, 2) / (2.0f * axis[0]);
    }
    else if (matrix(1, 1) > matrix(2, 2))
    {
      axis[1] = 0.5f * sqrt(matrix(1, 1) - matrix(0, 0) - matrix(2, 2) + 1.0f);
      axis[0] = matrix(0, 1) / (2.0f * axis[1]);
      axis[2] = matrix(1, 2) / (2.0f * axis[1]);
    }
    else
    {
      axis[2] = 0.5f * sqrt(matrix(2, 2) - matrix(0, 0) - matrix(1, 1) + 1.0f);
      axis[0] = matrix(0, 2) / (2.0f * axis[2]);
      axis[2] = matrix(1, 2) / (2.0f * axis[2]);
    }
  }
  else
  {
    axis[0] = matrix(2, 1) - matrix(1, 2);
    axis[1] = matrix(0, 2) - matrix(2, 0);
    axis[2] = matrix(1, 0) - matrix(0, 1);
  }
  axis.normalise();
}*/
