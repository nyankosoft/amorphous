
#include "GI_Aircraft.h"
#include "Stage/Serialization_BaseEntityHandle.h"
#include "GI_MissileLauncher.h"

#include "3DMath/MathMisc.h"
#include "Support/memory_helpers.h"

#include "GameInput/3DActionCode.h"
#include "GameInput/InputHandler.h"
#include "GameCommon/MeshBoneController_Aircraft.h"

#include "3DCommon/D3DXMeshObjectBase.h"

/*
void CBE_PseudoAircraft::Init()
{
	CWeaponSystem& weapon_system = aircraft.WeaponSystem();

	weapon_system.GetWeaponSlot(0).Mount( machine_gun );
	weapon_system.GetWeaponSlot(1).Mount( missile_launcher );
	weapon_system.GetWeaponSlot(2).Mount( sp_weapon );
}
*/


template<typename T>
const T& GetLimited( const T& val, const T& min = 0.0f, const T& max = 1.0f )
{
	if( val < min )
		return min;
	else if( max < val )
		return max;

	return val;
}



CGI_Aircraft::~CGI_Aircraft()
{
	SafeDeleteVector( m_vecpMeshController );
}


float CGI_Aircraft::GetManeuverability() const
{
	return GetLimited( m_fMaxPitchAccel * m_fMaxRollAccel / 25.0f, 0.0f, 1.0f );
}


float CGI_Aircraft::GetStealthiness() const
{
//	float s; s = (1.0f / m_fRCS); Limit( s, 0.0f, 1.0f ); return s;
	return GetLimited( (1.0f / m_fRCS), 0.0f, 1.0f );
}


float CGI_Aircraft::GetArmorScaled() const
{
	return  GetLimited( GetArmor() * 0.01f, 0.0f, 1.0f );
}


void CGI_Aircraft::InitWeaponSystem()
{
	// slot 0: machine gun
	m_WeaponSystem.GetWeaponSlot(0).LocalPose = Matrix34( m_vGunMuzzleEndLocalPos, Matrix33Identity() );

	// slot 1: standard missile launcher
	CGI_Weapon* pWeapon = m_WeaponSystem.GetWeaponSlot(1).pWeapon;
	if( pWeapon && pWeapon->GetArchiveObjectID() == CGameItem::ID_MISSILELAUNCHER )
	{
		CGI_MissileLauncher* pMissileLauncher = (CGI_MissileLauncher *)(m_WeaponSystem.GetWeaponSlot(1).pWeapon);

		pMissileLauncher->SetNumReleasePositions( 2 );
		int i;//, num_weapons = ;
		for( i=0; i<2; i++ )
		{
			pMissileLauncher->SetLocalReleasePose( i, m_AmmoReleaseLocalPose[i] );
		}
	}

	// update selected & non-selected weapons every physics timestep
	// - missile launcher is responsible for updating world poses of the loaded missiles
	m_WeaponSystem.SetWeaponUpdateMode( CWeaponSystem::MODE_UPDATE_ALL_WEAPONS );

/*	for( i=0; i<num_weapons; i++ )
	{
		switch()
		{
		case CGameItem::WEAPON:
			missile_launcher = ()(m_WeaponSystem.GetWeaponSlot(i).pWeapon);
			for( i=0; i<2; i++ )
			{
				missile_launcher.SetLocalReleasePose( m_AmmoReleaseLocalPose[] );
			}
			break;
		}
	}*/
}


void CGI_Aircraft::Update( float dt )
{
	CPseudoAircraftSimulator& sim = PseudoSimulator();

	WeaponSystem().UpdateWorldProperties( sim.GetWorldPose(), sim.GetVelocity(), Vector3(0,0,0)/*sim.GetAngVelocity()*/ );

	WeaponSystem().Update( dt );

	float accel = m_fAccel + m_fBoostAccel * m_fCurrentBoost + m_fBrakeAccel * m_fCurrentBrake;
	Limit( accel, 5.0f, 1000.0f );

	sim.SetAccel( accel );

	// update rotor(s)
	size_t i, num = m_vecRotor.size();
	for( i=0; i<num; i++ )
	{
		CAircraftRotor& rotor = m_vecRotor[i];
		rotor.fAngle += rotor.fRotationSpeed * dt;
	}

	// udpate mesh controllers
	num = m_vecpMeshController.size();
	for( i=0; i<num; i++ )
	{
		if( m_vecpMeshController[i]->GetArchiveObjectID() == CMeshBoneControllerBase::ID_AIRCRAFT_ROTOR
		 && 0 < m_vecRotor.size() )
			((CMeshBoneController_Rotor *)m_vecpMeshController[i])->SetRotationAngle( m_vecRotor[0].fAngle );

		m_vecpMeshController[i]->UpdateTransforms();
	}
}


