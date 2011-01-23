/*
*/

#include "misc.hpp"

#include <math.h>
#include <stdio.h>
#include <string.h> // for memcpy


//########################################################################
// 
//                       Matrix44
//
//########################################################################
inline Matrix44::Matrix44() {};
inline Matrix44::~Matrix44() {};

inline void Matrix44::SetTo(Scalar val)
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

inline Matrix44::Matrix44(Scalar val) {SetTo(val);}

inline Matrix44::Matrix44(Scalar v11, Scalar v21, Scalar v31, Scalar v41, // first column
                          Scalar v12, Scalar v22, Scalar v32, Scalar v42, // 2nd column
                          Scalar v13, Scalar v23, Scalar v33, Scalar v43,
						  Scalar v14, Scalar v24, Scalar v34, Scalar v44  )
{
	data[ 0] = v11;
	data[ 1] = v21;
	data[ 2] = v31;
	data[ 3] = v41;
	data[ 4] = v12;
	data[ 5] = v22;
	data[ 6] = v32;
	data[ 7] = v42;
	data[ 8] = v13;
	data[ 9] = v23;
	data[10] = v33;
	data[11] = v43;
	data[12] = v14;
	data[13] = v24;
	data[14] = v34;
	data[15] = v44;
}


inline Matrix44::Matrix44(const Vector3 & v1, // first column
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


inline void Matrix44::SetData(const Scalar * d)
{
	memcpy( data, d, NUM_ELEMENTS*sizeof(Scalar) );
}


inline Vector4 Matrix44::GetColumn(unsigned int i) const
{
  const uint o = i*4;
  return Vector4(data[o], data[o+1], data[o+2], data[o+3]);
}


inline void Matrix44::SetColumn(unsigned int i, const Vector4 & col)
{
  const uint o = i*4;
  data[o]   = col[0];
  data[o+1] = col[1];
  data[o+2] = col[2];
  data[o+3] = col[3];
}


inline bool Matrix44::IsSensible() const
{
	for (unsigned i = 0 ; i < NUM_ELEMENTS ; ++i)
	{
		if (!is_finite(data[i]))
		return false;
	}
	return true;
}

/*
inline void Matrix44::show(const char * str) const
{
  uint i, j;
  TRACE("%s Matrix44::this = 0x%x \n", str, (int) this);
  for (i = 0 ; i < 3 ; i++)
  {
    for (j = 0 ; j < 3 ; j++)
    {
      TRACE("%4f ", operator()(i, j));
    }
    TRACE("\n");
  }
}*/


inline Matrix44 & Matrix44::operator+=(const Matrix44 & rhs)
{
  for (uint i = NUM_ELEMENTS ; i-- != 0 ;)
    data[i] += rhs.data[i];
  return *this;
}


inline Matrix44 & Matrix44::operator-=(const Matrix44 & rhs)
{
  for (uint i = NUM_ELEMENTS ; i-- != 0 ;)
    data[i] -= rhs.data[i];
  return *this;
}


inline Matrix44 & Matrix44::operator*=(const Scalar rhs)
{
  for (uint i = NUM_ELEMENTS ; i-- != 0 ;)
		data[i] *= rhs;
  return *this;
}


inline Matrix44 & Matrix44::operator/=(const Scalar rhs)
{
  const Scalar inv_rhs = 1.0f/rhs;
  for (uint i = NUM_ELEMENTS ; i-- != 0 ;)
		data[i] *= inv_rhs;
  return *this;
}


inline Matrix44 Matrix44::operator+(const Matrix44 & rhs) const
{
  return Matrix44(*this) += rhs;
}


inline Matrix44 Matrix44::operator-(const Matrix44 & rhs) const
{
  return Matrix44(*this) -= rhs;
}


inline void Matrix44::SetIdentity()
{
	*this = Matrix44Identity();
}

/*
inline void Matrix44::SetRotationX( const Scalar angle )
{
	*this = Matrix44RotationX( angle );
}
inline void Matrix44::SetRotationY( const Scalar angle )
{
	*this = Matrix44RotationY( angle );
}
inline void Matrix44::SetRotationZ( const Scalar angle )
{
	*this = Matrix44RotationZ( angle );
}*/


inline bool Matrix44::operator==( const Matrix44& rhs ) const
{
	for (uint i = NUM_ELEMENTS ; i-- != 0 ;)
	{
		if( data[i] != rhs.data[i] )
			return false;
	}

	return true;
}

/*
inline void Matrix44::Orthonormalize()
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
		*this = Matrix44(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
	}
}


inline void Matrix44::TransformByTranspose( Vector3 & dest, const Vector3 & src ) const
{
	dest[0] = operator()(0,0) * src[0] + operator()(1,0) * src[1] + operator()(2,0) * src[2];
	dest[1] = operator()(0,1) * src[0] + operator()(1,1) * src[1] + operator()(2,1) * src[2];
	dest[2] = operator()(0,2) * src[0] + operator()(1,2) * src[1] + operator()(2,2) * src[2];
}
*/

inline void Matrix44::SetRowMajorMatrix44( const Scalar *pSrcData )
{
	operator()(0, 0) = pSrcData[0];
	operator()(1, 0) = pSrcData[1];
	operator()(2, 0) = pSrcData[2];
	operator()(3, 0) = pSrcData[3];

	operator()(0, 1) = pSrcData[4];
	operator()(1, 1) = pSrcData[5];
	operator()(2, 1) = pSrcData[6];
	operator()(3, 1) = pSrcData[7];

	operator()(0, 2) = pSrcData[8];
	operator()(1, 2) = pSrcData[9];
	operator()(2, 2) = pSrcData[10];
	operator()(3, 2) = pSrcData[11];

	operator()(0, 3) = pSrcData[12];
	operator()(1, 3) = pSrcData[13];
	operator()(2, 3) = pSrcData[14];
	operator()(3, 3) = pSrcData[15];
}


inline void Matrix44::GetRowMajorMatrix44( Scalar *pDest ) const
{
	// first row
	pDest[4 * 0 + 0] = operator()(0, 0);
	pDest[4 * 0 + 1] = operator()(1, 0);
	pDest[4 * 0 + 2] = operator()(2, 0);
	pDest[4 * 0 + 3] = operator()(3, 0);

	// second row
	pDest[4 * 1 + 0] = operator()(0, 1);
	pDest[4 * 1 + 1] = operator()(1, 1);
	pDest[4 * 1 + 2] = operator()(2, 1);
	pDest[4 * 1 + 3] = operator()(3, 1);

	// third row
	pDest[4 * 2 + 0] = operator()(0, 2);
	pDest[4 * 2 + 1] = operator()(1, 2);
	pDest[4 * 2 + 2] = operator()(2, 2);
	pDest[4 * 2 + 3] = operator()(3, 2);

	// fourth row
	pDest[4 * 3 + 0] = operator()(0, 3);
	pDest[4 * 3 + 1] = operator()(1, 3);
	pDest[4 * 3 + 2] = operator()(2, 3);
	pDest[4 * 3 + 3] = operator()(3, 3);
}


/// Produces the same result as the global multiplication operator of Matrix44 * Vector3.
/// This function requires slightly fewer calculations than the operator version.
inline Vector3 Matrix44::TransformCoord(const Vector3 & rhs) const
{
//	return Vector4((*this) * Vector4(rhs.x,rhs.y,rhs.z,1.0f)).ToVector3();
	const Matrix44& lhs = *this;
	Vector3 out;
	Scalar w = 0.0f;
	out.x = lhs(0,0) * rhs.x + lhs(0,1) * rhs.y + lhs(0,2) * rhs.z + lhs(0,3);
	out.y = lhs(1,0) * rhs.x + lhs(1,1) * rhs.y + lhs(1,2) * rhs.z + lhs(1,3);
	out.z = lhs(2,0) * rhs.x + lhs(2,1) * rhs.y + lhs(2,2) * rhs.z + lhs(2,3);
	    w = lhs(3,0) * rhs.x + lhs(3,1) * rhs.y + lhs(3,2) * rhs.z + lhs(3,3);
	return out / w;
}



//=============================================================================
// global operators
//=============================================================================

inline Matrix44 operator*(const Matrix44 & lhs, const Scalar rhs)
{
  Matrix44 result;
  
  for (uint i = Matrix44::NUM_ELEMENTS ; i-- != 0 ; )
    result.data[i] = rhs * lhs.data[i];
  return result;
}


inline Matrix44 operator*(const Matrix44 & lhs, const Matrix44 & rhs)
{
  static Matrix44 out; // avoid ctor/dtor
  
  for (uint oj = Matrix44::NUM_COLUMNS ; oj-- != 0 ;)
  {
    for (uint oi = Matrix44::NUM_ROWS ; oi-- != 0 ;)
    {
      out(oi, oj) =
        lhs(oi, 0)*rhs(0, oj) +
        lhs(oi, 1)*rhs(1, oj) +
        lhs(oi, 2)*rhs(2, oj) +
        lhs(oi, 3)*rhs(3, oj);
    }
  }
  return out;
}


/// Matrix44 * Vector4
inline Vector4 operator*(const Matrix44 & lhs, const Vector4 & rhs)
{
	Vector4 out;
	out.x = lhs(0,0) * rhs.x + lhs(0,1) * rhs.y + lhs(0,2) * rhs.z + lhs(0,3) * rhs.w;
	out.y = lhs(1,0) * rhs.x + lhs(1,1) * rhs.y + lhs(1,2) * rhs.z + lhs(1,3) * rhs.w;
	out.z = lhs(2,0) * rhs.x + lhs(2,1) * rhs.y + lhs(2,2) * rhs.z + lhs(2,3) * rhs.w;
	out.w = lhs(3,0) * rhs.x + lhs(3,1) * rhs.y + lhs(3,2) * rhs.z + lhs(3,3) * rhs.w;
	return out;
}


/// Matrix44 * Vector3
inline Vector3 operator*(const Matrix44 & lhs, const Vector3 & rhs)
{
	const Vector4 out( lhs * Vector4(rhs.x,rhs.y,rhs.z,1.0f) );
	return Vector3(out.x,out.y,out.z) / out.w;
}



//=============================================================================
// global functions
//=============================================================================

inline Matrix44 Matrix44Transpose(const Matrix44 & rhs)
{
  return Matrix44( rhs(0, 0), rhs(0, 1), rhs(0, 2), rhs(0, 3),
                   rhs(1, 0), rhs(1, 1), rhs(1, 2), rhs(1, 3),
                   rhs(2, 0), rhs(2, 1), rhs(2, 2), rhs(2, 3),
                   rhs(3, 0), rhs(3, 1), rhs(3, 2), rhs(3, 3) );
}

/*
inline Matrix44 GetSkewSymmetricMatrix( const Vector3 & rhs )
{
	return Matrix44(     0, -rhs.z,  rhs.y,
		             rhs.z,      0, -rhs.x,
					-rhs.y,  rhs.x,      0 );
}


inline Scalar trace(const Matrix44 & rhs)
{
  return rhs(0,0) + rhs(1,1) + rhs(2,2);
}
*/


inline Matrix44 Matrix44Scaling( Scalar x, Scalar y, Scalar z )
{
	Matrix44 out;
	out(0,0) = x; out(0,1) = 0; out(0,2) = 0; out(0,3) = 0;
	out(1,0) = 0; out(1,1) = y; out(1,2) = 0; out(1,3) = 0;
	out(2,0) = 0; out(2,1) = 0; out(2,2) = z; out(2,3) = 0;
	out(3,0) = 0; out(3,1) = 0; out(3,2) = 0; out(3,3) = 1;
	return out;
}


inline Matrix44 Matrix44PerspectiveFoV_LH( Scalar fov_y = (Scalar)PI / 4.0f, Scalar aspect_ratio__w_by_h = 4.0f / 3.0f, Scalar z_near = 0.1f, Scalar z_far = 500.0f )
{
	Scalar y_scale = 1.0f / (Scalar)tan(fov_y * 0.5f);//(cot(fov_y * 0.5f);
	Scalar x_scale = y_scale / aspect_ratio__w_by_h;
	Scalar _22 = z_far / (z_far - z_near);
	Scalar _23 = -z_near*z_far / (z_far - z_near);

	Matrix44 mat;
	mat(0,0) = x_scale; mat(0,1) = 0;       mat(0,2) = 0;         mat(0,3) = 0;
	mat(1,0) = 0;       mat(1,1) = y_scale; mat(1,2) = 0;         mat(1,3) = 0;
	mat(2,0) = 0;       mat(2,1) = 0;       mat(2,2) = _22;       mat(2,3) = _23;
	mat(3,0) = 0;       mat(3,1) = 0;       mat(3,2) = 1;         mat(3,3) = 0;

	return mat;
}


/// Builds a left-handed orthographic projection matrix
inline void Matrix44OrthoLH( Scalar w,
                             Scalar h,
                             Scalar z_near,
                             Scalar z_far,
                             Matrix44& dest
                             )
{
	Scalar _22 = 1.0f / (z_far - z_near);
	Scalar _23 = z_near / (z_near - z_far);
	dest(0,0) = 2.0f/w; dest(0,1) = 0;      dest(0,2) = 0;   dest(0,3) = 0;
	dest(1,0) = 0;      dest(1,1) = 2.0f/h; dest(1,2) = 0;   dest(1,3) = 0;
	dest(2,0) = 0;      dest(2,1) = 0;      dest(2,2) = _22; dest(2,3) = _23;
	dest(3,0) = 0;      dest(3,1) = 0;      dest(3,2) = 0;   dest(3,3) = 1;
}


inline Matrix44 Matrix44OrthoLH( Scalar w,
                                 Scalar h,
                                 Scalar z_near,
                                 Scalar z_far
                                 )
{
	Matrix44 dest;
	Matrix44OrthoLH( w, h, z_near, z_far, dest );
	return dest;
}


inline Matrix44 Matrix44Mirror( const SPlane& plane )
{
	Matrix44 out;
	const Vector3& n = plane.normal;
	const float k = plane.dist;

	out(0,0) = 1-2*n.x*n.x;  out(0,1) =  -2*n.x*n.y;  out(0,2) =  -2*n.x*n.z;  out(0,3) = 2*n.x*k;
	out(1,0) =  -2*n.y*n.x;  out(1,1) = 1-2*n.y*n.y;  out(1,2) =  -2*n.y*n.z;  out(1,3) = 2*n.y*k;
	out(2,0) =  -2*n.z*n.x;  out(2,1) =  -2*n.z*n.y;  out(2,2) = 1-2*n.z*n.z;  out(2,3) = 2*n.z*k;
	out(3,0) = 0;  out(3,1) = 0;  out(3,2) = 0;  out(3,3) = 1;

	return out;
}
