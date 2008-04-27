
#include "PhysicsManager_Visualizer.h"
#include "JL_PhysicsManager.h"

#include "3DMath/Vector3.h"
#include "3DMath/aabb3.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/UnitCube.h"
#include "Support/memory_helpers.h"
#include "Support/LineSegmentRenderer.h"


#include <d3d9.h>
#include <d3dx9.h>


CPhysicsManager_Visualizer::CPhysicsManager_Visualizer( CJL_PhysicsManager *pPhysicsManager )
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


CPhysicsManager_Visualizer::~CPhysicsManager_Visualizer()
{
//	Release();
	delete( m_pUnitCube );
}


//void CPhysicsManager_Visualizer::Init(){}
//void CPhysicsManager_Visualizer::Release(){}


void CPhysicsManager_Visualizer::RenderVisualInfo()
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


void CPhysicsManager_Visualizer::DisplayContactPoints()
{

	int i;

	D3DXVECTOR3 avLineSegment[6], vContactPoint;
	D3DXVECTOR3 avContactNormal[2];

	vector<CJL_ContactInfo>& collision_buffer = m_pPhysicsManager->m_vecCollision;

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


}


void CPhysicsManager_Visualizer::DisplayActorInfo()
{
	D3DXMATRIX matWorld, matIdentity;
	D3DXVECTOR3 vExtent;
	D3DXVECTOR3 avLineSegment[2];
	Vector3 vPos, vVel;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matIdentity );
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	itrActor = m_pPhysicsManager->m_ActorList.Begin();

	while( itrActor != m_pPhysicsManager->m_ActorList.End() )
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


void CPhysicsManager_Visualizer::ReleaseGraphicsResources()
{
	m_pUnitCube->Release();
}


void CPhysicsManager_Visualizer::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	m_pUnitCube->Init();
	m_pUnitCube->SetUniformColor( 1.0f, 1.0f, 1.0f );
	m_pUnitCube->SetRenderMode( CUnitCube::RS_WIREFRAME );
}