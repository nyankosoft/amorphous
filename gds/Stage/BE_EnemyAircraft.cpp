#include "BE_EnemyAircraft.h"

#include "GameMessage.h"
#include "CopyEntity.h"
#include "CopyEntityDesc.h"
#include "trace.h"
#include "Stage.h"
#include "OverlapTestAABB.h"
#include "PlayerInfo.h"
#include "BE_HomingMissile.h"

#include "3DCommon/D3DXMeshObjectBase.h"
#include "3DMath/MathMisc.h"
#include "JigLib/JL_PhysicsActor.h"
#include "Item/ItemDatabaseManager.h"

#include "GameCommon/MTRand.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Log/StateLog.h"
#include "Support/StringAux.h"


void CBEC_EnemyAircraftExtraData::Release()
{
	SafeDelete( m_pAircraft );
	SafeDeleteVector( m_vecpWeapon );
	SafeDeleteVector( m_vecpAmmo );
}


void CBEC_EnemyAircraftExtraData::UpdateTarget( CCopyEntity *pTargetEntity, double current_time )
{
	m_pCurrentTarget = pTargetEntity;
	m_TimeOfLastTargetUpdate = current_time;
}


CBE_EnemyAircraft::CBE_EnemyAircraft()
{
	m_BoundingVolumeType = BVTYPE_AABB;

	// register to physics simulator for UpdatePhysics() to be called
	RaiseEntityFlag( BETYPE_RIGIDBODY );

	// Do not use the results of physics simulator
	// - pose & velocity are calculated by pseudo aircraft simulator
	ClearEntityFlag( BETYPE_USE_PHYSSIM_RESULTS );

	// register as no-clip to physics simulator
	// and do the collision detection by itself
	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_NOCLIP;

	m_ActorDesc.ActorFlag |= JL_ACTOR_KINEMATIC;
}


CBE_EnemyAircraft::~CBE_EnemyAircraft()
{
	size_t i, num_extra_data = m_vecExtraData.size();
	for( i=0; i<num_extra_data; i++ )
		m_vecExtraData[i].Release();
}


void CBE_EnemyAircraft::Init()
{
	CBE_Enemy::Init();

	Init3DModel();

//	m_Missile.SetBaseEntityName( "mssl" );
	m_Missile.SetBaseEntityName( "enemy_missile" );
	LoadBaseEntity( m_Missile );

	m_vecExtraData.reserve( NUM_INIT_ENEMY_AIRCRAFTS );
	for( size_t i=0; i<NUM_INIT_ENEMY_AIRCRAFTS; i++ )
		AddExtraData();
}


void CBE_EnemyAircraft::InitCopyEntity(CCopyEntity* pCopyEnt)
{
//	CBE_Enemy::InitCopyEntity(pCopyEnt);

	pCopyEnt->fLife = m_fLife;

	// create a new state for the copy entity
	pCopyEnt->iExtraDataIndex = GetNewExtraDataIndex();

	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt);

	CPseudoAircraftSimulator& pseudo_sim = ex.GetPseudoSimulator();

	pseudo_sim.SetWorldPose( pCopyEnt->GetWorldPose() );
	pseudo_sim.SetForwardVelocity( pCopyEnt->Velocity() );

	if( pCopyEnt->pPhysicsActor )
		pCopyEnt->pPhysicsActor->SetAllowFreezing( false );

	// random last fire time for each enemy
	ex.m_dLastFireTime = m_pStage->GetElapsedTime() - RangedRand( 0.0f, 5.0f );

	if( ex.m_pAircraft )
		m_NozzleFlames.CreateNozzleFlames( pCopyEnt, ex.m_pAircraft, m_pStage );
}

/*
void CBE_EnemyAircraft::SearchManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData){}

// performs simple maneuvers
void CBE_EnemyAircraft::AttackManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData){}
*/


void CBE_EnemyAircraft::Fire( CCopyEntity* pCopyEnt, Vector3 vToTarget )
{
	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt);

	if( !IsValidEntity( ex.m_pCurrentTarget ) )
		return; // no target

	float fDistToTarget = Vec3Length( vToTarget );
	Vector3 vDirToTarget = vToTarget / fDistToTarget;

	if( 0.3f < Vec3Dot( vDirToTarget, pCopyEnt->GetDirection() ) // check if the aircraft is directing to the target
	 && fDistToTarget < 25000.0f )
	{
		if( ex.m_dLastFireTime + 10.0 < m_pStage->GetElapsedTime() )
		{
			CCopyEntityDesc missile;
			missile.SetWorldOrient( pCopyEnt->GetWorldPose().matOrient );
			missile.SetWorldPosition( pCopyEnt->Position() + pCopyEnt->GetDirection() * 50.0f );
			missile.vVelocity = pCopyEnt->Velocity();
			missile.pBaseEntityHandle = &m_Missile;
			missile.sGroupID = GetEntityGroupID( m_ProjectileEntityGroup ); //CE_GROUP_ENEMY_PROJECTILE;

			CCopyEntity* pMissileEntity = m_pStage->CreateEntity( missile );

			if( IsValidEntity(pMissileEntity) 
			 && pMissileEntity->pBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_HOMINGMISSILE )
			{
				((CBE_HomingMissile*)pMissileEntity->pBaseEntity)->Ignite(pMissileEntity);
			}

			ex.m_dLastFireTime = m_pStage->GetElapsedTime();
		}
	}
}


