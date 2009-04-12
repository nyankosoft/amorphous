#ifndef __Matrix23_H__
#define __Matrix23_H__


#include "Vector2.hpp"
#include "Matrix22.hpp"


class Matrix23
{
public:

	/// rotation
	Matrix22 matOrient;

	/// translation
	Vector2 vPosition;

public:

	inline Matrix23() {}

	inline Matrix23( const Vector2& pos, const Matrix22& orientation );

	inline void Identity();

	inline void Default() { Identity(); }

	/// apply the affine transformation to a three element vector
	inline void Transform( Vector2& rvDest, const Vector2& rvSrc ) const;

	/// apply the inverse transformation to a three element vector
	inline void InvTransform( Vector2& rvDest, const Vector2& rvSrc ) const;

	/// get the inverse -- assumes that the orientation matrix is orthogonal
	inline void GetInverseROT( Matrix23& dest ) const;

	/// Non-peformance friendly version of GetInverseROT()
	inline Matrix23 GetInverseROT() const;

	/// copy orientation & translation from 4x4 row major matrix
	/// translation needs to be stored in the fourth row of
	/// the original row major matrix
//	inline void CopyFromRowMajorMatrix44( Scalar *pSrcData );

	/// get data in the form of 4x4 row major matrix
//	inline void GetRowMajorMatrix44( Scalar *pDest ) const;

	friend Matrix23 operator*(const Matrix23 & lhs, const Matrix23 & rhs);

};



//===================================================================================
// inline implementations
//===================================================================================

inline Matrix23::Matrix23( const Vector2& pos, const Matrix22& orientation )
:
vPosition(pos),
matOrient(orientation) {}


inline void Matrix23::Identity()
{
	vPosition = Vector2(0,0);
	matOrient = Matrix22Identity();
}


inline void Matrix23::Transform( Vector2& rvDest, const Vector2& rvSrc ) const
{
	rvDest = matOrient * rvSrc;
	rvDest += vPosition;
}


inline void Matrix23::InvTransform( Vector2& rvDest, const Vector2& rvSrc ) const
{
	matOrient.TransformByTranspose( rvDest, ( rvSrc - vPosition ) );
}


inline void Matrix23::GetInverseROT( Matrix23& dest ) const
{
	dest.matOrient = Matrix22Transpose( matOrient );
	dest.vPosition = dest.matOrient * ( -1.0f * vPosition );
}


inline Matrix23 Matrix23::GetInverseROT() const
{
	Matrix23 dest;
	GetInverseROT( dest );
	return dest;
}


// used to calc the combination of two poses
// usually, 
//   lhs == world pose
//   rhs == local pose
inline Matrix23 operator*(const Matrix23 & lhs, const Matrix23 & rhs)
{
	Matrix23 out;

	out.vPosition = lhs.matOrient * rhs.vPosition + lhs.vPosition;
	out.matOrient = lhs.matOrient * rhs.matOrient;
	return out;
}


inline Vector2 operator*(const Matrix23 & pose, const Vector2 & pos)
{
	return pose.matOrient * pos + pose.vPosition;
}


// global functions

inline Matrix23 Matrix23Identity()
{
	return Matrix23( Vector2(0,0), Matrix22Identity() );

}

#endif  /*  __Matrix23_H__  */
