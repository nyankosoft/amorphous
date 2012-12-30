#ifndef __CAMERA_H__
#define __CAMERA_H__


#include "gds/3DMath/Matrix34.hpp"
#include "gds/3DMath/Matrix44.hpp"
#include "gds/3DMath/Plane.hpp"
#include "gds/3DMath/Sphere.hpp"
#include "gds/3DMath/AABB3.hpp"
#include <math.h>


namespace amorphous
{


class CCamera
{
	/// position and direction of this camera
	Vector3 m_vPosition;
	Vector3 m_vFront;
	Vector3 m_vRight;
	Vector3 m_vUp;

	/// view frustum of this camera
	float m_fFarClip;
	float m_fNearClip;
	float m_fFieldOfView;      // field of view in the y direction, in radians
	float m_fAspectRatio;

//	SNode_f m_aVFTree[6];
//	SPlane m_aWorldVFPlane[6];

	/// planes that represent view frustum of the camera (local space)
	SPlane m_aLocalVFPlane[6];

	struct BinaryNode
	{
		SPlane plane;
		int child[2];
		
		enum eChild
		{
			FRONT,
			BACK
		};

		enum eSubSpaceContents
		{
			SUBSPACE_EMPTY,
			SUBSPACE_SOLID
		};
	};

	BinaryNode m_WorldBSPTree[6];

	/// updates left, right, top, bottom places of local view frustum tree
	/// called after FOV / aspect ratio is changed
	void UpdateLocalVFTreeSidePlanes();

public:

	CCamera(float fov = 3.141592f / 3.0f, float aspectratio = 4.0f / 3.0f,
				 float farclip = 500.0f, float nearclip = 0.05f);

	void CreateVFTree();

	void UpdateVFTreeForWorldSpace();

	inline void SetFOV( float fFieldOfView );

	inline void SetAspectRatio( float fAspectRatio );

	/// set near/far clip plane
	/// world properties are also updated, so too many calls
	/// of the following two functions may decrease the performance
	inline void SetNearClip( float fNearClip );
	inline void SetFarClip( float fFarClip );

	void UpdatePosition( const Vector3& vPos, const Vector3& vDir,
					     const Vector3& vRight, const Vector3& vUp);

	/// world view frustum tree is not updated after the call
	/// don't forget to call UpdateVFTreeForWorldSpace() after calling this
	inline void SetPose( const Matrix34& rSrcPose );

	inline void SetPosition( const Vector3& vPos ) { m_vPosition = vPos; }

	inline void SetOrientation( const Matrix33& matOrient );

	/// after you change the camera properties by SetX() functions
	/// call this function for the camera setting changes to take effect
//	inline void CommitChanges();
	float GetFarClip() const { return m_fFarClip; }
	float GetNearClip() const { return m_fNearClip; }
	float GetFOV() const { return m_fFieldOfView; }
	float GetAspectRatio() const { return m_fAspectRatio; }

	inline Vector3 GetPosition()       const { return m_vPosition; }
	inline Vector3 GetRightDirection() const { return m_vRight; }
	inline Vector3 GetFrontDirection() const { return m_vFront; }
	inline Vector3 GetUpDirection()    const { return m_vUp; }

	inline void GetPose( Matrix34& rDestPose ) const;

	inline Matrix34 GetPose() const { Matrix34 pose; GetPose(pose); return pose; }

	inline void GetOrientation( Matrix33& matDestOrient ) const;


	inline void GetRowMajorCameraMatrix44( float *pDest ) const;

	inline void GetRowMajorProjectionMatrix44( float *pDest ) const;

	inline void GetCameraMatrix( Matrix44& dest ) const;

	Matrix44 GetCameraMatrix() const { Matrix44 dest; GetCameraMatrix(dest); return dest; }

	inline void GetProjectionMatrix( Matrix44& dest ) const;

	Matrix44 GetProjectionMatrix() const { Matrix44 dest; GetProjectionMatrix(dest); return dest; }

	inline bool ViewFrustumIntersectsWith( const Sphere& rSphere )  const;

	inline bool ViewFrustumIntersectsWith( const AABB3& raabb ) const;

