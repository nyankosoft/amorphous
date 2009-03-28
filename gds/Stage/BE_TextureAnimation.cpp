#include "BE_TextureAnimation.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "ScreenEffectManager.hpp"

#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

using namespace std;


CBE_TextureAnimation::CBE_TextureAnimation()
{
	m_BoundingVolumeType = BVTYPE_AABB;
	m_bNoClip = true;

	m_iDestBlend = D3DBLEND_ONE;

	m_AnimTypeFlag = TA_BILLBOARD;

	m_fExpansionFactor = 1.0f;

	m_aShaderTechHandle[0].SetTechniqueName( "Billboard" );
}


CBE_TextureAnimation::~CBE_TextureAnimation()
{
	ReleaseGraphicsResources();
}


void CBE_TextureAnimation::Init()
{
	m_AnimTexture.Load( m_AnimTextureFilepath );

	// set the local coordinate of the billboard rectangle
	D3DXVECTOR3 vRight = D3DXVECTOR3(1,0,0);
	D3DXVECTOR3 vUp = D3DXVECTOR3(0,1,0);
	float fHalfWidth = this->m_aabb.vMax.x;
	m_avRectangle1[0].vPosition = -vRight * fHalfWidth + vUp * fHalfWidth;
	m_avRectangle1[1].vPosition =  vRight * fHalfWidth + vUp * fHalfWidth;
	m_avRectangle1[2].vPosition =  vRight * fHalfWidth - vUp * fHalfWidth;
	m_avRectangle1[3].vPosition = -vRight * fHalfWidth - vUp * fHalfWidth;

	int i;
	// secondary billboard - shifted slightly forward to avoid z-fighting
	// --- corrected 050505 - this is not needed if we disable z-writing during rendering
	for(i=0; i<4; i++)
		m_avRectangle2[i].vPosition = m_avRectangle1[i].vPosition /* + D3DXVECTOR3(0, 0, 0.02f) */;


	for(i=0; i<4; i++)
		m_avRectangle1[i].color = m_avRectangle2[i].color = 0xFFFFFFFF;

}


void CBE_TextureAnimation::InitCopyEntity( CCopyEntity* pCopyEnt )
{
//	float& rfCurrentAnimationTime = pCopyEnt->f1;
//	rfCurrentAnimationTime = 0;
	// animation itme must be set explicitly before this function

//	pCopyEnt->bUseZSort = true;
	pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;

	float& rfRotAngle = pCopyEnt->f2;
	rfRotAngle = 2.0f * 3.141592f * (float)rand() / (float)RAND_MAX;
}


void CBE_TextureAnimation::Act(CCopyEntity* pCopyEnt)
{
	float& rfCurrentAnimationTime = pCopyEnt->f1;

	if( m_fTotalAnimationTime <= rfCurrentAnimationTime)
///		pCopyEnt-Terminate();
		m_pStage->TerminateEntity( pCopyEnt );
	else
		rfCurrentAnimationTime += m_pStage->GetFrameTime();
}


