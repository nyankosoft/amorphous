
#include "GI_Weapon.h"
#include "GI_Ammunition.h"
#include "WeaponSystem.h"

#include "GameInput/3DActionCode.h"
#include "GameInput/InputHandler.h"

#include "Stage/Stage.h"
#include "Stage/CopyEntityDesc.h"
#include "Stage/PlayerInfo.h"
#include "Stage/GameMessage.h"
#include "Stage/CopyEntity.h"

#include "GameCommon/BasicGameMath.h"
#include "GameCommon/Timer.h"
#include "GameCommon/MTRand.h"

#include "3DMath/MathMisc.h"
#include "3DCommon/3DGameMath.h"

#include "Sound/SoundManager.h"


//======================================================================================
// CGI_Weapon
//======================================================================================

CStage* CGI_Weapon::ms_pStage = NULL;


void CGI_Weapon::SetData(char *pcName, char *pcAmmoType, float fFireInterval)
{
	SetName( pcName );
	strcpy( m_acAmmoType, pcAmmoType );
	m_fFireInterval = fFireInterval;
}


void CGI_Weapon::SetAmmoType( const char* pcAmmoType )
{
	strcpy( m_acAmmoType, pcAmmoType );
}


void CGI_Weapon::SetFireSoundName( const char* pcFireSoundName )
{
	m_FireSound.SetResourceName( pcFireSoundName );
}


void CGI_Weapon::UpdateWorldProperties( const Matrix34& rShooterWorldPose,
									    const Vector3& rvShooterVelocity,
										const Vector3& rvShooterAngVelocity )
{
	if( !m_pWeaponSlot )
		return;

	m_MuzzleEndWorldPose = m_pWeaponSlot->WorldPose * m_MuzzleEndLocalPose;

	m_vMuzzleEndVelocity
		= rvShooterVelocity;// + Vec3Cross( rvShooterAngVelocity, m_MuzzleEndWorldPose.vPosition - rShooterWorldPose.vPosition );
}

static bool SafetyOff() { return true; }

// standard update - fire if both triggers are pulled
void CGI_Weapon::Update( float dt )
{
	if( !IsWeaponSelected() )
		return;

	if( m_aTriggerState[0] == 1 && SafetyOff() /*m_aTriggerState[1] == 1*/ )
	{
		if( 0 < m_iCurrentBurstCount && m_iCurrentBurstCount < m_iNumBursts )
		{
			Fire();
		}
	}
}


bool CGI_Weapon::HandleInput( int input_code, int input_type, float fParam )
{
	switch( input_code )
	{
	case ACTION_ATK_FIRE:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[0] = 1;

			// the first shot is fired by trigger pull event
			if( SafetyOff()/*m_aTriggerState[1] == 1*/
				&& m_iCurrentBurstCount == 0 )
			{
				Fire();
				return true;
			}
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[0] = 0;
			m_iCurrentBurstCount = 0;
			return true;
		}
		break;

	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[1] = 1;
			return true;
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[1] = 0;
			return true;
		}
		break;

//	case GIC_MOUSE_M:
//		break;
	}

	return false;
}


