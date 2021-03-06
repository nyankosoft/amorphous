#include "GI_Aircraft.hpp"
#include "GI_MissileLauncher.hpp"
#include "amorphous/Stage/Serialization_BaseEntityHandle.hpp"
#include "amorphous/3DMath/MathMisc.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/XML/XMLNode.hpp"
#include "amorphous/Input/InputHandler.hpp"
#include "amorphous/GameCommon/3DActionCode.hpp"
#include "amorphous/GameCommon/MeshBoneController_Aircraft.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/MeshBonesUpdateCallback.hpp"
#include "amorphous/Input/ForceFeedback/ForceFeedbackEffect.hpp"


namespace amorphous
{

using namespace std;

/*
void CBE_PseudoAircraft::Init()
{
	CWeaponSystem& weapon_system = aircraft.WeaponSystem();

	weapon_system.GetWeaponSlot(0).Mount( machine_gun );
	weapon_system.GetWeaponSlot(1).Mount( missile_launcher );
	weapon_system.GetWeaponSlot(2).Mount( sp_weapon );
}
*/



/*
<Gear00_Shaft>
<Gear00_Tire>,
*/
/*
void CPseudoAircraftGear::LoadFromXMLNode( XMLNode& reader )
{
	reader.GetChildElementTextContent( "Tire/Pose/Pos", m_TirePose.vPosition );
	reader.GetChildElementTextContent( "Shaft/Pose/Pos", m_ShaftPose.vPosition );
}
*/

void CGI_Aircraft::AmmoPayload::LoadFromXMLNode( XMLNode& reader )
{
	reader.GetChildElementTextContent( "AmmoName",        AmmoName );
	reader.GetChildElementTextContent( "WeaponSlotIndex", WeaponSlot );
	reader.GetChildElementTextContent( "MaxQuantity",     MaxQuantity );
}



CGI_Aircraft::CGI_Aircraft()
{
	m_TypeFlag |= (TYPE_VEHICLE);

	m_CockpitLocalPose = Matrix34( Vector3( 0.0f, 1.0f, 20.0f ), Matrix33Identity() );

	m_vThirdPersonViewOffset = Vector3(0,1,0);

	m_fAccel = 30.0f;
	m_fBoostAccel = 150.0f;
	m_fBrakeAccel = 5.0f;

	m_fCurrentBoost = 0.0f;
	m_fCurrentBrake = 0.0f;

	//m_PitchRange;
	//m_RollRange;

	m_fDefaultForwardAirFriction = 0.2f;

	float m_fMaxPitchAccel = 5.0f;
	float m_fMaxRollAccel = 5.0f;
	float m_fMaxYawAccel = 5.0f;

	m_AmmoReleaseLocalPose.resize( NUM_AMMO_RELEASE_POSITIONS, Matrix34Identity() );

	m_vGunMuzzleEndLocalPos = Vector3( 0.0f, 1.0f, 20.0f );

	m_fGearUnitHeight = 0.0f;

	m_fCeiling = 100000.0f;

	m_fArmor = 0.5f;
	m_fRCS = 1.0f;

	m_pFFBrake.reset( new CForceFeedbackEffect );
	CConstantForceFeedbackEffectDesc ffb;
	ffb.gain = 10000;
	ffb.duration = 10000;//CForceFeedbackEffect::INFINITE_DURATION;
	ffb.magnitude = 5000;
	m_pFFBrake->Init( ffb );
}


CGI_Aircraft::~CGI_Aircraft()
{
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


class ValidWeakPtr
{
public:
	bool operator() ( std::weak_ptr<CGI_Ammunition>& p ) { return !(p.lock()); }
};


void CGI_Aircraft::UpdateAvailableAmmoCache( int num_weapon_slots, vector< shared_ptr<CGI_Ammunition> >& vecpAmmo )
{
	m_vecpAvailableAmmoCache.resize( num_weapon_slots );

	const int num_ammo_items = (int)vecpAmmo.size();
	for( int i=0; i<num_ammo_items; i++ )
	{
		for( int j=0; j<num_weapon_slots; j++ )
		{
			if( 0 < GetPayloadForAmmunition( *vecpAmmo[i], j ) )
				m_vecpAvailableAmmoCache[j].push_back( vecpAmmo[i] );
		}
	}

	for( int j=0; j<num_weapon_slots; j++ )
	{
		std::remove_if(
			m_vecpAvailableAmmoCache[j].begin(),
			m_vecpAvailableAmmoCache[j].end(),
			ValidWeakPtr() );
	}

}


shared_ptr<CGI_Ammunition> CGI_Aircraft::GetAvailableAmmoFromCache( int weapon_slot_index, int ammo_index )
{
	if( (int)m_vecpAvailableAmmoCache.size() <= weapon_slot_index )
		return shared_ptr<CGI_Ammunition>();

	if( (int)m_vecpAvailableAmmoCache[weapon_slot_index].size() <= ammo_index )
		return shared_ptr<CGI_Ammunition>();

	return m_vecpAvailableAmmoCache[weapon_slot_index][ammo_index].lock();
}


std::string CGI_Aircraft::GetPrevUsedAmmoName( int weapon_slot_index )
{
	std::map<int,std::string>::iterator itr = m_PrevUsedAmmo.find(weapon_slot_index);
	if( itr != m_PrevUsedAmmo.end() )
		return itr->second;
	else
		return "";
}


/// Returns the index of the ammunition previouly loaded the specified weapon slot.
/// The returned index is the index to the current cached ammunition.
/// Returns -1 if no previouly loaded ammunition was found.
int CGI_Aircraft::GetPrevUsedAmmoIndex( int weapon_slot_index )
{
	if( (int)m_vecpAvailableAmmoCache.size() <= weapon_slot_index )
		return -1;

	const string ammo_name = GetPrevUsedAmmoName(weapon_slot_index);

	if( ammo_name.length() == 0 )
		return -1;

	const int num_cached_ammo = (int)m_vecpAvailableAmmoCache[weapon_slot_index].size();
	for( int i=0; i<num_cached_ammo; i++ )
	{
		shared_ptr<CGI_Ammunition> pAmmo = m_vecpAvailableAmmoCache[weapon_slot_index][i].lock();
		if( pAmmo && pAmmo->GetName() == ammo_name )
			return i;
	}

	return -1;
}


void CGI_Aircraft::InitWeaponSystem()
{
	// slot 0: machine gun
	m_WeaponSystem.GetWeaponSlot(0).LocalPose = Matrix34( m_vGunMuzzleEndLocalPos, Matrix33Identity() );

	// slot 1: standard missile launcher
	CGI_Weapon* pWeapon = m_WeaponSystem.GetWeaponSlot(1).pWeapon;
	if( pWeapon && pWeapon->GetArchiveObjectID() == GameItem::ID_MISSILELAUNCHER )
	{
		MissileLauncher* pMissileLauncher = (MissileLauncher *)(m_WeaponSystem.GetWeaponSlot(1).pWeapon);

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
		case GameItem::WEAPON:
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
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return;

	CPseudoAircraftSimulator& sim = PseudoSimulator();

	WeaponSystem().UpdateWorldProperties( sim.GetWorldPose(), sim.GetVelocity(), Vector3(0,0,0)/*sim.GetAngVelocity()*/ );

	WeaponSystem().Update( dt );

	float accel = m_fAccel + m_fBoostAccel * m_fCurrentBoost + m_fBrakeAccel * m_fCurrentBrake;
	Limit( accel, 5.0f, 1000.0f );

	sim.SetAccel( accel );

	// update rotor(s)
/*	size_t i, num = m_vecRotor.size();
	for( i=0; i<num; i++ )
	{
		CAircraftRotor& rotor = m_vecRotor[i];
		rotor.fAngle += rotor.fRotationSpeed * dt;
	}
*/
	// udpate mesh controllers
	const size_t num = m_vecpMeshController.size();
	for( size_t i=0; i<num; i++ )
	{
		m_vecpMeshController[i]->SetCurrentTime( pStage->GetElapsedTime() );

		m_vecpMeshController[i]->Update( dt );

//		if( m_vecpMeshController[i]->GetArchiveObjectID() == MeshBoneControllerBase::ID_AIRCRAFT_ROTOR
//		 && 0 < m_vecRotor.size() )
//			((MeshBoneController_Rotor *)m_vecpMeshController[i])->SetRotationAngle( m_vecRotor[0].fAngle );

		m_vecpMeshController[i]->UpdateTransforms();
	}
}


void CGI_Aircraft::UpdateGraphics()
{
	UpdateTargetMeshTransforms();
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
		else if( input_type == ITYPE_KEY_RELEASED )
			m_fCurrentBoost = 0.0f;
		return true;

	case ACTION_MOV_BRAKE:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_fCurrentBrake = 1.0f;
			m_pFFBrake->Start( 1, 0 );
		}
		else if( input_type == ITYPE_KEY_RELEASED )
			m_fCurrentBrake = 0.0f;
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
	}

	return false;
}


void CGI_Aircraft::ResetAircraftControls()
{
	m_fCurrentBoost = 0;
	m_fCurrentBrake = 0;
}


/// Returns the maximum number of ammunitions that can be loaded
/// to the weapon slot specified by the index.
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


bool CGI_Aircraft::InitMeshController( shared_ptr<SkeletalMesh> pMesh )
{
	// determine the target mesh
	// - This is either mesh of this aircraft or the argument 'pMesh'.
	// - The target mesh has to be a skeletal mesh (i.e. mesh type must be CD3DXMeshObjectBase::TYPE_SMESH )
	shared_ptr<SkeletalMesh> pTargetMesh;
	if( pMesh )
	{
		// init mesh controller with external mesh object
		// NOTE: ResetMeshController() must be called after the user is done
		//       with the external mesh
		pTargetMesh = pMesh;
	}
	else
	{
//		shared_ptr<CD3DXMeshObjectBase> pMeshObject = m_MeshObjectContainer.m_MeshObjectHandle.GetMesh();
		shared_ptr<BasicMesh> pMeshObject;
		if( 0 < m_MeshContainerRootNode.GetNumMeshContainers() )
			pMeshObject = m_MeshContainerRootNode.MeshContainer(0)->m_MeshObjectHandle.GetMesh();

		if( pMeshObject && pMeshObject->GetMeshType() == MeshType::SKELETAL )
		{
			pTargetMesh = std::dynamic_pointer_cast<SkeletalMesh,BasicMesh>(pMeshObject);
		}
	}

	// valid target mesh has been acquired

	if( !pTargetMesh )
		return false;

	size_t i, num = m_vecpMeshController.size();
	for( i=0; i<num; i++ )
	{
//		m_vecpMeshController[i]->SetTargetMesh( pTargetMesh );
		m_vecpMeshController[i]->Init( *pTargetMesh );
	}

	return true;
}


// 
void CGI_Aircraft::ResetMeshController()
{
//	MeshObjectContainer& mesh_container = m_MeshObjectContainer;

	if( m_MeshContainerRootNode.GetNumMeshContainers() == 0 )
		return;

	MeshObjectContainer& mesh_container = *(m_MeshContainerRootNode.MeshContainer(0).get());
	if( !mesh_container.m_MeshObjectHandle.GetMesh() )
		return;

//	size_t i, num = m_vecpMeshController.size();
//	for( i=0; i<num; i++ )
//		m_vecpMeshController[i]->SetTargetMesh( shared_ptr<SkeletalMesh>() );
}


void CGI_Aircraft::UpdateTargetMeshTransforms()
{
//	if( !m_MeshObjectContainer.m_MeshObjectHandle.GetMesh() )
	if( m_MeshContainerRootNode.GetNumMeshContainers() == 0 )
		return;

	const int num = (int)m_vecpMeshController.size();
//	for( int i=0; i<num; i++ )
//		m_vecpMeshController[i]->UpdateTargetMeshTransforms();


	EntityHandle<ItemEntity> item_entity = GetItemEntity();
	shared_ptr<ItemEntity> pEntity = item_entity.Get();
	if( !pEntity )
		return;

	shared_ptr<CMeshBonesUpdateCallback> pMeshUpdate = item_entity.Get()->MeshBonesUpdateCallback();
	if( !pMeshUpdate )
		return;

	for( int i=0; i<num; i++ )
		m_vecpMeshController[i]->UpdateMeshBoneLocalTransforms( pMeshUpdate->MeshBoneLocalTransforms() );

	pMeshUpdate->UpdateGraphics();
}


void CGI_Aircraft::DeployGears()
{
	for( size_t i=0; i<m_vecpGear.size(); i++ )
		m_vecpGear[i]->Open();
}


void CGI_Aircraft::RetractGears()
{
	for( size_t i=0; i<m_vecpGear.size(); i++ )
		m_vecpGear[i]->Close();
}


void CGI_Aircraft::Serialize( IArchive& ar, const unsigned int version )
{
	GameItem::Serialize( ar, version );

    ar & m_CockpitLocalPose;

    ar & m_vThirdPersonViewOffset;

	ar & m_vecNozzleFlameParams;

	ar & m_fAccel & m_fBoostAccel & m_fBrakeAccel;

//	ar & m_PitchRange & m_RollRange;

	ar & m_fMaxPitchAccel & m_fMaxRollAccel & m_fMaxYawAccel;

//	ar & m_WeaponSystem;

	ar & m_AmmoReleaseLocalPose;

	ar & m_vGunMuzzleEndLocalPos;

	ar & m_fGearUnitHeight;

	ar & m_fCeiling;
	ar & m_fArmor;
	ar & m_fRCS;

	ar & m_vecSupportedAmmo;

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		m_fCurrentBoost = 0.0f;
		m_fCurrentBrake = 0.0f;
	}

