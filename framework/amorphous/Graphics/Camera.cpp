#include "Camera.hpp"
#include "./Support/Log/DefaultLog.hpp"


namespace amorphous
{


Camera::Camera( float fov, float aspectratio, float farclip, float nearclip )
{
	m_fFieldOfView = fov;
	m_fAspectRatio = aspectratio;
	m_fFarClip = farclip;
	m_fNearClip = nearclip;

	SetPose( Matrix34Identity() );

	if( farclip < nearclip )
		LOG_PRINT_WARNING( " - farclip < nearclip." );

	CreateVFTree();
}


void Camera::UpdateLocalVFTreeSidePlanes()
{
	float fovy = m_fFieldOfView;
	float aspect = m_fAspectRatio;

	// top
	m_aLocalVFPlane[2].normal = Vector3( 0, (float)cos(fovy / 2.0), -(float)sin(fovy / 2.0) );
	m_aLocalVFPlane[2].dist   = 0;

	// bottom
	m_aLocalVFPlane[3].normal = m_aLocalVFPlane[2].normal;
	m_aLocalVFPlane[3].normal.y *= (-1.0f);
	m_aLocalVFPlane[3].dist = 0;

	// right
	float z = aspect * (float)sin(fovy / 2.0);
	m_aLocalVFPlane[4].normal = Vector3( (float)sqrt(1.0 - z*z), 0, -z );
	m_aLocalVFPlane[4].dist   = 0;

	// left
	m_aLocalVFPlane[5].normal =  m_aLocalVFPlane[4].normal;
	m_aLocalVFPlane[5].normal.x *= (-1.0f);
	m_aLocalVFPlane[5].dist = 0;
}


void Camera::CreateVFTree()
{

	for(int i=0; i<6; i++)
	{
//		m_WorldBSPTree[i].sPlaneIndex = i;
		m_WorldBSPTree[i].child[BinaryNode::FRONT] = BinaryNode::SUBSPACE_EMPTY;
		m_WorldBSPTree[i].child[BinaryNode::BACK]  = BinaryNode::SUBSPACE_SOLID;
	}

	//The planes of the view frustum are facing inward

	// near clipping plane of the view frustum
	m_aLocalVFPlane[0].normal = Vector3( 0, 0,-1);
	m_aLocalVFPlane[0].dist   = -m_fNearClip;

	// far clipping plane of the view frustum
	m_aLocalVFPlane[1].normal = Vector3( 0, 0, 1);
	m_aLocalVFPlane[1].dist   = m_fFarClip;

	// create left, right, top and bottom planes
    UpdateLocalVFTreeSidePlanes();

	for(int i=0; i<6; i++)
	{	// the camera view frumstum in world coordinate is not axis-aligned
		m_aLocalVFPlane[i].type = 4;
		m_WorldBSPTree[i].plane.type = 4;
	}

}


void Camera::UpdatePosition( const Vector3& vPos, const Vector3& vDir,
							  const Vector3& vRight, const Vector3& vUp)
{
	m_vPosition = vPos;
	m_vFront = vDir;
	m_vRight = vRight;
	m_vUp = vUp;

	UpdateVFTreeForWorldSpace();
}


void Camera::UpdateVFTreeForWorldSpace()
{
	const Vector3& vPosition = m_vPosition;

	Matrix33 matRot;

	matRot.SetColumn( 0, m_vRight );
	matRot.SetColumn( 1, m_vUp );
	matRot.SetColumn( 2, m_vFront );

	for(int i=0; i<6; i++)
	{
//		matRot.TransformByTranspose( m_WorldBSPTree[i].plane.normal, m_aLocalVFPlane[i].normal );
		m_WorldBSPTree[i].plane.normal = matRot * m_aLocalVFPlane[i].normal;

		m_WorldBSPTree[i].plane.dist
			= m_aLocalVFPlane[i].dist
			+ Vec3Dot( m_WorldBSPTree[i].plane.normal, vPosition );
	}

	return;
}


void Camera::RotateYaw(float fYaw)
{
	Matrix33 matRotYaw = Matrix33RotationAxis( fYaw, m_vUp );

	m_vFront = matRotYaw * m_vFront;
	Vec3Normalize( m_vFront, m_vFront );
	m_vRight = matRotYaw * m_vRight;
	Vec3Normalize( m_vRight, m_vRight );

	// TODO: otrho-nomalize 

	UpdateVFTreeForWorldSpace();
}

void Camera::RotatePitch(float fPitch)
{
	Matrix33 matRotPitch = Matrix33RotationAxis( fPitch, m_vRight );

	m_vFront = matRotPitch * m_vFront;
	Vec3Normalize( m_vFront, m_vFront );
	m_vUp = matRotPitch * m_vUp;
	Vec3Normalize( m_vUp, m_vUp );

	// TODO: otrho-nomalize 

	UpdateVFTreeForWorldSpace();
}


} // namespace amorphous