void CGI_Weapon::Fire()
{
	if( !ms_pStage || !m_pWeaponSlot )
		return;

	if( !m_pWeaponSlot->pChargedAmmo )
		return;		// ammo is not loaded

//	CStageSharedPtr pStage = m_pStage.lock();
//	if( !pStage )
//		return;

	CGI_Ammunition& rCurrentAmmo = *(m_pWeaponSlot->pChargedAmmo);

	if( rCurrentAmmo.GetCurrentQuantity() == 0 )
		return;	// out of ammo

	// TODO: use a large integer for current time
	double dCurrentTime = TIMER.GetTime();
	double dTimeSinceLastFire = dCurrentTime - m_dLastFireTime;

	// return if enough time has not elapsed since the last fire
	if( dTimeSinceLastFire < (double)m_fFireInterval )
		return;

	m_dLastFireTime = dCurrentTime;
//	m_dLastFireTime += m_fFireInterval;

	// save burst shot count
	m_iCurrentBurstCount++;

	// ------------------ new fire ------------------

	const Vector3& rvMuzzleEndPosition	= m_MuzzleEndWorldPose.vPosition;
	const Matrix33& matMuzzleOrient		= m_MuzzleEndWorldPose.matOrient;
	const Vector3& rvMuzzleDir_Right	= matMuzzleOrient.GetColumn(0);
	const Vector3& rvMuzzleDir_Up		= matMuzzleOrient.GetColumn(1);
	const Vector3& rvMuzzleDirection	= matMuzzleOrient.GetColumn(2);

	// play sound for the shot
//	ms_pStage->PlaySoundAt( m_FireSound, rvMuzzleEndPosition );
	SoundManager().PlayAt( m_FireSound, rvMuzzleEndPosition );

	Vector3 vFireDirection;
	Vector3 vFireDir_Right, vUp = Vector3(0,1,0);
	CCopyEntityDesc bullet_entity;

	bullet_entity.SetWorldPosition( rvMuzzleEndPosition );

	for(int i=0; i<rCurrentAmmo.GetNumPellets(); i++)
	{
		// set pointer to the base entity which serves as the bullet of this weapon
		bullet_entity.pBaseEntityHandle = &rCurrentAmmo.GetBaseEntityHandle();

/*		vFireDirection = rvMuzzleDirection
					   + rvMuzzleDir_Right * m_fGrouping / 10.0f * RangedRand( -0.5f, 0.5f )
					   + rvMuzzleDir_Up    * m_fGrouping / 10.0f * RangedRand( -0.5f, 0.5f );
*/
		float x,y;
		GaussianRand( x, y );
		x /= 2.718f;
		y /= 2.718f;
		Limit( x, -1.0f, 1.0f );
		Limit( y, -1.0f, 1.0f );
		vFireDirection = rvMuzzleDirection * 10.0f
					   + rvMuzzleDir_Right * m_fGrouping * x
					   + rvMuzzleDir_Up    * m_fGrouping * y;

		// TODO: change vUp when vFireDirection is almost (0,1,0)
		Vec3Normalize( vFireDirection, vFireDirection );
		bullet_entity.SetWorldOrient( CreateOrientFromFwdDir(vFireDirection) );

		bullet_entity.vVelocity
			= vFireDirection * rCurrentAmmo.GetMuzzleSpeed() * m_fMuzzleSpeedFactor
			+ m_vMuzzleEndVelocity;

		// set group for the bullet
		// e.g.) projectile fired by the player is marked as 'CE_GROUP_PLAYER_PROJECTILE'
		// this is used to avoid hitting the shooter
		bullet_entity.sGroupID = m_pWeaponSlot->ProjectileGroup;

		ms_pStage->CreateEntity( bullet_entity );

		// create muzzle flash
		CCopyEntityDesc& rMuzzleFlashDesc = bullet_entity;	// reuse the desc object 
		rMuzzleFlashDesc.pBaseEntityHandle = &rCurrentAmmo.GetMuzzleFlashHandle();
		rMuzzleFlashDesc.vVelocity = m_vMuzzleEndVelocity * 0.8f; // Vector3(0,0,0);
		ms_pStage->CreateEntity( rMuzzleFlashDesc );

	}


	rCurrentAmmo.ReduceQuantity( 1 );

	// recoil effect
	SGameMessage msg;
	msg.iEffect = GM_IMPACT;
	msg.vParam = - vFireDirection * 5.0f;

/*	Vector3 vImpact = m_vLocalRecoilForce;
	msg.vParam = vImpact.z * rvMuzzleDirection
		       + vImpact.y * rvMuzzleDir_Up
			   + vImpact.x * rvMuzzleDir_Right;
*/

	SendGameMessageTo( msg, PLAYERINFO.GetCurrentPlayerBaseEntity()->GetPlayerCopyEntity() );

//	if( m_pOwnerEntity )
//		SendGameMessageTo( msg, m_pOwnerEntity );

}
