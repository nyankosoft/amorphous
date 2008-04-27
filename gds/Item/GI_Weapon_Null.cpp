
#include "GI_Weapon.h"
#include "GI_Ammunition.h"
#include "WeaponSystem.h"

#include "Stage/Stage.h"
#include "Stage/CopyEntityDesc.h"
#include "GameCommon/BasicGameMath.h"


//======================================================================================
// CGI_Weapon
//======================================================================================

CStage* CGI_Weapon::ms_pStage;


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
	m_FireSound.SetSoundName( pcFireSoundName );
}


// standard update - fire if both triggers are pulled
void CGI_Weapon::Update( float dt )
{
}


bool CGI_Weapon::HandleInput( int input_code, int input_type, float fParam )
{
	return false;
}

/*
void CGI_Weapon::Fire( SWeaponSlot& rWeaponSlot,
					Vector3& rvMuzzlePosition, Vector3& rvMuzzleDirection,
		            Vector3& rvMuzzleDir_Right, Vector3& rvMuzzleDir_Up,
					Vector3& rvShooterVelocity )
{
}
*/