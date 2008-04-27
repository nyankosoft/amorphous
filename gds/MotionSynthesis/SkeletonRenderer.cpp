
#include <boost/foreach.hpp>
#include "SkeletonRenderer.h"
#include "3DCommon/Direct3D9.h"

using namespace msynth;


CSkeletonRenderer::CSkeletonRenderer()
{
	m_Cube.Init();
	//m_Cube.SetUniformColor( 0.9f, 0.9f, 0.9f, 1.0f );
}


CSkeletonRenderer::~CSkeletonRenderer()
{
}


inline Vector3 GetBoneSize( const Vector3& vOffset )
{
	Vector3 vOut;
	for( int i=0; i<3; i++ )
	{
		if( vOffset[i] == 0.0f )
			vOut[i] = 0.05f;
		else
			vOut[i] = ((float)fabs(vOffset[i]) * 10.0f) * 0.1f;
	}

	return vOut;
}


void CSkeletonRenderer::StoreBonePose_r( const CBone& bone, const CTransformNode& node, const Matrix34& parent_transform )
{
	Matrix34 dest_transform;
	bone.CalculateWorldTransform( dest_transform, parent_transform, node );

	m_vecBonePose.push_back( dest_transform );

	Vector3 vOffset = bone.GetOffset();
	m_vecBoneSize.push_back( GetBoneSize( vOffset ) );

	const int num_children = bone.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		StoreBonePose_r( bone.GetChild(i), node.GetChildNode(i), dest_transform );
	}
}


void CSkeletonRenderer::UpdateBonePoses( const CKeyframe& keyframe )
{
	if( keyframe.GetRootNode().GetNumChildren() == 0 )
		return;

	boost::shared_ptr<CSkeleton> pSkeleton = m_pSkeleton.lock();

	if( pSkeleton )
	{
		// clear the buffer
		m_vecBonePose.resize(0);
		m_vecBoneSize.resize(0);

		Matrix34 root = Matrix34Identity();

		StoreBonePose_r( pSkeleton->GetRootBone(), keyframe.GetRootNode(), root );

		m_vecBoneSize[0] = Vector3(0.1f,0.1f,0.1f);
	}
}


void CSkeletonRenderer::Render()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	D3DXMATRIX matWorld, matScaling;

	const size_t num_bones = m_vecBonePose.size();
	for( size_t i=0; i<num_bones; i++ )
	{
		const Matrix34& pose = m_vecBonePose[i];
		const Vector3& vSize = m_vecBoneSize[i];

		pose.GetRowMajorMatrix44( (float *)&matWorld );

		D3DXMatrixScaling( &matScaling, vSize.x, vSize.y, vSize.z );
//		D3DXMatrixScaling( &matScaling, 0.05f, 0.05f, 0.05f );

		matWorld = matScaling * matWorld;

		pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

//		m_Cube.SetPose( pose );
		m_Cube.Draw();
	}
}
