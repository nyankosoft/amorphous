/*!
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
  
	\file matrix_vector3.inl
*/

#include "misc.hpp"
//#include "log_trace.hpp"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h> // for memcpy


//########################################################################
// 
//                       Matrix3
//
//########################################################################
inline Matrix33::Matrix33() {};
inline Matrix33::~Matrix33() {};

inline void Matrix33::SetTo(Scalar val)
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

inline Matrix33::Matrix33(Scalar val) {SetTo(val);}

inline Matrix33::Matrix33(Scalar v11, Scalar v21, Scalar v31, // first column
                          Scalar v12, Scalar v22, Scalar v32, // 2nd column
                          Scalar v13, Scalar v23, Scalar v33  )
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


inline Matrix33::Matrix33(const Vector3 & v1, // first column
                        const Vector3 & v2, 
                        const Vector3 & v3)
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


inline void Matrix33::SetData(const Scalar * d)
{
  memcpy(data, d, 9*sizeof(Scalar));
}


inline Vector3 Matrix33::GetColumn(uint i) const
{
  const uint o = i*3; 
  return Vector3(data[o], data[o+1], data[o+2]);
}


inline void Matrix33::SetColumn(uint i, const Vector3 & col)
{
  const uint o = i*3; 
  data[o]   = col[0];
  data[o+1] = col[1];
  data[o+2] = col[2];
}


inline bool Matrix33::IsSensible() const
{
  for (unsigned i = 0 ; i < 9 ; ++i)
  {
		if (!is_finite(data[i]))
	    return false;
  }
  return true;
}

/*
inline void Matrix33::show(const char * str) const
{
  uint i, j;
  TRACE("%s Matrix33::this = 0x%x \n", str, (int) this);
  for (i = 0 ; i < 3 ; i++)
  {
    for (j = 0 ; j < 3 ; j++)
    {
      TRACE("%4f ", operator()(i, j));
    }
    TRACE("\n");
  }
}*/


inline Matrix33 & Matrix33::operator+=(const Matrix33 & rhs)
{
  for (uint i = 9 ; i-- != 0 ;)
    data[i] += rhs.data[i];
  return *this;
}


inline Matrix33 & Matrix33::operator-=(const Matrix33 & rhs)
{
  for (uint i = 9 ; i-- != 0 ;)
    data[i] -= rhs.data[i];
  return *this;
}


inline Matrix33 & Matrix33::operator*=(const Scalar rhs)
{
  for (uint i = 9 ; i-- != 0 ;)
		data[i] *= rhs;
  return *this;
}


inline Matrix33 & Matrix33::operator/=(const Scalar rhs)
{
  const Scalar inv_rhs = 1.0f/rhs;
  for (uint i = 9 ; i-- != 0 ;)
		data[i] *= inv_rhs;
  return *this;
}


inline Matrix33 Matrix33::operator+(const Matrix33 & rhs) const
{
  return Matrix33(*this) += rhs;
}


inline Matrix33 Matrix33::operator-(const Matrix33 & rhs) const
{
  return Matrix33(*this) -= rhs;
}


inline void Matrix33::SetIdentity()
{
	*this = Matrix33Identity();
}


inline void Matrix33::SetRotationX( const Scalar angle )
{
	*this = Matrix33RotationX( angle );
}


inline void Matrix33::SetRotationY( const Scalar angle )
{
	*this = Matrix33RotationY( angle );
}


inline void Matrix33::SetRotationZ( const Scalar angle )
{
	*this = Matrix33RotationZ( angle );
}


inline void Matrix33::Orthonormalize()
{
	Vector3 u1(operator()(0, 0), operator()(1, 0), operator()(2, 0));
	Vector3 u2(operator()(0, 1), operator()(1, 1), operator()(2, 1));
	Vector3 u3(operator()(0, 2), operator()(1, 2), operator()(2, 2));

	Vector3 w1, w2, w3;
	Vec3Normalize( w1, u1 );
	Vec3Normalize( w2, (u2 - Vec3Project(w1, u2)) );
	Vec3Normalize( w3, (u3 - Vec3Project(w1, u3) - Vec3Project(w2, u3)) );

	//  Vector3 w1 = u1.normalise();

	//  Vector3 w2 = (u2 - proj(w1, u2)).normalise();
	//  Vector3 w3 = (u3 - proj(w1, u3) - proj(w2, u3)).normalise();

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
		*this = Matrix33(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
	}
}


inline void Matrix33::TransformByTranspose( Vector3 & dest, const Vector3 & src ) const
{
	dest[0] = operator()(0,0) * src[0] + operator()(1,0) * src[1] + operator()(2,0) * src[2];
	dest[1] = operator()(0,1) * src[0] + operator()(1,1) * src[1] + operator()(2,1) * src[2];
	dest[2] = operator()(0,2) * src[0] + operator()(1,2) * src[1] + operator()(2,2) * src[2];
}


