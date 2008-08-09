#include "BE_Decal.h"

#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"

#include "3DCommon/FVF_TextureVertex.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "GameCommon/MTRand.h"

#include "Support/Profile.h"

//int CBE_Decal::ms_iNumTotalUsedDecals = 0;


CBE_Decal::CBE_Decal()
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
	m_avDecalRect[0].tex = TEXCOORD2(0,0);
	m_avDecalRect[1].tex = TEXCOORD2(1,0);
	m_avDecalRect[2].tex = TEXCOORD2(1,1);
	m_avDecalRect[3].tex = TEXCOORD2(0,1);

	for(int i=0; i<4; i++)
		m_avDecalRect[i].color = 0xFFFFFFFF;
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
	pCopyEnt->Position() = pCopyEnt->Position() + rvNormal * 0.005f;

	float fDecalRadius = m_fDecalRadius;
	pCopyEnt->v1 =  vRight * fDecalRadius + vUp * fDecalRadius + pCopyEnt->Position();
	pCopyEnt->v2 =  vRight * fDecalRadius - vUp * fDecalRadius + pCopyEnt->Position();
	pCopyEnt->v3 = -vRight * fDecalRadius - vUp * fDecalRadius + pCopyEnt->Position();
	//pCopyEnt->v4 = -rvRight * fDecalRadius - vUp * fDecalRadius + pCopyEnt->Position();

	// define texture coodinates
	const int num_segments = m_iNumSegments;
	int iRandomPatternX = RangedRand( 0, num_segments - 1 );
	int iRandomPatternY = RangedRand( 0, num_segments - 1 );
	pCopyEnt->f1 = (float)iRandomPatternX / (float)num_segments;
	pCopyEnt->f2 = (float)iRandomPatternY / (float)num_segments;

	// check if the decal is actually on some surface
	// TODO: make this unnecesssary
	Vector3 vStart = pCopyEnt->Position();
	Vector3 vGoal = vStart - rvNormal * 0.1f;
	STrace tr;
	tr.bvType = BVTYPE_DOT;
	tr.pvStart = &vStart;
	tr.pvGoal  = &vGoal;
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

/**
	m_avDecalRect[0].vPosition = pCopyEnt->v1;
	m_avDecalRect[1].vPosition = pCopyEnt->v2;
	m_avDecalRect[2].vPosition = pCopyEnt->v3;
	m_avDecalRect[3].vPosition = pCopyEnt->v3 + (pCopyEnt->v1 - pCopyEnt->v2);

	float fSegmentLength = 1.0f / m_iNumSegments - 0.001f;
	float u,v;
	u = pCopyEnt->f1;
	v = pCopyEnt->f2;
	m_avDecalRect[0].vTexture = D3DXVECTOR2( u,                  v );
	m_avDecalRect[1].vTexture = D3DXVECTOR2( u + fSegmentLength, v );
	m_avDecalRect[2].vTexture = D3DXVECTOR2( u + fSegmentLength, v + fSegmentLength );
	m_avDecalRect[3].vTexture = D3DXVECTOR2( u,                  v + fSegmentLength );

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	// use the texture color only
	if( CDIRECTX9.m_pEffect )
	{
		CDIRECTX9.m_pEffect->SetTexture( "Texture0", m_pDecalTexture );
		CDIRECTX9.m_pEffect->SetMatrix( "World", &matWorld );
		CDIRECTX9.m_pEffect->CommitChanges();
	}
	else
		pd3dDev->SetTexture( 0, m_pDecalTexture );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetFVF( D3DFVF_TEXTUREVERTEX );
	pd3dDev->DrawPrimitiveUP(
		D3DPT_TRIANGLEFAN, 2,
		m_avDecalRect, sizeof(TEXTUREVERTEX) );
**/
}


void CBE_Decal::SweepRender()
{
//	ProfileBegin( "Decal" );

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	CShaderManager *pShaderManager = NULL;
	LPD3DXEFFECT pEffect = NULL;
	UINT cPasses;

	if( (pShaderManager = CShader::Get()->GetCurrentShaderManager()) &&
		(pEffect = pShaderManager->GetEffect()) )
	{
		pShaderManager->SetTechnique( m_MeshProperty.m_ShaderTechnique(0,0) );
		pEffect->Begin( &cPasses, 0 );
		pEffect->BeginPass( 0 );
	}
	else
		pd3dDev->SetVertexShader( NULL );

	// set texture
	pd3dDev->SetTexture( 0, m_DecalTexture.GetTexture() );

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

	pd3dDev->SetFVF( D3DFVF_TEXTUREVERTEX );
	
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	size_t i, num_entities = m_vecpSweepRenderTable.size();
	float fSegmentLength = 1.0f / m_iNumSegments - 0.001f;
	float u,v;
	CCopyEntity *pEntity;
	for( i=0; i<num_entities; i++ )
	{
		pEntity = m_vecpSweepRenderTable[i];

		m_avDecalRect[0].vPosition = pEntity->v1;
		m_avDecalRect[1].vPosition = pEntity->v2;
		m_avDecalRect[2].vPosition = pEntity->v3;
		m_avDecalRect[3].vPosition = pEntity->v3 + (pEntity->v1 - pEntity->v2);

		u = pEntity->f1;
		v = pEntity->f2;
		m_avDecalRect[0].tex = TEXCOORD2( u,                  v );
		m_avDecalRect[1].tex = TEXCOORD2( u + fSegmentLength, v );
		m_avDecalRect[2].tex = TEXCOORD2( u + fSegmentLength, v + fSegmentLength );
		m_avDecalRect[3].tex = TEXCOORD2( u,                  v + fSegmentLength );

		pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avDecalRect, sizeof(TEXTUREVERTEX) );
	}

	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	if( pEffect )
	{
		pEffect->EndPass();
		pEffect->End();
	}

//	ProfileEnd( "Decal" );
}


bool CBE_Decal::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( scanner.TryScanLine( "DCL_RADIUS",   m_fDecalRadius ) ) return true;

	if( scanner.TryScanLine( "TEX_FILENAME", m_DecalTextureFilepath ) ) return true;

	if( scanner.TryScanLine( "NUM_SEGS",     m_iNumSegments ) ) return true;

	return false;
}


void CBE_Decal::ReleaseGraphicsResources()
{
	CBaseEntity::ReleaseGraphicsResources();
}
	

void CBE_Decal::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	CBaseEntity::LoadGraphicsResources( rParam );
}


void CBE_Decal::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_DecalTextureFilepath;
	ar & m_fDecalRadius;
	ar & m_iNumSegments;
}
