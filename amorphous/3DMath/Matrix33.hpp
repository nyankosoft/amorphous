#ifndef __3DMATH_MATRIX33_H__
#define __3DMATH_MATRIX33_H__

#include "../base.hpp"
#include "Vector3.hpp"


namespace amorphous
{


/// A 3x3 matrix
template<typename T>
class tMatrix33
{
public:
	inline tMatrix33( T v11, T v21, T v31, // first column
					T v12, T v22, T v32, // 2nd column
					T v13, T v23, T v33  );

	inline tMatrix33( const tVector3<T> & v1, // first column
					const tVector3<T> & v2, 
					const tVector3<T> & v3);

	inline tMatrix33();
	explicit inline tMatrix33(T val);
	inline ~tMatrix33();

	inline void SetTo(T val);

	inline void Orthonormalize();

	inline bool IsSensible() const; // indicates if all is OK

	T & operator()(const uint i, const uint j) {return data[i + 3*j];}
	const T & operator()(const uint i, const uint j) const {return data[i + 3*j];}

	inline tVector3<T> operator()(const int iCol) const { return GetColumn(iCol); }

	/// returns pointer to the first element
	inline const T * GetData() {return data;} 
	inline const T * GetData() const {return data;} 

	/// pointer to value returned from get_data
	inline void SetData(const T * d); 

	/// Returns a column - no range checking!
	inline tVector3<T> GetColumn(uint i) const;

	/// sets a column
	inline void SetColumn(uint i, const tVector3<T> & col);

	/// transform a vector by the transpose of this matrix
	inline void TransformByTranspose( tVector3<T> & dest, const tVector3<T> & src ) const;

	/// copy data from the row major matrix with a stride of 4
	inline void CopyRowMajorMatrix4( const T *pSrcData );

	/// get matrix data in the form of the row major matrix with a stride of 4
	inline void GetRowMajorMatrix44( T *pDest ) const;

	// operators
	inline tMatrix33 & operator+=(const tMatrix33 & rhs);
	inline tMatrix33 & operator-=(const tMatrix33 & rhs);

	inline tMatrix33 & operator*=(const T rhs);
	inline tMatrix33 & operator/=(const T rhs);

	inline tMatrix33 operator+(const tMatrix33 & rhs) const;
	inline tMatrix33 operator-(const tMatrix33 & rhs) const;

	inline void SetIdentity();

	/// sets a rotation matrix that rotates a point around x-axis
	inline void SetRotationX( const T angle );

	/// sets a rotation matrix that rotates a point around y-axis
	inline void SetRotationY( const T angle );

	/// sets a rotation matrix that rotates a point around z-axis
	inline void SetRotationZ( const T angle );

	inline bool operator==( const tMatrix33& rhs ) const;

	bool operator!=( const tMatrix33& rhs ) const { return !(*this == rhs); }

//	friend tMatrix33<T> operator*(const tMatrix33<T> & lhs, const T rhs); // This causes the "unresolved external symbol" link error. The same this applies to the other friend operators and functions.
	template<typename U> friend tMatrix33<U> operator*(const tMatrix33<U> & lhs, const U rhs);
	template<typename U> friend tMatrix33<U> operator*(const T lhs, const tMatrix33<T> & rhs);
	template<typename U> friend tMatrix33<U> operator*(const tMatrix33<T> & lhs, const tMatrix33<T> & rhs);

//  inline void show(const char * str = "") const;
  
private:

	T data[9];
};


// global operators

template<typename T> inline tMatrix33<T> operator*(const tMatrix33<T> & lhs, const T rhs);
template<typename T> inline tMatrix33<T> operator*(const T lhs, const tMatrix33<T> & rhs) {return rhs * lhs;}
template<typename T> inline tMatrix33<T> operator*(const tMatrix33<T> & lhs, const tMatrix33<T> & rhs);
template<typename T> inline tMatrix33<T> Matrix33Transpose(const tMatrix33<T> & rhs);


// matrix * vector
template<typename T> inline tVector3<T> operator*(const tMatrix33<T> & lhs, const tVector3<T> & rhs);


// matrix for rotation around X, Y, or Z axis
template<typename T> inline tMatrix33<T> Matrix33RotationX( T alpha );
template<typename T> inline tMatrix33<T> Matrix33RotationY( T beta );
template<typename T> inline tMatrix33<T> Matrix33RotationZ( T gamma );

/// returns the matrix that rotates a vertex around an axis
template<typename T> inline tMatrix33<T> Matrix33RotationAxis( T angle, const tVector3<T>& axis );

/**
function name HPR (heading, pitch, and roll) is based on the matrix concatination order
rotation is applied to Vector3 in the following order.
- 1. roll (rotation around z-axis)
- 2. pitch (rotation around x-axis)
- 3. heading (rotation around y-axis)
*/
template<typename T> inline tMatrix33<T> Matrix33RotationHPR( T heading, T pitch, T roll );

/// Same as Matrix33RotationHPR, but rotation angles are all interpreted in degrees.
template<typename T> inline tMatrix33<T> Matrix33RotationHPR_deg( T heading, T pitch, T roll );

template<typename T>
inline const tMatrix33<T> & tMatrix33Identity()
{
  static const tMatrix33<T> result(1, 0, 0,
                                   0, 1, 0,
                                   0, 0, 1);
  return result;
}


//inline tMatrix33 rotation_matrix(T ang, const tVector3<T> & dir);

// converts a rotation matrix into a rotation of degrees about axis
//inline void calculate_rot_from_matrix(const tMatrix33 & matrix, tVector3<T> & axis, T & degrees);


typedef tMatrix33<float> Matrix33;
typedef tMatrix33<double> dMatrix33;

#define Matrix33Identity tMatrix33Identity<float>
#define dMatrix33Identity tMatrix33Identity<double>


} // namespace amorphous


#include "Matrix33.inl"

#endif  /*  __3DMATH_MATRIX33_H__  */

