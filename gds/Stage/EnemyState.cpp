#include "EnemyState.hpp"
#include "BE_Enemy.hpp"
#include "CopyEntity.hpp"


namespace amorphous
{


CEnemyState::CEnemyState()
{
}

CEnemyState::~CEnemyState()
{
}


//===============================================================================================
// CES_Search::Act()
//===============================================================================================

void CES_Search::Act( CCopyEntity& rEntity, CBE_Enemy& rBaseEntity, float dt )
{
	// first, get the extra data of 'pCopyEnt'
	SBE_EnemyExtraData* pExtraData = rBaseEntity.GetExtraData( rEntity.iExtraDataIndex );

	short& rsCurrentState = rEntity.s1;
	float& rfSensoringInterval	= rEntity.f1;
	Vector3& rvDesiredDirection  = rEntity.v1;
	float fSqDistToPlayer;

	rfSensoringInterval += dt;

	if( 0.32f <= rfSensoringInterval )
	{
		rfSensoringInterval = 0;

		// check if the player is in a visible position and update 'rvDesiredDirection' and 'rsMode'
		short sSearchResult = 0;
		rBaseEntity.SearchPlayer( &rEntity, sSearchResult, rvDesiredDirection, &fSqDistToPlayer );

		if( sSearchResult == STATE_ATTACK )
		{	// the player is in sight - engage
			rsCurrentState = STATE_ATTACK;
			rBaseEntity.UpdateDesiredYawAndPitch( &rEntity, rvDesiredDirection );
			pExtraData->vLastCheckedDirectionToPlayer = rvDesiredDirection;
			pExtraData->fLastCheckedSqDistToPlayer    = fSqDistToPlayer;
			pExtraData->vLastCheckedPlayerPosition = rvDesiredDirection * (float)sqrt(fSqDistToPlayer);
			return;
		}
		else
		{	// lost sight of the player
			if( pExtraData->vLastCheckedPlayerPosition != Vector3(0,0,0) )
			{
				Vector3 vDir = pExtraData->vLastCheckedPlayerPosition - rEntity.GetWorldPosition();
				Vec3Normalize( rvDesiredDirection, vDir );
			}
		}
	}

	rBaseEntity.SearchManeuver(&rEntity,pExtraData);
}


//===============================================================================================
// CES_Attack::Act()
//===============================================================================================

void CES_Attack::Act( CCopyEntity& rEntity, CBE_Enemy& rBaseEntity, float dt )
{
	// first, get the extra data of 'pCopyEnt'
	SBE_EnemyExtraData* pExtraData = rBaseEntity.GetExtraData( rEntity.iExtraDataIndex );

	short& rsCurrentState = rEntity.s1;
	float& rfSensoringInterval	= rEntity.f1;
	Vector3& rvDesiredDirection  = rEntity.v1;
	float fSqDistToPlayer;

	rfSensoringInterval += dt;

	if( 0.16f <= rfSensoringInterval )
	{
		rfSensoringInterval = 0;

		short sSearchResult = 0;
		// check if the player is in a visible position and update 'rvDesiredDirection' and 'rsMode'
		rBaseEntity.SearchPlayer( &rEntity, sSearchResult, rvDesiredDirection, &fSqDistToPlayer );

		if( sSearchResult == STATE_ATTACK )
		{	// update the position of the player
			rBaseEntity.UpdateDesiredYawAndPitch( &rEntity, rvDesiredDirection );
			pExtraData->vLastCheckedDirectionToPlayer = rvDesiredDirection;
			pExtraData->fLastCheckedSqDistToPlayer    = fSqDistToPlayer;
			pExtraData->vLastCheckedPlayerPosition = rvDesiredDirection * (float)sqrt(fSqDistToPlayer);
		}
		else
		{
			rsCurrentState = STATE_SEARCH;
			return;
		}
	}
	
	// aim at a certain direction
	rBaseEntity.AimAlong(&rEntity,rvDesiredDirection);

	rBaseEntity.FireAtPlayer( &rEntity );

	rBaseEntity.AttackManeuver(&rEntity,pExtraData);
}


void CES_UnderAttack::Act( CCopyEntity& rEntity, CBE_Enemy& rBaseEntity, float dt )
{
	// first, get the extra data of 'pCopyEnt'
	SBE_EnemyExtraData* pExtraData = rBaseEntity.GetExtraData( rEntity.iExtraDataIndex );

	short& rsCurrentState = rEntity.s1;
	float& rfSensoringInterval	= rEntity.f1;
	Vector3& rvDesiredDirection  = rEntity.v1;
	float fSqDistToPlayer;

	if( pExtraData->vLastHitFromThisDirection != Vector3(0,0,0) )
	{
		rvDesiredDirection = pExtraData->vLastHitFromThisDirection * (-1.0f);
		pExtraData->vLastHitFromThisDirection = Vector3(0,0,0);
	}

	rfSensoringInterval += dt;
	if( 0.16f <= rfSensoringInterval )
	{
		rfSensoringInterval = 0;

		short sSearchResult = 0;
		// check if the player is in a visible position and update 'rvDesiredDirection' and 'rsMode'
		rBaseEntity.SearchPlayer( &rEntity, sSearchResult, rvDesiredDirection, &fSqDistToPlayer );
		if( sSearchResult == STATE_ATTACK )
		{
			rsCurrentState = STATE_ATTACK;
			return;
		}
	}

	rBaseEntity.UpdateDesiredYawAndPitch(&rEntity,rvDesiredDirection);
	rBaseEntity.AimAlong(&rEntity,rvDesiredDirection);
	rBaseEntity.ManeuverUnderAttack(&rEntity,pExtraData);
}


} // namespace amorphous