bool CGI_Aircraft::HandleInput( int input_code, int input_type, float fParam )
{
	// let the weapons handle the input first
	if( WeaponSystem().HandleInput( input_code, input_type, fParam ) )
		return true;

	CPseudoAircraftSimulator& sim = PseudoSimulator();

	switch( input_code )
	{
	case ACTION_MOV_PITCH_ACCEL:
		sim.SetPitchAccel( fParam * m_fMaxPitchAccel/*5.0f*/ );
		break;

	case ACTION_MOV_ROLL_ACCEL:
		sim.SetRollAccel( fParam * m_fMaxRollAccel/*5.0f*/ );
		break;

	case ACTION_MOV_YAW_ACCEL:
		sim.SetYawAccel( fParam * m_fMaxYawAccel/*1.5f*/ );
		break;

	case ACTION_MOV_BOOST:
		if( input_type == ITYPE_KEY_PRESSED )
			m_fCurrentBoost = 1.0f;
 //			sim.SetAccel( m_fBoostAccel );
		else if( input_type == ITYPE_KEY_RELEASED )
			m_fCurrentBoost = 0.0f;
//			sim.SetAccel( m_fAccel );
		return true;

	case ACTION_MOV_BRAKE:
		if( input_type == ITYPE_KEY_PRESSED )
			m_fCurrentBrake = 1.0f;
//			sim.SetAccel( m_fBrakeAccel );
		else if( input_type == ITYPE_KEY_RELEASED )
			m_fCurrentBrake = 0.0f;
//			sim.SetAccel( m_fAccel );
		return true;

	case ACTION_MISC_CYCLE_WEAPON:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			// alternate between slot 1 & 2
			WeaponSystem().SelectPrimaryWeapon( WeaponSystem().GetPrimaryWeaponSlotIndex() == 1 ? 2 : 1 );
		}
//		else if( input_type == ITYPE_KEY_RELEASED )
//			sim.SetAccel( m_fAccel );
		return true;
/*
	case ACTION_MOV_YAW:
		return true;

	case ACTION_MOV_PITCH:
		return true;*/
	}

	return false;
}


void CGI_Aircraft::ResetAircraftControls()
{
	m_fCurrentBoost = 0;
	m_fCurrentBrake = 0;
}


int CGI_Aircraft::GetPayloadForAmmunition( const CGI_Ammunition& ammo, int weapon_slot_index ) const
{
	size_t i, num_supported_types = m_vecSupportedAmmo.size();
	for( i=0; i<num_supported_types; i++ )
	{
		const AmmoPayload& payload_info = m_vecSupportedAmmo[i];

		if( payload_info.AmmoName == ammo.GetName() 
		 && payload_info.WeaponSlot == weapon_slot_index )
			return payload_info.MaxQuantity;
	}

	return 0;
}


bool CGI_Aircraft::InitMeshController( CD3DXSMeshObject* pMesh )
{
	// determine the target mesh
	// - This is either mesh of this aircraft or the argument 'pMesh'.
	// - The target mesh has to be a skeletal mesh (i.e. mesh type must be CD3DXMeshObjectBase::TYPE_SMESH )
	CD3DXSMeshObject* pTargetMesh = NULL;
	if( pMesh )
	{
		// init mesh controller with external mesh object
		// NOTE: ResetMeshController() must be called after the user is done
		//       with the external mesh
		pTargetMesh = pMesh;
	}
	else
	{
		CD3DXMeshObjectBase *pMeshObject = m_MeshObjectContainer.m_MeshObjectHandle.GetMeshObject();
		if( pMeshObject && pMeshObject->GetMeshType() == CD3DXMeshObjectBase::TYPE_SMESH )
		{
			pTargetMesh = (CD3DXSMeshObject *)pMeshObject;
		}
	}

	// valid target mesh has been acquired

	if( !pTargetMesh )
		return false;

	size_t i, num = m_vecpMeshController.size();
	for( i=0; i<num; i++ )
	{
		m_vecpMeshController[i]->SetTargetMesh( pTargetMesh );
		m_vecpMeshController[i]->Init();
	}

	return true;
}


// 
void CGI_Aircraft::ResetMeshController()
{
	size_t i, num = m_vecpMeshController.size();
	for( i=0; i<num; i++ )
		m_vecpMeshController[i]->SetTargetMesh( NULL );
}


void CGI_Aircraft::UpdateTargetMeshTransforms()
{
	size_t i, num = m_vecpMeshController.size();
	for( i=0; i<num; i++ )
		m_vecpMeshController[i]->UpdateTargetMeshTransforms();
}