void CBE_EnemyAircraft::SelectTarget( CCopyEntity* pCopyEnt )
{
	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt);

	static vector<CCopyEntity *> s_vecpEntityBuffer;
	s_vecpEntityBuffer.resize( 0 );

	static vector<CCopyEntity *> s_vecpTgtCandidateBuffer;
	s_vecpTgtCandidateBuffer.resize( 0 );

	float effective_radar_radius = 100000.0f;
	const float r = effective_radar_radius;//200000.0f;

	// cube with each edge 25[km]
	effective_radar_radius = 25000.0f;
	AABB3 aabb = AABB3(
		Vector3(-1,-1,-1) * effective_radar_radius + pCopyEnt->Position(),
		Vector3( 1, 1, 1) * effective_radar_radius + pCopyEnt->Position() );

	// set the entity group of the projectile
	// - Entities that can be hit by a projectile of this entity will be collected
	int entity_group_id = GetEntityGroupID( m_ProjectileEntityGroup );
//	int entity_group_id = GetEntityGroupID( m_TargetFilter );
	COverlapTestAABB overlap_test( aabb, &s_vecpEntityBuffer, entity_group_id );

	m_pStage->GetEntitySet()->GetOverlappingEntities( overlap_test );

	size_t i, num_entities = s_vecpEntityBuffer.size();
	for( i=0; i<num_entities; i++ )
	{
		CCopyEntity *pEntity = s_vecpEntityBuffer[i];

		if( !IsValidEntity( pEntity ) )
			continue;

//		if( !MatchesTargetPolicy( pEntity ) )
//			continue;

		s_vecpTgtCandidateBuffer.push_back( pEntity );
	}

	// update if there is at least one valid target candidate
	int num_candidates = (int)s_vecpTgtCandidateBuffer.size();
	if( 0 < num_candidates )
	{
		// choose one randomly from the candidates
		ex.UpdateTarget(
			s_vecpTgtCandidateBuffer[ RangedRand( 0, num_candidates ) ],
			m_pStage->GetElapsedTime() );
	}
}


void CBE_EnemyAircraft::UpdateTarget( CCopyEntity* pCopyEnt )
{
	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt);

	CCopyEntity* pTargetEntity = NULL;

	enum eTargetingPolicy
	{
		TP_ALWAYS_TARGET_PLAYER,
//		TP_CHOOSE_RANDOMLY_FROM_TARGET_GROUP,
		TP_SELECT_ANY_TARGET, ///< choose a target randomly from the target group
		TP_SELECT_SURFACE_TARGET,
		TP_SELECT_AIR_TARGET,
		NUM_TARGETING_POLICIES
	};

	enum eActionPolicy
	{
		AP_ENGAGE_AT_WILL,
		AP_COVER,
		NUM_ACTION_POLICIES
	};

	int targeting_policy = TP_ALWAYS_TARGET_PLAYER;

	switch( targeting_policy )
	{
	case TP_ALWAYS_TARGET_PLAYER:
		pTargetEntity = PLAYERINFO.GetCurrentPlayerEntity();
		if( IsValidEntity( pTargetEntity ) )
			ex.m_pCurrentTarget = pTargetEntity;
		else
			ex.m_pCurrentTarget = NULL;
		break;

	case TP_SELECT_ANY_TARGET:
	case TP_SELECT_AIR_TARGET:
	case TP_SELECT_SURFACE_TARGET:
		SelectTarget( pCopyEnt );
		break;

	default:
		break;
	}
}


void CBE_EnemyAircraft::Act( CCopyEntity* pCopyEnt )
{
	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt);
	CPseudoAircraftSimulator& pseudo_sim = ex.GetPseudoSimulator();
	float frametime = m_pStage->GetFrameTime();

	if( ex.m_pAircraft )
		m_NozzleFlames.UpdateNozzleFlames( pCopyEnt, 0, ex.m_pAircraft );

	// if the entity has scripted motion path, update pose for the current time in stage
	if( ex.m_Path.IsAvailable(0) )
	{
		// 19:27 10/3/2007 moved to UpdatePhysics()
		// player pose is updated in UpdatePhysics()
//		UpdateScriptedMotionPath( pCopyEnt );
		return;
	}

