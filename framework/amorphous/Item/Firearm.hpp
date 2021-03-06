#ifndef  __GameItem_Firearm_HPP__
#define  __GameItem_Firearm_HPP__

#include "../GameCommon/Caliber.hpp"
#include "GI_Weapon.hpp"


namespace amorphous
{

class CWeaponSystem;
struct SWeaponSlot;
class CGI_Ammunition;
class Cartridge;
class Magazine;


//======================================================================================
// Firearm
//======================================================================================


/**
 game item that represents firearm
*/
class Firearm : public CGI_Weapon
{
public:

	enum FirearmState
	{
		FS_SLIDE_FORWARD,
		FS_SLIDE_MOVING_FORWARD,
		FS_SLIDE_OPEN,
		NUM_FIREARM_STATES
	};

protected:

	Caliber::Name m_PrimaryCaliber;

	std::vector<Caliber::Name> m_Calibers;

	int m_StandardMagazineCapacity;

	std::shared_ptr<Cartridge> m_pChamberedCartridge;

	std::shared_ptr<Magazine> m_pMagazine;

	std::vector<std::string> m_ComplientMagazineNames;

	FirearmState m_FirearmState;

	bool m_IsSlideHeld;

	bool m_IsSlideStopEngaged;

	float m_fSlidePosition;

	float m_fSlideStrokeDistance;

	Vector3 m_vLocalHammerPivot;

	U32 m_NumTotalFiredRounds;

//	SWeaponSlot *m_pWeaponSlot;

	/// caliber
//	std::string m_strAmmoType;

	/// fire rate measured by the time between shots
	// (0.1 means the weapon fires 10 bulletes per sec)
//	float m_fFireInterval;

	/// grouping in 10[m]
	float m_fGrouping;

/*	float m_fMuzzleSpeedFactor;

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
*/

	/// weapon state - see CGI_Weapon

/*	Matrix34 m_MuzzleEndWorldPose;
	Vector3 m_vMuzzleEndVelocity;

	/// Stores the last time when the weapon fired ammo
	double m_dLastFireTime;

	int m_iCurrentBurstCount;

	int m_aTriggerState[2];

	int m_WeaponState;

	/// the entity which owns the weapon (borrowed reference)
	CCopyEntity *m_pOwnerEntity;
*/

private:

	void UpdateSlideMotionFromRearwardToForward( CStage& stage );

	void UpdateFirearmState( CStage& stage );

public:

/*	enum eWeaponStates
	{
		STATE_SELECTED_AS_PRIMARY_WEAPON,
		STATE_SELECTED_AS_SECONDARY_WEAPON,
		NUM_WEAPON_STETES
	};
*/
public:

	Firearm();

	virtual ~Firearm() {}

	virtual void Update( float dt );

	virtual bool HandleInput( int action_code, int input_type, float fParam );

	virtual void GetStatusForDebugging( std::string& dest_buffer ) const;

//	virtual void UpdateWorldProperties( const Matrix34& rShooterWorldPose,
//							            const Vector3& rvShooterVelocity,
//										const Vector3& rvShooterAngVelocity );

	/// return true if selected as either primary or secondary weapon in weapon system
//	bool IsWeaponSelected() { return ( GetWeaponState() == STATE_SELECTED_AS_PRIMARY_WEAPON || GetWeaponState() == STATE_SELECTED_AS_SECONDARY_WEAPON ); }

//	void SetOwner( CCopyEntity *pOwnerEntity ) { m_pOwnerEntity = pOwnerEntity; }

	// immediate action that follows a pull of the trigger
//	inline virtual void ImmediateTriggerAction( int iTrigger, CWeaponSystem& rWeaponSystem );

	virtual void Fire();

//	const std::string& GetAmmoType() const { return m_strAmmoType; }

//	void SetData(char *pcName, char *pcAmmoType, float fFireInterval);
//	void SetAmmoType( const char* pcAmmoType );
//	void SetFireInterval(float fFireInterval) { m_fFireInterval = fFireInterval; }
	void SetGrouping(float fGrouping) { m_fGrouping = fGrouping; }
//	void SetMuzzleSpeedFactor(float fMuzzleSpeedFactor) { m_fMuzzleSpeedFactor = fMuzzleSpeedFactor; }
//	void SetFireSoundName( const char* pcFireSoundName );

	void SetMuzzleEndLocalPose( const Matrix34& MuzzleEndLocalPose ) { m_MuzzleEndLocalPose = MuzzleEndLocalPose; }

	/// called when the owner left the stage.
	/// missile lauchner needs to unload the loaded missiles
	/// since those loaded missiles are held as entity pointers
	virtual void Disarm() {}

	void ChangeMagazine( std::shared_ptr<Magazine> pNewMagazine );

	std::shared_ptr<Magazine> DropMagazine() { std::shared_ptr<Magazine> pMag = m_pMagazine; m_pMagazine.reset(); return pMag; }

	const std::shared_ptr<Magazine> GetMagazine() const { return m_pMagazine; }

	bool IsSlideOpen() const;

	bool IsCartridgeChambered() const { return m_pChamberedCartridge ? true : false; }

	void FeedNextCartridge();

	void DisengageSlideStop();

	bool IsMagazineCompliant( const std::shared_ptr<Magazine>& pMagazine ) const;

	FirearmState GetFirearmState() const { return m_FirearmState; }

	void PullSlide( float fraction = 1.0f, bool hold_slide = false );

	void ReleaseSlide();

	void PushDownSlideRelease();

	void Decock();

	bool IsSlideAtItsMostRearwardPosition( float error = 0.001f ) const;

	bool IsReadyToFire() const;

	bool IsSlideHeld() const { return m_IsSlideHeld; }

	bool IsSlideStopEngaged() const { return m_IsSlideStopEngaged; }

	void InitFirearmMesh();

	virtual unsigned int GetArchiveObjectID() const { return ID_FIREARM; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( XMLNode& reader );
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
inline void Firearm::ImmediateTriggerAction( int iTrigger, CWeaponSystem& rWeaponSystem )
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



#endif  __GameItem_Firearm_HPP__
