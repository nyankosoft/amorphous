#include "Radar.hpp"
#include "amorphous/Stage/EntitySet.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/BE_PlayerPseudoAircraft.hpp"
#include "amorphous/Stage/BE_HomingMissile.hpp"
#include "amorphous/Stage/OverlapTestAABB.hpp"
#include "amorphous/Stage/HUD_PlayerAircraft.hpp"
#include "amorphous/XML/XMLNode.hpp"


namespace amorphous
{

using std::map;
using namespace boost;


Radar::Radar()
:
m_fEffectiveRangeRadius( 500000.0f ),
m_fSensoringFrequency( 5 ),
m_TargetInfoUpdateFrequency( 10 ),
m_fNextSensoringTime(0),
m_fNextTargetUpdateTime(0)
{
}


void Radar::Update( float dt )
{
	std::shared_ptr<CStage> pStage = m_pStage.lock();

	if( !pStage )
		return;

	double sensoring_time_rand_offset[2] = { 0.035, 0.012 };

	double fSensoringInterval = 1.0 / m_fSensoringFrequency;
	const double current_time_in_stage = pStage->GetElapsedTime();
	if( m_fNextSensoringTime < current_time_in_stage )
	{
		m_fNextSensoringTime
			= current_time_in_stage
			- fmod( current_time_in_stage, fSensoringInterval )
			+ fSensoringInterval
			+ sensoring_time_rand_offset[0];

		UpdateEntitiesList();
	}

	double fUpdateInterval = 1.0 / (double)m_TargetInfoUpdateFrequency;
	if( m_fNextTargetUpdateTime < current_time_in_stage )
	{
		m_fNextTargetUpdateTime
			= current_time_in_stage
			- fmod( current_time_in_stage, fUpdateInterval )
			+ fUpdateInterval
			+ sensoring_time_rand_offset[1];

		UpdateTargetInfo();
	}
}


/// list of entities that are in the radar range
void Radar::UpdateEntitiesList()
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return;

	// clear all the previous entities
	m_vecEntityBuffer.resize( 0 );
	m_vecpEntityBuffer.resize( 0 );

	float effective_radar_radius = m_fEffectiveRangeRadius;//500000.0f; // 500[km]
	const float r = effective_radar_radius;

	Matrix34 radar_world_pose = GetRadarWorldPose();

	// cube with each edge 1000[km]
	// TODO: use a proper bounding-box that contains the entire stage
	AABB3 aabb = AABB3(
		Vector3(-1,-1,-1) * effective_radar_radius + radar_world_pose.vPosition,
		Vector3( 1, 1, 1) * effective_radar_radius + radar_world_pose.vPosition );	// m_pStage->GetAABB();

	OverlapTestAABB overlap_test( aabb, &m_vecpEntityBuffer, ENTITY_GROUP_MIN_ID );

	pStage->GetEntitySet()->GetOverlappingEntities( overlap_test );

	const size_t num_entities = m_vecpEntityBuffer.size();
	m_vecEntityBuffer.resize( num_entities );
	for( size_t i=0; i<num_entities; i++ )
	{
		m_vecEntityBuffer[i] = EntityHandle<>( m_vecpEntityBuffer[i]->Self() );
	}
}


