#include "BE_Platform.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "GameMessage.hpp"
#include "amorphous/Sound/SoundManager.hpp"
#include "amorphous/Physics/Actor.hpp"


namespace amorphous
{

using namespace physics;


CBE_Platform::CBE_Platform()
{
	m_bNoClip = false;
	m_bNoClipAgainstMap = true;

	SetLighting( true );

//	m_fSpring = 1.20f;
//	m_fDamper = 0.05f;

//	m_fOpenDuration = 5.0f;
}


CBE_Platform::~CBE_Platform()
{
}


void CBE_Platform::Init()
{
	Init3DModel();

	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_DOOR;
//	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;

//	m_ActorDesc.ActorFlag = JL_ACTOR_KINEMATIC;
	m_ActorDesc.BodyDesc.Flags = BodyFlag::Kinematic;
}


void CBE_Platform::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	// set state of the door
	short& rState = pCopyEnt->s1;
	rState = CBE_Platform::STATE_REST;

	float& rfRestTime = pCopyEnt->f4;
	rfRestTime = 0;

	// set activity threshold to lower values than other actors
//	pCopyEnt->pPhysicsActor->SetActivityThreshold(0.02f, 20.0f);

}


void CBE_Platform::Act(CCopyEntity* pCopyEnt)
{
	float frametime = m_pStage->GetFrameTime();

	short& rState = pCopyEnt->s1;
	float& rfRestTime = pCopyEnt->f4;
	float& rfTarget = pCopyEnt->f5;
//	float&		 rfOpenTime		= pCopyEnt->f1;

	Vector3 vToCover, vImpulse;
//	Vector3 vCovered, vForce;

	Vector3 vDestPos, vDestDir;

	physics::CActor *pPhysicsActor = pCopyEnt->GetPrimaryPhysicsActor();

	switch( rState )
	{
	case CBE_Platform::STATE_MOVING:
		if( rfTarget == 1.0f )
			vDestPos = pCopyEnt->v1;
		else
			vDestPos = pCopyEnt->v2;


		vToCover = vDestPos - pCopyEnt->GetWorldPosition();
//		vCovered = rvDoorPosition - rvFullOpenPosition;

		if( Vec3LengthSq( vToCover ) < 0.0005f )
//			|| 0 < Vec3Dot( vToCover, rvOpenMotionDirection ) )
		{	// almost reached or passed the closed position
			pCopyEnt->SetWorldPosition( vDestPos );
			if( pPhysicsActor )
			{
				pPhysicsActor->SetWorldPosition( vDestPos );
				pPhysicsActor->SetLinearVelocity( Vector3(0,0,0) );
			}
			pCopyEnt->SetVelocity( Vector3(0,0,0) );
			rState = CBE_Platform::STATE_REST;
///			SoundManager.PlayAt( m_acStopSound, rvClosedPosition );
			return;
		}

		Vec3Normalize( vDestDir, vToCover );

		vImpulse = vDestDir * ( 0.3f * 0.3f + 0.02f ) * 30.0f;

/*		if( Vec3LengthSq(vCovered) < 0.3f * 0.3f )
//			vImpulse = - rvOpenMotionDirection * ( Vec3LengthSq(vCovered) + 0.02f ) * 30.0f;
			vImpulse = vDestDir * ( 0.3f * 0.3f + 0.02f ) * 30.0f;
		else if( Vec3LengthSq(vToCover) < 0.3f * 0.3f )
			vImpulse = vDestDir * ( Vec3LengthSq(vToCover) + 0.02f ) * 30.0f;
		else
			vImpulse = vDestDir * ( 0.3f * 0.3f + 0.02f ) * 30.0f;
*/
///		vImpulse = m_fSpring * vToCover - m_fDamper * pCopyEnt->Velocity();
//		pCopyEnt->ApplyWorldImpulse( vImpulse, rvDoorPosition );

		pCopyEnt->SetVelocity( vImpulse );
		if( pPhysicsActor )
			pPhysicsActor->SetLinearVelocity( vImpulse );

//		vForce = m_fSpring * vToCover - m_fDamper * pCopyEnt->Velocity();
//		pCopyEnt->pPhysicsActor->AddWorldForce( vForce ); 
//		pCopyEnt->pPhysicsActor->ApplyWorldImpulse( Vector3(0,0,0) );	// wake up

		break;

	case CBE_Platform::STATE_REST:
		rfRestTime += frametime;
		break;
	}

}


void CBE_Platform::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
//	short& rsDoorState = pCopyEnt_Self->s1;
/*
	if( pCopyEnt_Other && pCopyEnt_Other->bNoClip == false )
	{	// door is touched by a collidable entity
		// send message to the parent door controller
		CCopyEntity *pController = pCopyEnt_Self->GetParent();
		if( !pController )
			return;

		GameMessage msg;
		msg.effect = GM_DOOR_TOUCHED;
		msg.sender = pCopyEnt_Self->Self();
		msg.pEntity0      = pCopyEnt_Other;
		// door controller may need access to the entity that touched the door
		// bacause it may a send key code request to the entity if it is locked
		SendGameMessageTo( msg, pController );
	}
*/
}


void CBE_Platform::MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	short& rState = pCopyEnt_Self->s1;
	float& rfTarget = pCopyEnt_Self->f5;
	float& rfRestTime = pCopyEnt_Self->f4;

	switch( rGameMessage.effect )
	{
	case GM_ENTITY_DETECTED_BY_SENSOR:
		if( rState == CBE_Platform::STATE_REST &&
			3.0f <= rfRestTime )
		{
			// set the target id
//			rfTarget = (float)rGameMessage.s1;
			rState = CBE_Platform::STATE_MOVING;
			rfRestTime = 0;

			// decide the destination
			// a destination which is the closest to the sensor will be selected

			std::shared_ptr<CCopyEntity> pSenderEntity = rGameMessage.sender.Get();
			if( !pSenderEntity )
				return;

			Vector3 vSensorPos = pSenderEntity->GetWorldPosition();
			float dist1 = Vec3LengthSq( pCopyEnt_Self->v1 - vSensorPos );
			float dist2 = Vec3LengthSq( pCopyEnt_Self->v2 - vSensorPos );
			if( dist1 < dist2 )
			{
				rfTarget = 1.0f;	// choose pEntity->v1 as a destination
			}
			else
			{
				rfTarget = 2.0f;
			}
		}
		else
		{
			// already moving toward another target position
		}
		break;
	}
}


void CBE_Platform::Draw(CCopyEntity* pCopyEnt)
{
	Draw3DModel(pCopyEnt);
}


bool CBE_Platform::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( scanner );

//	if( scanner.TryScanLine( "SLD_WIDTH", m_fOpenLength ) ) return true;

//	if( scanner.TryScanLine( "OPEN_DIRECTION", m_vLocalOpenDirection ) ) return true;

//	if( scanner.TryScanLine( "KEYCODE", m_acKeyCode ) ) return true;

//	if( scanner.TryScanLine( "START_SND", m_StartSound ) ) /* set sound name to sound handle */return true;
//	if( scanner.TryScanLine( "STOP_SND", m_StopSound ) ) /* set sound name to sound handle */return true;

//	if( scanner.TryScanLine( "DOOR_SPRING", m_fSpring ) ) return true;
//	if( scanner.TryScanLine( "DOOR_DAMPER", m_fDamper ) ) return true;

	return false;
}


void CBE_Platform::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );

//	ar & m_fOpenLength & m_vLocalOpenDirection;
//	ar & m_fSpring & m_fDamper;
//	ar & 
}


} // namespace amorphous
