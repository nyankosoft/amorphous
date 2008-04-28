
#include "FG_AIAircraftEntity.h"
#include "FG_AIAircraftEntityDesc.h"

#include "Stage/Stage.h"
#include "Stage/CopyEntityDesc.h"
#include "Stage/PlayerInfo.h"
#include "Stage/BE_HomingMissile.h"
#include "JigLib/JL_PhysicsActor.h"
#include "Support/Log/StateLog.h"
#include "Support/memory_helpers.h"
#include "GameCommon/MTRand.h"

using namespace std;


CFG_AIAircraftEntity::CFG_AIAircraftEntity()
:
m_dLastFireTime(0.0),
m_pAircraft(NULL),
m_pCurrentTarget(NULL),
m_TimeOfLastTargetUpdate(0)
{
	m_fTargetSensoringInterval = 2.0f;
	m_fTargetSensorElapsedTime = RangedRand( 0.0f, m_fTargetSensoringInterval );
}


CFG_AIAircraftEntity::~CFG_AIAircraftEntity()
{
}


/*
void CFG_AIAircraftEntity::Init( CCopyEntityDesc& desc )
{
//	CFG_AIAircraftEntityDesc *pAircraftDesc
//		= dynamic_cast<CFG_AIAircraftEntityDesc *> (&desc);

//	if( pAircraftDesc )
//		Init( *pAircraftDesc );
}


void CFG_AIAircraftEntity::Init( CFG_AIAircraftEntityDesc& desc )
{
	desc.
}*/


void CFG_AIAircraftEntity::Update( float dt )
{
	float frametime = GetStage()->GetFrameTime();
	CPseudoAircraftSimulator& pseudo_sim = GetPseudoSimulator();

//	if( m_pAircraft )
//		m_NozzleFlames.UpdateNozzleFlames( pCopyEnt, 0, m_pAircraft );

	// if the entity has scripted motion path, update pose for the current time in stage
	if( m_Path.IsAvailable(0) )
	{
		// 19:27 10/3/2007 moved to UpdatePhysics()
		// player pose is updated in UpdatePhysics()
//		UpdateScriptedMotionPath( pCopyEnt );
		return;
	}

	m_fTargetSensorElapsedTime += frametime;
	if( m_fTargetSensoringInterval <= m_fTargetSensorElapsedTime )
	{
		m_fTargetSensorElapsedTime
			= fmodf( m_fTargetSensorElapsedTime, m_fTargetSensoringInterval );

		UpdateTarget();
	}

	if( !IsValidEntity( m_pCurrentTarget ) )
	{
		SetTarget( NULL, GetStage()->GetElapsedTime() );
		return;
	}

	CCopyEntity *pTargetEntity = m_pCurrentTarget;

	Vector3 vTargetPos;
	vTargetPos = pTargetEntity->Position() - pTargetEntity->GetDirection() * 500.0f;
	const Vector3 vToTarget = vTargetPos - this->Position();
	
	float fDistToTarget = Vec3Length( vToTarget );
	Vector3 vDirToTarget = vToTarget / fDistToTarget;
	float accel = fDistToTarget * 0.05f * ( Vec3Dot( this->GetDirection(), vDirToTarget ) + 1.0f ) * 0.5f;
	Limit( accel, 20.0f, 150.0f );

	pseudo_sim.SetAccel( accel );


	// head for the rear of the target
	const Matrix34& my_pose = this->GetWorldPose();

	float pitch_dist = Vec3Dot( this->GetUpDirection(),    vToTarget );
	float side_dist  = Vec3Dot( this->GetRightDirection(), vToTarget );

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

	Fire( vToTarget );
}


void CFG_AIAircraftEntity::UpdatePhysics( float dt )
{
	if( !this->pPhysicsActor )
	{
		assert( this->pPhysicsActor );
		return;
	}

	CPseudoAircraftSimulator& pseudo_sim = GetPseudoSimulator();

	// aircraft item needs to be updated for mesh bone controllers
	m_pAircraft->Update( dt );

	if( m_Path.IsAvailable(0) )
	{
		// entity follows the scripted motion path

		GetBaseEntity()->UpdateScriptedMotionPath( this, m_Path );

		pseudo_sim.SetWorldPose( GetWorldPose() );
	}
	else
	{
		// entity is controlled by the pseudo aircraft simulator

		CJL_PhysicsActor& rPhysicsActor = *this->pPhysicsActor;

		pseudo_sim.Update( dt );

		// update the properties of copy entity & physics actor
		this->SetWorldPose( pseudo_sim.GetWorldPose() );
		this->SetVelocity( pseudo_sim.GetVelocity() );
		this->fSpeed = Vec3Length( this->Velocity() );

		this->pPhysicsActor->SetWorldPose( pseudo_sim.GetWorldPose() );
		this->pPhysicsActor->SetVelocity( pseudo_sim.GetVelocity() );
	}
}

