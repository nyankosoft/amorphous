#include "Camera.hpp"
#include "./Support/Log/DefaultLog.hpp"


CCamera::CCamera( float fov, float aspectratio, float farclip, float nearclip )
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


void CCamera::UpdateLocalVFTreeSidePlanes()
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


void CCamera::CreateVFTree()
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


void CCamera::UpdatePosition( const Vector3& vPos, const Vector3& vDir,
							  const Vector3& vRight, const Vector3& vUp)
{
	m_vPosition = vPos;
	m_vFront = vDir;
	m_vRight = vRight;
	m_vUp = vUp;

	UpdateVFTreeForWorldSpace();
}


void CCamera::UpdateVFTreeForWorldSpace()
{
/*	D3DXMATRIX mat;

	Vector3& vRight = m_vRight;	// local direction x
	Vector3& vUp = m_vUp;		// local direction y (up)
	Vector3& vFront = m_vFront;	// local direction z (forward)
	Vector3& vPosition = m_vPosition;

	mat._11=vRight.x; mat._12=vRight.y; mat._13=vRight.z; mat._14=0;
	mat._21=vUp.x;    mat._22=vUp.y;    mat._23=vUp.z;    mat._24=0;
	mat._31=vFront.x; mat._32=vFront.y; mat._33=vFront.z; mat._34=0;
	mat._41=0;		  mat._42=0;		mat._43=0;		  mat._44=1;

	SPlane* pWorldVFPlane = m_aWorldVFPlane;
	SPlane* pOrigVFPlane = m_aLocalVFPlane;
	char i;
	for(i=0; i<6; i++)
	{
		D3DXVec3TransformCoord( &pWorldVFPlane[i].normal, &pOrigVFPlane[i].normal, &mat );
		pWorldVFPlane[i].dist = pOrigVFPlane[i].dist
			+ D3DXVec3Dot( &pWorldVFPlane[i].normal, &vPosition );
	}*/

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


void CCamera::RotateYaw(float fYaw)
{
/*	D3DXMATRIX matRotYaw;
	D3DXMatrixRotationAxis( &matRotYaw, &m_vUp, fYaw );

	// rotate right direction and front direction
	D3DXVec3TransformCoord( &m_vFront, &m_vFront, &matRotYaw );
	D3DXVec3Normalize( &m_vFront, &m_vFront );
	D3DXVec3TransformCoord( &m_vRight, &m_vRight, &matRotYaw );
	D3DXVec3Normalize( &m_vRight, &m_vRight );*/

	Matrix33 matRotYaw = Matrix33RotationAxis( fYaw, m_vUp );

	m_vFront = matRotYaw * m_vFront;
	Vec3Normalize( m_vFront, m_vFront );
	m_vRight = matRotYaw * m_vRight;
	Vec3Normalize( m_vRight, m_vRight );

	// TODO: otrho-nomalize 

	UpdateVFTreeForWorldSpace();

}

void CCamera::RotatePitch(float fPitch)
{
/*	D3DXMATRIX matRotPitch;
	D3DXMatrixRotationAxis( &matRotPitch, &m_vRight, fPitch );

	// rotate front direction up direction
	D3DXVec3TransformCoord( &m_vFront, &m_vFront, &matRotPitch );
	D3DXVec3Normalize( &m_vFront, &m_vFront );
	D3DXVec3TransformCoord( &m_vUp, &m_vUp, &matRotPitch );
	D3DXVec3Normalize( &m_vUp, &m_vUp );*/

	Matrix33 matRotPitch = Matrix33RotationAxis( fPitch, m_vRight );

	m_vFront = matRotPitch * m_vFront;
	Vec3Normalize( m_vFront, m_vFront );
	m_vUp = matRotPitch * m_vUp;
	Vec3Normalize( m_vUp, m_vUp );

	// TODO: otrho-nomalize 

	UpdateVFTreeForWorldSpace();

}

