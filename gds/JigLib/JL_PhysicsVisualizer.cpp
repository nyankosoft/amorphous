#include "JL_PhysicsVisualizer.hpp"
#include "JL_PhysicsManager.hpp"
#include "3DMath/AABB3.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/PrimitiveRenderer.hpp"


namespace amorphous
{
//#include "Graphics/UnitCube.hpp"

using namespace std;


CJL_PhysicsVisualizer::CJL_PhysicsVisualizer( CJL_PhysicsManager *pPhysicsManager )
{
	m_pPhysicsManager = pPhysicsManager;

#ifdef _DEBUG
	m_RenderStateFlag = JL_VIS_CONTACT_POINTS | JL_VIS_CONTACT_NORMALS | JL_VIS_AABB;
#else
	m_RenderStateFlag = 0;
#endif

//	m_pUnitCube = new CUnitCube;
//	m_pUnitCube->Init();
//	m_pUnitCube->SetUniformColor( 1.0f, 1.0f, 1.0f );
//	m_pUnitCube->SetRenderMode( CUnitCube::RS_WIREFRAME );
}


CJL_PhysicsVisualizer::~CJL_PhysicsVisualizer()
{
//	SafeDelete( m_pUnitCube );
}


//void CJL_PhysicsVisualizer::Init(){}
//void CJL_PhysicsVisualizer::Release(){}


void CJL_PhysicsVisualizer::RenderVisualInfo()
{
	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	CShaderManager& shader_mgr = FixedFunctionPipelineManager();
	shader_mgr.SetWorldTransform( Matrix44Identity() );

	// render visual debug info
	DisplayContactPoints();

	DisplayActorInfo();
}


void CJL_PhysicsVisualizer::DisplayContactPoints()
{
//	MsgBox( "CJL_PhysicsVisualizer::DisplayContactPoints()" );

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
				GetPrimitiveRenderer::DrawLine( avLineSegment[j], avLineSegment[j+1], SFloatRGBAColor( 0.19f, 0.19f, 1.00f, 1.00f ) );
			}
		}

		if( m_RenderStateFlag & JL_VIS_CONTACT_NORMALS )
		{	// render contact normal
			avContactNormal[0] = vContactPoint;
			avContactNormal[1] = vContactPoint + collision_buffer[i].vNormal * 0.3f;
			GetPrimitiveRenderer::DrawLine( avContactNormal[0], avContactNormal[1], SFloatRGBAColor( 1.00f, 0.19f, 0.19f, 1.00f ) );
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
					GetPrimitiveRenderer().DrawLine( avLineSegment[k], avLineSegment[k+1], SFloatRGBAColor(0.19f, 0.19f, 1.00f, 1.00f ) );
//					CLineSegmentRenderer::Draw(avLineSegment[k], avLineSegment[k+1], 0xFF3030FF);
				}
			}
		}

		if( m_RenderStateFlag & JL_VIS_CONTACT_NORMALS )
		{	// render contact normal
			vNormal = collision_buffer[i].m_vDirToBody0;
			avContactNormal[0] = vContactPos;
			avContactNormal[1] = vContactPos + vNormal * 0.3f;
			GetPrimitiveRenderer().DrawLine( avContactNormal[0], avContactNormal[1], SFloatRGBAColor( 1.00f, 0.19f, 0.19f, 1.00f ) );
//			CLineSegmentRenderer::Draw( avContactNormal[0], avContactNormal[1], 0xFFFF3030);
		}
	}


#endif


//	MsgBox( "CJL_PhysicsVisualizer::DisplayContactPoints() - end" );


}


void CJL_PhysicsVisualizer::DisplayActorInfo()
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

/*			if( itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE )
			{
				if( 0 < itrActor->GetCollisionIndexBuffer().size() )
					m_pUnitCube->SetUniformColor( 1.0f, 0.68f, 0.49f );	// colliding with other actors
				else
					m_pUnitCube->SetUniformColor( 1.0f, 1.0f, 1.0f );
			}
			else
				m_pUnitCube->SetUniformColor( 0.5f, 0.5f, 0.5f );

			m_pUnitCube->Draw();*/
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
			GetPrimitiveRenderer().DrawLine( avLineSegment[0], avLineSegment[1], SFloatRGBAColor( 0.35f, 0.00f, 0.93f, 1.00f ) );
//			CLineSegmentRenderer::Draw(avLineSegment[0], avLineSegment[1], 0xFF5A00EE);
		}

		itrActor++;
	}
}


void CJL_PhysicsVisualizer::ReleaseGraphicsResources()
{
//	m_pUnitCube->Release();
}


void CJL_PhysicsVisualizer::LoadGraphicsResources( const GraphicsParameters& rParam )
{
//	m_pUnitCube->Init();
//	m_pUnitCube->SetUniformColor( 1.0f, 1.0f, 1.0f );
//	m_pUnitCube->SetRenderMode( CUnitCube::RS_WIREFRAME );
}


} // namespace amorphous