//	CCopyEntity* pPlayerEntity = PLAYERINFO.GetCurrentPlayerEntity();

	ex.m_fTargetSensorElapsedTime += frametime;
	if( ex.m_fTargetSensoringInterval <= ex.m_fTargetSensorElapsedTime )
	{
		ex.m_fTargetSensorElapsedTime
			= fmodf( ex.m_fTargetSensorElapsedTime, ex.m_fTargetSensoringInterval );

		UpdateTarget( pCopyEnt );
	}

	if( !IsValidEntity( ex.m_pCurrentTarget ) )
	{
		ex.UpdateTarget( NULL, m_pStage->GetElapsedTime() );
		return;
	}

	CCopyEntity *pTargetEntity = ex.m_pCurrentTarget;

/*	target the player's side
	Matrix33& player_orient = pPlayerEntity->GetWorldPose().matOrient;
	TargetPosition(pCopyEnt) = pPlayerEntity->Position()
		                     + pPlayerEntity->GetRightDirection() * 700.0f;
*/

	Vector3 vTargetPos;
	vTargetPos = pTargetEntity->Position() - pTargetEntity->GetDirection() * 500.0f;
	const Vector3 vToTarget = vTargetPos - pCopyEnt->Position();
	
	float fDistToTarget = Vec3Length( vToTarget );
	Vector3 vDirToTarget = vToTarget / fDistToTarget;
	float accel = fDistToTarget * 0.05f * ( Vec3Dot( pCopyEnt->GetDirection(), vDirToTarget ) + 1.0f ) * 0.5f;
	Limit( accel, 20.0f, 150.0f );

	pseudo_sim.SetAccel( accel );


	// target the player's rear
	const Matrix34& my_pose = pCopyEnt->GetWorldPose();

	float pitch_dist = Vec3Dot( pCopyEnt->GetUpDirection(),    vToTarget );
	float side_dist  = Vec3Dot( pCopyEnt->GetRightDirection(), vToTarget );

	// set accel for orientation
	if( fabsf(side_dist) < 25.0f )
	{
		float pitch_accel = - pitch_dist *  0.01f;
		Limit( pitch_accel, -1.0f, 1.0f );
		pseudo_sim.SetPitchAccel( pitch_accel );
		StateLog.Update( 15, "applying pitch accel" + to_string(pitch_accel) );
	}
	else
		StateLog.Update( 15, "" );

	if( 30.0f < fabsf(side_dist) )
	{
		float roll_accel = - side_dist * 0.02f;
		Limit( roll_accel, -2.5f, 2.5f );
		pseudo_sim.SetRollAccel( roll_accel );
		StateLog.Update( 16, "applying roll accel" + to_string(roll_accel) );
	}
	else
		StateLog.Update( 16, "" );

	Fire( pCopyEnt, vToTarget );
}


void CBE_EnemyAircraft::UpdatePhysics( CCopyEntity *pCopyEnt, float dt )
{
	if( !pCopyEnt->pPhysicsActor )
	{
		assert( pCopyEnt->pPhysicsActor );
		return;
	}

	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt);
	CPseudoAircraftSimulator& pseudo_sim = ex.GetPseudoSimulator();

	// aircraft item needs to be updated for mesh bone controllers
	ex.m_pAircraft->Update( dt );

	if( ex.m_Path.IsAvailable(0) )
	{
		// entity follows the scripted motion path

		UpdateScriptedMotionPath( pCopyEnt, ex.m_Path );

		pseudo_sim.SetWorldPose( pCopyEnt->GetWorldPose() );
	}
	else
	{
		// entity is controlled by the pseudo aircraft simulator

		CJL_PhysicsActor& rPhysicsActor = *pCopyEnt->pPhysicsActor;

		pseudo_sim.Update( dt );

		// update the properties of copy entity & physics actor
		pCopyEnt->SetWorldPose( pseudo_sim.GetWorldPose() );
		pCopyEnt->SetVelocity( pseudo_sim.GetVelocity() );
		pCopyEnt->fSpeed = Vec3Length( pCopyEnt->Velocity() );

		pCopyEnt->pPhysicsActor->SetWorldPose( pseudo_sim.GetWorldPose() );
		pCopyEnt->pPhysicsActor->SetVelocity( pseudo_sim.GetVelocity() );
	}
}


void CBE_EnemyAircraft::Draw(CCopyEntity* pCopyEnt)
{
	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt);

	// When the aircraft has no hierarchical mesh,
	// CMeshBoneControllerBase::UpdateTargetMeshTransforms() has no target mesh
	// and returns without doing anything
	if( ex.m_pAircraft )
		ex.m_pAircraft->UpdateTargetMeshTransforms();

