#ifndef __GAMEITEM_AIRCRAFT_H__
#define __GAMEITEM_AIRCRAFT_H__

#include <boost/shared_ptr.hpp>
#include "GameItem.hpp"
#include "WeaponSystem.hpp"

#include "Stage/BaseEntityHandle.hpp"
#include "Stage/Serialization_BaseEntityHandle.hpp"

#include "Graphics/fwd.hpp"
#include "GameCommon/PseudoAircraftSimulator.hpp"
#include "GameCommon/NozzleFlameParams.hpp"
#include "GameCommon/MeshBoneController_Aircraft.hpp"
#include "GameCommon/MeshBoneControllerFactory.hpp"
#include "GameCommon/RangedSet.hpp"
#include "XML/fwd.hpp"


//class CMeshBoneController_AircraftBase;

/*
class AccelProperty
{
public:
	float accel;
	float friction;

	AccelProperty( float a, float f ) : accel(a), friction(f) {}
};*/



/**
  CGI_Aircraft

*/
class CGI_Aircraft : public CGameItem
{
	Matrix34 m_CockpitLocalPose;

	/// local camera position for the third person viewpoint
	Vector3 m_vThirdPersonViewOffset;

	std::vector<NozzleFlameParams> m_vecNozzleFlameParams;

//	float m_fMaxAccel;	// maximum foward thrust

	float m_fAccel;
	float m_fBoostAccel;	///< additional accel of the boost
	float m_fBrakeAccel;	///< negative accel to reduce accel

	float m_fCurrentBoost;	///< current boost strength [0,1]
	float m_fCurrentBrake;	///< current brake strength [0,1]

	/**
	 * set by aircraft which has limitation to turn upward/downward.
	 * e.g.) large aircrafts like bombers. helicopters
	 */
	RangedSet<float> m_PitchRange;

	RangedSet<float> m_RollRange;

	float m_fMaxPitchAccel;	///< ... 3.0(not very maneuverable?) ... 5.0(highly maneuverable) ...
	float m_fMaxRollAccel;	///< ... 3.0(not very maneuverable?) ... 5.0(highly maneuverable) ...
	float m_fMaxYawAccel;

	CWeaponSystem m_WeaponSystem;

//	float m_iNumWeaponSlots;

	enum ammo_release_pose
	{
		PYLON_MAIN_0,
		PYLON_MAIN_1,
		PYLON_SUB_0,
		PYLON_SUB_1,
		PYLON_SUB_2,
		PYLON_SUB_3,
		NUM_AMMO_RELEASE_POSITIONS
	};

	std::vector<Matrix34> m_AmmoReleaseLocalPose;

	Vector3 m_vGunMuzzleEndLocalPos;

	CPseudoAircraftSimulator m_PseudoSimulator;

	float m_fCeiling;		///< maximum altitude the plane can climb up to

	float m_fArmor;			///< min: 50.0 - max: 100.0

	float m_fRCS;			///< radar cross section. the smaller, the harder to detect

	float m_fGearUnitHeight;

	class AmmoPayload : public IArchiveObjectBase
	{
	public:

		std::string AmmoName;
		int WeaponSlot;			///< weapon slot index where the ammo can be loaded
		int MaxQuantity;		///< how many waepons can be loaded

	public:

		AmmoPayload() : WeaponSlot(0), MaxQuantity(0) {}

		AmmoPayload( std::string name, int weapon_slot_index, int max_quantity )
			: AmmoName(name), WeaponSlot(weapon_slot_index), MaxQuantity(max_quantity) {}

		void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & AmmoName & WeaponSlot & MaxQuantity;
		}

		void LoadFromXMLNode( CXMLNodeReader& reader );
	};

	/// list of ammo that can be fired from this aircraft
	std::vector<AmmoPayload> m_vecSupportedAmmo;

	/// cached ammunition items
	/// - Borrowed reference. Owned reference needs to be maintained by the owner of the aircraft item
	/// - Not serialized.
	std::vector<std::vector<boost::weak_ptr<CGI_Ammunition>>> m_vecpAvailableAmmoCache;

	/// weapon slot index : name of the previously used ammunition
	std::map<int,std::string> m_PrevUsedAmmo;

//	CBaseEntityHandle m_EngineNozzleFlame;
//	CBaseEntityHandle m_MuzzleFlashBaseEntity;

	std::vector< boost::shared_ptr<CMeshBoneController_AircraftBase> > m_vecpMeshController;

