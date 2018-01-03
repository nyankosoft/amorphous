#ifndef __3DMATH_MATRIX22_H__
#define __3DMATH_MATRIX22_H__

#include "../base.hpp"
#include "precision.h"
#include "Vector2.hpp"
#include <math.h>


namespace amorphous
{


/// A 2x2 matrix
template<typename T>
class tMatrix22
{
public:
	inline tMatrix22( T v11, T v21,	// first column
					 T v12, T v22 );	// 2nd column

	inline tMatrix22( const tVector2<T> & v1, // first column
					 const tVector2<T> & v2);

	inline tMatrix22();
	explicit inline tMatrix22(T val);
	inline ~tMatrix22();

	inline void SetTo(T val);

	inline void Orthonormalize();

	inline bool IsSensible() const; // indicates if all is OK

	T & operator()(const uint i, const uint j) {return data[i + 2*j];}
	const T & operator()(const uint i, const uint j) const {return data[i + 2*j];}

	inline tVector2<T> operator()(const int iCol) const { return GetColumn(iCol); }

	/// returns pointer to the first element
	inline const T * GetData() {return data;} 
	inline const T * GetData() const {return data;} 

	/// pointer to value returned from get_data
	inline void SetData(const T * d); 

	/// Returns a column - no range checking!
	inline tVector2<T> GetColumn(uint i) const;

	/// sets a column
	inline void SetColumn(uint i, const tVector2<T> & col);

	/// transform a vector by the transpose of this matrix
	inline void TransformByTranspose( tVector2<T> & dest, const tVector2<T> & src ) const;

	/// copy data from the row major matrix with a stride of 4
//	inline void CopyRowMajorMatrix4( const T *pSrcData );

	/// get matrix data in the form of the row major matrix with a stride of 4
//	inline void GetRowMajorMatrix44( T *pDest ) const;

	// operators
	inline tMatrix22 & operator+=(const tMatrix22 & rhs);
	inline tMatrix22 & operator-=(const tMatrix22 & rhs);

	inline tMatrix22 & operator*=(const T rhs);
	inline tMatrix22 & operator/=(const T rhs);

	inline tMatrix22 operator+(const tMatrix22 & rhs) const;
	inline tMatrix22 operator-(const tMatrix22 & rhs) const;

	inline bool operator==(const tMatrix22 & rhs) const;

	inline void SetIdentity();

	/// sets a rotation matrix that rotates a 2D point
	inline void SetRotation( const T angle );

	template<typename U> friend tMatrix22<U> operator*(const tMatrix22<U> & lhs, const U rhs);
	template<typename U> friend tMatrix22<U> operator*(const T lhs, const tMatrix22<T> & rhs);
	template<typename U> friend tMatrix22<U> operator*(const tMatrix22<T> & lhs, const tMatrix22<T> & rhs);

//	friend tMatrix22 tMatrix22Transpose(const tMatrix22 & rhs);
//	friend T trace(const tMatrix22 & rhs);
  
private:

	T data[4];
};


// global operators

template<typename T> inline tMatrix22<T> operator*(const tMatrix22<T> & lhs, const T rhs);
template<typename T> inline tMatrix22<T> operator*(const T lhs, const tMatrix22<T> & rhs) {return rhs * lhs;}
template<typename T> inline tMatrix22<T> operator*(const tMatrix22<T> & lhs, const tMatrix22<T> & rhs);
template<typename T> inline tMatrix22<T> Matrix22Transpose(const tMatrix22<T> & rhs);


// matrix * vector
template<typename T> inline tVector2<T> operator*(const tMatrix22<T> & lhs, const tVector2<T> & rhs);

template<typename T> inline tMatrix22<T> Matrix22Rotation( T theta );


template<typename T> inline const tMatrix22<T> & tMatrix22Identity()
{
  static const tMatrix22<T> result(1, 0,
                               0, 1 );
  return result;
}


//inline tMatrix22 rotation_matrix(T ang, const tVector2<T> & dir);

// converts a rotation matrix into a rotation of degrees about axis
//inline void calculate_rot_from_matrix(const tMatrix22 & matrix, tVector2<T> & axis, T & degrees);


typedef tMatrix22<float> Matrix22;
typedef tMatrix22<double> dMatrix22;

#define Matrix22Identity tMatrix22Identity<float>
#define dMatrix22Identity tMatrix22Identity<double>


} // namespace amorphous


#include "Matrix22.inl"

#endif  /*  __3DMATH_MATRIX22_H__  */