void CBE_TextureAnimation::Draw(CCopyEntity* pCopyEnt)
{
	float& rfCurrentAnimationTime = pCopyEnt->f1;
	if( rfCurrentAnimationTime < 0 || m_fTotalAnimationTime < rfCurrentAnimationTime )
		return;	// animation hasn't started yet or is already over

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// set the world transformation matrix
	D3DXMATRIX matWorld, matRotZ;

	if( m_AnimTypeFlag & TA_BILLBOARD )
	{
		// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
//		m_pStage->GetBillboardRotationMatrix( matWorld );
		Matrix34 billboard_pose;
		m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
		billboard_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );
	}
	else // i.e. animation of arbitrary direction
	{
		pCopyEnt->GetWorldPose().GetRowMajorMatrix44( (Scalar *)&matWorld );
//		pCopyEnt->GetOrientation( matWorld );
	}


	D3DXVECTOR3& rvPos =  pCopyEnt->Position();	// current position of this billboard
	matWorld._41 =   rvPos.x; matWorld._42 =   rvPos.y;	matWorld._43 =   rvPos.z; matWorld._44 = 1;

	if( /* random rotation is */ true )
	{	// randomly rotates billboards to make them look more diverse
		float& rfRotAngle = pCopyEnt->f2;
		D3DXMatrixRotationZ( &matRotZ, rfRotAngle );
		D3DXMatrixMultiply( &matWorld, &matRotZ, &matWorld );
	}

	// set animation
	int iNumTotalFrames = m_iNumTextureSegments * m_iNumTextureSegments;
	int iCurrentFrame = (int)( (float)iNumTotalFrames * rfCurrentAnimationTime / m_fTotalAnimationTime );
	if( iNumTotalFrames <= iCurrentFrame )
		iCurrentFrame = iNumTotalFrames - 1;

	float fTimePerFrame = m_fTotalAnimationTime / (float)iNumTotalFrames;
	float fCurrentFrameTime = ( rfCurrentAnimationTime - fTimePerFrame * (float)iCurrentFrame ) / fTimePerFrame;
	int i;
	for(i=0; i<4; i++)
	{	// set alpha values for smooth animation between frames
		m_avRectangle1[i].color = D3DCOLOR_ARGB( ((int)(255.0f * (1.0f - fCurrentFrameTime))), 255, 255, 255 );
		m_avRectangle2[i].color = D3DCOLOR_ARGB( ((int)(255.0f * fCurrentFrameTime         )), 255, 255, 255 );
	}

	if( m_fExpansionFactor != 1.0f )
	{
		float fRadius = this->m_aabb.vMax.x * ( 1.0f + fCurrentFrameTime * (m_fExpansionFactor - 1.0f) );
		m_avRectangle2[0].vPosition = m_avRectangle1[0].vPosition = D3DXVECTOR3(-fRadius, fRadius, 0 );
		m_avRectangle2[1].vPosition = m_avRectangle1[1].vPosition = D3DXVECTOR3( fRadius, fRadius, 0 );
		m_avRectangle2[2].vPosition = m_avRectangle1[2].vPosition = D3DXVECTOR3( fRadius,-fRadius, 0 );
		m_avRectangle2[3].vPosition = m_avRectangle1[3].vPosition = D3DXVECTOR3(-fRadius,-fRadius, 0 );
	}

	SetTextureCoord( m_avRectangle1, iCurrentFrame );
	if( iCurrentFrame < iNumTotalFrames - 1 )	// if 'iCurrentFrame' is not the last frame of the texture animation
		SetTextureCoord( m_avRectangle2, iCurrentFrame + 1 );	// set texture for the secondary billboard

	// debug
	if( 0.99f < fCurrentFrameTime && 0xF0FFFFFF < m_avRectangle1[0].color )
		int iUnexpected = 1;


	// use the texture color only
	CShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();
	LPD3DXEFFECT pEffect = NULL;
	UINT cPasses;

	if( pShaderManager &&
		(pEffect = pShaderManager->GetEffect()) )
	{
/*		if( m_pStage->GetScreenEffectManager()->GetEffectFlag() & ScreenEffect::PseudoNightVision )
		{
            pShaderManager->SetTechnique( SHADER_TECH_BILLBOARD_PNV );
		}
		else
		{
//			pShaderManager->SetTechnique( SHADER_TECH_DEFAULT );
			pShaderManager->SetTechnique( SHADER_TECH_BILLBOARD );
		}*/

		pShaderManager->SetTechnique( m_aShaderTechHandle[0] );

		pEffect->Begin( &cPasses, 0 );
		pEffect->BeginPass( 0 );

//		if( m_iDestBlend == D3DBLEND_ONE )			pEffect->BeginPass( SHADER_PASS_ALPHA_BLEND_DEST_ADD );
//		else			pEffect->BeginPass( SHADER_PASS_ALPHA_BLEND_DEST_INVSRCALPHA );

		pShaderManager->SetTexture( 0, m_AnimTexture.GetTexture() );
		pShaderManager->SetWorldTransform( matWorld );

		pEffect->CommitChanges();
	}
	else
	{
		pd3dDev->SetVertexShader( NULL );
//		iFixedFunctionShader = 1;
	}

