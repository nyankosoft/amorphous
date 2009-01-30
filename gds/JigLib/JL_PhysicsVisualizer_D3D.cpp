#include "JL_PhysicsVisualizer_D3D.hpp"

#include "JL_PhysicsManager.hpp"

#include "3DMath/Vector3.hpp"
#include "3DMath/aabb3.hpp"

#include "Graphics/Direct3D9.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

//#include "Support/memory_helpers.hpp"
#include "Support/LineSegmentRenderer.hpp"

#include "Graphics/UnitCube.hpp"
#include "Support/SafeDelete.hpp"

#include "Support/msgbox.hpp"

#include <d3d9.h>
#include <d3dx9.h>


CJL_PhysicsVisualizer_D3D::CJL_PhysicsVisualizer_D3D( CJL_PhysicsManager *pPhysicsManager )
{
	m_pPhysicsManager = pPhysicsManager;

#ifdef _DEBUG
	m_RenderStateFlag = JL_VIS_CONTACT_POINTS | JL_VIS_CONTACT_NORMALS | JL_VIS_AABB;
#else
	m_RenderStateFlag = 0;
#endif

	m_pUnitCube = new CUnitCube;
	m_pUnitCube->Init();
	m_pUnitCube->SetUniformColor( 1.0f, 1.0f, 1.0f );
	m_pUnitCube->SetRenderMode( CUnitCube::RS_WIREFRAME );
}


CJL_PhysicsVisualizer_D3D::~CJL_PhysicsVisualizer_D3D()
{
//	Release();
//	MsgBox( "CJL_PhysicsVisualizer_D3D::dtor() - releaseing the unit cube..." );
	SafeDelete( m_pUnitCube );
}


//void CJL_PhysicsVisualizer_D3D::Init(){}
//void CJL_PhysicsVisualizer_D3D::Release(){}


void CJL_PhysicsVisualizer_D3D::RenderVisualInfo()
{
 	DIRECT3D9.GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	CShaderManager *pShaderMgr = CShader::Get()->GetCurrentShaderManager();
	LPD3DXEFFECT pEffect = NULL;
	if( pShaderMgr )
	{
		pEffect = pShaderMgr->GetEffect();

		UINT cPasses;
		pEffect->SetTechnique( "Default" );	// no vertex & pixel shader
		pEffect->Begin( &cPasses, 0 );
		pEffect->BeginPass(0);	// assumes that the technique has only one pass

//		pEffect->SetMatrix( "World", &matWorld );
		pEffect->CommitChanges();
	}


	// render visual debug info
	DisplayContactPoints();

	DisplayActorInfo();


	if( pEffect )
	{
		pEffect->EndPass();
		pEffect->End();
	}
}


