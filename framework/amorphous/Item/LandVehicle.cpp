#include "LandVehicle.hpp"
#include "amorphous/Stage/EntityMotionPathRequest.hpp"
#include "amorphous/Stage/BaseEntity.hpp"
#include "amorphous/Stage/Trace.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/XML/XMLNode.hpp"
#include "amorphous/XML/LoadFromXMLNode_3DMath.hpp"
#include "amorphous/Support/Serialization/Serialization_BoostSmartPtr.hpp"
#include "amorphous/Support/Serialization/Serialization_3DMath.hpp"

//ArmedVehicle
#include "amorphous/Item/Radar.hpp"
#include "amorphous/Item/RotatableTurret.hpp"
#include "amorphous/Item/ItemDatabaseManager.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using boost::shared_ptr;
using namespace item;


inline shared_ptr<ItemEntity> LandVehicle::GetVehicleEntity()
{
	shared_ptr<ItemEntity> pEntity = m_Entity.Get();

	if( !pEntity )
	{
		// Does not linked to the entity - check the owner's entity
		if( m_pOwner )
			pEntity = m_pOwner->GetItemEntity().Get();
	}

	return pEntity;
}


void LandVehicle::Update( float dt )
{
	shared_ptr<ItemEntity> pEntity = GetVehicleEntity();

	if( !pEntity )
		return;

	pEntity->pBaseEntity->UpdateScriptedMotionPath( pEntity.get(), m_Path );

	float test_trace_length = 50.0f;
	if( m_PrevPose != pEntity->GetWorldPose() )
	{
		Vector3 vEntityPos = pEntity->GetWorldPose().vPosition;
		// fix y
		//CGroundContactPoint
		//m_vecGroundContactPoint

		STrace tr;
		tr.bvType = BVTYPE_DOT;
		tr.vStart = vEntityPos + Vector3(0,1,0) * test_trace_length * 0.5f;
		tr.vGoal  = vEntityPos - Vector3(0,1,0) * test_trace_length * 0.5f;
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

			pEntity->SetWorldPosition( vEntityPos );

			Matrix33 matEntityOrient = pEntity->GetWorldPose().matOrient;
			float angle = Vec3GetAngleBetween( vPlaneNormal, matEntityOrient.GetColumn(1) );
			if( 0.001f < fabs(angle) )
			{
				Vector3 vRotationAxis = Vec3Cross( matEntityOrient.GetColumn(1), vPlaneNormal );
				pEntity->SetWorldOrientation( Matrix33RotationAxis( angle, vRotationAxis ) * matEntityOrient );
			}
		}

		m_vecpEntityBuffer.resize( 0 );
	}

	m_PrevPose = pEntity->GetWorldPose();
}


void LandVehicle::Render()
{
	shared_ptr<ItemEntity> pEntity = GetVehicleEntity();

	Matrix34 world_pose
		= pEntity ? pEntity->GetWorldPose() : Matrix34Identity();

	m_MeshContainerRootNode.UpdateWorldTransforms( world_pose );
	m_MeshContainerRootNode.Render();
}


void LandVehicle::Serialize( IArchive& ar, const unsigned int version )
{
	GameItem::Serialize( ar, version );

	ar & m_fMaxAccel;
	ar & m_fMaxTurnSpeed;

	ar & m_fCurrentAccel;
	ar & m_fCurrentTurnSpeed;
}


void LandVehicle::LoadFromXMLNode( XMLNode& reader )
{
	GameItem::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "MaxAccel",         m_fMaxAccel );
	reader.GetChildElementTextContent( "MaxTurnSpeed",     m_fMaxTurnSpeed );
				  
	reader.GetChildElementTextContent( "CurrentAccel",     m_fCurrentAccel );
	reader.GetChildElementTextContent( "CurrentTurnSpeed", m_fCurrentTurnSpeed );
}



void ArmedVehicle::CTurretHolder::Serialize( IArchive& ar, const unsigned int version )
{
	ar & TurretName;
	ar & pTurret;
	ar & LocalPose;
	ar & UseInvLocalTransformForMeshTransform;
}


void ArmedVehicle::Init()
{
	m_pRadar = GetItemDatabaseManager().GetItem<Radar>( m_RadarName, 1 );

	m_pLandVehicleItem = GetItemDatabaseManager().GetItem<LandVehicle>( m_LandVehicleName, 1 );
	m_pLandVehicleItem->SetOwner( m_pMyself.lock() );

	for( size_t i=0; i<m_vecTurret.size(); i++ )
	{
		m_vecTurret[i].pTurret
			= GetItemDatabaseManager().GetItem<CRotatableTurret>( m_vecTurret[i].TurretName, 1 );
		if( m_vecTurret[i].pTurret )
		{
			m_vecTurret[i].pTurret->SetOwner( m_pMyself.lock() );

			if( m_vecTurret[i].UseInvLocalTransformForMeshTransform )
				m_vecTurret[i].pTurret->SetMeshTransform( m_vecTurret[i].LocalPose.GetInverseROT() );
		}
	}
}


