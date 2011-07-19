#ifndef __BE_Enemy_H__
#define __BE_Enemy_H__

#include "BE_PhysicsBaseEntity.hpp"
#include "BaseEntityHandle.hpp"
#include "CopyEntity.hpp"
#include "EnemyState.hpp"
#include "Serialization_BaseEntityHandle.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"

#include "Sound/SoundHandle.hpp"
#include "GameCommon/RangedSet.hpp"


class CBEC_MotionPath;

struct SBE_EnemyExtraData : public IArchiveObjectBase
{
	float fCurrentManeuverTime;
	float fTotalManeuverTime;
	Vector3 vManeuverDirection;
	Vector3 vLastCheckedPlayerPosition;
	Vector3 vLastCheckedDirectionToPlayer;
	float fLastCheckedSqDistToPlayer;
	Vector3 vLastHitFromThisDirection;
	Vector3 vOriginalPosition;
	Vector3 vTargetPosition;
	float fSensoringInterval2;

	CBEC_MotionPath *pPath;

	SBE_EnemyExtraData();

	void Serialize( IArchive& ar, const unsigned int version );
};


class CRewardItem : public IArchiveObjectBase
{
public:
//	string m_strName;

	CBaseEntityHandle m_ItemEntity;

	float m_fProbability;

	CRewardItem() : m_fProbability(0) {}

	virtual void Serialize( IArchive& ar, const unsigned int version ) { ar & m_ItemEntity & m_fProbability; }
};



class CEnemyState;

class CBE_Enemy : public CBE_PhysicsBaseEntity
{
protected:

	enum eParams
	{
		NUM_BULLET_TYPES    = 2,
		NUM_MAX_FRAGMENTS   = 16,
		NUM_MAX_EXTRADATA   = 96,
	};

	SBE_EnemyExtraData m_aExtraData[NUM_MAX_EXTRADATA];
	int m_iExtraDataIndex;
	CEnemyState *m_apEnemyState[CEnemyState::NUM_STATES];

	CEntityGroupHandle m_ProjectileEntityGroup;

	enum eAttrib
	{
		ATTRIB_OPTCAM	= (1 << 0),	// optical camouflage capability
	};

	unsigned int m_AttribFlag;

	float m_fMobility;
	float m_fYawSpeed;
	float m_fPitchSpeed;

	CBaseEntityHandle m_Bullet[NUM_BULLET_TYPES];
	float m_fBulletPower;
	float m_fBulletSpeed;
	float m_fFiringRate;	///, intervals between each shot [sec]
	float m_fFireKeepDuration;
	float m_fFireCeaseInterval;
	float m_fRandomFireDirection;

	CSoundHandle m_FireSound;

	/// muzzle position in local space
	Vector3 m_vLocalMuzzlePosition;

	CBaseEntityHandle m_MuzzleFlash;

	int m_iRandomSearchManeuver;

	CBaseEntityHandle m_Explosion;

	CBaseEntityHandle m_Wreck;

	/// debris that are generated when the entity is destroyed
	class Fragment : public IArchiveObjectBase
	{
	public:
		CBaseEntityHandle BaseEntity;
		Vector3 vOffset;
		RangedSet<float> InitVelocity;

		virtual void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & BaseEntity & vOffset & InitVelocity.min, & InitVelocity.max;
		}
	};

	Fragment m_aFragment[NUM_MAX_FRAGMENTS];

//	CBaseEntityHandle	m_aFragment[NUM_MAX_FRAGMENTS];
//	Vector3 m_avFragmentOffset[NUM_MAX_FRAGMENTS];
	int m_iNumFragments;

	/// items that may be created after the enemy is destroyed by the player
	std::vector<CRewardItem> m_vecRewardItem;

protected:

	void Fire( CCopyEntity* pCopyEnt );

	bool CheckRayToPlayer( CCopyEntity* pCopyEnt );

	virtual void OnDestroyed( CCopyEntity* pCopyEnt );

public:

	CBE_Enemy();

	virtual ~CBE_Enemy();

	virtual void Init();

	virtual void InitCopyEntity( CCopyEntity* pCopyEnt );

	virtual void Act(CCopyEntity* pCopyEnt);	// behavior in in one frame

	virtual void Draw(CCopyEntity* pCopyEnt);

	virtual void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);

	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );

	virtual void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_ENEMY; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	/// see if the player is in a visible position by checking the distance,
	/// obstacle, and angle. When the player is visible, 'rsMode' is updated to
	/// attack mode (ES_ATTACK)
	void SearchPlayer( CCopyEntity* pCopyEnt,
		               short& rsMode,
		               Vector3& rvDesiredDirection,
					   float* pfSqDistToPlayer = NULL );

	void UpdateDesiredYawAndPitch(CCopyEntity* pCopyEnt, Vector3& rvDesiredDirection);

	/// turn toward the desired direction and update the orientation
	void AimAlong(CCopyEntity* pCopyEnt, Vector3& rvDesiredDirection);

	void FireAtPlayer(CCopyEntity* pCopyEnt);

	virtual void SearchManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData) {}
	virtual void AttackManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData) {}
	virtual void ManeuverUnderAttack(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData) {}


	inline int GetNewExtraDataID()
	{
		if( m_iExtraDataIndex < NUM_MAX_EXTRADATA - 1 )
			return m_iExtraDataIndex++;
		else
			return 0;	// out of stock
	}

	inline SBE_EnemyExtraData* GetExtraData(int iIndex) { return (m_aExtraData + iIndex); }

	static float& DesiredDeltaYaw(CCopyEntity *pEntity) { return pEntity->f2; }
	static float& FireCycleTime(CCopyEntity *pEntity)   { return pEntity->f4; }
	static float& LastFireTime(CCopyEntity *pEntity)    { return pEntity->f5; }
};


#endif  /*  __BE_Enemy_H__  */
