#include "GI_GravityGun.h"
#include "WeaponSystem.h"
#include "GameInput/3DActionCode.h"
#include "GameInput/InputHandler.h"
#include "GameCommon/CriticalDamping.h"
#include "Stage/CopyEntity.h"
#include "Stage/trace.h"
#include "Stage/Stage.h"

#include "JigLib/JL_PhysicsActor.h"


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

	if( m_pTarget )
	{
		// calc the translation from the center of the target to the muzzle position
		Vector3 vDist = vOwnerMuzzlePos - m_pTarget->Position();

		float fDistSq = Vec3LengthSq(vDist);
		if( m_fMaxRange * m_fMaxRange < fDistSq )
		{
			m_iHoldingTargetToggle = 0;
			m_pTarget->pPhysicsActor->SetAllowFreezing( true );
			m_pTarget = NULL;
			return;
		}

		STrace tr;
		tr.bvType = BVTYPE_DOT;
		tr.pvStart = &vOwnerMuzzlePos;
		tr.pvGoal = &m_pTarget->Position();
		tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

		// check trace
		CStageSharedPtr pStage = m_pStage.lock();
		if( pStage )
			pStage->ClipTrace( tr );

		if( tr.pTouchedEntity != m_pTarget )
		{	// found obstacle between the player and the target object
			// - unable to hold the target any more
			m_iHoldingTargetToggle = 0;
			m_pTarget->pPhysicsActor->SetAllowFreezing( true );
			m_pTarget = NULL;
			return;
		}

		if( fDistSq < m_fGraspRange * m_fGraspRange || m_aTriggerState[1] == 1 )
		{
			// the gravity gun is holding the target object

			// account for the target object's size so that it does not bump into the shooter
			float fDist = sqrtf(fDistSq);
			Vector3 vDir = vDist / fDist;		// normalization
			fDist -= ( m_pTarget->fRadius + 0.2f );
			vDist = vDir * fDist;

			// calc relative velocity
			Vector3 vRVel = m_pTarget->Velocity() - m_vMuzzleEndVelocity;

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

			m_pTarget->pPhysicsActor->SetVelocity( vForce );

/*			Vector3 vPos = m_pTarget->pPhysicsActor->GetPosition();
			Vector3 vVel = m_pTarget->pPhysicsActor->GetVelocity();
			SmoothCD( vPos, vPos + vDist, vVel, 0.25f, FPS.GetFrameTime() );

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
	if( m_pTarget && m_pTarget->pPhysicsActor )
		m_pTarget->pPhysicsActor->SetAllowFreezing( true );

	m_pTarget = NULL;
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
		!(tr.pTouchedEntity->pPhysicsActor->GetActorFlag() & JL_ACTOR_KINEMATIC) )
	{
		// locked a target
		m_pTarget = tr.pTouchedEntity;
		m_pTarget->pPhysicsActor->SetAllowFreezing( false );
		return true;
	}
	else
	{
		m_pTarget = NULL;
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

			if( m_pTarget )
			{
				Vector3 vDist = vOwnerMuzzlePos - m_pTarget->Position();

				float fDistSq = Vec3LengthSq(vDist);
				if( fDistSq < m_fGraspRange * m_fGraspRange )
				{
					Vector3 vImpulse = m_MuzzleEndLocalPose.matOrient.GetColumn(2) * m_fPower;

					// shoot object
//					m_pTarget->ApplyWorldImpulse( vImpulse, m_pTarget->Position() );
					m_pTarget->pPhysicsActor->SetVelocity( vImpulse );

					// release object
					m_pTarget->pPhysicsActor->SetAllowFreezing( true );
					m_pTarget = NULL;
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

			if( !m_pTarget )
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