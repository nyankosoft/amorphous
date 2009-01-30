#ifndef __3DMATH_MATRIX22_H__
#define __3DMATH_MATRIX22_H__

#include "../base.hpp"
#include "precision.h"
#include "Vector2.hpp"
#include <math.h>


/// A 2x2 matrix
class Matrix22
{
public:
	inline Matrix22( Scalar v11, Scalar v21,	// first column
					 Scalar v12, Scalar v22 );	// 2nd column

	inline Matrix22( const Vector2 & v1, // first column
					 const Vector2 & v2);

	inline Matrix22();
	explicit inline Matrix22(Scalar val);
	inline ~Matrix22();

	inline void SetTo(Scalar val);

	inline void Orthonormalize();

	inline bool IsSensible() const; // indicates if all is OK

	Scalar & operator()(const uint i, const uint j) {return data[i + 2*j];}
	const Scalar & operator()(const uint i, const uint j) const {return data[i + 2*j];}

	inline Vector2 operator()(const int iCol) const { return GetColumn(iCol); }

	/// returns pointer to the first element
	inline const Scalar * GetData() {return data;} 
	inline const Scalar * GetData() const {return data;} 

	/// pointer to value returned from get_data
	inline void SetData(const Scalar * d); 

	/// Returns a column - no range checking!
	inline Vector2 GetColumn(uint i) const;

	/// sets a column
	inline void SetColumn(uint i, const Vector2 & col);

	/// transform a vector by the transpose of this matrix
	inline void TransformByTranspose( Vector2 & dest, const Vector2 & src ) const;

	/// copy data from the row major matrix with a stride of 4
//	inline void CopyRowMajorMatrix4( const Scalar *pSrcData );

	/// get matrix data in the form of the row major matrix with a stride of 4
//	inline void GetRowMajorMatrix44( Scalar *pDest ) const;

	// operators
	inline Matrix22 & operator+=(const Matrix22 & rhs);
	inline Matrix22 & operator-=(const Matrix22 & rhs);

	inline Matrix22 & operator*=(const Scalar rhs);
	inline Matrix22 & operator/=(const Scalar rhs);

	inline Matrix22 operator+(const Matrix22 & rhs) const;
	inline Matrix22 operator-(const Matrix22 & rhs) const;

	inline void SetIdentity();
/*
	/// sets a rotation matrix that rotates a point around x-axis
	inline void SetRotationX( const Scalar angle );

	/// sets a rotation matrix that rotates a point around y-axis
	inline void SetRotationY( const Scalar angle );

	/// sets a rotation matrix that rotates a point around z-axis
	inline void SetRotationZ( const Scalar angle );
*/
	friend Matrix22 operator*(const Matrix22 & lhs, const Scalar rhs);
	friend Matrix22 operator*(const Scalar lhs, const Matrix22 & rhs);
	friend Matrix22 operator*(const Matrix22 & lhs, const Matrix22 & rhs);

//	friend Matrix22 Matrix22Transpose(const Matrix22 & rhs);
//	friend Scalar trace(const Matrix22 & rhs);

	friend Vector2 operator*(const Matrix22 & lhs, const Vector2 & rhs);

//  inline void show(const char * str = "") const;
  
private:

	Scalar data[4];
};


// global operators

inline Matrix22 operator*(const Matrix22 & lhs, const Scalar rhs);
inline Matrix22 operator*(const Scalar lhs, const Matrix22 & rhs) {return rhs * lhs;}
inline Matrix22 operator*(const Matrix22 & lhs, const Matrix22 & rhs);
inline Matrix22 Matrix22Transpose(const Matrix22 & rhs);


// matrix * vector
inline Vector2 operator*(const Matrix22 & lhs, const Vector2 & rhs);


// Some useful rotation Matrix22's
// alpha returns a matrix that wil rotate alpha around the x axis (etc)
//inline Matrix22 m3alpha(Scalar alpha);
//inline Matrix22 m3beta(Scalar beta);
//inline Matrix22 m3gamma(Scalar gamma);

inline Matrix22 Matrix22Rotation( Scalar theta );
//inline Matrix22 Matrix22RotationX( Scalar alpha );
//inline Matrix22 Matrix22RotationY( Scalar beta );
//inline Matrix22 Matrix22RotationZ( Scalar gamma );

/// returns the matrix that rotates a vertex around an axis
inline Matrix22 Matrix22RotationAxis( Scalar angle, const Vector2& axis );


inline const Matrix22 & Matrix22Identity()
{
  static const Matrix22 result(1, 0,
                               0, 1 );
  return result;
}


//inline Matrix22 rotation_matrix(Scalar ang, const Vector2 & dir);

// converts a rotation matrix into a rotation of degrees about axis
//inline void calculate_rot_from_matrix(const Matrix22 & matrix, Vector2 & axis, Scalar & degrees);


#include "Matrix22.inl"

#endif  /*  __3DMATH_MATRIX22_H__  */

