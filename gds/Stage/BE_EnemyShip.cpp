#include "BE_EnemyShip.h"
#include "GameMessage.h"
#include "CopyEntity.h"
#include "CopyEntityDesc.h"
#include "trace.h"
#include "Stage.h"
#include "PlayerInfo.h"

#include "3DMath/MathMisc.h"
#include "Physics/Actor.h"
#include "Item/ItemDatabaseManager.h"

#include "GameCommon/MTRand.h"

#include "Item/GI_Weapon.h"
#include "Item/GI_Ammunition.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Log/StateLog.h"
#include "Support/StringAux.h"

using namespace physics;


void CBEC_EnemyShipExtraData::Release()
{
//	SafeDelete( m_pShip );
	SafeDeleteVector( m_vecpWeapon );
	SafeDeleteVector( m_vecpAmmo );
}


CBE_EnemyShip::CBE_EnemyShip()
{
	m_BoundingVolumeType = BVTYPE_AABB;

	// register to physics simulator for UpdatePhysics() to be called
	RaiseEntityFlag( BETYPE_RIGIDBODY );

	// register as no-clip to physics simulator
	// and do the collision detection by itself
	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_NOCLIP;

//	m_ActorDesc.ActorFlag |= JL_ACTOR_KINEMATIC;
	m_ActorDesc.BodyDesc.Flags = BodyFlag::Kinematic;

	m_bNoClipAgainstMap = true;
}


CBE_EnemyShip::~CBE_EnemyShip()
{
	size_t i, num_extra_data = m_vecExtraData.size();
	for( i=0; i<num_extra_data; i++ )
		m_vecExtraData[i].Release();
}


void CBE_EnemyShip::Init()
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


void CBE_EnemyShip::InitCopyEntity(CCopyEntity* pCopyEnt)
{
//	CBE_Enemy::InitCopyEntity(pCopyEnt);
	pCopyEnt->fLife = m_fLife;
//	pCopyEnt->GroupIndex = CE_GROUP_ENEMY;

	// create a new state for the copy entity
	pCopyEnt->iExtraDataIndex = GetNewExtraDataIndex();

	CBEC_EnemyShipExtraData& ex = GetExtraData(pCopyEnt);

//	CPseudoAircraftSimulator& pseudo_sim = ex.GetPseudoSimulator();

//	pseudo_sim.SetWorldPose( pCopyEnt->GetWorldPose() );
//	pseudo_sim.SetForwardVelocity( pCopyEnt->Velocity() );

//	if( pCopyEnt->pPhysicsActor )
//		pCopyEnt->pPhysicsActor->SetAllowFreezing( false );

	// random last fire time for each enemy
	ex.m_dLastFireTime = m_pStage->GetElapsedTime() - RangedRand( 0.0f, 5.0f );
}

/*
void CBE_EnemyShip::SearchManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData){}

// performs simple maneuvers
void CBE_EnemyShip::AttackManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData){}
*/


void CBE_EnemyShip::Fire( CCopyEntity* pCopyEnt, Vector3 vToTarget )
{
	CBEC_EnemyShipExtraData& ex = GetExtraData(pCopyEnt);

	float fDistToTarget = Vec3Length( vToTarget );
	Vector3 vDirToTarget = vToTarget / fDistToTarget;

	if( 0.0f < Vec3Dot( vDirToTarget, pCopyEnt->GetDirection() )
	 && fDistToTarget < 25000.0f )
	{
		if( ex.m_dLastFireTime + 10.0 < m_pStage->GetElapsedTime() )
		{
			CCopyEntityDesc missile;
			missile.SetWorldOrient( pCopyEnt->GetWorldPose().matOrient );
			missile.SetWorldPosition( pCopyEnt->Position() + pCopyEnt->GetDirection() * 50.0f );
			missile.vVelocity = pCopyEnt->Velocity();
			missile.pBaseEntityHandle = &m_Missile;
			missile.sGroupID = GetEntityGroupID( m_ProjectileEntityGroup ); // CE_GROUP_ENEMY_PROJECTILE;

			m_pStage->CreateEntity( missile );

			ex.m_dLastFireTime = m_pStage->GetElapsedTime();
		}
	}
}

