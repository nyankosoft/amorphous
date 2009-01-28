/*!
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
  
	\file matrix_vector3.inl
*/

#include "misc.hpp"

#include <assert.h>


//########################################################################
// 
//                       Matrix22
//data: [0][2]
//      [1][3]
//########################################################################
inline Matrix22::Matrix22() {};
inline Matrix22::~Matrix22() {};

inline void Matrix22::SetTo(Scalar val)
{
  data[0] = val;
  data[1] = val;
  data[2] = val;
  data[3] = val;
}

inline Matrix22::Matrix22(Scalar val) {SetTo(val);}

inline Matrix22::Matrix22(Scalar v11, Scalar v21,	// first column
                          Scalar v12, Scalar v22 )	// 2nd column
{
  data[0] = v11;
  data[1] = v21;

  data[2] = v12;
  data[3] = v22;
}


inline Matrix22::Matrix22( const Vector2 & v1, // first column
                           const Vector2 & v2 )
{
  data[0] = v1[0];
  data[1] = v1[1];
  
  data[2] = v2[0];
  data[3] = v2[1];
}


inline void Matrix22::SetData(const Scalar * d)
{
  memcpy(data, d, 4*sizeof(Scalar));
}


inline Vector2 Matrix22::GetColumn(uint i) const
{
  const uint o = i*2; 
  return Vector2(data[o], data[o+1]);
}


inline void Matrix22::SetColumn(uint i, const Vector2 & col)
{
  const uint o = i*2; 
  data[o]   = col[0];
  data[o+1] = col[1];
}


inline bool Matrix22::IsSensible() const
{
  for (unsigned i = 0 ; i < 4 ; ++i)
  {
		if (!is_finite(data[i]))
	    return false;
  }
  return true;
}

/*
inline void Matrix22::show(const char * str) const
{
  uint i, j;
  TRACE("%s Matrix22::this = 0x%x \n", str, (int) this);
  for (i = 0 ; i < 3 ; i++)
  {
    for (j = 0 ; j < 3 ; j++)
    {
      TRACE("%4f ", operator()(i, j));
    }
    TRACE("\n");
  }
}*/


inline Matrix22 & Matrix22::operator+=(const Matrix22 & rhs)
{
  for (uint i = 4 ; i-- != 0 ;)
    data[i] += rhs.data[i];
  return *this;
}


inline Matrix22 & Matrix22::operator-=(const Matrix22 & rhs)
{
  for (uint i = 4 ; i-- != 0 ;)
    data[i] -= rhs.data[i];
  return *this;
}


inline Matrix22 & Matrix22::operator*=(const Scalar rhs)
{
  for (uint i = 4 ; i-- != 0 ;)
		data[i] *= rhs;
  return *this;
}


inline Matrix22 & Matrix22::operator/=(const Scalar rhs)
{
  const Scalar inv_rhs = 1.0f/rhs;
  for (uint i = 4 ; i-- != 0 ;)
		data[i] *= inv_rhs;
  return *this;
}


inline Matrix22 Matrix22::operator+(const Matrix22 & rhs) const
{
  return Matrix22(*this) += rhs;
}


inline Matrix22 Matrix22::operator-(const Matrix22 & rhs) const
{
  return Matrix22(*this) -= rhs;
}


inline void Matrix22::SetIdentity()
{
	*this = Matrix22Identity();
}

/*
inline void Matrix22::SetRotationX( const Scalar angle )
{
	*this = Matrix22RotationX( angle );
}


inline void Matrix22::SetRotationY( const Scalar angle )
{
	*this = Matrix22RotationY( angle );
}


inline void Matrix22::SetRotationZ( const Scalar angle )
{
	*this = Matrix22RotationZ( angle );
}*/

/*
inline void Matrix22::Orthonormalize()
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
		*this = Matrix22(Vector2(1, 0, 0), Vector2(0, 1, 0), Vector2(0, 0, 1));
	}
}
*/