void CJL_PhysicsVisualizer_D3D::DisplayContactPoints()
{
//	MsgBox( "CJL_PhysicsVisualizer_D3D::DisplayContactPoints()" );

	int i;

	D3DXVECTOR3 avLineSegment[6], vContactPoint;
	D3DXVECTOR3 avContactNormal[2];

#ifndef USE_COMBINED_COLLISION_POINTS_INFO  ///////////////////////////////////////////////////

//	vector<CJL_ContactInfo>& collision_buffer = m_pPhysicsManager->GetCollisionInfo();
	vector<CJL_ContactInfo> collision_buffer;

 	int j, iNumContacts = collision_buffer.size();

	for( i=0; i<iNumContacts; i++ )
	{
		if( m_RenderStateFlag & JL_VIS_CONTACT_POINTS )
		{	// render contact point
			vContactPoint = collision_buffer[i].vContactPoint;
			avLineSegment[0] = vContactPoint - D3DXVECTOR3( 0.2f,  0.0f,  0.0f);
			avLineSegment[1] = vContactPoint + D3DXVECTOR3( 0.2f,  0.0f,  0.0f);
			avLineSegment[2] = vContactPoint - D3DXVECTOR3( 0.0f,  0.2f,  0.0f);
			avLineSegment[3] = vContactPoint + D3DXVECTOR3( 0.0f,  0.2f,  0.0f);
			avLineSegment[4] = vContactPoint - D3DXVECTOR3( 0.0f,  0.0f,  0.2f);
			avLineSegment[5] = vContactPoint + D3DXVECTOR3( 0.0f,  0.0f,  0.2f);
			for( j=0; j<6; j+=2 )
			{
				CLineSegmentRenderer::Draw(avLineSegment[j], avLineSegment[j+1], 0xFF3030FF);
			}
		}

		if( m_RenderStateFlag & JL_VIS_CONTACT_NORMALS )
		{	// render contact normal
			avContactNormal[0] = vContactPoint;
			avContactNormal[1] = vContactPoint + collision_buffer[i].vNormal * 0.3f;
			CLineSegmentRenderer::Draw( avContactNormal[0], avContactNormal[1], 0xFFFF3030);
		}
	}

#else

	vector<CJL_CollisionInfo>& collision_buffer = m_pPhysicsManager->GetCollisionInfo();

 	int j, k, num_colls = collision_buffer.size();
	D3DXVECTOR3 vContactPos, vNormal;

	for( i=0; i<num_colls; i++ )
	{
		for( j=0; j<collision_buffer[i].m_vecPointInfo.size(); j++ )
		{
			CJL_CollPointInfo& pt_info = collision_buffer[i].m_vecPointInfo[j];

			if( m_RenderStateFlag & JL_VIS_CONTACT_POINTS )
			{	// render contact point
				vContactPos = pt_info.vContactPosition;
				avLineSegment[0] = vContactPos - D3DXVECTOR3( 0.2f,  0.0f,  0.0f);
				avLineSegment[1] = vContactPos + D3DXVECTOR3( 0.2f,  0.0f,  0.0f);
				avLineSegment[2] = vContactPos - D3DXVECTOR3( 0.0f,  0.2f,  0.0f);
				avLineSegment[3] = vContactPos + D3DXVECTOR3( 0.0f,  0.2f,  0.0f);
				avLineSegment[4] = vContactPos - D3DXVECTOR3( 0.0f,  0.0f,  0.2f);
				avLineSegment[5] = vContactPos + D3DXVECTOR3( 0.0f,  0.0f,  0.2f);
				for( k=0; k<6; k+=2 )
				{
					CLineSegmentRenderer::Draw(avLineSegment[k], avLineSegment[k+1], 0xFF3030FF);
				}
			}
		}

		if( m_RenderStateFlag & JL_VIS_CONTACT_NORMALS )
		{	// render contact normal
			vNormal = collision_buffer[i].m_vDirToBody0;
			avContactNormal[0] = vContactPos;
			avContactNormal[1] = vContactPos + vNormal * 0.3f;
			CLineSegmentRenderer::Draw( avContactNormal[0], avContactNormal[1], 0xFFFF3030);
		}
	}


#endif


//	MsgBox( "CJL_PhysicsVisualizer_D3D::DisplayContactPoints() - end" );


}


void CJL_PhysicsVisualizer_D3D::DisplayActorInfo()
{
	D3DXMATRIX matWorld, matIdentity;
	D3DXVECTOR3 vExtent;
	D3DXVECTOR3 avLineSegment[2];
	Vector3 vPos, vVel;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matIdentity );
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

	TCPreAllocDynamicLinkList<CJL_PhysicsActor>& rActorList = m_pPhysicsManager->GetActorList();

	itrActor = rActorList.Begin();

	while( itrActor != rActorList.End() )
	{
		if( m_RenderStateFlag & JL_VIS_AABB )
		{
			// draw aabb
			AABB3& raabb = itrActor->GetWorldAABB();
			vExtent = raabb.GetExtents() * 2.0f;
	
			matWorld._11 = vExtent.x;
			matWorld._22 = vExtent.y;
			matWorld._33 = vExtent.z;
			memcpy( &matWorld._41, &raabb.GetCenterPosition(), sizeof(D3DXVECTOR3) );

			DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

			if( itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE )
			{
				if( 0 < itrActor->GetCollisionIndexBuffer().size() )
					m_pUnitCube->SetUniformColor( 1.0f, 0.68f, 0.49f );	// colliding with other actors
				else
					m_pUnitCube->SetUniformColor( 1.0f, 1.0f, 1.0f );
			}
			else
				m_pUnitCube->SetUniformColor( 0.5f, 0.5f, 0.5f );

			m_pUnitCube->Draw();
		}

		if( m_RenderStateFlag & JL_VIS_VELOCITY )
		{
			// draw velocity vector
			vPos = itrActor->GetPosition();
			vVel = itrActor->GetVelocity();
			avLineSegment[0][0] = vPos[0];
			avLineSegment[0][1] = vPos[1];
			avLineSegment[0][2] = vPos[2];
			avLineSegment[1][0] = vVel[0];
			avLineSegment[1][1] = vVel[1];
			avLineSegment[1][2] = vVel[2];


			DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matIdentity );

			avLineSegment[1] += avLineSegment[0];
			CLineSegmentRenderer::Draw(avLineSegment[0], avLineSegment[1], 0xFF5A00EE);
		}

		itrActor++;
	}
}


void CJL_PhysicsVisualizer_D3D::ReleaseGraphicsResources()
{
	m_pUnitCube->Release();
}


void CJL_PhysicsVisualizer_D3D::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	m_pUnitCube->Init();
	m_pUnitCube->SetUniformColor( 1.0f, 1.0f, 1.0f );
	m_pUnitCube->SetRenderMode( CUnitCube::RS_WIREFRAME );
}