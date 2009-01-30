#ifndef __CAMERA_H__
#define __CAMERA_H__


#include "3DMath/Matrix34.hpp"
#include "3DMath/Plane.hpp"
#include "3DMath/Sphere.hpp"
#include "3DMath/aabb3.hpp"

#include <math.h>

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

	inline void GetOrientation( Matrix33& matDestOrient ) const;


	inline void GetCameraMatrix(D3DXMATRIX& rMat) const;

	inline void GetProjectionMatrix(D3DXMATRIX& rMat) const;

	inline bool ViewFrustumIntersectsWith( const Sphere& rSphere )  const;

	inline bool ViewFrustumIntersectsWith( const AABB3& raabb ) const;

	void RotateYaw(float fYaw);
	void RotatePitch(float fPitch);

};


// ================================ inline implementations ================================ 

inline void CCamera::GetCameraMatrix(D3DXMATRIX& rMat) const
{

	const Vector3& w = m_vFront;
	const Vector3& u = m_vRight;
	const Vector3& v = m_vUp;
	const Vector3& p = m_vPosition;
	
	rMat._11=u.x; rMat._12=v.x; rMat._13=w.x; rMat._14=0;
	rMat._21=u.y; rMat._22=v.y; rMat._23=w.y; rMat._24=0;
	rMat._31=u.z; rMat._32=v.z; rMat._33=w.z; rMat._34=0;
	rMat._41= -Vec3Dot( u, p );
	rMat._42= -Vec3Dot( v, p );
	rMat._43= -Vec3Dot( w, p );
	rMat._44=1;
}


inline void CCamera::GetProjectionMatrix(D3DXMATRIX& rMat) const
{
	D3DXMatrixPerspectiveFovLH(&rMat, m_fFieldOfView, m_fAspectRatio, m_fNearClip, m_fFarClip);
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
		d = rPlane.GetDistanceFromPoint( rSphere.vCenter ) - rSphere.radius;

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



#endif	/*  __CAMERA_H__  */
