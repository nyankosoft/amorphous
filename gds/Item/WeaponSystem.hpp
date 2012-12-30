#ifndef  __WEAPONSYSTEM_H__
#define  __WEAPONSYSTEM_H__


#include "GI_Weapon.hpp"
#include "GI_Ammunition.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Stage/fwd.hpp"
//#include "gds/Stage/BaseEntityHandle.hpp"
#include "gds/Stage/EntityGroupHandle.hpp"


namespace amorphous
{


// forward declerations
class CGI_Weapon;
class CGI_Ammunition;



//======================================================================================
// SWeaponSlot
//======================================================================================

/**
 * holds a weapon and ammunition
 * only the ammo that matches the weapon can be loaded
 */
struct SWeaponSlot
{
	/// borrowed reference
	CGI_Weapon *pWeapon;

	/// borrowed reference
	CGI_Ammunition *pChargedAmmo;

	Matrix34 LocalPose;	///< the local pose of the mounted weapon
	Matrix34 WorldPose;	///< the world pose of the mounted weapon

	/// group index given to entities which represent projectiles fired from the weapon
	int ProjectileGroup;

	/// velocity at the muzzle end of the mounted weapon
//	Vector3 vVelocity;

	inline SWeaponSlot();

	inline void MountWeapon( CGI_Weapon* pWeapon );

	/// links the specified ammo with the weapon
	inline bool Load( CGI_Ammunition *pAmmo );
};


inline SWeaponSlot::SWeaponSlot()
:
pWeapon(NULL),
pChargedAmmo(NULL),
ProjectileGroup(ENTITY_GROUP_INVALID_ID)
{
	LocalPose.Identity();
	WorldPose.Identity();
//	vVelocity = Vector3(0,0,0);
}


inline void SWeaponSlot::MountWeapon( CGI_Weapon* _pWeapon )
{
	if( !_pWeapon )
	{
		LOG_PRINT_WARNING( " An invalid weapon mount request has been detected." );
		return;
	}

	pWeapon = _pWeapon;
	pWeapon->SetWeaponSlot( this );

	// unload ammo
	pChargedAmmo = NULL;
}


inline bool SWeaponSlot::Load( CGI_Ammunition *pAmmo )
{
	if( pWeapon &&
		pAmmo &&
//		pAmmo->GetTypeFlag() & CGameItem::TYPE_AMMO &&
		pAmmo->GetAmmoType() == pWeapon->GetAmmoType() )
	{
		pChargedAmmo = pAmmo;
		return true;
	}
	else
		return false;
}



//======================================================================================
// CWeaponSystem
//======================================================================================

#define WS_NOWEAPON_IN_SLOT -1

class CWeaponSystem
{
public:

	enum params { NUM_WEAPONSLOTS = 8 };

private:

	SWeaponSlot m_aWeaponSlot[NUM_WEAPONSLOTS];

	int m_PrimaryWeaponIndex;
	int m_SecondaryWeaponIndex;

	int m_WeaponUpdateMode;

public:

	enum eWeaponUpdateMode
	{
		MODE_UPDATE_ALL_WEAPONS,      ///< update all weapons
		MODE_UPDATE_SELECTED_WEAPONS, ///< update only the primary and secondary weapons
		NUM_UPDATE_MODES
	};

public:

	CWeaponSystem();

	~CWeaponSystem();

	void Init( CStage* pStage );

	/// link the weapon to a slot
//	inline void MountWeapon( int slot, CGI_Weapon* pWeapon );

//	bool SupplyAmmunition( char* pcAmmoName, int iSupplyQuantity );

	inline void SelectPrimaryWeapon( const int slot_index );

	inline void SelectSecondaryWeapon( const int slot_index );

	inline int GetPrimaryWeaponSlotIndex() const { return m_PrimaryWeaponIndex; }

	inline int GetSecondaryWeaponSlotIndex() const { return m_SecondaryWeaponIndex; }

	inline SWeaponSlot& GetPrimaryWeaponSlot() { return m_aWeaponSlot[m_PrimaryWeaponIndex]; }

	inline SWeaponSlot& GetSecondaryWeaponSlot() { return m_aWeaponSlot[m_SecondaryWeaponIndex]; }

	inline CGI_Weapon *GetPrimaryWeapon() { return GetPrimaryWeaponSlot().pWeapon; }

