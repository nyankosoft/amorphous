#ifndef  __GAMEITEM_MISSILELAUNCHER_H__
#define  __GAMEITEM_MISSILELAUNCHER_H__

#include "GI_Weapon.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"

#include "Graphics/Camera.h"
#include "Stage/fwd.h"
#include "Stage/ViewFrustumTest.h"
#include "../base.h"


class CWeaponSystem;
struct SWeaponSlot;


class CMissileHolder : public IArchiveObjectBase
{
public:

	// storage

	Matrix34 ReleaseLocalPose;

	// states - used during runtime

	Matrix34 ReleaseWorldPose;

	//----------------- used when m_LauncherType == TYPE_LOAD_AND_RELEASE

	/// entities that represents loaded ammunition (borrowed reference)
	CCopyEntity *pLoadedAmmo;

	U32 LastFireTimeMS;

	Vector3 vVelocityAtReleasePos;

public:

	CMissileHolder();

	void Serialize( IArchive& ar, const unsigned int version );

	void LoadFromXMLNode( CXMLNodeReader& reader );

	void InitStates();
};


/**
 * missile launcher that is capable of sensoring the environment
 * to find targets.
 * The entity that represents the missile fired from the missile launcher
 * is given the target through a message
 */
class CGI_MissileLauncher : public CGI_Weapon
{
protected:

	// types - saved to and loaded from the disk

	int m_LauncherType;

	CCamera m_SensorCamera;

	float m_fValidSensorAngle;	///< in [rad]
	float m_fMaxSensorRange;

	/// the number of targets this launcher can simultaneously lock on
	int m_NumMaxSimulTargets;

	int m_NumReleasePositions;

	// storage & states

	std::vector<CMissileHolder> m_vecMissileHolder;

	// states - used during runtime

	int m_CurrentReleasePoseIndex;

	CCopyEntity *m_pFocusedEntity;

	/// list of targets locked on by the missile launcher
	std::vector<CCopyEntity *> m_vecpCurrentTarget;

	int m_FireTargetIndex;

	vector<CCopyEntity *> m_vecpVisibleEntity;

	//----------------- used when m_LauncherType == TYPE_LOAD_AND_RELEASE

	float m_fTargetSensoringInterval;

	float m_fFrameTimeAccumulation;

protected:

	bool ReadyToFire();

	void UpdateTargets();

	inline void SetTargetForMissile( CCopyEntity *pMissileEntity );

	bool ReleaseAmmo();

public:

	CGI_MissileLauncher();

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
		m_vecMissileHolder.resize( num );
	}

	void SetLocalReleasePose( int index, const Matrix34& pose );

	void InitStates();

	/// called when the owner left the stage.
	/// - need to release the loaded ammo stored as entity pointers
	virtual void Disarm();

	virtual unsigned int GetArchiveObjectID() const { return ID_MISSILELAUNCHER; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	enum eLauncherType
	{
		TYPE_LOAD_AND_RELEASE,
		TYPE_FIRE_IMMEDIATE,
		NUM_LAUNCHER_TYPES
	};

	friend class CItemDatabaseBuilder;
};




#endif  __GAMEITEM_MISSILELAUNCHER_H__
