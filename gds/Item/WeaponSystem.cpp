#include "WeaponSystem.hpp"

#include "GameCommon/3DActionCode.hpp"
#include "Stage/Stage.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/CopyEntityDesc.hpp"


//======================================================================================
// CWeaponSystem
//======================================================================================

CWeaponSystem::CWeaponSystem()
{
	m_PrimaryWeaponIndex   = 0;
	m_SecondaryWeaponIndex = -1;

	m_WeaponUpdateMode = MODE_UPDATE_SELECTED_WEAPONS;
}


void CWeaponSystem::Init( CStage* pStage )
{
}


CWeaponSystem::~CWeaponSystem()
{
}


void CWeaponSystem::Update( float dt )
{
	switch( GetWeaponUpdateMode() )
	{
	case MODE_UPDATE_SELECTED_WEAPONS: // update only the primary and secondary weapons
		if( 0 <= m_PrimaryWeaponIndex )
		{
			CGI_Weapon *pWeapon = m_aWeaponSlot[m_PrimaryWeaponIndex].pWeapon;
			if( pWeapon )
				pWeapon->Update( dt );
		}

		if( 0 <= m_SecondaryWeaponIndex )
		{
			CGI_Weapon *pWeapon = m_aWeaponSlot[m_SecondaryWeaponIndex].pWeapon;
			if( pWeapon )
				pWeapon->Update( dt );
		}
		break;

	case MODE_UPDATE_ALL_WEAPONS:
		{
			for( int i=0; i<NUM_WEAPONSLOTS; i++ )
			{
				if( m_aWeaponSlot[i].pWeapon )
					m_aWeaponSlot[i].pWeapon->Update( dt );
			}
		}
		break;

	default:
		break;
	}
}


bool CWeaponSystem::HandleInput( int input_code, int input_type, float fParam )
{
	bool input_handled = false;

	if( 0 <= m_PrimaryWeaponIndex )
	{
		CGI_Weapon *pWeapon = m_aWeaponSlot[m_PrimaryWeaponIndex].pWeapon;
		if( pWeapon )
			input_handled = pWeapon->HandleInput( input_code, input_type, fParam );
	}

//	if( input_handled )
//		return true;

	if( input_code == ACTION_ATK_FIRE )
		return false;
	else if( input_code == ACTION_ATK_FIRE1 )
		input_code = ACTION_ATK_FIRE;

	if( 0 <= m_SecondaryWeaponIndex )
	{
		CGI_Weapon *pWeapon = m_aWeaponSlot[m_SecondaryWeaponIndex].pWeapon;
		if( pWeapon )
			input_handled |= pWeapon->HandleInput( input_code, input_type, fParam );
	}

	return input_handled;
}


void CWeaponSystem::SetProjectileGroup( int group_index )
{
	for( int i=0; i<NUM_WEAPONSLOTS; i++ )
	{
		m_aWeaponSlot[i].ProjectileGroup = group_index;
	}
}


void CWeaponSystem::Disarm()
{
	for( int i=0; i<NUM_WEAPONSLOTS; i++ )
	{
		if( m_aWeaponSlot[i].pWeapon )
            m_aWeaponSlot[i].pWeapon->Disarm();

		if( m_aWeaponSlot[i].pChargedAmmo )
			m_aWeaponSlot[i].pChargedAmmo->OnOwnerLeftStage();
	}
}
