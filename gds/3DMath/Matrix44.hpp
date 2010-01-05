#ifndef __3DMATH_MATRIX44_H__
#define __3DMATH_MATRIX44_H__

#include "../base.hpp"
#include "precision.h"
#include "Vector3.hpp"
#include <math.h>


/// A column-major 4x4 matrix
class Matrix44
{
public:
	inline Matrix44( Scalar v11, Scalar v21, Scalar v31, Scalar v41, // first column
					 Scalar v12, Scalar v22, Scalar v32, Scalar v42, // 2nd column
					 Scalar v13, Scalar v23, Scalar v33, Scalar v43, // 3rd column
					 Scalar v14, Scalar v24, Scalar v34, Scalar v44  );

	inline Matrix44( const Vector3 & v1, // first column
					 const Vector3 & v2, 
					 const Vector3 & v3);

	inline Matrix44();
	explicit inline Matrix44(Scalar val);
	inline ~Matrix44();

	inline void SetTo(Scalar val);

//	inline void Orthonormalize();

	inline bool IsSensible() const; ///< returns true if all is OK

	Scalar & operator()(const uint i, const uint j) {return data[i + NUM_COLUMNS*j];}
	const Scalar & operator()(const uint i, const uint j) const {return data[i + NUM_COLUMNS*j];}

//	inline Vector3 operator()(const int iCol) const { return GetColumn(iCol); }

	/// returns pointer to the first element
	inline const Scalar * GetData() { return data; }
	inline const Scalar * GetData() const { return data; }

	/// pointer to value returned from get_data
	inline void SetData(const Scalar * d); 

	/// Returns a column - no range checking!
//	inline Vector3 GetColumn(uint i) const;

	/// sets a column
//	inline void SetColumn(uint i, const Vector3 & col);

	/// transform a vector by the transpose of this matrix
//	inline void TransformByTranspose( Vector3 & dest, const Vector3 & src ) const;

	/// copy data from the row major matrix with a stride of 4
	inline void SetRowMajorMatrix44( const Scalar *pSrcData );

	/// get matrix data in the form of the row major matrix with a stride of 4
	inline void GetRowMajorMatrix44( Scalar *pDest ) const;

	// operators
	inline Matrix44 & operator+=(const Matrix44 & rhs);
	inline Matrix44 & operator-=(const Matrix44 & rhs);

	inline Matrix44 & operator*=(const Scalar rhs);
	inline Matrix44 & operator/=(const Scalar rhs);

	inline Matrix44 operator+(const Matrix44 & rhs) const;
	inline Matrix44 operator-(const Matrix44 & rhs) const;

	inline void SetIdentity();

	/// sets a rotation matrix that rotates a point around x-axis
/*	inline void SetRotationX( const Scalar angle );
	/// sets a rotation matrix that rotates a point around y-axis
	inline void SetRotationY( const Scalar angle );
	/// sets a rotation matrix that rotates a point around z-axis
	inline void SetRotationZ( const Scalar angle );
*/
	inline bool operator==( const Matrix44& rhs ) const;

	bool operator!=( const Matrix44& rhs ) const { return !(*this == rhs); }

	friend Matrix44 operator*(const Matrix44 & lhs, const Scalar rhs);
	friend Matrix44 operator*(const Scalar lhs, const Matrix44 & rhs);
	friend Matrix44 operator*(const Matrix44 & lhs, const Matrix44 & rhs);

	friend Matrix44 Matrix44Transpose(const Matrix44 & rhs);

	friend Scalar trace(const Matrix44 & rhs);

//	friend Vector3 operator*(const Matrix44 & lhs, const Vector3 & rhs);

//  inline void show(const char * str = "") const;

	static const uint NUM_ROWS = 4;
	static const uint NUM_COLUMNS = 4;
	static const uint NUM_ELEMENTS = NUM_ROWS * NUM_COLUMNS;

private:
/*
	enum Params
	{
		NUM_ROWS = 4,
		NUM_COLUMNS = 4
		NUM_ELEMENTS = NUM_ROWS * NUM_COLUMNS,
	};
*/
	Scalar data[NUM_ELEMENTS];
};


// global operators

inline Matrix44 operator*(const Matrix44 & lhs, const Scalar rhs);
inline Matrix44 operator*(const Scalar lhs, const Matrix44 & rhs) {return rhs * lhs;}
inline Matrix44 operator*(const Matrix44 & lhs, const Matrix44 & rhs);
inline Matrix44 Matrix44Transpose(const Matrix44 & rhs);


// matrix * vector
//inline Vector3 operator*(const Matrix44 & lhs, const Vector3 & rhs);


/*
inline Matrix44 Matrix44RotationX( Scalar alpha );
inline Matrix44 Matrix44RotationY( Scalar beta );
inline Matrix44 Matrix44RotationZ( Scalar gamma );
/// returns the matrix that rotates a vertex around an axis
inline Matrix44 Matrix44RotationAxis( Scalar angle, const Vector3& axis );
*/

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



inline const Matrix44 & Matrix44Identity()
{
  static const Matrix44 result(1, 0, 0, 0,
                               0, 1, 0, 0,
                               0, 0, 1, 0,
                               0, 0, 0, 1);
  return result;
}


//inline Matrix44 rotation_matrix(Scalar ang, const Vector3 & dir);

// converts a rotation matrix into a rotation of degrees about axis
//inline void calculate_rot_from_matrix(const Matrix44 & matrix, Vector3 & axis, Scalar & degrees);


#include "Matrix44.inl"

#endif  /*  __3DMATH_MATRIX44_H__  */