bool ArmedVehicle::LoadMeshObject()
{
	// ArmedVehicle has no mesh itself - the mesh is a combination of those of land vehicle and turrets.
//	GameItem::LoadMeshObject();

	const size_t num_turrets = m_vecTurret.size();
	for( size_t i=0; i<num_turrets; i++ )
	{
		if( m_vecTurret[i].pTurret )
			m_vecTurret[i].pTurret->LoadMeshObject();
	}

	if( m_pLandVehicleItem )
		m_pLandVehicleItem->LoadMeshObject();

	return true;
}


void ArmedVehicle::Update( float dt )
{
	shared_ptr<CCopyEntity> pMyEntity = m_Entity.Get();
	const size_t num_turrets = m_vecTurret.size();
	for( size_t i=0; i<num_turrets; i++ )
	{
		if( pMyEntity )
			m_vecTurret[i].pTurret->SetParentWorldPose( pMyEntity->GetWorldPose() );
		m_vecTurret[i].pTurret->Update( dt );
	}

	if( m_pLandVehicleItem )
		m_pLandVehicleItem->Update( dt );

	if( !m_pRadar )
		return;

	m_pRadar->Update( dt );

	UpdateTarget();
}


void ArmedVehicle::Render()
{
	const size_t num_turrets = m_vecTurret.size();
	for( size_t i=0; i<num_turrets; i++ )
	{
		m_vecTurret[i].pTurret->Render();
	}

	if( m_pLandVehicleItem )
		m_pLandVehicleItem->Render();
}


void ArmedVehicle::UpdateTarget()
{
	shared_ptr<CCopyEntity> pMyEntity = m_Entity.Get();
	if( !pMyEntity )
		return;

	const float max_range = 30000.0f; // 30[km]

	shared_ptr<CCopyEntity> pTargetEntity = m_Target.Get();
	if( pTargetEntity )
	{
		float dist_to_target
			= Vec3Length( pTargetEntity->GetWorldPosition() - pMyEntity->GetWorldPosition() );

		if( dist_to_target < max_range )
		{
			// keep aiming and firing at the current target
			return;
		}
	}

	vector< EntityHandle<> >& vecEntity = m_pRadar->EntityBuffer();
	const size_t num_entities = vecEntity.size();
	for( size_t i=0; i<num_entities; i++ )
	{
		shared_ptr<CCopyEntity> pEntity = vecEntity[i].Get();
		if( !pEntity )
			continue;

		if( !IsTargetGroupIndex(pEntity->GroupIndex) )
			continue;

		float dist_to_target
			= Vec3Length( pTargetEntity->GetWorldPosition() - pMyEntity->GetWorldPosition() );

		if( dist_to_target < max_range )
		{
			//vecCandidate.push_back( vecEntity[i] );
			m_Target = EntityHandle<>( pEntity );
		}
	}

	const size_t num_turrets = m_vecTurret.size();
	for( size_t i=0; i<num_turrets; i++ )
	{
		m_vecTurret[i].pTurret->SetTarget( m_Target );
	}

}


void ArmedVehicle::Serialize( IArchive& ar, const unsigned int version )
{
	GameItem::Serialize( ar, version );

	ar & m_RadarName;
	ar & m_pRadar;

	ar & m_LandVehicleName;
	ar & m_pLandVehicleItem;

	ar & m_vecTurret;

	if( ar.GetMode() == IArchive::MODE_INPUT )
		m_Target = EntityHandle<>();

//	if( ar.GetMode() == IArchive::MODE_INPUT )
//	{
//		Init();
//	}
}


void ArmedVehicle::LoadFromXMLNode( XMLNode& reader )
{
	GameItem::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "Radar", m_RadarName );

	reader.GetChildElementTextContent( "LandVehicle", m_LandVehicleName );

	vector<XMLNode> vecTurret = reader.GetImmediateChildren( "Turret" );
	m_vecTurret.resize( vecTurret.size() );
	for( size_t i=0; i<vecTurret.size(); i++ )
	{
		m_vecTurret[i].TurretName = vecTurret[i].GetChild( "Name" ).GetTextContent();
//		m_vecTurret[i].pTurret = GetItemDatabaseManager().GetItem<CRotatableTurret>( m_vecTurret[i].TurretName, 1 );
//		if( !m_vecTurret[i].pTurret )
//			continue;

		amorphous::LoadFromXMLNode( vecTurret[i].GetChild( "LocalPose" ), m_vecTurret[i].LocalPose );
//		m_vecTurret[i].pTurret->LoadFromXMLNode( vecTurret[i] );

		bool use_inv_local_pose_for_mesh_transform = true;
		string yes_or_no = vecTurret[i].GetAttributeText( "use_inv_local_pose_for_mesh_transform" );
		if( yes_or_no == "yes" )
			use_inv_local_pose_for_mesh_transform = true;
		else if( yes_or_no == "no" )
			use_inv_local_pose_for_mesh_transform = false;

		m_vecTurret[i].UseInvLocalTransformForMeshTransform
			= use_inv_local_pose_for_mesh_transform;

//		if( use_inv_local_pose_for_mesh_transform )
//			m_vecTurret[i].pTurret->SetMeshTransform( m_vecTurret[i].LocalPose.GetInverseROT() );
	}
}


bool ArmedVehicle::IsTargetGroupIndex( int group )
{
	return true;
}


} // namespace amorphous
