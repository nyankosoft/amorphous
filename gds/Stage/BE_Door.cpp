
#include "BE_Door.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "GameMessage.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DXMeshObject.h"

#include "Sound/SoundManager.h"

#include "3DMath/Vector3.h"

#include "JigLib/JL_PhysicsActor.h"

CBE_Door::CBE_Door()
{
	m_bNoClip = false;
	m_bNoClipAgainstMap = true;

	m_bLighting = true;

	m_fOpenLength = 1.0f;
	m_vLocalOpenDirection = Vector3(1,0,0);

//	m_acStartSound[0] = '\0';
//	m_acStopSound[0] = '\0';

	m_fSpring = 1.20f;
	m_fDamper = 0.05f;

	m_fOpenDuration = 5.0f;
}


CBE_Door::~CBE_Door()
{
}


void CBE_Door::Init()
{
	Init3DModel();

	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_DOOR;
//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;

	m_ActorDesc.ActorFlag = JL_ACTOR_KINEMATIC;
}


void CBE_Door::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	Vector3& rvOpenMotionDirection	= pCopyEnt->v1;

	rvOpenMotionDirection = pCopyEnt->GetWorldPose().matOrient * m_vLocalOpenDirection;

//	D3DXVec3TransformCoord( &rvOpenMotionDirection, &m_vLocalOpenDirection, &pCopyEnt->GetOrientation() );
//	rvOpenMotionDirection = pCopyEnt->Right();

	Vector3& rvClosedPosition		= pCopyEnt->v2;
	rvClosedPosition = pCopyEnt->Position();

	Vector3& rvFullOpenPosition		= pCopyEnt->v3;
	rvFullOpenPosition = rvClosedPosition + rvOpenMotionDirection * m_fOpenLength;

	// set state of the door
	short& sDoorState = pCopyEnt->s1;
	sDoorState = DOOR_CLOSED;

	float& rfOpenTime = pCopyEnt->f1;
	rfOpenTime = 0;

	// set activity threshold to lower values than other actors
	pCopyEnt->pPhysicsActor->SetActivityThreshold(0.02f, 20.0f);

}


void CBE_Door::Act(CCopyEntity* pCopyEnt)
{
//	CCopyEntity *pDoorController = pCopyEnt->GetParent();
//	if( !pDoorController )
//		return;

//	short		 sDoorState		= pDoorController->s1;
	short&	 	 sDoorState		= pCopyEnt->s1;
	Vector3& rvDoorPosition = pCopyEnt->Position();	//current center position of the door
	Vector3& rvOpenMotionDirection = pCopyEnt->v1;
	Vector3& rvClosedPosition      = pCopyEnt->v2;
	Vector3& rvFullOpenPosition    = pCopyEnt->v3;
	float&		 rfOpenTime		= pCopyEnt->f1;

	Vector3 vToCover, vImpulse;
	Vector3 vCovered, vForce;

	switch( sDoorState )
	{
	case DOOR_CLOSED:
		break;

	case DOOR_CLOSING:
		vToCover = rvClosedPosition - rvDoorPosition;
		vCovered = rvDoorPosition - rvFullOpenPosition;

		if( Vec3LengthSq( vToCover ) < 0.0001f 
			|| 0 < Vec3Dot( vToCover, rvOpenMotionDirection ) )
		{	// almost reached or passed the closed position
			rvDoorPosition = rvClosedPosition;
			pCopyEnt->pPhysicsActor->SetPosition(rvDoorPosition);
			pCopyEnt->pPhysicsActor->SetVelocity( Vector3(0,0,0) );
			pCopyEnt->SetVelocity( Vector3(0,0,0) );
			sDoorState = DOOR_CLOSED;
///			this->SoundManager().PlayAt( m_acStopSound, rvClosedPosition );
			return;
		}

		if( Vec3LengthSq(vCovered) < 0.3f * 0.3f )
//			vImpulse = - rvOpenMotionDirection * ( Vec3LengthSq(vCovered) + 0.02f ) * 30.0f;
			vImpulse = - rvOpenMotionDirection * ( 0.3f * 0.3f + 0.02f ) * 30.0f;
		else if( Vec3LengthSq(vToCover) < 0.3f * 0.3f )
			vImpulse = - rvOpenMotionDirection * ( Vec3LengthSq(vToCover) + 0.02f ) * 30.0f;
		else
			vImpulse = - rvOpenMotionDirection * ( 0.3f * 0.3f + 0.02f ) * 30.0f;

///		vImpulse = m_fSpring * vToCover - m_fDamper * pCopyEnt->Velocity();
//		pCopyEnt->ApplyWorldImpulse( vImpulse, rvDoorPosition );
		pCopyEnt->SetVelocity( vImpulse );
		pCopyEnt->pPhysicsActor->SetVelocity( vImpulse );

//		vForce = m_fSpring * vToCover - m_fDamper * pCopyEnt->Velocity();
//		pCopyEnt->pPhysicsActor->AddWorldForce( vForce ); 
//		pCopyEnt->pPhysicsActor->ApplyWorldImpulse( Vector3(0,0,0) );	// wake up
		break;

	case DOOR_OPEN:
		rfOpenTime += m_pStage->GetFrameTime();

		if( rfOpenTime < m_fOpenDuration )
			return;

		// The door starts to close. 
		rfOpenTime = 0;
		sDoorState = DOOR_CLOSING;
///		this->SoundManager().PlayAt( m_acStartSound, rvDoorPosition );
		break;

	case DOOR_OPENING:
		vToCover = rvFullOpenPosition - rvDoorPosition;
		vCovered = rvDoorPosition - rvClosedPosition;

		if( Vec3LengthSq( vToCover ) < 0.0001f
			|| Vec3Dot( vToCover, rvOpenMotionDirection ) < 0 )
		{
			rvDoorPosition = rvFullOpenPosition;
			pCopyEnt->pPhysicsActor->SetPosition(rvDoorPosition);
			pCopyEnt->pPhysicsActor->SetVelocity( Vector3(0,0,0) );
			pCopyEnt->SetVelocity( Vector3(0,0,0) );
			sDoorState = DOOR_OPEN;
///			this->SoundManager().PlayAt( m_acStopSound, rvFullOpenPosition );
			return;
		}

		if( Vec3LengthSq(vCovered) < 0.3f * 0.3f )
//			vImpulse = rvOpenMotionDirection * ( Vec3LengthSq(vCovered) + 0.02f ) * 30.0f;
			vImpulse = rvOpenMotionDirection * ( 0.3f * 0.3f + 0.02f ) * 30.0f;
		else if( Vec3LengthSq(vToCover) < 0.3f * 0.3f )
			vImpulse = rvOpenMotionDirection * ( Vec3LengthSq(vToCover) + 0.02f ) * 30.0f;
		else
			vImpulse = rvOpenMotionDirection * ( 0.3f * 0.3f + 0.02f ) * 30.0f;

///		vImpulse = m_fSpring * vToCover - m_fDamper * pCopyEnt->Velocity();
//		pCopyEnt->ApplyWorldImpulse( vImpulse, rvDoorPosition );
		pCopyEnt->SetVelocity( vImpulse );
		pCopyEnt->pPhysicsActor->SetVelocity( vImpulse );

//		vForce = m_fSpring * vToCover - m_fDamper * pCopyEnt->Velocity();
//		pCopyEnt->pPhysicsActor->AddWorldForce( vForce );
//		pCopyEnt->pPhysicsActor->ApplyWorldImpulse( Vector3(0,0,0) );	// wake up
		break;
	}

}


