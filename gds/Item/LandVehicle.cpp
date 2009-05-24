#include "LandVehicle.hpp"
#include "Stage/EntityMotionPathRequest.hpp"
#include "Stage/BaseEntity.hpp"
#include "Stage/Trace.hpp"
#include "Stage/Stage.hpp"
#include "XML/XMLNodeReader.hpp"
#include "XML/LoadFromXMLNode_3DMath.hpp"
#include "Support/Serialization/Serialization_BoostSmartPtr.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"

//CArmedVehicle
#include "Item/Radar.hpp"
#include "Item/RotatableTurret.hpp"
#include "Item/ItemDatabaseManager.hpp"


using namespace std;
using namespace boost;
using namespace item;


void CLandVehicle::Update( float dt )
{
	CCopyEntity *pEntity = m_Entity.GetRawPtr();
	if( !pEntity )
		return;

	pEntity->pBaseEntity->UpdateScriptedMotionPath( pEntity, m_Path );

	float test_trace_length = 50.0f;
	if( m_PrevPose != pEntity->GetWorldPose() )
	{
		Vector3 vEntityPos = pEntity->GetWorldPose().vPosition;
		// fix y
		//CGroundContactPoint
		//m_vecGroundContactPoint

		STrace tr;
		tr.bvType = BVTYPE_DOT;
		Vector3 vStart = vEntityPos + Vector3(0,1,0) * test_trace_length * 0.5f;
		tr.pvStart = &vStart;
		Vector3 vGoal  = vEntityPos - Vector3(0,1,0) * test_trace_length * 0.5f;
		tr.pvGoal  = &vGoal;
//		tr.SetLineSegment( vStart, vGoal );
		m_vecpEntityBuffer.resize( 0 );
//		tr.SetTouchEntityBuffer( &m_vecpEntityBuffer );
//		tr.SetTraceType( TRACETYPE_IGNORE_NOCLIP_ENTITIES );	// skip checking the no-clip entities
		tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

		shared_ptr<CStage> pStage = m_pStage.lock();
		if( pStage )
			pStage->ClipTrace( tr );

		float height_from_ground_to_center = 0.8f;
		if( tr.fFraction < 1.0f )
		{
			Vector3 vPlaneNormal = Vector3(0,1,0);//tr.GetPlaneNormal();
			vEntityPos
				= tr.vEnd
				+ Vector3(0,1,0) * height_from_ground_to_center / Vec3Dot( vPlaneNormal, Vector3(0,1,0) );

			pEntity->Position() = vEntityPos;

			Matrix33 matEntityOrient = pEntity->GetWorldPose().matOrient;
			float angle = Vec3GetAngleBetween( vPlaneNormal, matEntityOrient.GetColumn(1) );
			Vector3 vRotationAxis = Vec3Cross( matEntityOrient.GetColumn(1), vPlaneNormal );
			pEntity->SetOrientation( Matrix33RotationAxis( angle, vRotationAxis ) * matEntityOrient );
		}

		m_vecpEntityBuffer.resize( 0 );
	}

	m_PrevPose = pEntity->GetWorldPose();
}


void CLandVehicle::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_fMaxAccel;
	ar & m_fMaxTurnSpeed;

	ar & m_fCurrentAccel;
	ar & m_fCurrentTurnSpeed;
}


void CLandVehicle::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGameItem::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "MaxAccel",         m_fMaxAccel );
	reader.GetChildElementTextContent( "MaxTurnSpeed",     m_fMaxTurnSpeed );
				  
	reader.GetChildElementTextContent( "CurrentAccel",     m_fCurrentAccel );
	reader.GetChildElementTextContent( "CurrentTurnSpeed", m_fCurrentTurnSpeed );
}



void CArmedVehicle::CTurretHolder::Serialize( IArchive& ar, const unsigned int version )
{
	ar & pTurret;
	ar & LocalPose;
}


void CArmedVehicle::Update( float dt )
{
	m_pLandVehicleItem->Update( dt );

	if( !m_pRadar )
		return;

	m_pRadar->Update( dt );

	UpdateTarget();
}


void CArmedVehicle::Render()
{
	const size_t num_turrets = m_vecTurret.size();
	for( size_t i=0; i<num_turrets; i++ )
	{
		m_vecTurret[i].pTurret->Render();
	}

	m_pLandVehicleItem->Render();
}


void CArmedVehicle::UpdateTarget()
{
	shared_ptr<CCopyEntity> pMyEntity = m_Entity.Get();
	if( !pMyEntity )
		return;

	const float max_range = 30000.0f; // 30[km]

	shared_ptr<CCopyEntity> pTargetEntity = m_Target.Get();
	if( pTargetEntity )
	{
		float dist_to_target
			= Vec3Length( pTargetEntity->Position() - pMyEntity->Position() );

		if( dist_to_target < max_range )
		{
			// keep aiming and firing at the current target
			return;
		}
	}

	vector< CEntityHandle<> >& vecEntity = m_pRadar->EntityBuffer();
	const size_t num_entities = vecEntity.size();
	for( size_t i=0; i<num_entities; i++ )
	{
		shared_ptr<CCopyEntity> pEntity = vecEntity[i].Get();
		if( !pEntity )
			continue;

		if( !IsTargetGroupIndex(pEntity->GroupIndex) )
			continue;

		float dist_to_target
			= Vec3Length( pTargetEntity->Position() - pMyEntity->Position() );

		if( dist_to_target < max_range )
		{
			//vecCandidate.push_back( vecEntity[i] );
			m_Target = CEntityHandle<>( pEntity );
		}
	}

	const size_t num_turrets = m_vecTurret.size();
	for( size_t i=0; i<num_turrets; i++ )
	{
		m_vecTurret[i].pTurret->SetTarget( m_Target );
	}

}


void CArmedVehicle::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_pLandVehicleItem;
	ar & m_vecTurret;
}


void CArmedVehicle::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGameItem::LoadFromXMLNode( reader );

	string land_vehicle_name;

	reader.GetChildElementTextContent( "LandVehicle", land_vehicle_name );
	m_pLandVehicleItem = ItemDatabaseManager().GetItem<CLandVehicle>( land_vehicle_name, 1 );

	vector<CXMLNodeReader> vecTurret = reader.GetImmediateChildren( "Turret" );
	m_vecTurret.resize( vecTurret.size() );
	for( size_t i=0; i<vecTurret.size(); i++ )
	{
		m_vecTurret[i].pTurret = ItemDatabaseManager().GetItem<CRotatableTurret>( vecTurret[i].GetChild( "Name" ).GetTextContent(), 1 );
		if( !m_vecTurret[i].pTurret )
			continue;

		::LoadFromXMLNode( vecTurret[i].GetChild( "LocalPose" ), m_vecTurret[i].LocalPose );
		m_vecTurret[i].pTurret->LoadFromXMLNode( vecTurret[i] );

		bool use_inv_local_pose_for_mesh_transform = true;
		string yes_or_no = vecTurret[i].GetAttributeText( "use_inv_local_pose_for_mesh_transform" );
		if( yes_or_no == "yes" )
			use_inv_local_pose_for_mesh_transform = true;
		else if( yes_or_no == "no" )
			use_inv_local_pose_for_mesh_transform = false;

		if( use_inv_local_pose_for_mesh_transform )
			m_vecTurret[i].pTurret->SetMeshTransform( m_vecTurret[i].LocalPose.GetInverseROT() );
	}
}


bool CArmedVehicle::IsTargetGroupIndex( int group )
{
	return true;
}
