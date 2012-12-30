#include "SkeletonRenderer.hpp"
#include "MotionPrimitive.hpp"
//#include "Support/LineSegmentRenderer.hpp"
#include "gds/3DMath/MatrixConversions.hpp"
#include "gds/Graphics/PrimitiveRenderer.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"


namespace amorphous
{
//#include "gds/Graphics/3DtoScreenSpaceConversions.hpp"

using namespace msynth;


CSkeletonRenderer::CSkeletonRenderer()
:
m_DisplayBoneNames(false)
{/*
	m_Cube.Init();
	//m_Cube.SetUniformColor( 0.9f, 0.9f, 0.9f, 1.0f );*/
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


void CSkeletonRenderer::RenderSkeletonAsLines()
{
	SFloatRGBAColor start_color = SFloatRGBAColor::Aqua(), end_color = SFloatRGBAColor::White();
	for( size_t i=0; i<m_vecLineSegment.size(); i++ )
//		GetPrimitiveRenderer().DrawLine( m_vecLineSegment[i].vStart, m_vecLineSegment[i].vGoal );
		GetPrimitiveRenderer().DrawLine( m_vecLineSegment[i].vStart, m_vecLineSegment[i].vGoal, start_color, end_color );
//		CLineSegmentRenderer::Draw( m_vecLineSegment[i].vStart, m_vecLineSegment[i].vGoal );

/*	Matrix34 dest_transform;
	bone.CalculateWorldTransform( dest_transform, parent_transform, node );

	m_vecBonePose.push_back( dest_transform );

	Vector3 vOffset = bone.GetOffset();
	m_vecBoneSize.push_back( GetBoneSize( vOffset ) );

	const int num_children = bone.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		StoreBonePose_r( bone.GetChild(i), node.GetChildNode(i), dest_transform );
	}*/
}


void CSkeletonRenderer::StoreBonePose_r( const CBone& bone, const CTransformNode& node, const Matrix34& parent_transform, int depth )
{
	Matrix34 dest_transform;
	bone.CalculateWorldTransform( dest_transform, parent_transform, node );

	m_vecBonePose.push_back( dest_transform );

	Vector3 vOffset = bone.GetOffset();
	m_vecBoneSize.push_back( GetBoneSize( vOffset ) );

	if( 0 < depth )
	{
		m_vecLineSegment.push_back( CLineSegment() );
		m_vecLineSegment.back().vStart = parent_transform.vPosition;
		m_vecLineSegment.back().vGoal  = dest_transform.vPosition;
	}

	const int num_children = bone.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		StoreBonePose_r( bone.GetChild(i), node.GetChildNode(i), dest_transform, depth + 1 );
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

		m_vecLineSegment.resize(0);

		Matrix34 root = Matrix34Identity();

		StoreBonePose_r( pSkeleton->GetRootBone(), keyframe.GetRootNode(), root );

		m_vecBoneSize[0] = Vector3(0.1f,0.1f,0.1f);
	}
}


void CSkeletonRenderer::RenderSkeletonAsBoxes()
{
//	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	Matrix44 matWorld, matScaling;

	const size_t num_bones = m_vecBonePose.size();
	for( size_t i=0; i<num_bones; i++ )
	{
		const Matrix34& pose = m_vecBonePose[i];
		const Vector3& vSize = m_vecBoneSize[i];

		matWorld = ToMatrix44( pose );
//		pose.GetRowMajorMatrix44( (float *)&matWorld );

		Matrix44 matScaling = Matrix44Scaling( vSize.x, vSize.y, vSize.z );
/*		D3DXMatrixScaling( &matScaling, vSize.x, vSize.y, vSize.z );
//		D3DXMatrixScaling( &matScaling, 0.05f, 0.05f, 0.05f );
*/
		matWorld = matWorld * matScaling;

		FixedFunctionPipelineManager().SetWorldTransform( matWorld );
//		pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );
/*
//		m_Cube.SetPose( pose );
		m_Cube.Draw();*/
	}
}


void CSkeletonRenderer::Render()
{
//	RenderSkeletonAsBoxes();
	RenderSkeletonAsLines();

	if( m_DisplayBoneNames )
	{
		if( !m_pFont )
			m_pFont = CreateDefaultBuiltinFont();

		if( m_pFont )
		{
			m_pFont->SetFontColor( SFloatRGBAColor::White() );
//			m_pFont->SetFontSize( 16, 32 );
//			m_pFont->DrawText( "Drawing some text for test...", 150, 150 );
		}

		const int num_bones = (int)m_vecLineSegment.size();
		for( int i=0; i<num_bones; i++ )
		{
//			CalculateScreenCoordsFromWorldPosition( camera, m_vecLineSegment[i].vStart );
		}
	}
}


} // namespace amorphous