//	std::vector< boost::shared_ptr<CMeshBoneController_Rotor> > m_vecpRotor;

	std::vector< boost::shared_ptr<CMeshBoneController_GearUnit> > m_vecpGear;

	float m_fDefaultForwardAirFriction;

public:

	CGI_Aircraft();
	virtual ~CGI_Aircraft();

	void InitWeaponSystem();

	CWeaponSystem &WeaponSystem() { return m_WeaponSystem; }

	/// CPseudoAircraftSimulator::Update() is not called inside this function,
	/// and it needs to be called separately.
	/// Calls Update() of weapon items currently used by the aircraft
	virtual void Update( float dt );

	/// controls pseudo-aircraft simulator
	virtual bool HandleInput( int input_code, int input_type, float fParam );

	// return handle to a base entity used as ammunition
//	inline CBaseEntityHandle& GetBaseEntityHandle() { return m_AmmoBaseEntity; }

	// return handle to a base entity is used as muzzle flash of this ammo
//	inline CBaseEntityHandle& GetMuzzleFlashHandle() { return m_MuzzleFlashBaseEntity; }

	CPseudoAircraftSimulator &PseudoSimulator() { return m_PseudoSimulator; }

	float GetDefaultAccel() { return m_fAccel; }

	void ResetAircraftControls();

	Matrix34 GetCockpitLocalPose() const { return m_CockpitLocalPose; }

	Vector3 GetThirdPersonCameraOffset() const { return m_vThirdPersonViewOffset; }

	std::vector<NozzleFlameParams>& GetNozzleFlameParams() { return m_vecNozzleFlameParams; }

	/// \return the max quantity of specified ammunition aircraft can carry
	/// \return 0 if aircraft cannot load the ammunition
	int GetPayloadForAmmunition( const CGI_Ammunition& ammo, int weapon_slot_index ) const;

	float GetManeuverability() const;// { return m_fMaxPitchAccel * m_fMaxRollAccel; }

	float GetStealthiness() const;// { float s; s = (1.0f / m_fRCS); Limit( s, 0.0f, 1.0f ); return s; }

	float GetArmor() const { return m_fArmor; }

	float GetArmorScaled() const;

//	int GetNumRotors() const { return (int)m_vecRotor.size(); }
//	const CAircraftRotor& GetRotor( int index ) const { return m_vecRotor[index]; }


	void UpdateAvailableAmmoCache(int num_weapon_slots, std::vector< boost::shared_ptr<CGI_Ammunition> >& vecpAmmo);

	std::vector< std::vector< boost::weak_ptr<CGI_Ammunition> > >& AvailableAmmoCache() { return m_vecpAvailableAmmoCache; }
	

	/// update prev used ammo
	void UpdatePrevUsedAmmo( int weapon_slot_index, const std::string& ammo_item_name ) { m_PrevUsedAmmo[weapon_slot_index] = ammo_item_name; }

	std::string GetPrevUsedAmmoName( int weapon_slot_index );

	int GetPrevUsedAmmoIndex( int weapon_slot_index );

	boost::shared_ptr<CGI_Ammunition> GetAvailableAmmoFromCache( int weapon_slot_index, int ammo_index );

	/// initializes mesh bone controller
	/// If called without an argument, the mesh bone controllers use the mesh
	/// of the aircraft item for their target mesh
	/// If a mesh is given as the argument, the mesh bone controllers use it
	/// as the target mesh.
	/// In the latter case, the mesh is maintained as a simple borrowed reference,
	/// and the user is responsible for calling ResetMeshController(),
	/// before the mesh is destroyed
	bool InitMeshController( boost::shared_ptr<CD3DXSMeshObject> pMesh = boost::shared_ptr<CD3DXSMeshObject>() );

	/// disconnet the target mesh from the mesh bone controllers.
	/// The target mesh object is held as borrowed reference
	/// and not released/destroyed by this call
	void ResetMeshController();

	// update local transforms of the target mesh controlled by mesh bone controllers
	// usually called before rendering the mesh
	void UpdateTargetMeshTransforms();

	void DeployGears();

	void RetractGears();

	unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	friend class CItemDatabaseBuilder;

};


#endif  /*  __GAMEITEM_AIRCRAFT_H__  */