///	CBE_Enemy::Draw( pCopyEnt );

//	int shader_tech_id;
//	if( /* use high res mesh == */ true )
//		shader_tech_id = SHADER_TECH_VERTBLEND_DIRLIGHT;
//	else
//		shader_tech_id = SHADER_TECH_MESH_NOBUMP;

//	Draw3DModel( pCopyEnt, shader_tech_id );

	// use the shader technique stored in 'm_MeshProperty'
	Draw3DModel( pCopyEnt );
}


void CBE_EnemyAircraft::MessageProcedure( SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self )
{
	CBEC_EnemyAircraftExtraData& ex = GetExtraData(pCopyEnt_Self);

	switch( rGameMessage.iEffect )
	{
	case GM_SET_MOTION_PATH:
		{
		// set motion path
		EntityMotionPathRequest *pReq = (EntityMotionPathRequest *)rGameMessage.pUserData;
		ex.m_Path.SetKeyPoses( pReq->vecKeyPose );
		ex.m_Path.SetMotionPathType( pReq->MotionPathType );

//		MsgBoxFmt( "set motion path for enemy entity: %s", pCopyEnt_Self->GetName().c_str() );
		// do not send the message to the message procedure of CBE_Enemy,
		// because CBE_Enemy & CBE_EnemyAircraft have different motion path module
		return;
		}
	}

	CBE_Enemy::MessageProcedure( rGameMessage, pCopyEnt_Self );
}



bool CBE_EnemyAircraft::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_Enemy::LoadSpecificPropertiesFromFile(scanner) )
		return true;

	string item_name;
	string tgt_filter;

	if( scanner.TryScanLine( "TARGET_FILTER", tgt_filter ) )
	{
		m_TargetFilter.SetGroupName( tgt_filter );
		return true;
	}

	if( scanner.TryScanLine( "AIRCRAFT_NAME",	m_strAircraftItemName ) ) return true;

	if( scanner.TryScanLine( "WEAPON_NAME",		item_name ) )
	{
		m_vecWeaponItemName.push_back(item_name);
		return true;
	}

	if( scanner.TryScanLine( "AMMO_NAME",		item_name ) )
	{
		m_vecAmmoItemName.push_back(item_name);
		return true;
	}

	return false;
}


void CBE_EnemyAircraft::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Enemy::Serialize( ar, version );

	ar & m_TargetFilter;

	ar & m_strAircraftItemName;
	ar & m_vecWeaponItemName;
	ar & m_vecAmmoItemName;
}



void CBE_EnemyAircraft::AddExtraData()
{
	m_vecExtraData.push_back( CBEC_EnemyAircraftExtraData() );
	CBEC_EnemyAircraftExtraData& ex = m_vecExtraData.back();

	ex.m_bInUse = true;

	CGameItem *pItem = ItemDatabaseManager().GetItemRawPtr( m_strAircraftItemName, 1 );

	if( !pItem )
	{
		LOG_PRINT_ERROR( "cannot find the aircraft item: " + m_strAircraftItemName );
		return;
	}

	if( pItem->GetArchiveObjectID() != CGameItem::ID_AIRCRAFT )
	{
		LOG_PRINT_ERROR( "The item is not an aircraft: " + m_strAircraftItemName );
		return;
	}

//	CGI_Airfcraft *pAircraftItem = (CGI_Airfcraft *)pItem;
//	ex.m_pAircraft = CGI_AircraftSharedPtr( pAircraftItem );

    ex.m_pAircraft = (CGI_Aircraft *)pItem;

	if( m_MeshProperty.m_MeshObjectHandle.GetMeshObject()
	 && m_MeshProperty.m_MeshObjectHandle.GetMeshType() == CD3DXMeshObjectBase::TYPE_SMESH )
	{
		ex.m_pAircraft->ResetMeshController();
		ex.m_pAircraft->InitMeshController( (CD3DXSMeshObject *)m_MeshProperty.m_MeshObjectHandle.GetMeshObject() );
	}
}


void CBE_EnemyAircraft::OnDestroyed( CCopyEntity* pCopyEnt )
{
	CBE_Enemy::OnDestroyed(pCopyEnt);

	if( 0 < strlen(m_Wreck.GetBaseEntityName()) )
	{
		g_Log.Print( "creating a aircraft wreck" );
		CCopyEntityDesc wreck;
		wreck.pBaseEntityHandle = &m_Wreck;
		wreck.SetWorldPose( pCopyEnt->GetWorldPose() );
		wreck.vVelocity = pCopyEnt->Velocity();//Vector3(0,0,0);
		m_pStage->CreateEntity( wreck );
	}
}
