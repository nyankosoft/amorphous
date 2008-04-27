#ifndef  __GAMEITEM_MISSILELAUNCHER_H__
#define  __GAMEITEM_MISSILELAUNCHER_H__

#include "GI_Weapon.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"

#include "3DCommon/Camera.h"
#include "Stage/ViewFrustumTest.h"
#include "../base.h"


class CCopyEntity;
class CWeaponSystem;
struct SWeaponSlot;


/**
 * missile launcher that is capable of sensoring the environment
 * to find targets.
 * The entity that represents the missile fired from the missile launcher
 * is given the target through a message
 */
class CGI_MissileLauncher : public CGI_Weapon
{
protected:

	int m_LauncherType;

	CCamera m_SensorCamera;

	float m_fValidSensorAngle;	///< in [rad]
	float m_fMaxSensorRange;

	/// the number of targets this launcher can simultaneously lock on
	int m_NumMaxSimulTargets;

	int m_NumReleasePositions;

	std::vector<Matrix34> m_ReleaseLocalPose;

	// state

	std::vector<Matrix34> m_ReleaseWorldPose;

	int m_CurrentReleasePoseIndex;

	CCopyEntity *m_pFocusedEntity;

	/// list of targets locked on by the missile launcher
	std::vector<CCopyEntity *> m_vecpCurrentTarget;

	int m_FireTargetIndex;

	//----------------- used when m_LauncherType == TYPE_LOAD_AND_RELEASE

	/// entities that represents loaded ammunition (borrowed reference)
	std::vector<CCopyEntity *> m_vecpLoadedAmmo;

	std::vector<U32> m_vecLastFireTimeMS;

	std::vector<Vector3> m_vecVelocityAtReleasePos;

	float m_fTargetSensoringInterval;

	float m_fFrameTimeAccumulation;

protected:

	bool ReadyToFire();

	void UpdateTargets();

	inline void SetTargetForMissile( CCopyEntity *pMissileEntity );

	bool ReleaseAmmo();

public:

	inline CGI_MissileLauncher();

	virtual ~CGI_MissileLauncher() {}

	virtual void Update( float dt );

	virtual bool HandleInput( int input_code, int input_type, float fParam );

	virtual void UpdateWorldProperties( const Matrix34& rShooterWorldPose,
							            const Vector3& rvShooterVelocity,
								        const Vector3& rvShooterAngVelocity );

	virtual void Fire();

	std::vector<CCopyEntity *>& GetCurrentTargetList() { return m_vecpCurrentTarget; }

	bool IsLockingOn( CCopyEntity *pEntity );

	/// set an entity for a primary target to lock on
	/// \return true entity has been registered as focused target
	/// \return false cannot register the entity as primary target.
	/// This is either the entity is not in effective area of the sensor to lock-on,
	/// or it cannot be targeted by the ammunition of this missile launcher
	/// (e.g. air-to-air missile on ground target)
	bool SetPrimaryTarget( CCopyEntity* target_entity );

	void SetNumMaxSimultaneousTargets( int num_targets );

	void SetNumReleasePositions( int num )
	{
		m_NumReleasePositions = num;
		m_ReleaseLocalPose.resize( num, Matrix34Identity() );
		m_ReleaseWorldPose.resize( num, Matrix34Identity() );

		/// used when m_LauncherType == TYPE_LOAD_AND_RELEASE
		m_vecLastFireTimeMS.resize( num, 0 );
		m_vecpLoadedAmmo.resize( num, NULL );
		m_vecVelocityAtReleasePos.resize( num, Vector3(0,0,0) );
	}

	void SetLocalReleasePose( int index, const Matrix34& pose );

	/// called when the owner left the stage.
	/// - need to release the loaded ammo stored as entity pointers
	virtual void Disarm();

	virtual unsigned int GetArchiveObjectID() const { return ID_MISSILELAUNCHER; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	enum eLauncherType
	{
		TYPE_LOAD_AND_RELEASE,
		TYPE_FIRE_IMMEDIATE,
		NUM_LAUNCHER_TYPES
	};

	friend class CItemDatabaseBuilder;
};


inline CGI_MissileLauncher::CGI_MissileLauncher()
{
	m_TypeFlag |= (TYPE_WEAPON);

	m_fValidSensorAngle	 = 1.5f;
	m_fMaxSensorRange	 = 200000.0f;

//	m_NumMaxSimulTargets = 1;
	SetNumMaxSimultaneousTargets( 1 );

	SetNumReleasePositions( 1 );

	m_CurrentReleasePoseIndex = 0;

	m_pFocusedEntity = NULL;

	m_FireTargetIndex = 0;

	m_vecpLoadedAmmo.resize( m_NumReleasePositions );
	m_vecLastFireTimeMS.resize( m_NumReleasePositions );

	m_LauncherType = TYPE_LOAD_AND_RELEASE;
//	m_LauncherType = TYPE_FIRE_IMMEDIATE;

	m_fTargetSensoringInterval = 0.12f;

	m_fFrameTimeAccumulation = 0.0f;
}


inline void CGI_MissileLauncher::Serialize( IArchive& ar, const unsigned int version )
{
	CGI_Weapon::Serialize( ar, version );

	ar & m_fValidSensorAngle;
	ar & m_fMaxSensorRange;

	ar & m_NumMaxSimulTargets;

	ar & m_NumReleasePositions;

	ar & m_ReleaseLocalPose;
	ar & m_ReleaseWorldPose;

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// initialize states
		m_CurrentReleasePoseIndex = 0;
		m_pFocusedEntity = NULL;
		m_vecpCurrentTarget.resize( 0 );
		m_FireTargetIndex = 0;

		m_vecpLoadedAmmo.resize( m_NumReleasePositions, NULL );
		m_vecLastFireTimeMS.resize( m_NumReleasePositions, 0 );
		m_vecVelocityAtReleasePos.resize( m_NumReleasePositions, Vector3(0,0,0) );

		m_fFrameTimeAccumulation = 0.0f;
	}

	ar & m_LauncherType;

	ar & m_fTargetSensoringInterval;
}




#endif  __GAMEITEM_MISSILELAUNCHER_H__
