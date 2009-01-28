#include "GI_GravityGun.h"
#include "WeaponSystem.h"
#include "GameInput/3DActionCode.h"
#include "GameInput/InputHandler.h"
#include "GameCommon/CriticalDamping.h"
#include "Stage/CopyEntity.h"
#include "Stage/trace.h"
#include "Stage/Stage.h"
#include "XML/XMLNodeReader.h"

#include "Physics/Actor.h"

using namespace physics;


/*
CGI_GravityGun::~CGI_GravityGun()
{
}*/


void CGI_GravityGun::Update( float dt )
{
	if( !IsWeaponSelected() )
		return;

//	Vector3 vOwnerMuzzlePos = rWeaponSystem.m_vMuzzlePosition + rWeaponSystem.m_vMuzzleDirection * 0.25f
//		                                                      + rWeaponSystem.m_vMuzzleDir_Up * 0.90f;

    Vector3 vOwnerMuzzlePos = m_MuzzleEndWorldPose.vPosition;

	CCopyEntity *pTarget = m_Target.GetRawPtr();
	if( pTarget )
	{
		// calc the translation from the center of the target to the muzzle position
		Vector3 vDist = vOwnerMuzzlePos - pTarget->Position();

		float fDistSq = Vec3LengthSq(vDist);
		if( m_fMaxRange * m_fMaxRange < fDistSq )
		{
			m_iHoldingTargetToggle = 0;
//			m_pTarget->pPhysicsActor->SetAllowFreezing( true );
			m_Target.Reset();
			return;
		}

		STrace tr;
		tr.bvType = BVTYPE_DOT;
		tr.pvStart = &vOwnerMuzzlePos;
		tr.pvGoal = &pTarget->Position();
		tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
/*
		CTrace tr;
		tr.BVType    = BVTYPE_DOT;
		tr.vStart    = &vOwnerMuzzlePos;
		tr.vGoal     = &pTarget->Position();
		tr.TypeFlags = CTrace::FLAG_IGNORE_NOCLIP_ENTITIES;//TRACETYPE_IGNORE_NOCLIP_ENTITIES;
*/
		// check trace
		CStageSharedPtr pStage = m_pStage.lock();
		if( pStage )
			pStage->ClipTrace( tr );

		if( tr.pTouchedEntity != pTarget )
		{
			// found an obstacle between the player and the target object
			// - unable to hold the target any more
			m_iHoldingTargetToggle = 0;
//			m_pTarget->pPhysicsActor->SetAllowFreezing( true );
			m_Target.Reset();
			return;
		}

		if( fDistSq < m_fGraspRange * m_fGraspRange || m_aTriggerState[1] == 1 )
		{
			// the gravity gun is holding the target object

			// account for the target object's size so that it does not bump into the shooter
			float fDist = sqrtf(fDistSq);
			Vector3 vDir = vDist / fDist;		// normalization
			fDist -= ( pTarget->fRadius + 0.2f );
			vDist = vDir * fDist;

			// calc relative velocity
			Vector3 vRVel = pTarget->Velocity() - m_vMuzzleEndVelocity;

			Vector3 vForce;
//			vForce = m_fPosGain * vDist - m_fSpeedGain * vRVel;
//			m_pTarget->ApplyWorldImpulse( vForce, m_pTarget->Position());


			if( 6.0f < fDist )
				vForce = vDir * 6.0f * 2.5f;
			else if( fDist < 0.6f )
				vForce = vDir * fDist * 8.0f;
			else
//				vForce = vDir * fDist * 2.5f;
				vForce = vDir * ( fDist * 2.0f + 3.0f );

			vForce += m_vMuzzleEndVelocity;

			pTarget->pPhysicsActor->SetLinearVelocity( vForce );

/*			Vector3 vPos = m_pTarget->pPhysicsActor->GetPosition();
			Vector3 vVel = m_pTarget->pPhysicsActor->GetVelocity();
			SmoothCD( vPos, vPos + vDist, vVel, 0.25f, dt );

			if( 12.0f * 12.0f < Vec3LengthSq(vVel) )
			{
				Vec3Normalize( vVel, vVel );
				vVel *= 12.0f;
			}
			m_pTarget->pPhysicsActor->SetVelocity( vVel );
*/
			return;
		}
		else
		{
			// lost control of the target
			m_iHoldingTargetToggle = 0;

			ReleaseObject();

			return;
		}
	}
}