	// serialization of mesh controllers
	MeshBoneControllerFactory factory;
	ar.Polymorphic( m_vecpMeshController, factory );

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		size_t i, num = m_vecpMeshController.size();
		for( i=0; i<num; i++ )
		{
//			if( m_vecpMeshController[i]->GetArchiveObjectID() == MeshBoneControllerBase::ID_... )
//				((MeshBoneController_AircraftBase *)m_vecpMeshController[i])->SetPseudoAircraftSimulator( &m_PseudoSimulator );
				m_vecpMeshController[i]->SetPseudoAircraftSimulator( &m_PseudoSimulator );
		}
	}

//	ar & m_vecRotor;

	ar & m_PrevUsedAmmo;
}


void CGI_Aircraft::LoadFromXMLNode( XMLNode& reader )
{
	GameItem::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "Armor",           m_fArmor );
	reader.GetChildElementTextContent( "RCS",             m_fRCS );
	reader.GetChildElementTextContent( "Ceiling",         m_fCeiling );
	reader.GetChildElementTextContent( "CockpitLocalPos", m_CockpitLocalPose.vPosition );

	reader.GetChildElementTextContent( "ThirdPersonView/CameraPosition", m_vThirdPersonViewOffset );

//	reader.GetChildElementTextContent( m_vecNozzleFlameParams,  );

	XMLNode accels_reader = reader.GetChild( "Accel" );
	accels_reader.GetChildElementTextContent( "Default",    m_fAccel );
	accels_reader.GetChildElementTextContent( "Boost",      m_fBoostAccel );
	accels_reader.GetChildElementTextContent( "Brake",      m_fBrakeAccel );

