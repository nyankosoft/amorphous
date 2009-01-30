#ifndef __MATRIX34_H__
#define __MATRIX34_H__


#include "Vector3.hpp"
#include "Matrix33.hpp"


class Matrix34
{
public:

	/// rotation
	Matrix33 matOrient;

	/// translation
	Vector3 vPosition;

public:

	inline Matrix34() {}

	inline Matrix34( const Vector3& pos, const Matrix33& orientation );

	inline void Identity();

	inline void Default() { Identity(); }

	/// apply the affine transformation to a three element vector
	inline void Transform( Vector3& rvDest, const Vector3& rvSrc ) const;

	/// apply the inverse transformation to a three element vector
	inline void InvTransform( Vector3& rvDest, const Vector3& rvSrc ) const;

	/// get the inverse -- assumes that the orientation matrix is orthogonal
	inline void GetInverseROT( Matrix34& dest ) const;

	/// Non-peformance friendly version of GetInverseROT()
	inline Matrix34 GetInverseROT() const;

	/// copy orientation & translation from 4x4 row major matrix
	/// translation needs to be stored in the fourth row of
	/// the original row major matrix
	inline void CopyFromRowMajorMatrix44( Scalar *pSrcData );

	/// get data in the form of 4x4 row major matrix
	inline void GetRowMajorMatrix44( Scalar *pDest ) const;

	friend Matrix34 operator*(const Matrix34 & lhs, const Matrix34 & rhs);

};



//===================================================================================
// inline implementations
//===================================================================================

inline Matrix34::Matrix34( const Vector3& pos, const Matrix33& orientation ) : vPosition(pos), matOrient(orientation) {}


inline void Matrix34::Identity()
{
	vPosition = Vector3(0,0,0);
	matOrient = Matrix33Identity();
}


inline void Matrix34::Transform( Vector3& rvDest, const Vector3& rvSrc ) const
{
	rvDest = matOrient * rvSrc;
	rvDest += vPosition;
}


inline void Matrix34::InvTransform( Vector3& rvDest, const Vector3& rvSrc ) const
{
	matOrient.TransformByTranspose( rvDest, ( rvSrc - vPosition ) );
}


inline void Matrix34::GetInverseROT( Matrix34& dest ) const
{
	dest.matOrient = Matrix33Transpose( matOrient );
	dest.vPosition = dest.matOrient * ( -1.0f * vPosition );
}


inline Matrix34 Matrix34::GetInverseROT() const
{
	Matrix34 dest;
	GetInverseROT( dest );
	return dest;
}


// used to calc the combination of two poses
// usually, 
//   lhs == world pose
//   rhs == local pose
inline Matrix34 operator*(const Matrix34 & lhs, const Matrix34 & rhs)
{
	Matrix34 out;

	out.vPosition = lhs.matOrient * rhs.vPosition + lhs.vPosition;
	out.matOrient = lhs.matOrient * rhs.matOrient;
	return out;
}


inline Vector3 operator*(const Matrix34 & pose, const Vector3 & pos)
{
	return pose.matOrient * pos + pose.vPosition;
}


// copy orientation & translation from 4x4 row major matrix
inline void Matrix34::CopyFromRowMajorMatrix44( Scalar *pSrcData )
{
	matOrient.CopyRowMajorMatrix4( pSrcData );
	vPosition.x = *(pSrcData + 4 * 3 + 0);
	vPosition.y = *(pSrcData + 4 * 3 + 1);
	vPosition.z = *(pSrcData + 4 * 3 + 2);
}


inline void Matrix34::GetRowMajorMatrix44( Scalar *pDest ) const
{
	matOrient.GetRowMajorMatrix44( pDest );
	*(pDest + 4 * 3 + 0) = vPosition.x;
	*(pDest + 4 * 3 + 1) = vPosition.y;
	*(pDest + 4 * 3 + 2) = vPosition.z;
}


// global functions

inline Matrix34 Matrix34Identity()
{
	return Matrix34( Vector3(0,0,0), Matrix33Identity() );

}

#endif  /*  __MATRIX34_H__  */
