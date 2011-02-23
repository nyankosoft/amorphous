#include "BE_Decal.hpp"

#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Support/MTRand.hpp"
#include "Support/Profile.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"



CBE_Decal::CBE_Decal()
:
m_fDecalRadius(1.0f),
m_iNumSegments(1)
{
	m_bNoClip = true;
	m_NumTotalUsedDecals = 0;

	m_BoundingVolumeType = BVTYPE_AABB;

	m_bSweepRender = true;

	m_MeshProperty.m_ShaderTechnique.resize( 1, 1 );
	m_MeshProperty.m_ShaderTechnique( 0, 0 ).SetTechniqueName( "Default" );
}


CBE_Decal::~CBE_Decal()
{
}


void CBE_Decal::Init()
{
	m_DecalTexture.Load( m_DecalTextureFilepath );

	// set the local coordinate of the billboard rectangle
	m_DecalRect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	m_DecalRect.SetColor( SFloatRGBAColor::White() );
//	m_DecalRect.SetDestAlphaBlendMode( ??? );

/*	m_DecalRectSet.SetNumRects( NUM_MAX_DECALS );
	m_DecalRectSet.SetColor( SFloatRGBAColor::White() );
//	m_DecalRectSet.SetDestAlphaBlendMode( ??? );
	for(int i=0; i<NUM_MAX_DECALS; i++)
		m_DecalRectSet.SetTextureCoordMinMax( i, TEXCOORD2(0,0), TEXCOORD2(1,1) );
*/
	m_DecalRectsMesh.Init( NUM_MAX_DECALS, VFF::POSITION | VFF::TEXCOORD2_0 );
	for(int i=0; i<NUM_MAX_DECALS; i++)
		m_DecalRectsMesh.SetTextureCoordMinMax( i, TEXCOORD2(0,0), TEXCOORD2(1,1) );

	CMeshMaterial& mat = m_DecalRectsMesh.Material(0);
	mat.TextureDesc.resize( 1 );
	mat.Texture.resize( 1 );
	mat.TextureDesc[0].ResourcePath = m_DecalTextureFilepath;
	mat.Texture[0] = m_DecalTexture;
}


void CBE_Decal::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	Vector3& rvNormal = pCopyEnt->GetDirection();
//	Vector3& rvRight  = pCopyEnt->GetRightDirection();
	Vector3 vRight, vUp;

	// create local space for the decal
	// calculate right(x) and up(y) directions
	if( rvNormal.y == 0 )
	{	// vertical surface - up vector is (0,1,0)
		vUp = Vector3(0,1,0);
		Vec3Cross( vRight, rvNormal, vUp );

	}
	else if( 0.999f < rvNormal.y )
	{	// horizontal floor - up vector is (0,0,1)
		vRight = Vector3(1,0,0);
		vUp = Vector3(0,0,1);
	}
	else if( rvNormal.y < -0.999f )
	{	// horizontal ceiling - up vector is (0,0,-1)
		vRight = Vector3(-1,0,0);
		vUp = Vector3(0,0,1);
	}
	else
	{	// slope
		vUp = Vector3(0,1,0);
		Vec3Cross( vRight, rvNormal, vUp );
		Vec3Normalize( vRight, vRight );
		Vec3Cross( vUp, vRight, rvNormal );
		Vec3Normalize( vUp, vUp );
	}
	pCopyEnt->SetDirection_Right( vRight );
	pCopyEnt->SetDirection_Up( vUp );

	// put the decal slightly above the surface to avoid z-fighting
	pCopyEnt->SetWorldPosition( pCopyEnt->GetWorldPosition() + rvNormal * 0.005f );

	float fDecalRadius = m_fDecalRadius;
	pCopyEnt->v1 =  vRight * fDecalRadius + vUp * fDecalRadius + pCopyEnt->GetWorldPosition();
	pCopyEnt->v2 =  vRight * fDecalRadius - vUp * fDecalRadius + pCopyEnt->GetWorldPosition();
	pCopyEnt->v3 = -vRight * fDecalRadius - vUp * fDecalRadius + pCopyEnt->GetWorldPosition();
	//pCopyEnt->v4 = -rvRight * fDecalRadius - vUp * fDecalRadius + pCopyEnt->GetWorldPosition();

	// define texture coodinates
	const int num_segments = m_iNumSegments;
	int iRandomPatternX = RangedRand( 0, num_segments - 1 );
	int iRandomPatternY = RangedRand( 0, num_segments - 1 );
	pCopyEnt->f1 = (float)iRandomPatternX / (float)num_segments;
	pCopyEnt->f2 = (float)iRandomPatternY / (float)num_segments;

	// check if the decal is actually on some surface
	// TODO: make this unnecesssary
	Vector3 vStart = pCopyEnt->GetWorldPosition();
	Vector3 vGoal = vStart - rvNormal * 0.1f;
	STrace tr;
	tr.bvType = BVTYPE_DOT;
	tr.vStart = vStart;
	tr.vGoal  = vGoal;
	tr.vEnd = vGoal;
	tr.aabb.vMin = tr.aabb.vMax = Vector3(0,0,0);
	tr.SetAABB();

	m_pStage->ClipTrace( tr );

	if( tr.fFraction == 1.0f )
	{
		// turned out to be not on any surface
		// - an invalid position for decal
		m_pStage->TerminateEntity( pCopyEnt );
	}
	else
	{
        pCopyEnt->s1 = m_NumTotalUsedDecals++;
	}
}