void CBE_Door::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
//	short& rsDoorState = pCopyEnt_Self->s1;

	if( pCopyEnt_Other && pCopyEnt_Other->bNoClip == false )
	{	// door is touched by a collidable entity
		// send message to the parent door controller
		CCopyEntity *pController = pCopyEnt_Self->GetParent();
		if( !pController )
			return;

		SGameMessage msg;
		msg.iEffect = GM_DOOR_TOUCHED;
		msg.pSenderEntity = pCopyEnt_Self;
		msg.pEntity0      = pCopyEnt_Other;
		// door controller may need access to the entity that touched the door
		// bacause it may a send key code request to the entity if it is locked
		SendGameMessageTo( msg, pController );
	}

}


void CBE_Door::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	short& rsDoorState = pCopyEnt_Self->s1;

/*	switch( rGameMessage.iEffect )
	{
	case GM_KEYCODE_INPUT:
		if( rsDoorState == DOOR_OPENING )
			return; // the door has already been started to open - no need to try opening

		if( strcmp(rGameMessage.pcStrParam, m_acKeyCode) == 0 )
		{	// valid key-code - open the door
			if( rsDoorState == DOOR_CLOSED )
				this->SoundManager().PlayAt( m_acStartSound, pCopyEnt_Self->Position() );
			rsDoorState = DOOR_OPENING;
		}
		break;
	}*/
}


void CBE_Door::Draw(CCopyEntity* pCopyEnt)
{
	Draw3DModel(pCopyEnt);
}


bool CBE_Door::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( scanner ) )
		return true;

	if( scanner.TryScanLine( "SLD_WIDTH", m_fOpenLength ) ) return true;

	if( scanner.TryScanLine( "OPEN_DIRECTION", m_vLocalOpenDirection ) ) return true;

//	if( scanner.TryScanLine( "KEYCODE", m_acKeyCode ) ) return true;

//	if( scanner.TryScanLine( "START_SND", m_acStartSound ) ) return true;
//	if( scanner.TryScanLine( "STOP_SND", m_acStopSound ) ) return true;

	if( scanner.TryScanLine( "DOOR_SPRING", m_fSpring ) ) return true;

	if( scanner.TryScanLine( "DOOR_DAMPER", m_fDamper ) ) return true;

	return false;
}


void CBE_Door::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );

	ar & m_fOpenLength & m_vLocalOpenDirection;

	ar & m_fSpring & m_fDamper;
}