inline void Matrix22::TransformByTranspose( Vector2 & dest, const Vector2 & src ) const
{
	dest[0] = operator()(0,0) * src[0] + operator()(1,0) * src[1];
	dest[1] = operator()(0,1) * src[0] + operator()(1,1) * src[1];
}

/*
inline void Matrix22::CopyRowMajorMatrix4( const Scalar *pSrcData )
{
	operator()(0, 0) = pSrcData[0];
	operator()(1, 0) = pSrcData[1];

	operator()(0, 1) = pSrcData[2];
	operator()(1, 1) = pSrcData[3];

}


inline void Matrix22::GetRowMajorMatrix44( Scalar *pDest ) const
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

inline Matrix22 operator*(const Matrix22 & lhs, const Scalar rhs)
{
  Matrix22 result;
  
  for (uint i = 4 ; i-- != 0 ; )
    result.data[i] = rhs * lhs.data[i];
  return result;
}


inline Matrix22 operator*(const Matrix22 & lhs, const Matrix22 & rhs)
{
  static Matrix22 out; // avoid ctor/dtor
  
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
inline Vector2 operator*(const Matrix22 & lhs, const Vector2 & rhs)
{
  return Vector2(
    lhs(0,0) * rhs[0] +
    lhs(0,1) * rhs[1],
    lhs(1,0) * rhs[0] +
    lhs(1,1) * rhs[1]);
}


inline Matrix22 Matrix22Transpose(const Matrix22 & rhs)
{
  return Matrix22(rhs(0, 0), rhs(0, 1),
                  rhs(1, 0), rhs(1, 1) );
}


//inline Matrix22 m3gamma(Scalar gamma)
inline Matrix22 Matrix22Rotation( Scalar theta )
{
  Matrix22 result(0);
  Scalar s = (Scalar) sin(theta);
  Scalar c = (Scalar) cos(theta);
  
  result(0,0) = c;
  result(1,1) = c;
  result(1,0) = s;
  result(0,1) = -s;
  
  return result;
}


/*
inline Matrix22 GetSkewSymmetricMatrix( const Vector2 & rhs )
{
	return Matrix22(     0, -rhs.z,  rhs.y,
		             rhs.z,      0, -rhs.x,
					-rhs.y,  rhs.x,      0 );
}


inline Scalar trace(const Matrix22 & rhs)
{
  return rhs(0,0) + rhs(1,1) + rhs(2,2);
}


// Some useful rotation Matrix3's
//inline Matrix22 m3alpha(Scalar alpha)
inline Matrix22 Matrix22RotationX( Scalar alpha )
{
  Matrix22 result(0);
  Scalar s = (Scalar) sin(alpha);
  Scalar c = (Scalar) cos(alpha);
  
  result(0,0) = 1;
  result(1,1) = c;
  result(2,2) = c;
  result(2,1) = s;
  result(1,2) = -s;
  
  return result;
}

//inline Matrix22 m3beta(Scalar beta)
inline Matrix22 Matrix22RotationY( Scalar beta )
{
  Matrix22 result(0);
  Scalar s = (Scalar) sin(beta);
  Scalar c = (Scalar) cos(beta);
  
  result(1,1) = 1;
  result(2,2) = c;
  result(0,0) = c;
  result(0,2) = s;
  result(2,0) = -s;
  
  return result;
}

//inline Matrix22 m3gamma(Scalar gamma)
inline Matrix22 Matrix22RotationZ( Scalar gamma )
{
  Matrix22 result(0);
  Scalar s = (Scalar) sin(gamma);
  Scalar c = (Scalar) cos(gamma);
  
  result(2,2) = 1;
  result(0,0) = c;
  result(1,1) = c;
  result(1,0) = s;
  result(0,1) = -s;
  
  return result;
}


inline Matrix22 Matrix22RotationAxis( Scalar angle, const Vector2& axis )
{
	Matrix22 s(       0,  axis.z, -axis.y,
		        -axis.z,       0,  axis.x,
				 axis.y, -axis.x,       0 );

	return Matrix22Identity() + (Scalar)sin(angle) * s + (1.0f - (Scalar)cos(angle)) * s * s;
}*/