//	reader.m_PitchRange reader.m_RollRange;

	reader.GetChildElementTextContent(  "MaxPitchAccel",  m_fMaxPitchAccel );
	reader.GetChildElementTextContent(  "MaxRollAccel",   m_fMaxRollAccel );
	reader.GetChildElementTextContent(  "MaxYawAccel",    m_fMaxYawAccel );

	reader.GetChildElementTextContent( "GunMuzzleEndPos", m_vGunMuzzleEndLocalPos );

	std::vector<XMLNode> payloads = reader.GetImmediateChildren( "AmmoPayload" );

	const size_t num_payload_info = payloads.size();
	m_vecSupportedAmmo.resize( num_payload_info );
	for( size_t i = 0; i<num_payload_info; i++ )
	{
		m_vecSupportedAmmo[i].LoadFromXMLNode( payloads[i] );
	}

	XMLNode components_reader = reader.GetChild( "Components" );
	std::vector<XMLNode> components = components_reader.GetImmediateChildren();
//	std::vector<XMLNode> components = reader.GetImmediateChildren( "Components" );
	for( size_t i = 0; i<components.size(); i++ )
	{
		XMLNode& component_reader = components[i];
		shared_ptr<MeshBoneController_AircraftBase> pComponent;
		if( component_reader.GetName() == "Flap" )
		{
			pComponent = shared_ptr<MeshBoneController_AircraftBase>( new MeshBoneController_Flap() );
		}
		else if( component_reader.GetName() == "VFlap" )
		{
			pComponent = shared_ptr<MeshBoneController_AircraftBase>( new MeshBoneController_VFlap() );
		}
		else if( component_reader.GetName() == "Rotor" )
		{
			pComponent = shared_ptr<MeshBoneController_AircraftBase>( new MeshBoneController_Rotor() );
		}
		else if( component_reader.GetName() == "GearUnit" )
		{
			// store to the gear unit array to open/close the gear box(es)
			shared_ptr<MeshBoneController_GearUnit> pGearUnit;
			pGearUnit = shared_ptr<MeshBoneController_GearUnit>( new MeshBoneController_GearUnit() );
			m_vecpGear.push_back( pGearUnit );

			pComponent = pGearUnit;
		}

		if( pComponent )
		{
			pComponent->LoadFromXMLNode( component_reader );

			m_vecpMeshController.push_back( pComponent );
		}
	}

}



} // namespace amorphous