inline void Matrix33::CopyRowMajorMatrix4( const Scalar *pSrcData )
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


inline void Matrix33::GetRowMajorMatrix44( Scalar *pDest ) const
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

inline Matrix33 operator*(const Matrix33 & lhs, const Scalar rhs)
{
  Matrix33 result;
  
  for (uint i = 9 ; i-- != 0 ; )
    result.data[i] = rhs * lhs.data[i];
  return result;
}


inline Matrix33 operator*(const Matrix33 & lhs, const Matrix33 & rhs)
{
  static Matrix33 out; // avoid ctor/dtor
  
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
inline Vector3 operator*(const Matrix33 & lhs, const Vector3 & rhs)
{
  return Vector3(
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


inline Matrix33 Matrix33Transpose(const Matrix33 & rhs)
{
  return Matrix33(rhs(0, 0), rhs(0, 1), rhs(0, 2),
                 rhs(1, 0), rhs(1, 1), rhs(1, 2),
                 rhs(2, 0), rhs(2, 1), rhs(2, 2) );
}


inline Matrix33 GetSkewSymmetricMatrix( const Vector3 & rhs )
{
	return Matrix33(     0, -rhs.z,  rhs.y,
		             rhs.z,      0, -rhs.x,
					-rhs.y,  rhs.x,      0 );
}


inline Scalar trace(const Matrix33 & rhs)
{
  return rhs(0,0) + rhs(1,1) + rhs(2,2);
}


// Some useful rotation Matrix3's
//inline Matrix33 m3alpha(Scalar alpha)
inline Matrix33 Matrix33RotationX( Scalar alpha )
{
  Matrix33 result(0);
  Scalar s = (Scalar) sin(alpha);
  Scalar c = (Scalar) cos(alpha);
  
  result(0,0) = 1;
  result(1,1) = c;
  result(2,2) = c;
  result(2,1) = s;
  result(1,2) = -s;
  
  return result;
}

//inline Matrix33 m3beta(Scalar beta)
inline Matrix33 Matrix33RotationY( Scalar beta )
{
  Matrix33 result(0);
  Scalar s = (Scalar) sin(beta);
  Scalar c = (Scalar) cos(beta);
  
  result(1,1) = 1;
  result(2,2) = c;
  result(0,0) = c;
  result(0,2) = s;
  result(2,0) = -s;
  
  return result;
}

//inline Matrix33 m3gamma(Scalar gamma)
inline Matrix33 Matrix33RotationZ( Scalar gamma )
{
  Matrix33 result(0);
  Scalar s = (Scalar) sin(gamma);
  Scalar c = (Scalar) cos(gamma);
  
  result(2,2) = 1;
  result(0,0) = c;
  result(1,1) = c;
  result(1,0) = s;
  result(0,1) = -s;
  
  return result;
}


inline Matrix33 Matrix33RotationAxis( Scalar angle, const Vector3& axis )
{
	Matrix33 s(       0,  axis.z, -axis.y,
		        -axis.z,       0,  axis.x,
				 axis.y, -axis.x,       0 );

	return Matrix33Identity() + (Scalar)sin(angle) * s + (1.0f - (Scalar)cos(angle)) * s * s;
}


/*
inline Matrix33 rotation_matrix(Scalar ang, const Vector3 & dir)
{
  // from page 32(45) of glspec.dvi
  Matrix33 uut(dir[0]*dir[0], dir[1]*dir[0], dir[2]*dir[0],
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
  
  Matrix33 s(0, dir[2], -dir[1],
            -dir[2], 0, dir[0],
            dir[1], -dir[0], 0);
  
//    s.set(0,1, -dir[2]);
//    s.set(0,2,  dir[1]);
  
//    s.set(1,0,  dir[2]);
//    s.set(1,2, -dir[0]);
  
//    s.set(2,0, -dir[1]);
//    s.set(2,1,  dir[0]);
  
  return (uut + (Scalar) cos_deg(ang) * 
          (matrix3_identity() - uut) + (Scalar) sin_deg(ang) * s);
}

// converts a rotation matrix into a rotation of degrees about axis
void calculate_rot_from_matrix(const Matrix33 & matrix, Vector3 & axis, Scalar & degrees)
{
  Scalar factor = (trace(matrix) - 1.0f) * 0.5f;
  if (factor > 1.0f)
    factor = 1.0f;
  else if (factor < -1.0f)
    factor = -1.0f;
  degrees = acos_deg(factor);

  if (degrees == 0.0f)
  {
    axis = Vector3(1.0f, 0.0f, 0.0f);
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
