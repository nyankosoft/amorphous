#include "GI_Weapon.hpp"
#include "GI_Ammunition.hpp"
#include "WeaponSystem.hpp"

#include "3DMath/MathMisc.hpp"
#include "GameCommon/3DActionCode.hpp"
#include "Support/MTRand.hpp"
#include "Sound/SoundManager.hpp"
#include "XML/XMLNodeReader.hpp"

#include "Stage/Stage.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Stage/PlayerInfo.hpp"


namespace amorphous
{

using namespace std;


//======================================================================================
// CGI_Weapon
//======================================================================================

CGI_Weapon::CGI_Weapon()
{
	m_TypeFlag |= (TYPE_WEAPON);

	m_pWeaponSlot = NULL;

	m_fFireInterval = 0.2f;

	m_fMuzzleSpeedFactor = 1.0f;

	m_vLocalRecoilForce = Vector3(0,0,-5);

	// fire mode is set to full-auto by default
	m_iNumBursts = ~( 1 << (sizeof(int)*8-1) );	// a large value

	m_dLastFireTime = 0;

	m_iCurrentBurstCount = 0;

	m_aTriggerState[0] = m_aTriggerState[1] = 0;

	m_WeaponState = 0;

//	m_fBurstInterval = 1.0f;

	m_MuzzleEndLocalPose.Identity();
	m_MuzzleEndLocalPose.vPosition = Vector3(0,0,10);
	m_MuzzleEndWorldPose = m_MuzzleEndLocalPose;

	m_vMuzzleEndVelocity = Vector3(0,0,0);

	m_pOwnerEntity = NULL;
}


void CGI_Weapon::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_strAmmoType;

	/* weapon data */
	ar & m_fFireInterval;
	ar & m_fMuzzleSpeedFactor;

	ar & m_vLocalRecoilForce;

	ar & m_FireSound;

	ar & m_iNumBursts;

//	ar & m_fBurstInterval;

	ar & m_MuzzleEndLocalPose;

	ar & m_MuzzleEndWorldPose;

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// reset state
		m_dLastFireTime = 0;
		m_iCurrentBurstCount = 0;
		m_aTriggerState[0] = m_aTriggerState[1] = 0;
		m_WeaponState = 0;

		//m_MuzzleEndWorldPose = m_MuzzleEndLocalPose;
		//m_vMuzzleEndVelocity = Vector3(0,0,0);
		//m_pOwnerEntity = ???
	}
}


void CGI_Weapon::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGameItem::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "AmmoType",           m_strAmmoType );
	reader.GetChildElementTextContent( "FireInterval",       m_fFireInterval );
	reader.GetChildElementTextContent( "MuzzleSpeedFactor",  m_fMuzzleSpeedFactor );
	reader.GetChildElementTextContent( "LocalRecoilForce",   m_vLocalRecoilForce );

	string fire_sound_name;
	reader.GetChildElementTextContent( "FireSound",          fire_sound_name );
	m_FireSound.SetResourceName( fire_sound_name );

	reader.GetChildElementTextContent( "NumBursts",          m_iNumBursts );

//	reader.GetChildElementTextContent( "fBurstInterval;", m_fBurstInterval );

	reader.GetChildElementTextContent( "MuzzleEndLocalPos", m_MuzzleEndLocalPose.vPosition );
	
}


void CGI_Weapon::SetData(char *pcName, char *pcAmmoType, float fFireInterval)
{
	SetName( pcName );
	m_strAmmoType = pcAmmoType;
	m_fFireInterval = fFireInterval;
}


void CGI_Weapon::SetAmmoType( const char* pcAmmoType )
{
	m_strAmmoType = pcAmmoType;
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


} // namespace amorphous
