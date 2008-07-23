
#include "GameMessage.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"

#include "BE_Turret.h"
#include "BE_Floater.h"

#include "../3DCommon/fps.h"

#include "Sound/SoundManager.h"


CBE_Floater::CBE_Floater()
{
	m_BoundingVolumeType = BVTYPE_AABB;
}

void CBE_Floater::Init()
{
	CBE_Enemy::Init();
	Init3DModel();
}

void CBE_Floater::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	CBE_Enemy::InitCopyEntity(pCopyEnt);

	pCopyEnt->vVelocity = D3DXVECTOR3(0,0,0);

}

void CBE_Floater::SearchManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData)
{
	Vector3& rvDesiredDirection  = pCopyEnt->v1;
	Vector3& rvTargetPosition    = pExtraData->vTargetPosition;
	float& rfSensoringInterval2      = pExtraData->fSensoringInterval2;
	float& rfCurrentManeuverTime     = pExtraData->fCurrentManeuverTime;
	float& rfTotalManeuverTime       = pExtraData->fTotalManeuverTime;
	Vector3& rvManeuverDir       = pExtraData->vManeuverDirection;
	float fWishSpeed = 6.0f;

//	if( pCopyEnt->vVelocity == Vector3(0,0,0) )
//		return;

	ApplyFriction( pCopyEnt, 1.5f );

	if( m_iRandomSearchManeuver != 0 )
	{
		Vector3 vFromCurrentPosToDest = rvTargetPosition - pCopyEnt->Position();
		float fDist = D3DXVec3LengthSq( &vFromCurrentPosToDest );

		if( 0.20f < rfCurrentManeuverTime - rfTotalManeuverTime ||
			rvManeuverDir == Vector3(0,0,0) || fDist < 0.2f )
		{
			rfCurrentManeuverTime = 0;
			rfTotalManeuverTime = 1.0f + 0.6f * (float)rand() / (float)RAND_MAX;

			// set up a target locaion
			rvTargetPosition
				= pExtraData->vOriginalPosition
				+ Vector3(1,0,0) * ( 12.0f * (float)rand()/(float)RAND_MAX - 6.0f )
				+ Vector3(0,1,0) * (  6.0f * (float)rand()/(float)RAND_MAX - 3.0f )
				+ Vector3(0,0,1) * ( 12.0f * (float)rand()/(float)RAND_MAX - 6.0f )
				+ pCopyEnt->GetDirection() * 2.5f;

			Vec3Normalize( rvManeuverDir, rvManeuverDir );
		}
		else
			rfCurrentManeuverTime += FPS.GetFrameTime();

		if( 0.16f < rfSensoringInterval2 )
		{
			rfSensoringInterval2 = 0.0f;
			Vec3Normalize( rvManeuverDir, vFromCurrentPosToDest );
		}
		else
			rfSensoringInterval2 += FPS.GetFrameTime();

		if( 0.1f < rfTotalManeuverTime - rfCurrentManeuverTime )
		{
			float fWishSpeed = 4.5f;
			Accelerate( pCopyEnt, rvManeuverDir, fWishSpeed, 2.0f );
		}

		UpdateDesiredYawAndPitch(pCopyEnt, rvManeuverDir);
		AimAlong(pCopyEnt, rvManeuverDir);
	}

	SlideMove( pCopyEnt );
}

// performs simple maneuvers
void CBE_Floater::AttackManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData)
{
	float& rfSensoringInterval	= pCopyEnt->f1;
	Vector3& rvDesiredDirection  = pCopyEnt->v1;
	float& rfCurrentManeuverTime = pExtraData->fCurrentManeuverTime;
	float& rfTotalManeuverTime = pExtraData->fTotalManeuverTime;
	Vector3& rvManeuverDir = pExtraData->vManeuverDirection;
	float fSqDistToPlayer = pExtraData->fLastCheckedSqDistToPlayer;

	ApplyFriction( pCopyEnt, 1.8f );

	// evasive maneuver
	if( 0.20f < rfCurrentManeuverTime - rfTotalManeuverTime ||
		rvManeuverDir == Vector3(0,0,0) )
	{
		rfCurrentManeuverTime = 0;
		rfTotalManeuverTime = 0.7f + 0.5f * (float)rand() / (float)RAND_MAX;

		// set up  a new direction
		rvManeuverDir
			+= pCopyEnt->GetRightDirection()	* ( 3.0f * (float)rand()/(float)RAND_MAX - 1.5f )
			+  pCopyEnt->GetUpDirection()		* ( 3.0f * (float)rand()/(float)RAND_MAX - 1.5f )
			+  pCopyEnt->GetDirection()			* ( 1.6f * (float)rand()/(float)RAND_MAX - 0.8f );

		if( 36.0f < fSqDistToPlayer )	// if over 6m away from player
			rvManeuverDir += rvDesiredDirection * fSqDistToPlayer / 50.0f;	// move toward the player
			
		Vec3Normalize( rvManeuverDir, rvManeuverDir );
	}
	else
		rfCurrentManeuverTime += FPS.GetFrameTime();

	if( 0.5f < rfTotalManeuverTime - rfCurrentManeuverTime )
	{
//		float fWishSpeed = 12.0f;
		float fWishSpeed = 10.0f * m_fMobility;
		Accelerate( pCopyEnt, rvManeuverDir, fWishSpeed, 4.0f );
	}


	if( rfSensoringInterval == 0 )
	{	// check if there is an obstacle in the direction to which the entity is heading
		STrace tr;
		tr.bvType = BVTYPE_AABB;
		tr.aabb   = this->m_aabb;
		tr.pSourceEntity = pCopyEnt;
		tr.pvStart       = &pCopyEnt->Position();
		D3DXVECTOR3 vGoal = pCopyEnt->Position() + pCopyEnt->vVelocity * 1.2f;
		tr.pvGoal        = &vGoal;
		tr.SetAABB();
		m_pStage->ClipTrace(tr);
		// since the entity is heading for some obstacle, correct the velocity to avoid it
		if( tr.fFraction < 1.0f )
		{
			pCopyEnt->vVelocity += tr.plane.normal * (1.0f - tr.fFraction ) * 3.0f; // FPS.GetFrameTime();
		}
	}

	SlideMove( pCopyEnt );
}


/*
void CBE_Floater::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.iEffect )
	{
	case GM_DAMAGE:
		float& rfLife = pCopyEnt_Self->fLife;
		rfLife -= rGameMessage.fParam1;

		this->SoundManager().PlayAt( "bosu21", pCopyEnt_Self->Position() );

		if( rfLife <= 0 )
		{
			// show explosion animation
			this->m_pStage->CreateEntity( "Bang", pCopyEnt_Self->Position(),
				D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0) );

			this->SoundManager().PlayAt( "bom30", pCopyEnt_Self->Position() );

			// terminate myself
///			pCopyEnt_Self->Terminate();

			// leave a life item
			m_pStage->CreateEntity( "s_Life", pCopyEnt_Self->Position(),
				D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,1) );
		}
		break;
	}
}*/



bool CBE_Floater::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_Enemy::LoadSpecificPropertiesFromFile(scanner) )
		return true;

	return false;
}


void CBE_Floater::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Enemy::Serialize( ar, version );
}
