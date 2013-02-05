#include "BE_LaserDot.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "3DMath/3DGameMath.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/TextureStage.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"


namespace amorphous
{


CBE_LaserDot::CBE_LaserDot()
{
	m_bNoClip = true;

	SetLighting( false );

	Vector3 avRectVertex[4];

	avRectVertex[0] = Vector3(-0.25f, 0.25f, 0.0f );
	avRectVertex[1] = Vector3( 0.25f, 0.25f, 0.0f );
	avRectVertex[2] = Vector3( 0.25f,-0.25f, 0.0f );
	avRectVertex[3] = Vector3(-0.25f,-0.25f, 0.0f );

	m_LaserDotRect.SetPositions( avRectVertex );//, 4 );

	m_LaserDotRect.SetNormals( Vector3(0,0,0) );

	m_LaserDotRect.SetDiffuseColors( SFloatRGBAColor::White() );

	m_MeshProperty.m_ShaderTechnique.resize( 1, 1 );
	m_MeshProperty.m_ShaderTechnique( 0, 0 ).SetTechniqueName( "Default" );

}


void CBE_LaserDot::Init()
{
	Init3DModel();

	m_LaserDotTex.Load( m_LaserDotTexFilepath );

//	CRectMeshGenerator rect_mesh_generator;
//	rect_mesh_generator.Generate();
//	m_LaserDotRect.LoadFromArchive( rect_mesh_generator.GetMeshArchive() );

//	m_LaserDotRect.SetTexture( 0, m_LaserDotTex );
}


void CBE_LaserDot::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->RaiseEntityFlags( BETYPE_USE_ZSORT );
//	pCopyEnt->bUseZSort = true;
}


void CBE_LaserDot::Act(CCopyEntity* pCopyEnt)
{
	CCopyEntity *pLaserEmitter = pCopyEnt->GetParent();

	Vector3& rvLocalMuzzlePos = pCopyEnt->v1;
	Vector3 vWorldMuzzlePos;
	pLaserEmitter->GetWorldPose().Transform( vWorldMuzzlePos, rvLocalMuzzlePos );

/*	Vector3 vWorldMuzzlePos = pLaserEmitter->Position()
		                        + pLaserEmitter->GetRightDirection()     * rvLocalMuzzlePos.x
		                        + pLaserEmitter->GetUpDirection()        * rvLocalMuzzlePos.y
		                        + pLaserEmitter->Direction() * rvLocalMuzzlePos.z;
*/
	Vector3 vGoal  = vWorldMuzzlePos + pLaserEmitter->GetDirection() * 20.0f;

//	Vector3 vStart = pLaserEmitter->Position() + pLaserEmitter->Direction() * 1.5f;
//	Vector3 vGoal  = pLaserEmitter->Position() + pLaserEmitter->Direction() * 20.0f;

	STrace tr;
	tr.vStart = vWorldMuzzlePos;
	tr.vGoal  = vGoal;
	tr.bvType  = BVTYPE_DOT;
	tr.pSourceEntity = pLaserEmitter;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

	m_pStage->ClipTrace( tr );

	if( tr.fFraction == 1.0f )
	{
		// laser is not reaching any surface
		pCopyEnt->s1 |= BE_LASERDOT_OFF_SURFACE;
		return;
	}

	pCopyEnt->s1 &= ~BE_LASERDOT_OFF_SURFACE;

	pCopyEnt->SetWorldPosition( tr.vEnd );
	pCopyEnt->touch_plane = tr.plane;

	Matrix33 matOrient = CreateOrientFromFwdDir( tr.plane.normal );
	matOrient.Orthonormalize();

	pCopyEnt->SetWorldOrientation( matOrient );
}


void CBE_LaserDot::Draw(CCopyEntity* pCopyEnt)
{
	if( !m_MeshProperty.m_MeshObjectHandle.GetMesh() )
		return;

	if( !(pCopyEnt->s1 & BE_LASERDOT_ON) ||
		pCopyEnt->s1 & BE_LASERDOT_OFF_SURFACE )
		return;	// laser is either swithed off or not reaching a surface

	// increase the size of the dot to improve visibility
//	D3DXMatrixScaling( &matScale, 5.0f, 5.0f, 1.0f );
	Matrix44 matScale( Matrix44Scaling( 5.0f, 5.0f, 1.0f ) );

	// set up world transformation matrix to place the laser dot at the contact point
//	Vector3 vPos = tr.vEnd + tr.plane.normal * 0.01f;	// put slightly above the surface to avoid z-fighting
	Vector3 vPos = pCopyEnt->GetWorldPosition() + pCopyEnt->GetDirection() * 0.012f;	// put slightly above the surface to avoid z-fighting

	Matrix34 rect_pose( Matrix34Identity() );
	pCopyEnt->GetWorldOrientation( rect_pose.matOrient );
	rect_pose.vPosition = vPos;

	Matrix44 matWorld;
	ToMatrix44( rect_pose, matWorld );

	matWorld = matWorld * matScale;

	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::One );
	GraphicsDevice().SetDestBlendMode(   AlphaBlend::One );
//	GraphicsDevice().SetSourceBlendMode( AlphaBlend::SrcAlpha );
//	GraphicsDevice().SetDestBlendMode(  AlphaBlend::InvSrcAlpha );

	ShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();
	if( pShaderManager )
	{
		pShaderManager->SetWorldTransform( matWorld );

		pShaderManager->SetTechnique( m_MeshProperty.m_ShaderTechnique(0,0) );

		pShaderManager->SetTexture( 0, m_LaserDotTex );

//		m_LaserDotRect.Render( *pShaderManager );
	}
	else
	{
		FixedFunctionPipelineManager().SetWorldTransform( matWorld );

		CTextureStage ts0, ts1;
		ts0.ColorArg0 = TexStageArg::DIFFUSE;
		ts0.ColorArg1 = TexStageArg::TEXTURE;
		ts0.ColorOp   = TexStageOp::MODULATE;
		ts0.AlphaArg0 = TexStageArg::DIFFUSE;
		ts0.AlphaArg1 = TexStageArg::TEXTURE;
		ts0.AlphaOp   = TexStageOp::MODULATE;

		ts1.ColorOp   = TexStageOp::DISABLE;
		ts1.AlphaOp   = TexStageOp::DISABLE;

		GraphicsDevice().SetTextureStageParams( 0, ts0 );
		GraphicsDevice().SetTextureStageParams( 1, ts1 );
/*
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
//		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
*/
		FixedFunctionPipelineManager().SetTexture( 0, m_LaserDotTex );

//		m_LaserDotRect.Render();
	}
}


bool CBE_LaserDot::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_LaserDot::Serialize( IArchive& ar, const unsigned int version )
{
	BaseEntity::Serialize( ar, version );

	ar & m_LaserDotTexFilepath;
}


} // namespace amorphous