void CFG_AIAircraftEntity::HandleMessage( SGameMessage& msg )
{
	switch( msg.iEffect )
	{
	case GM_SET_MOTION_PATH:
		{
			// set motion path
			EntityMotionPathRequest *pReq = (EntityMotionPathRequest *)msg.pUserData;
			m_Path.SetKeyPoses( pReq->vecKeyPose );
			m_Path.SetMotionPathType( pReq->MotionPathType );

//			MsgBoxFmt( "set motion path for enemy entity: %s", this->GetName().c_str() );
			// do not send the message to the message procedure of CBE_Enemy,
			// because CBE_Enemy & CBE_EnemyAircraft have different motion path module
			return;
		}

	case GM_DAMAGE:
		this->fLife -= msg.fParam1;

//		ex->vLastHitFromThisDirection = msg.pSenderEntity->GetDirection();

//		if( rsCurrentState == CEnemyState::STATE_SEARCH )
//		{
//			rsCurrentState = CEnemyState::STATE_UNDERATTACK;
//		}

//		GAMESOUNDMANAGER.Play3D( "bosu21", this->Position() );

		if( this->fLife <= 0 )
		{
//			OnDestroyed(this);

			SGameMessage msg_destroyed( GM_DESTROYED );
			msg_destroyed.pSenderEntity = this;
			msg_destroyed.fParam1 = 100.0f;	// score ?
			SendGameMessageTo( msg_destroyed, PLAYERINFO.GetCurrentPlayerEntity() );
/*
			KillReport rep;
			rep.base_name = GetName();
			rep.entity_name = this->GetName();
			rep.score = ???;
			rep.time = m_pStage->GetElapsedTime();	ms or s ???
			rep.vWorldPos = this->Position();
			rep.kill_type = ;
			rep.type = ;
*/
			// terminate the entity

			// terminate all child entities
			const int num_children = this->GetNumChildren();
			for( int i=0; i<num_children; i++ )
			{
				this->apChild[i]->pParent = NULL;
				GetStage()->TerminateEntity( this->apChild[i] );
			}
			this->iNumChildren = 0;

			// terminate myself
			// Error: CStage::TerminateEntity() takes (CCopyEntity *&) as its argument
			// - needs to be terminated by base entity
//			GetStage()->TerminateEntity( this );
		}
		break;
}

//	GetBaseEntity()->MessageProcedure( msg, this );
}


void CFG_AIAircraftEntity::TerminateDerived()
{
	SafeDelete( m_pAircraft );

	for( size_t i=0; i<m_vecpAmmo.size(); i++ )
		SafeDelete( m_vecpWeapon[i] );

	for( size_t i=0; i<m_vecpAmmo.size(); i++ )
		SafeDelete( m_vecpAmmo[i] );
}


void CFG_AIAircraftEntity::Fire( Vector3 vToTarget )
{
	if( !IsValidEntity( m_pCurrentTarget ) )
		return; // no target

	float fDistToTarget = Vec3Length( vToTarget );
	Vector3 vDirToTarget = vToTarget / fDistToTarget;

	if( 0.3f < Vec3Dot( vDirToTarget, this->GetDirection() ) // check if the aircraft is directing to the target
	 && fDistToTarget < 25000.0f )
	{
		if( m_dLastFireTime + 10.0 < GetStage()->GetElapsedTime() )
		{
			CCopyEntityDesc missile;
			missile.SetWorldOrient( this->GetWorldPose().matOrient );
			missile.SetWorldPosition( this->Position() + this->GetDirection() * 50.0f );
			missile.vVelocity = this->Velocity();
			missile.pBaseEntityHandle = &m_Missile;
			missile.sGroupID = this->GetBaseEntity()->GetEntityGroupID( m_ProjectileEntityGroup );

			CCopyEntity* pMissileEntity = GetStage()->CreateEntity( missile );

			if( IsValidEntity(pMissileEntity) 
			 && pMissileEntity->pBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_HOMINGMISSILE )
			{
				((CBE_HomingMissile*)pMissileEntity->pBaseEntity)->Ignite(pMissileEntity);
			}

			m_dLastFireTime = GetStage()->GetElapsedTime();
		}
	}
}


void CFG_AIAircraftEntity::SelectTarget()
{
	static vector<CCopyEntity *> s_vecpEntityBuffer;
	s_vecpEntityBuffer.resize( 0 );

	static vector<CCopyEntity *> s_vecpTgtCandidateBuffer;
	s_vecpTgtCandidateBuffer.resize( 0 );

	float effective_radar_radius = 100000.0f;
	const float r = effective_radar_radius;//200000.0f;

	// cube with each edge 25[km]
	effective_radar_radius = 25000.0f;
	AABB3 aabb = AABB3(
		Vector3(-1,-1,-1) * effective_radar_radius + this->Position(),
		Vector3( 1, 1, 1) * effective_radar_radius + this->Position() );

	// set the entity group of the projectile
	// - Entities that can be hit by a projectile of this entity will be collected
	int entity_group_id = this->GetBaseEntity()->GetEntityGroupID( m_ProjectileEntityGroup );
//	int entity_group_id = this->GetBaseEntity()->GetEntityGroupID( m_TargetFilter );
	COverlapTestAABB overlap_test( aabb, &s_vecpEntityBuffer, entity_group_id );

	GetStage()->GetEntitySet()->GetOverlappingEntities( overlap_test );

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
		SetTarget(
			s_vecpTgtCandidateBuffer[ RangedRand( 0, num_candidates ) ],
			GetStage()->GetElapsedTime() );
	}
}


void CFG_AIAircraftEntity::SetTarget( CCopyEntity *pTargetEntity, double current_time )
{
	m_pCurrentTarget = pTargetEntity;
	m_TimeOfLastTargetUpdate = current_time;
}


void CFG_AIAircraftEntity::UpdateTarget()
{
	CCopyEntity* pTargetEntity = NULL;

	int targeting_policy = TP_ALWAYS_TARGET_PLAYER;

	switch( targeting_policy )
	{
	case TP_ALWAYS_TARGET_PLAYER:
		pTargetEntity = PLAYERINFO.GetCurrentPlayerEntity();
		if( IsValidEntity( pTargetEntity ) )
			m_pCurrentTarget = pTargetEntity;
		else
			m_pCurrentTarget = NULL;
		break;

	case TP_SELECT_ANY_TARGET:
	case TP_SELECT_AIR_TARGET:
	case TP_SELECT_SURFACE_TARGET:
		SelectTarget();
		break;

	default:
		break;
	}
}
