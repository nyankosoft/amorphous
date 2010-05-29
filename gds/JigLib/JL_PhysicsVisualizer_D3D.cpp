#include "JL_PhysicsVisualizer_D3D.hpp"
#include "JL_PhysicsManager.hpp"
#include "3DMath/Vector3.hpp"
#include "3DMath/AABB3.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/UnitCube.hpp"
//#include "Support/memory_helpers.hpp"
#include "Support/LineSegmentRenderer.hpp"
#include "Support/SafeDelete.hpp"
//#include "Support/msgbox.hpp"


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
	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	CShaderManager& shader_mgr = FixedFunctionPipelineManager();
	shader_mgr.SetWorldTransform( Matrix44Identity() );

/*	CShaderManager *pShaderMgr = CShader::Get()->GetCurrentShaderManager();
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
	}*/


	// render visual debug info
	DisplayContactPoints();

	DisplayActorInfo();


/*	if( pEffect )
	{
		pEffect->EndPass();
		pEffect->End();
	}*/
}


void CJL_PhysicsVisualizer_D3D::DisplayContactPoints()
{
//	MsgBox( "CJL_PhysicsVisualizer_D3D::DisplayContactPoints()" );

	int i;

	Vector3 avLineSegment[6], vContactPoint;
	Vector3 avContactNormal[2];

#ifndef USE_COMBINED_COLLISION_POINTS_INFO  ///////////////////////////////////////////////////

//	vector<CJL_ContactInfo>& collision_buffer = m_pPhysicsManager->GetCollisionInfo();
	vector<CJL_ContactInfo> collision_buffer;

 	int j, iNumContacts = collision_buffer.size();

	for( i=0; i<iNumContacts; i++ )
	{
		if( m_RenderStateFlag & JL_VIS_CONTACT_POINTS )
		{	// render contact point
			vContactPoint = collision_buffer[i].vContactPoint;
			avLineSegment[0] = vContactPoint - Vector3( 0.2f,  0.0f,  0.0f);
			avLineSegment[1] = vContactPoint + Vector3( 0.2f,  0.0f,  0.0f);
			avLineSegment[2] = vContactPoint - Vector3( 0.0f,  0.2f,  0.0f);
			avLineSegment[3] = vContactPoint + Vector3( 0.0f,  0.2f,  0.0f);
			avLineSegment[4] = vContactPoint - Vector3( 0.0f,  0.0f,  0.2f);
			avLineSegment[5] = vContactPoint + Vector3( 0.0f,  0.0f,  0.2f);
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

 	int j, k, num_colls = (int)collision_buffer.size();
	Vector3 vContactPos, vNormal;

	for( i=0; i<num_colls; i++ )
	{
		for( j=0; j<collision_buffer[i].m_vecPointInfo.size(); j++ )
		{
			CJL_CollPointInfo& pt_info = collision_buffer[i].m_vecPointInfo[j];

			if( m_RenderStateFlag & JL_VIS_CONTACT_POINTS )
			{	// render contact point
				vContactPos = pt_info.vContactPosition;
				avLineSegment[0] = vContactPos - Vector3( 0.2f,  0.0f,  0.0f);
				avLineSegment[1] = vContactPos + Vector3( 0.2f,  0.0f,  0.0f);
				avLineSegment[2] = vContactPos - Vector3( 0.0f,  0.2f,  0.0f);
				avLineSegment[3] = vContactPos + Vector3( 0.0f,  0.2f,  0.0f);
				avLineSegment[4] = vContactPos - Vector3( 0.0f,  0.0f,  0.2f);
				avLineSegment[5] = vContactPos + Vector3( 0.0f,  0.0f,  0.2f);
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
	Vector3 vExtent;
	Vector3 avLineSegment[2];
	Vector3 vPos, vVel;

	CShaderManager& shader_mgr = FixedFunctionPipelineManager();

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
	
			Matrix34 pose( Matrix34Identity() );
			pose.vPosition = raabb.GetCenterPosition();
			ToMatrix44( pose );

			shader_mgr.SetWorldTransform( ToMatrix44( pose ) * Matrix44Scaling( vExtent.x, vExtent.y, vExtent.z ) );

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


			shader_mgr.SetWorldTransform( Matrix44Identity() );

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