	void RotateYaw(float fYaw);
	void RotatePitch(float fPitch);

};


// ================================ inline implementations ================================ 

inline void CCamera::GetRowMajorCameraMatrix44( float *pDest ) const
{
	const Vector3& w = m_vFront;
	const Vector3& u = m_vRight;
	const Vector3& v = m_vUp;
	const Vector3& p = m_vPosition;

	pDest[4*0+0] = u.x; pDest[4*0+1] = v.x; pDest[4*0+2] = w.x; pDest[4*0+3]=0;
	pDest[4*1+0] = u.y; pDest[4*1+1] = v.y; pDest[4*1+2] = w.y; pDest[4*1+3]=0;
	pDest[4*2+0] = u.z; pDest[4*2+1] = v.z; pDest[4*2+2] = w.z; pDest[4*2+3]=0;
	pDest[4*3+0] = -Vec3Dot( u, p );
	pDest[4*3+1] = -Vec3Dot( v, p );
	pDest[4*3+2] = -Vec3Dot( w, p );
	pDest[4*3+3] = 1;
}


inline void CCamera::GetRowMajorProjectionMatrix44( float *pDest ) const
{
	Matrix44 src;
	GetProjectionMatrix( src );
	src.GetRowMajorMatrix44( pDest );
}


inline void CCamera::GetCameraMatrix( Matrix44& dest ) const
{
	const Vector3& w = m_vFront;
	const Vector3& u = m_vRight;
	const Vector3& v = m_vUp;
	const Vector3& p = m_vPosition;

	dest(0,0) = u.x; dest(0,1) = u.y; dest(0,2) = u.z; dest(0,3) = -Vec3Dot( u, p );
	dest(1,0) = v.x; dest(1,1) = v.y; dest(1,2) = v.z; dest(1,3) = -Vec3Dot( v, p );
	dest(2,0) = w.x; dest(2,1) = w.y; dest(2,2) = w.z; dest(2,3) = -Vec3Dot( w, p );
	dest(3,0) = 0;   dest(3,1) = 0;   dest(3,2) = 0;   dest(3,3) = 1;
}


inline void CCamera::GetProjectionMatrix( Matrix44& dest ) const
{
	dest = Matrix44PerspectiveFoV_LH( m_fFieldOfView, m_fAspectRatio, m_fNearClip, m_fFarClip );
}


inline void CCamera::SetFOV( float fFieldOfView )
{
	m_fFieldOfView = fFieldOfView;

	UpdateLocalVFTreeSidePlanes();
	UpdateVFTreeForWorldSpace();
}


inline void CCamera::SetAspectRatio( float fAspectRatio )
{
	m_fAspectRatio = fAspectRatio;

	UpdateLocalVFTreeSidePlanes();
	UpdateVFTreeForWorldSpace();
}


inline void CCamera::SetNearClip( float fNearClip )
{
	m_fNearClip = fNearClip;

	m_aLocalVFPlane[0].dist   = - m_fNearClip;

	UpdateVFTreeForWorldSpace();
}


inline void CCamera::SetFarClip( float fFarClip )
{
	m_fFarClip = fFarClip;

	m_aLocalVFPlane[1].dist   = m_fFarClip;

	UpdateVFTreeForWorldSpace();
}

/*
inline void CCamera::CommitChanges()
{
	UpdateVFTreeForWorldSpace();
}*/


inline void CCamera::GetOrientation( Matrix33& matDestOrient ) const
{
	matDestOrient.SetColumn( 0, m_vRight );
	matDestOrient.SetColumn( 1, m_vUp );
	matDestOrient.SetColumn( 2, m_vFront );
}


inline void CCamera::SetOrientation( const Matrix33& matOrient )
{
	m_vRight = matOrient.GetColumn(0);
	m_vUp    = matOrient.GetColumn(1);
	m_vFront = matOrient.GetColumn(2);
}


inline void CCamera::GetPose( Matrix34& rDestPose ) const
{
	rDestPose.vPosition = m_vPosition;

	rDestPose.matOrient.SetColumn( 0, m_vRight );
	rDestPose.matOrient.SetColumn( 1, m_vUp );
	rDestPose.matOrient.SetColumn( 2, m_vFront );
}


inline void CCamera::SetPose( const Matrix34& rSrcPose )
{
	m_vRight	= rSrcPose.matOrient.GetColumn(0);
	m_vUp		= rSrcPose.matOrient.GetColumn(1);
	m_vFront	= rSrcPose.matOrient.GetColumn(2);

	m_vPosition = rSrcPose.vPosition;
}


inline bool CCamera::ViewFrustumIntersectsWith( const Sphere& rSphere ) const
{
	float d;

	for( int i=0; i<6; i++ )
	{
		const SPlane& rPlane = m_WorldBSPTree[i].plane;
		d = rPlane.GetDistanceFromPoint( rSphere.center ) - rSphere.radius;

		if(0 < d)
			return false;
	}
	return true;
}


inline bool CCamera::ViewFrustumIntersectsWith( const AABB3& raabb ) const
{
	int i;
	float d;

	Vector3 vCenter, vExtents;
	vCenter = raabb.GetCenterPosition();
	vExtents = raabb.GetExtents();

	for( i=0; i<6; i++ )
	{
		const SPlane& rPlane = m_WorldBSPTree[i].plane;

		d = Vec3Dot( rPlane.normal, vCenter ) - rPlane.dist - raabb.GetRadiusForPlane(rPlane);

		if(0 < d)
			return false;
	}
	return true;
}


} // namespace amorphous



#endif	/*  __CAMERA_H__  */
