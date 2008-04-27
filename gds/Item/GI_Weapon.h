#ifndef  __GAMEITEM_WEAPON_H__
#define  __GAMEITEM_WEAPON_H__

#include "GameItem.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"
#include "Sound/SoundHandle.h"
#include "Sound/Serialization_SoundHandle.h"
#include "Stage/fwd.h"
#include "Support/Serialization/Serialization_3DMath.h"

class CWeaponSystem;
struct SWeaponSlot;


//======================================================================================
// CGI_Weapon
//======================================================================================

class CGI_Weapon;
typedef boost::shared_ptr<CGI_Weapon> CGI_WeaponSharedPtr;

/**
 game item that represents firearm
*/
class CGI_Weapon : public CGameItem
{
protected:

	enum param
	{
		MAX_AMMOTYPE_LENGTH = 16
	};

	SWeaponSlot *m_pWeaponSlot;

	/// caliber
	char m_acAmmoType[MAX_AMMOTYPE_LENGTH];

	/// fire rate measured by the time between shots
	// (0.1 means the weapon fires 10 bulletes per sec)
	float m_fFireInterval;

	/// grouping in 10[m]
	float m_fGrouping;

	float m_fMuzzleSpeedFactor;

	/// recoil shock in the local coordinate frame
	/// where muzzle is pointing along z-axis
	Vector3 m_vLocalRecoilForce;

	CSoundHandle m_FireSound;

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

	static CStage* ms_pStage;

	inline CGI_Weapon();

	virtual ~CGI_Weapon() {}

	void SetWeaponSlot( SWeaponSlot *pSlot ) { m_pWeaponSlot = pSlot; }

//	virtual void Update( CWeaponSystem& rWeaponSystem );

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

	virtual void Fire();

	const char* GetAmmoType() const { return m_acAmmoType; }

	void SetData(char *pcName, char *pcAmmoType, float fFireInterval);
	void SetAmmoType( const char* pcAmmoType );
	void SetFireInterval(float fFireInterval) { m_fFireInterval = fFireInterval; }
	void SetGrouping(float fGrouping) { m_fGrouping = fGrouping; }
	void SetMuzzleSpeedFactor(float fMuzzleSpeedFactor) { m_fMuzzleSpeedFactor = fMuzzleSpeedFactor; }
	void SetFireSoundName( const char* pcFireSoundName );

	void SetMuzzleEndLocalPose( const Matrix34& MuzzleEndLocalPose ) { m_MuzzleEndLocalPose = MuzzleEndLocalPose; }

	/// called when the owner left the stage.
	/// missile lauchner needs to unload the loaded missiles
	/// since those loaded missiles are held as entity pointers
	virtual void Disarm() {}

	virtual unsigned int GetArchiveObjectID() const { return ID_FIREARMS; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;
};


inline CGI_Weapon::CGI_Weapon()
{
	m_TypeFlag |= (TYPE_WEAPON);

	m_pWeaponSlot = NULL;

	memset( m_acAmmoType, 0, sizeof(m_acAmmoType) );

	m_fFireInterval = 0.2f;

	m_fGrouping = 0.0f;

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


inline void CGI_Weapon::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	for( int i=0; i<MAX_AMMOTYPE_LENGTH; i++ )
		ar & m_acAmmoType[i];

	/* weapon data */
	ar & m_fFireInterval;
	ar & m_fGrouping;	// grouping in 10[m]
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

#endif  __GAMEITEM_WEAPON_H__