/*
// standard update - fire if both triggers are pulled
void CGI_GravityGun::Update( float dt )
{
	if( m_aTriggerState[0] == 1 && m_aTriggerState[1] == 1 )
	{
		if( 0 < m_iCurrentBurstCount && m_iCurrentBurstCount < m_iNumBursts )
		{
			Fire();
		}
	}
}*/


void CGI_GravityGun::ReleaseObject()
{
//	if( m_pTarget && m_pTarget->pPhysicsActor )
//		m_pTarget->pPhysicsActor->SetAllowFreezing( true );

	m_Target.Reset();
}


bool CGI_GravityGun::GraspObjectInAimDirection()
{
	Vector3 vMuzzleEndPos = m_MuzzleEndLocalPose.vPosition;
	Vector3 vGoal = vMuzzleEndPos + m_MuzzleEndLocalPose.matOrient.GetColumn(2) * m_fMaxRange;

	STrace tr;
	tr.pvStart = &vMuzzleEndPos;
	tr.pvGoal = &vGoal;
	tr.bvType = BVTYPE_DOT;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

	// check trace
	CStageSharedPtr pStage = m_pStage.lock();
	if( pStage )
		pStage->ClipTrace( tr );

	if( tr.pTouchedEntity &&
		tr.pTouchedEntity->EntityFlag & BETYPE_RIGIDBODY &&
//		!(tr.pTouchedEntity->pPhysicsActor->GetActorFlag() & JL_ACTOR_KINEMATIC) )
		(tr.pTouchedEntity->pPhysicsActor->GetMass() < 1000000.0f ) )
	{
		// locked a target
		m_Target = CEntityHandle<>( tr.pTouchedEntity->Self() );
//		m_pTarget->pPhysicsActor->SetAllowFreezing( false );
		return true;
	}
	else
	{
		m_Target.Reset();
		return true;	// input is treated as being processed even if no object is locked
	}
}


bool CGI_GravityGun::HandleInput( int input_code, int input_type, float fParam )
{
	Vector3 vOwnerMuzzlePos = m_MuzzleEndWorldPose.vPosition;

	switch( input_code )
	{
	case ACTION_ATK_FIRE:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[0] = 1;

			CCopyEntity *pTarget = m_Target.GetRawPtr();
			if( pTarget )
			{
				Vector3 vDist = vOwnerMuzzlePos - pTarget->Position();

				float fDistSq = Vec3LengthSq(vDist);
				if( fDistSq < m_fGraspRange * m_fGraspRange )
				{
					Vector3 vImpulse = m_MuzzleEndLocalPose.matOrient.GetColumn(2) * m_fPower;

					// shoot object
//					pTarget->ApplyWorldImpulse( vImpulse, m_pTarget->Position() );
					pTarget->pPhysicsActor->SetLinearVelocity( vImpulse );

					// release object
//					m_pTarget->pPhysicsActor->SetAllowFreezing( true );
					m_Target.Reset();
					m_iHoldingTargetToggle = 0;
					return true;
				}
			}
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[0] = 0;
			return true;
		}
		break;

	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[1] = 1;

			// the owner pulled the second trigger
			m_iHoldingTargetToggle = ~m_iHoldingTargetToggle;

			CCopyEntity *pTarget = m_Target.GetRawPtr();
			if( !pTarget )
			{
				// trigger is pulled and the gravity gun is not holding any object right now
				// - check if there is an object in the aim direction
				return GraspObjectInAimDirection();
			}
			else
			{
				// trigger is pulled when the gun is holding an object
				// - release the object
				ReleaseObject();
				return true;
			}
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[1] = 0;
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}


void CGI_GravityGun::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGI_Weapon::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "PosGain",   m_fPosGain );
	reader.GetChildElementTextContent( "SpeedGain", m_fSpeedGain );

	reader.GetChildElementTextContent( "MaxRange",   m_fMaxRange );
	reader.GetChildElementTextContent( "GraspRange", m_fGraspRange );
	reader.GetChildElementTextContent( "Power",      m_fPower );
}