//	if( iFixedFunctionShader )
//	{
		pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );
		pd3dDev->SetTexture( 0, m_AnimTexture.GetTexture() );

		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//		pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_iDestBlend );

		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		pd3dDev->SetFVF( D3DFVF_TEXTUREVERTEX );
//	}

	// disable z-writing
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	if( iCurrentFrame < iNumTotalFrames - 1 )	// if 'iCurrentFrame' is not the last frame of the texture animation
		pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avRectangle2, sizeof(TEXTUREVERTEX) );


	pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avRectangle1, sizeof(TEXTUREVERTEX) );

	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	if( pEffect )
	{
		pEffect->EndPass();
		pEffect->End();
	}
}


// set texture coordinate for billboard
void CBE_TextureAnimation::SetTextureCoord( TEXTUREVERTEX *pavRectangle, int iCurrentFrame )
{
	float fNumTextureSegments = (float)m_iNumTextureSegments;

	// set texture coordinate
	TEXCOORD2 vTexMin, vTexMax;
	vTexMin.u = (float)(iCurrentFrame % m_iNumTextureSegments) / fNumTextureSegments;
	vTexMin.v = (float)(iCurrentFrame / m_iNumTextureSegments) / fNumTextureSegments;

	float fSegmentWidth = 1.0f / fNumTextureSegments;
	vTexMax.u = vTexMin.u + fSegmentWidth - 0.001f;
	vTexMax.v = vTexMin.v + fSegmentWidth - 0.001f;

	pavRectangle[0].tex = vTexMin;
	pavRectangle[1].tex.u = vTexMax.u;
	pavRectangle[1].tex.v = vTexMin.v;
	pavRectangle[2].tex = vTexMax;
	pavRectangle[3].tex.u = vTexMin.u;
	pavRectangle[3].tex.v = vTexMax.v;
}


bool CBE_TextureAnimation::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string blend_mode, texanim_type;

	if( scanner.TryScanLine( "TEX_FILE",	m_AnimTextureFilepath ) )	return true;
	if( scanner.TryScanLine( "TEX_WIDTH",	m_iTextureWidth ) )				return true;
	if( scanner.TryScanLine( "TEX_SEGS",	m_iNumTextureSegments ) )		return true;
	if( scanner.TryScanLine( "ANIM_TIME",	m_fTotalAnimationTime ) )		return true;
	if( scanner.TryScanLine( "EXPANSION",	m_fExpansionFactor ) )			return true;

	if( scanner.TryScanLine( "BLEND", blend_mode ) )
	{
		if( blend_mode == "ONE")
			m_iDestBlend = D3DBLEND_ONE;
		else if( blend_mode == "INVSRCALPHA" )
			m_iDestBlend = D3DBLEND_INVSRCALPHA;
		else
			m_iDestBlend = D3DBLEND_INVSRCALPHA;
		return true;
	}

	if( scanner.TryScanLine( "TEXANIM_TYPE", texanim_type ) )
	{
		if( texanim_type == "TYPE_NON_BILLBOARD" )
			m_AnimTypeFlag &= ~TA_BILLBOARD;
		return true;
	}

	return false;
}

void CBE_TextureAnimation::ReleaseGraphicsResources()
{
}

void CBE_TextureAnimation::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


void CBE_TextureAnimation::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_AnimTextureFilepath;
	ar & m_fTotalAnimationTime;
	ar & m_iNumTextureSegments;
	ar & m_iTextureWidth;
	ar & m_iDestBlend;
	ar & m_fExpansionFactor;
	ar & m_AnimTypeFlag;
}