/*
void CBE_EnemyShip::UpdateScriptedMotionPath( CCopyEntity* pCopyEnt )
{
	CBEC_EnemyShipExtraData& ex = GetExtraData(pCopyEnt);

	const float time_in_stage = (float)m_pStage->GetElapsedTime();

	if( ex.m_Path.IsAvailable(time_in_stage) )
	{
		// follow the scripted path
		Matrix34 world_pose = ex.m_Path.GetPose( time_in_stage );
		pCopyEnt->SetWorldPose( world_pose );
		if( pCopyEnt->pPhysicsActor )
			pCopyEnt->pPhysicsActor->SetWorldPose( world_pose );

		// calc velocity - used when the entity is destroyed
		// and the vel of the frags have to be calculated
		float dt = m_pStage->GetFrameTime();
		pCopyEnt->Velocity() = ( pCopyEnt->Position() - pCopyEnt->vPrevPosition ) / dt;
	}
	else
	{
		// done with the scripted motion
		ex.m_Path.ReleaseMotionPath();
	}
}*/


void CBE_EnemyShip::Act( CCopyEntity* pCopyEnt )
{
	CBEC_EnemyShipExtraData& ex = GetExtraData(pCopyEnt);
//	CPseudoAircraftSimulator& pseudo_sim = ex.GetPseudoSimulator();

	// if the entity has scripted motion path, update pose for the current time in stage
	if( ex.m_Path.IsAvailable(0) )
	{
		UpdateScriptedMotionPath( pCopyEnt, ex.m_Path );
//		UpdateScriptedMotionPath( pCopyEnt );
		return;
	}

	CCopyEntity* pPlayerEntity = PLAYERINFO.GetCurrentPlayerEntity();

	if( !pPlayerEntity )
		return;
}


void CBE_EnemyShip::UpdatePhysics( CCopyEntity *pCopyEnt, float dt )
{
	if( !pCopyEnt->pPhysicsActor )
	{
//		assert( pCopyEnt->pPhysicsActor );
		return;
	}

	CBEC_EnemyShipExtraData& ex = GetExtraData(pCopyEnt);
//	CPseudoAircraftSimulator& pseudo_sim = ex.GetPseudoSimulator();

	if( ex.m_Path.IsAvailable(0) )
	{
//		pseudo_sim.SetWorldPose( pCopyEnt->GetWorldPose() );
		return;
	}
}


void CBE_EnemyShip::MessageProcedure( SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self )
{
	CBEC_EnemyShipExtraData& ex = GetExtraData(pCopyEnt_Self);

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
		// because CBE_Enemy & CBE_EnemyShip have different motion path module
		return;
		}
	}

	CBE_Enemy::MessageProcedure( rGameMessage, pCopyEnt_Self );
}



bool CBE_EnemyShip::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_Enemy::LoadSpecificPropertiesFromFile(scanner) )
		return true;

	string item_name;

	if( scanner.TryScanLine( "AIRCRAFT_NAME",	m_strShipItemName ) ) return true;

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


void CBE_EnemyShip::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Enemy::Serialize( ar, version );

	ar & m_strShipItemName;
	ar & m_vecWeaponItemName;
	ar & m_vecAmmoItemName;
}



void CBE_EnemyShip::AddExtraData()
{
	m_vecExtraData.push_back( CBEC_EnemyShipExtraData() );
	CBEC_EnemyShipExtraData& ex = m_vecExtraData.back();

	ex.m_bInUse = true;
/*
	CGameItem *pItem = CItemDatabaseManager::Get()->GetItem( m_strShipItemName.c_str(), 1 );

	if( !pItem )
	{
		g_Log.Print( WL_ERROR, "CBE_EnemyShip::AddExtraData() - cannot find the aircraft item: %s", m_strShipItemName.c_str() );
		return;
	}

	if( pItem->GetArchiveObjectID() != CGameItem::ID_AIRCRAFT )
	{
		g_Log.Print( WL_ERROR, "CBE_EnemyShip::AddExtraData() - the item '%s' is not an aircraft", m_strShipItemName.c_str() );
		return;
	}
*/
//	ex.m_pAircraft = (CGI_Aircraft *)pItem;
}


void CBE_EnemyShip::OnDestroyed( CCopyEntity* pCopyEnt )
{
	CBE_Enemy::OnDestroyed(pCopyEnt);

	if( 0 < strlen(m_Wreck.GetBaseEntityName()) )
	{
		g_Log.Print( "creating a shipwreck" );
		CCopyEntityDesc wreck;
		wreck.pBaseEntityHandle = &m_Wreck;
		wreck.SetWorldPose( pCopyEnt->GetWorldPose() );
		wreck.vVelocity = Vector3(0,0,0);
		m_pStage->CreateEntity( wreck );
	}
}
