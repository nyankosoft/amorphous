#ifndef  __GAMEITEM_WEAPON_H__
#define  __GAMEITEM_WEAPON_H__

#include "GameItem.hpp"

#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Sound/SoundHandle.hpp"
#include "amorphous/Sound/Serialization_SoundHandle.hpp"
#include "amorphous/Stage/fwd.hpp"
#include "amorphous/Support/Serialization/Serialization_3DMath.hpp"


namespace amorphous
{

class CWeaponSystem;
struct SWeaponSlot;


//======================================================================================
// CGI_Weapon
//======================================================================================

/**
 game item that represents firearm
*/
class CGI_Weapon : public GameItem
{
protected:

	SWeaponSlot *m_pWeaponSlot;

	/// caliber
	std::string m_strAmmoType;

	/// fire rate measured by the time between shots
	// (0.1 means the weapon fires 10 bulletes per sec)
	float m_fFireInterval;

	float m_fMuzzleSpeedFactor;

	/// recoil shock in the local coordinate frame
	/// where muzzle is pointing along z-axis
	Vector3 m_vLocalRecoilForce;

	SoundHandle m_FireSound;

	/// firing mechanism
	/// 1 or more: semi-auto burst triggers
	/// FIREMODE_FULLAUTO
	int m_iNumBursts;

	/// interval between burst shots [sec]
//	float m_fBurstInterval;

	/// holds a position at which the bullet entity is created.
	/// a valid muzzle end position must be set so that bullet entities 
	/// do not ovelap with the weapon when it is created
	Matrix34 m_MuzzleEndLocalPose;


	/// weapon state

	Matrix34 m_MuzzleEndWorldPose;
	Vector3 m_vMuzzleEndVelocity;

	/// Stores the last time when the weapon fired ammo
	double m_dLastFireTime;

	int m_iCurrentBurstCount;

	int m_aTriggerState[2];

	int m_WeaponState;

	/// the entity which owns the weapon (borrowed reference)
	CCopyEntity *m_pOwnerEntity;

public:

	enum eWeaponState
	{
		STATE_SELECTED_AS_PRIMARY_WEAPON,
		STATE_SELECTED_AS_SECONDARY_WEAPON,
		NUM_WEAPON_STETES
	};

public:

	CGI_Weapon();

	virtual ~CGI_Weapon() {}

	void SetWeaponSlot( SWeaponSlot *pSlot ) { m_pWeaponSlot = pSlot; }

	virtual void Update( float dt );

	virtual bool HandleInput( int input_code, int input_type, float fParam );

	virtual void UpdateWorldProperties( const Matrix34& rShooterWorldPose,
							            const Vector3& rvShooterVelocity,
							            const Vector3& rvShooterAngVelocity );

	int GetWeaponState() const { return m_WeaponState; }

	void SetWeaponState( int state ) { m_WeaponState = state; }

	/// return true if selected as either primary or secondary weapon in weapon system
	bool IsWeaponSelected() { return ( GetWeaponState() == STATE_SELECTED_AS_PRIMARY_WEAPON || GetWeaponState() == STATE_SELECTED_AS_SECONDARY_WEAPON ); }

	void SetOwner( CCopyEntity *pOwnerEntity ) { m_pOwnerEntity = pOwnerEntity; }

	// immediate action that follows a pull of the trigger
//	inline virtual void ImmediateTriggerAction( int iTrigger, CWeaponSystem& rWeaponSystem );

	virtual void Fire() {}

	const std::string& GetAmmoType() const { return m_strAmmoType; }

	void SetData(char *pcName, char *pcAmmoType, float fFireInterval);
	void SetAmmoType( const char* pcAmmoType );
	void SetFireInterval(float fFireInterval) { m_fFireInterval = fFireInterval; }
	void SetMuzzleSpeedFactor(float fMuzzleSpeedFactor) { m_fMuzzleSpeedFactor = fMuzzleSpeedFactor; }
	void SetFireSoundName( const char* pcFireSoundName );

	void SetMuzzleEndLocalPose( const Matrix34& MuzzleEndLocalPose ) { m_MuzzleEndLocalPose = MuzzleEndLocalPose; }

	/// called when the owner left the stage.
	/// missile lauchner needs to unload the loaded missiles
	/// since those loaded missiles are held as entity pointers
	virtual void Disarm() {}

	virtual unsigned int GetArchiveObjectID() const { return ID_WEAPON; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	friend class CItemDatabaseBuilder;
};


/*
inline Vector3 CJL_PhysicsActor::GetPointVelocity( const Vector3 & vWorldPoint ) 
{
	Vector3 v = Vec3Cross( GetAngularVelocity(), vWorldPoint - GetPosition() );
	v += GetVelocity();
	return v;
}
*/

/*
inline void CGI_Weapon::ImmediateTriggerAction( int iTrigger, CWeaponSystem& rWeaponSystem )
{
	if( iTrigger == 0 )
	{
		// trigger has been released
		if( 0 < m_iNumBursts )
		{
            m_iCurrentBurstCount = 0;
		}
	}
}
*/
} // namespace amorphous



#endif  __GAMEITEM_WEAPON_H__