void Radar::UpdateTargetInfo()
{
	// clear all the previous target info
	m_RadarInfo.ClearTargetInfo();

	std::map<int,int>& entity_type_id_to_target_type_flag = CBE_PlayerPseudoAircraft::EntityTypeIDtoTargetTypeFlagMap();
	std::map<int,int>& entity_group_to_target_group_flag  = CBE_PlayerPseudoAircraft::EntityGroupToTargetGroupFlagMap();
	std::map<int,int>& entity_group_to_target_type_flag  = CBE_PlayerPseudoAircraft::EntityGroupToTargetTypeFlagMap();
	
//	std::map<int,int>& entity_type_id_to_target_type_flag = ms_mapEntityTypeIDtoTargetTypeFlag;
//	std::map<int,int>& entity_group_to_target_group_flag  = ms_mapEntityGroupToTargetGroupFlag;

	const Matrix34 radar_world_pose = GetRadarWorldPose();
	Vector3 vCamFwdDir = radar_world_pose.matOrient.GetColumn(2);//m_Camera.GetFrontDirection();// pCopyEnt->GetDirection();

	size_t i, num_entities = m_vecEntityBuffer.size();
	for( i=0; i<num_entities; i++ )
	{
		shared_ptr<CCopyEntity> pEntity = m_vecEntityBuffer[i].Get();

		if( !pEntity )
			continue;

		BaseEntity* pBaseEntity = pEntity->pBaseEntity;

		unsigned int id = pBaseEntity->GetArchiveObjectID();
		int tgt_type = 0;

		switch(id)
		{
		case BaseEntity::BE_PLAYERPSEUDOAIRCRAFT:
			tgt_type = HUD_TargetInfo::PLAYER | HUD_TargetInfo::TGT_AIR;
			break;

		case BaseEntity::BE_HOMINGMISSILE:
			if( MissileState(pEntity.get()) == CBE_HomingMissile::MS_IGNITED )
				tgt_type = HUD_TargetInfo::MISSILE;
			break;

		default:
			map<int,int>::iterator itr_group
				= entity_group_to_target_group_flag.find( pEntity->GroupIndex );

			if( itr_group != entity_group_to_target_group_flag.end() )
			{
				// set the target type
				// - HUD_TargetInfo::PLAYER, ALLY or ENEMY
				tgt_type |= itr_group->second;
			}

			map<int,int>::iterator itr_type
				= entity_type_id_to_target_type_flag.find( pEntity->GetEntityTypeID() );

			if( itr_type != entity_type_id_to_target_type_flag.end() )
			{
				// set the target group
				// - UD_TargetInfo::TGT_AIR or TGT_SURFACE
				tgt_type |= itr_type->second;
			}

			itr_type = entity_group_to_target_type_flag.find( pEntity->GroupIndex );
			if( itr_type != entity_group_to_target_type_flag.end() )
			{
				tgt_type |= itr_type->second;
			}

			break;
		}

		if( tgt_type != 0 )
		{
/*			if( pEntity == m_pFocusedTarget )
			{
				tgt_type |= HUD_TargetInfo::FOCUSED;
				m_RadarInfo.m_FocusedTargetIndex = m_RadarInfo.m_vecTargetInfo.size();
			}

			if( pLauncher && pLauncher->IsLockingOn( pEntity.get() ) )
			{
				tgt_type |= HUD_TargetInfo::LOCKED_ON;
			}
*/
			m_RadarInfo.m_vecTargetInfo.push_back( HUD_TargetInfo( pEntity->GetWorldPosition(), "", tgt_type ) );
			m_RadarInfo.m_vecTargetInfo.back().direction = pEntity->GetDirection();
			m_RadarInfo.m_vecTargetInfo.back().radius    = pEntity->fRadius * 1.5f;
			m_RadarInfo.m_vecTargetInfo.back().entity_id = pEntity->GetID();

			Vector3 vPlayerToTargetDir = Vec3GetNormalized( pEntity->GetWorldPosition() - radar_world_pose.vPosition );
		}
	}
}

/*
void Radar::UpdateRadarInfo( CCopyEntity* pCopyEnt )
{

	m_State = STATE_NORMAL;
	for( i=0; i<num_entities; i++ )
	{
		CCopyEntity *pEntity = m_vecEntityBuffer[i].Get();

		if( pEntity )
			continue;

		if( pEntity->pBaseEntity->GetArchiveObjectID() == BE_HOMINGMISSILE )
		{
			if( pEntity->m_Target.GetRawPtr() == pCopyEnt )
				m_State = STATE_MISSILE_APPROACHING;
		}
	}
}*/


void Radar::Serialize( IArchive& ar, const unsigned int version )
{
	GameItem::Serialize( ar, version );

	ar & m_fEffectiveRangeRadius;
	ar & m_fSensoringFrequency;
	ar & m_TargetInfoUpdateFrequency;

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		m_fNextSensoringTime    = 0;
		m_fNextTargetUpdateTime = 0;
		m_vecEntityBuffer.resize( 0 );
		m_vecpEntityBuffer.resize( 0 );
		m_RadarInfo.m_vecTargetInfo.resize( 0 );
	}
}


void Radar::LoadFromXMLNode( XMLNode& reader )
{
	GameItem::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "EffectiveRangeRadius",      m_fEffectiveRangeRadius );
	reader.GetChildElementTextContent( "SensoringFrequency",        m_fSensoringFrequency );
	reader.GetChildElementTextContent( "TargetInfoUpdateFrequency", m_TargetInfoUpdateFrequency );
}


} // namespace amorphous