void CBE_Decal::Act(CCopyEntity* pCopyEnt)
{
	// Too many decals will decrease FPS, so we delete old ones
	if( NUM_MAX_DECALS < m_NumTotalUsedDecals - pCopyEnt->s1 )
		m_pStage->TerminateEntity( pCopyEnt );
}


void CBE_Decal::Draw(CCopyEntity* pCopyEnt)
{
	m_vecpSweepRenderTable.push_back( pCopyEnt );
	return;
}


void CBE_Decal::UpdateRects()
{
	const int num_entities = (int)m_vecpSweepRenderTable.size();
	float fSegmentLength = 1.0f / m_iNumSegments - 0.001f;
	float u,v;
	CCopyEntity *pEntity;
	for( int i=0; i<num_entities; i++ )
	{
		pEntity = m_vecpSweepRenderTable[i];

		// TODO: check the order of vertex indices (2nd arguments)
//		m_DecalRectSet.SetRectVertexPosition( i, 0, pEntity->v1 );
//		m_DecalRectSet.SetRectVertexPosition( i, 1, pEntity->v2 );
//		m_DecalRectSet.SetRectVertexPosition( i, 2, pEntity->v3 );
//		m_DecalRectSet.SetRectVertexPosition( i, 3, pEntity->v3 + (pEntity->v1 - pEntity->v2) );

		m_DecalRectsMesh.SetRectPosition(
			i,
			pEntity->v1,
			pEntity->v2,
			pEntity->v3,
			pEntity->v3 + (pEntity->v1 - pEntity->v2)
			);

//		Vector3 vNormal = pEntity->GetWorldPose().matOrient.GetColumn( 2 );
//		m_DecalRectsMesh.SetRectNormal( i, vNormal );

		u = pEntity->f1;
		v = pEntity->f2;
//		m_DecalRectSet.SetTextureCoordMinMax( i, TEXCOORD2(u,v), TEXCOORD2(u+fSegmentLength,v+fSegmentLength) );
		m_DecalRectsMesh.SetTextureCoordMinMax( i, TEXCOORD2(u,v), TEXCOORD2(u+fSegmentLength,v+fSegmentLength) );

//		pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avDecalRect, sizeof(TEXTUREVERTEX) );
	}
}


void CBE_Decal::SweepRender()
{
	PROFILE_FUNCTION();

	if( m_vecpSweepRenderTable.empty() )
		return;

	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

	CShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();
	CShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	shader_mgr.SetWorldTransform( Matrix44Identity() );

	// set texture
	shader_mgr.SetTexture( 0, m_DecalTexture );
/*
	// set render states
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
*/
	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );

	UpdateRects();

//	m_DecalRectsMesh.Render( shader_mgr, (int)m_vecpSweepRenderTable.size() );
	m_DecalRectsMesh.Render( shader_mgr );

	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
}


bool CBE_Decal::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( scanner.TryScanLine( "DCL_RADIUS",   m_fDecalRadius ) ) return true;

	if( scanner.TryScanLine( "TEX_FILENAME", m_DecalTextureFilepath ) ) return true;

	if( scanner.TryScanLine( "NUM_SEGS",     m_iNumSegments ) ) return true;

	return false;
}


void CBE_Decal::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_DecalTextureFilepath;
	ar & m_fDecalRadius;
	ar & m_iNumSegments;
}
