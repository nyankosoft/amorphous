
#ifndef __3DMATH_MATRIX33_H__
#define __3DMATH_MATRIX33_H__

#include "precision.h"
#include "Vector3.h"
#include <math.h>


typedef int uint;

/// A 3x3 matrix
class Matrix33
{
public:
	inline Matrix33( Scalar v11, Scalar v21, Scalar v31, // first column
					Scalar v12, Scalar v22, Scalar v32, // 2nd column
					Scalar v13, Scalar v23, Scalar v33  );

	inline Matrix33( const Vector3 & v1, // first column
					const Vector3 & v2, 
					const Vector3 & v3);

	inline Matrix33();
	explicit inline Matrix33(Scalar val);
	inline ~Matrix33();

	inline void SetTo(Scalar val);

	inline void Orthonormalize();

	inline bool IsSensible() const; // indicates if all is OK

	Scalar & operator()(const uint i, const uint j) {return data[i + 3*j];}
	const Scalar & operator()(const uint i, const uint j) const {return data[i + 3*j];}

	inline Vector3 operator()(const int iCol) const { return GetColumn(iCol); }

	/// returns pointer to the first element
	inline const Scalar * GetData() {return data;} 
	inline const Scalar * GetData() const {return data;} 

	/// pointer to value returned from get_data
	inline void SetData(const Scalar * d); 

	/// Returns a column - no range checking!
	inline Vector3 GetColumn(uint i) const;

	/// sets a column
	inline void SetColumn(uint i, const Vector3 & col);

	/// transform a vector by the transpose of this matrix
	inline void TransformByTranspose( Vector3 & dest, const Vector3 & src ) const;

	/// copy data from the row major matrix with a stride of 4
	inline void CopyRowMajorMatrix4( const Scalar *pSrcData );

	/// get matrix data in the form of the row major matrix with a stride of 4
	inline void GetRowMajorMatrix44( Scalar *pDest ) const;

	// operators
	inline Matrix33 & operator+=(const Matrix33 & rhs);
	inline Matrix33 & operator-=(const Matrix33 & rhs);

	inline Matrix33 & operator*=(const Scalar rhs);
	inline Matrix33 & operator/=(const Scalar rhs);

	inline Matrix33 operator+(const Matrix33 & rhs) const;
	inline Matrix33 operator-(const Matrix33 & rhs) const;

	inline void SetIdentity();

	/// sets a rotation matrix that rotates a point around x-axis
	inline void SetRotationX( const Scalar angle );

	/// sets a rotation matrix that rotates a point around y-axis
	inline void SetRotationY( const Scalar angle );

	/// sets a rotation matrix that rotates a point around z-axis
	inline void SetRotationZ( const Scalar angle );

	friend Matrix33 operator*(const Matrix33 & lhs, const Scalar rhs);
	friend Matrix33 operator*(const Scalar lhs, const Matrix33 & rhs);
	friend Matrix33 operator*(const Matrix33 & lhs, const Matrix33 & rhs);

	friend Matrix33 Matrix33Transpose(const Matrix33 & rhs);

	friend Scalar trace(const Matrix33 & rhs);

	friend Vector3 operator*(const Matrix33 & lhs, const Vector3 & rhs);

//  inline void show(const char * str = "") const;
  
private:

	Scalar data[9];
};


// global operators

inline Matrix33 operator*(const Matrix33 & lhs, const Scalar rhs);
inline Matrix33 operator*(const Scalar lhs, const Matrix33 & rhs) {return rhs * lhs;}
inline Matrix33 operator*(const Matrix33 & lhs, const Matrix33 & rhs);
inline Matrix33 Matrix33Transpose(const Matrix33 & rhs);


// matrix * vector
inline Vector3 operator*(const Matrix33 & lhs, const Vector3 & rhs);


// Some useful rotation Matrix33's
// alpha returns a matrix that wil rotate alpha around the x axis (etc)
//inline Matrix33 m3alpha(Scalar alpha);
//inline Matrix33 m3beta(Scalar beta);
//inline Matrix33 m3gamma(Scalar gamma);

inline Matrix33 Matrix33RotationX( Scalar alpha );
inline Matrix33 Matrix33RotationY( Scalar beta );
inline Matrix33 Matrix33RotationZ( Scalar gamma );

/// returns the matrix that rotates a vertex around an axis
inline Matrix33 Matrix33RotationAxis( Scalar angle, const Vector3& axis );


inline const Matrix33 & Matrix33Identity()
{
  static const Matrix33 result(1, 0, 0,
                              0, 1, 0,
                              0, 0, 1);
  return result;
}


//inline Matrix33 rotation_matrix(Scalar ang, const Vector3 & dir);

// converts a rotation matrix into a rotation of degrees about axis
//inline void calculate_rot_from_matrix(const Matrix33 & matrix, Vector3 & axis, Scalar & degrees);


#include "Matrix33.inl"

#endif  /*  __3DMATH_MATRIX33_H__  */

