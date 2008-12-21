
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "../3DCommon/Direct3D9.h"
#include "../3DCommon/D3DXMeshObject.h"
#include "../3DCommon/Shader/ShaderManager.h"

#include "BE_LaserDot.h"



CBE_LaserDot::CBE_LaserDot()
{
	m_bNoClip = true;
	m_bLighting = false;

	D3DXVECTOR3 avRectVertex[4];

	avRectVertex[0] = D3DXVECTOR3(-0.25f, 0.25f, 0.0f );
	avRectVertex[1] = D3DXVECTOR3( 0.25f, 0.25f, 0.0f );
	avRectVertex[2] = D3DXVECTOR3( 0.25f,-0.25f, 0.0f );
	avRectVertex[3] = D3DXVECTOR3(-0.25f,-0.25f, 0.0f );


	D3DXVECTOR3 avNormal[4];
	memset( avNormal, 0, sizeof(D3DXVECTOR3) * 4 );

	m_LaserDotRect.SetPosition( avRectVertex );
	m_LaserDotRect.SetNormal( avNormal );

	m_LaserDotRect.SetColor( 0xFFFFFFFF );

	m_MeshProperty.m_ShaderTechnique.resize( 1, 1 );
	m_MeshProperty.m_ShaderTechnique( 0, 0 ).SetTechniqueName( "Default" );

}


void CBE_LaserDot::Init()
{
	Init3DModel();

	m_LaserDotTex.Load( m_LaserDotTexFilepath );
}


void CBE_LaserDot::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;
//	pCopyEnt->bUseZSort = true;
}


void CBE_LaserDot::Act(CCopyEntity* pCopyEnt)
{
	CCopyEntity *pLaserEmitter = pCopyEnt->GetParent();

	Vector3& rvLocalMuzzlePos = pCopyEnt->v1;
	Vector3 vWorldMuzzlePos;
	pLaserEmitter->GetWorldPose().Transform( vWorldMuzzlePos, rvLocalMuzzlePos );

/*	D3DXVECTOR3 vWorldMuzzlePos = pLaserEmitter->Position()
		                        + pLaserEmitter->GetRightDirection()     * rvLocalMuzzlePos.x
		                        + pLaserEmitter->GetUpDirection()        * rvLocalMuzzlePos.y
		                        + pLaserEmitter->Direction() * rvLocalMuzzlePos.z;
*/
	Vector3 vGoal  = vWorldMuzzlePos + pLaserEmitter->GetDirection() * 20.0f;

//	Vector3 vStart = pLaserEmitter->Position() + pLaserEmitter->Direction() * 1.5f;
//	Vector3 vGoal  = pLaserEmitter->Position() + pLaserEmitter->Direction() * 20.0f;

	STrace tr;
	tr.pvStart = &vWorldMuzzlePos;
	tr.pvGoal  = &vGoal;
	tr.bvType  = BVTYPE_DOT;
	tr.pSourceEntity = pLaserEmitter;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

	m_pStage->ClipTrace( tr );

	if( tr.fFraction == 1.0f )
	{	// laser is not reaching any surface
		pCopyEnt->s1 |= BE_LASERDOT_OFF_SURFACE;
		return;
	}

	pCopyEnt->s1 &= ~BE_LASERDOT_OFF_SURFACE;

	pCopyEnt->Position()  = tr.vEnd;
	pCopyEnt->touch_plane = tr.plane;

	D3DXVECTOR3 vRight, vUp;
	if( 0.01 < 1.0 - fabs(tr.plane.normal.y) )
	{
		D3DXVec3Cross( &vRight, &D3DXVECTOR3(0,1,0), &tr.plane.normal );
		D3DXVec3Normalize( &vRight, &vRight );
		D3DXVec3Cross( &vUp, &tr.plane.normal, &vRight );
	}
	else
	{	// the normal of the contacted surface is almost along the y-axis (contact surface is probably floor or ground)
		vRight = D3DXVECTOR3(1,0,0);
		D3DXVec3Cross( &vUp, &tr.plane.normal, &vRight );
		D3DXVec3Normalize( &vUp, &vUp );
		D3DXVec3Cross( &vRight, &vUp, &tr.plane.normal );
	}

	pCopyEnt->SetDirection_Right( vRight );
	pCopyEnt->SetDirection_Up( vUp );
	pCopyEnt->SetDirection( tr.plane.normal );
}


void CBE_LaserDot::Draw(CCopyEntity* pCopyEnt)
{
	if( !m_MeshProperty.m_MeshObjectHandle.GetMesh() )
		return;

	if( !(pCopyEnt->s1 & BE_LASERDOT_ON) ||
		pCopyEnt->s1 & BE_LASERDOT_OFF_SURFACE )
		return;	// laser is either swithed off or not reaching a surface

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	D3DXMATRIX matScale, matWorld;

	// increase the size of the dot to improve visibility
	D3DXMatrixScaling( &matScale, 5.0f, 5.0f, 1.0f );

	// set up world transformation matrix to place the laser dot at the contact point
//	D3DXVECTOR3 vPos = tr.vEnd + tr.plane.normal * 0.01f;	// put slightly above the surface to avoid z-fighting
	D3DXVECTOR3 vPos = pCopyEnt->Position() + pCopyEnt->GetDirection() * 0.012f;	// put slightly above the surface to avoid z-fighting
	memcpy( &matWorld._11, &pCopyEnt->GetRightDirection(), sizeof(D3DXVECTOR3) ); matWorld._14 = 0.0f;
	memcpy( &matWorld._21, &pCopyEnt->GetUpDirection(),    sizeof(D3DXVECTOR3) ); matWorld._24 = 0.0f;
	memcpy( &matWorld._31, &pCopyEnt->GetDirection(),      sizeof(D3DXVECTOR3) ); matWorld._34 = 0.0f;
	memcpy( &matWorld._41, &vPos,                          sizeof(D3DXVECTOR3) ); matWorld._44 = 1.0f;

	D3DXMatrixMultiply( &matWorld, &matScale, &matWorld );
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );


	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_ONE );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_ONE );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );


	CShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();
	LPD3DXEFFECT pEffect = NULL;
	UINT cPasses;

	if( pShaderManager &&
		(pEffect = pShaderManager->GetEffect()) )
	{
		pShaderManager->SetWorldTransform( matWorld );

		pShaderManager->SetTechnique( m_MeshProperty.m_ShaderTechnique(0,0) );
		pEffect->Begin( &cPasses, 0 );
		pEffect->BeginPass( 0 );

		pShaderManager->SetTexture( 0, m_LaserDotTex );

		pEffect->CommitChanges();

		m_LaserDotRect.Draw();

		pEffect->EndPass();
		pEffect->End();
	}
	else
	{

		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
//		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

		pd3dDev->SetTexture( 0, m_LaserDotTex.GetTexture() );

		m_LaserDotRect.Draw();
	}
}


bool CBE_LaserDot::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_LaserDot::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_LaserDotTexFilepath;
}
