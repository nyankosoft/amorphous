#ifndef __Transform_HPP__
#define __Transform_HPP__


#include "Quaternion.hpp"
#include "Matrix34.hpp"


class Transform
{
public:

	/// rotation
	Quaternion qRotation;

	/// translation
	Vector3 vTranslation;

public:

	inline Transform() {}

	inline Transform( const Quaternion& _qRotation, const Vector3& _vTranslation );
	inline Transform( const Matrix34& src );

	inline void SetIdentity();

	/// apply the affine transformation to a three element vector
	inline void ApplyTransform( Vector3& rvDest, const Vector3& rvSrc ) const;
/*
	/// apply the inverse transformation to a three element vector
	inline void InvTransform( Vector3& rvDest, const Vector3& rvSrc ) const;

	/// get the inverse -- assumes that the orientation matrix is orthogonal
	inline void GetInverseROT( Transform& dest ) const;

	/// Non-peformance friendly version of GetInverseROT()
	inline Transform GetInverseROT() const;
*/
	/// copy orientation & translation from the pose stored in Matrix34
	inline void FromMatrix34( const Matrix34& src_pose );

	/// extract the transform as the pose in Matrix34
	inline void ToMatrix34( Matrix34& dest_pose ) const;

	/// extract the transform as the pose in Matrix34
	inline Matrix34 ToMatrix34() const;

	inline bool operator==( const Transform& rhs ) const;

	bool operator!=( const Transform& rhs ) const { return !(*this == rhs); }

	inline Vector3 operator*( const Vector3 &rhs ) const;

//	friend Transform operator*(const Transform & lhs, const Transform & rhs);
};



//===================================================================================
// inline implementations
//===================================================================================

inline Transform::Transform( const Quaternion& _qRotation, const Vector3& _vTranslation ) : qRotation(_qRotation), vTranslation(_vTranslation) {}


inline Transform::Transform( const Matrix34& src )
:
qRotation(src.matOrient),
vTranslation(src.vPosition)
{
}


inline void Transform::SetIdentity()
{
	vTranslation = Vector3(0,0,0);
	qRotation    = Quaternion(Matrix33Identity());
}


inline void Transform::ApplyTransform( Vector3& rvDest, const Vector3& rvSrc ) const
{
	rvDest = qRotation.Rotate( rvSrc );
	rvDest += vTranslation;
}

/*
inline void Transform::InvTransform( Vector3& rvDest, const Vector3& rvSrc ) const
{
	rotation.TransformByTranspose( rvDest, ( rvSrc - translation ) );
}


inline void Transform::GetInverseROT( Transform& dest ) const
{
	dest.qRotation = QuaternionTranspose( rotation );
	dest.vTranslation = dest.qRotation * ( -1.0f * translation );
}


inline Transform Transform::GetInverseROT() const
{
	Transform dest;
	GetInverseROT( dest );
	return dest;
}
*/

// copy orientation & translation from 4x4 row major matrix
inline void Transform::FromMatrix34( const Matrix34& src_pose )
{
	vTranslation = src_pose.vPosition;
	qRotation.FromRotationMatrix( src_pose.matOrient );
}


inline void Transform::ToMatrix34( Matrix34& dest_pose ) const
{
	dest_pose.vPosition = vTranslation;
	qRotation.ToRotationMatrix( dest_pose.matOrient );
}


inline Matrix34 Transform::ToMatrix34() const
{
	Matrix34 dest;
	ToMatrix34( dest );
	return dest;
}


inline bool Transform::operator==( const Transform& rhs ) const
{
	return ( (vTranslation == rhs.vTranslation) && (qRotation == rhs.qRotation) );
}


inline Vector3 Transform::operator*( const Vector3 &rhs ) const
{
//	return qRotation.ToMatrix33() * rhs + vTranslation;
	return qRotation.Rotate( rhs ) + vTranslation;
}



//===================================================================================
// global operators
//===================================================================================

// used to calc the combination of two poses
// usually, 
//   lhs == world pose
//   rhs == local pose
inline Transform operator*(const Transform & lhs, const Transform & rhs)
{
	Transform out;

//	out.vTranslation = lhs.qRotation * rhs.vTranslation + lhs.vTranslation;
	out.vTranslation = lhs.qRotation.Rotate( rhs.vTranslation ) + lhs.vTranslation;
	out.qRotation = lhs.qRotation * rhs.qRotation;
	return out;
}



//===================================================================================
// global functions
//===================================================================================

inline Transform IdentityTransform()
{
	return Transform( Quaternion(0,0,0,1), Vector3(0,0,0) );

}



#endif  /*  __Transform_HPP__  */
