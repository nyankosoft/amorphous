#ifndef __BE_EnemyAircraft_H__
#define __BE_EnemyAircraft_H__

#include "BE_Enemy.h"
#include "CopyEntity.h"
#include "EntityMotionPathRequest.h"

#include "BEC_PseudoAircraft.h"
#include "Item/GI_Aircraft.h"
#include "GameCommon/MTRand.h"


class CGI_Weapon;
class CGI_Ammunition;


class CBEC_EnemyAircraftExtraData
{
public:

	bool m_bInUse;

	CBEC_MotionPath m_Path;

//	CPseudoAircraftSimulator m_AircraftSimulator;

	/// aircraft item that represents each enemy aircraft
	/// - owned reference
	/// - includes aircraft simulator and weapon system for each entity
	/// - 3D model in the item is not loaded. model data is managed by the base entity
	CGI_Aircraft* m_pAircraft;

	/// owned reference
	std::vector<CGI_Weapon *> m_vecpWeapon;

	/// owned reference
	std::vector<CGI_Ammunition *> m_vecpAmmo;

	CCopyEntity *m_pCurrentTarget;

	/// holds when the current target was set
	double m_TimeOfLastTargetUpdate;

	float m_fTargetSensorElapsedTime;

	float m_fTargetSensoringInterval;

	double m_dLastFireTime;

public:

	CBEC_EnemyAircraftExtraData()
		:
	m_dLastFireTime(0.0),
	m_bInUse(false),
	m_pAircraft(NULL),
	m_pCurrentTarget(NULL),
	m_TimeOfLastTargetUpdate(0)
	{
		m_fTargetSensoringInterval = 2.0f;
		m_fTargetSensorElapsedTime = RangedRand( 0.0f, m_fTargetSensoringInterval );
	}

	~CBEC_EnemyAircraftExtraData() { Release(); }

	CPseudoAircraftSimulator& GetPseudoSimulator() { return m_pAircraft->PseudoSimulator(); }

	void Release();

	void UpdateTarget( CCopyEntity *pTargetEntity, double current_time );
};


class CBE_EnemyAircraft : public CBE_Enemy
{
private:

	enum PrivateParams
	{
		NUM_INIT_ENEMY_AIRCRAFTS = 16
	};

	CBaseEntityHandle m_Missile;

	CBEC_AircraftNozzleFlames m_NozzleFlames;

	/// holds extra information for each copy entity
	std::vector<CBEC_EnemyAircraftExtraData> m_vecExtraData;

	std::string m_strAircraftItemName;
	std::vector<std::string> m_vecWeaponItemName;
	std::vector<std::string> m_vecAmmoItemName;

	CEntityGroupHandle m_TargetFilter;

private:

    void Fire( CCopyEntity* pCopyEnt, Vector3 vToTarget );

	/// updates the world pose and velocity of the entity
//	void UpdateScriptedMotionPath( CCopyEntity* pCopyEnt );

	void AddExtraData();

	void OnDestroyed( CCopyEntity* pCopyEnt );

	void UpdateTarget( CCopyEntity* pCopyEnt );

	void SelectTarget( CCopyEntity* pCopyEnt );

public:

	CBE_EnemyAircraft();
	virtual ~CBE_EnemyAircraft();

	virtual void Init();
	virtual void InitCopyEntity( CCopyEntity* pCopyEnt );

	virtual void Act(CCopyEntity* pCopyEnt);	// behavior in a frame

	virtual void UpdatePhysics( CCopyEntity *pCopyEnt, float dt );

	virtual void Draw(CCopyEntity* pCopyEnt);

	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_ENEMYAIRCRAFT; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

//	void SearchManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
//	void AttackManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
//	void ManeuverUnderAttack(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
	

	inline int GetNewExtraDataIndex();

	inline CBEC_EnemyAircraftExtraData& GetExtraData( CCopyEntity *pCopyEnt ) { return m_vecExtraData[pCopyEnt->iExtraDataIndex]; }

};


inline int CBE_EnemyAircraft::GetNewExtraDataIndex()
{
	int i, num_extra_data = (int)m_vecExtraData.size();
	for( i=0; i<num_extra_data; i++ )
	{
		if( !m_vecExtraData[i].m_bInUse )
			return i;
	}

	// add new extra data
	AddExtraData();

	return num_extra_data;
}


#endif  /*  __BE_EnemyAircraft_H__  */