	inline CGI_Weapon *GetSecondaryWeapon() { return GetSecondaryWeaponSlot().pWeapon; }

	inline SWeaponSlot& GetWeaponSlot( int slot_index ) { return m_aWeaponSlot[slot_index]; }

	/// must be called every frame
	/// - updates primary & secondary weapons
	/// - handles updates for automatic fires
	void Update( float dt );

	/// let the current primary & secondary weapon handle the input
	/// returns true if the input has been used for some purpose
	bool HandleInput( int input_code, int input_type, float fParam );

	/// update the world poses of all the weapons & weapon slots
	inline void UpdateWorldProperties( const Matrix34& rShooterWorldPose,
									   const Vector3& rvShooterVelocity,
									   const Vector3& rvShooterAngVelocity );

	void SetProjectileGroup( int group_index );

	/// called when the owner leaves a stage
	void Disarm();

	int GetWeaponUpdateMode() const { return m_WeaponUpdateMode; }

	void SetWeaponUpdateMode( int mode ) { m_WeaponUpdateMode = mode; }

//	inline void UpdateShooterVelocity( const Vector3& rvShooterVelocity );

};


//===================================================================================
// inline implementations
//===================================================================================

/**
 * set the weapon slot for a primary weapon
 * \param slot_index index of the weapon slot to select
 * - 0 <= slot_index < NUM_WEAPONSLOTS ... select a weapon slot
 * - slot_index == negative value ... deselect the current primary weapon. Nothing will be fired by subsequent fire commands for the primary weapon until a slot is re-selected
 */
inline void CWeaponSystem::SelectPrimaryWeapon( const int slot_index )
{
	if( slot_index < NUM_WEAPONSLOTS && ( slot_index != m_SecondaryWeaponIndex || slot_index < 0 ) )
	{
		// deselect the current primary weapon
		if( 0 <= m_PrimaryWeaponIndex && GetPrimaryWeapon() )
			GetPrimaryWeapon()->SetWeaponState( 0 );

		m_PrimaryWeaponIndex = slot_index;

		if( 0 <= slot_index && m_aWeaponSlot[slot_index].pWeapon )
			m_aWeaponSlot[slot_index].pWeapon->SetWeaponState( CGI_Weapon::STATE_SELECTED_AS_PRIMARY_WEAPON );
	}
}


inline void CWeaponSystem::SelectSecondaryWeapon( const int slot_index )
{
	if( slot_index < NUM_WEAPONSLOTS && ( slot_index != m_PrimaryWeaponIndex || slot_index < 0 ) )
	{
		// deselect the current secondary weapon
		if( 0 <= m_SecondaryWeaponIndex && GetSecondaryWeapon() )
			GetSecondaryWeapon()->SetWeaponState( 0 );

		m_SecondaryWeaponIndex = slot_index;

		if( 0 <= slot_index && m_aWeaponSlot[slot_index].pWeapon )
			m_aWeaponSlot[slot_index].pWeapon->SetWeaponState( CGI_Weapon::STATE_SELECTED_AS_SECONDARY_WEAPON );
	}
}


inline void CWeaponSystem::UpdateWorldProperties( const Matrix34& rShooterWorldPose,
												  const Vector3& rvShooterVelocity,
												  const Vector3& rvShooterAngVelocity )
{
	for( int i=0; i<NUM_WEAPONSLOTS; i++ )
	{
		if( !m_aWeaponSlot[i].pWeapon )
			continue;

		m_aWeaponSlot[i].WorldPose = rShooterWorldPose * m_aWeaponSlot[i].LocalPose;

		m_aWeaponSlot[i].pWeapon->UpdateWorldProperties( rShooterWorldPose,
			                                             rvShooterVelocity,
														 rvShooterAngVelocity );

/*		m_aWeaponSlot[i].WorldPose = rShooterWorldPose * m_aWeaponSlot[i].LocalPose;
		CGI_Weapon* pWeapon = m_aWeaponSlot[i].pWeapon;
		if( !pWeapon )
			continue;
*/
//		pWeapon->UpdateWorldMuzzlePose( m_aWeaponSlot[i].WorldPose );
//		m_aWeaponSlot[i].MuzzleEndWorldPose
//			= m_aWeaponSlot[i].WorldPose * pWeapon->GetMuzzleEndLocalPose();
	}
}

} // namespace amorphous



#endif		/*  __WEAPONSYSTEM_H__  */
