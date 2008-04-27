#ifndef __BE_EnemyShip_H__
#define __BE_EnemyShip_H__

#include "BE_Enemy.h"
#include "CopyEntity.h"
#include "EntityMotionPathRequest.h"


class CGI_Weapon;
class CGI_Ammunition;


class CBEC_EnemyShipExtraData
{
public:

	bool m_bInUse;

	CBEC_MotionPath m_Path;

//	CPseudoAircraftSimulator m_AircraftSimulator;

	/// aircraft item that represents each enemy aircraft
	/// - owned reference
	/// - includes aircraft simulator and weapon system for each entity
	/// - 3D model in the item is not loaded. model data is managed by the base entity
//	CGI_Ship* m_pShip;

	/// owned reference
	std::vector<CGI_Weapon *> m_vecpWeapon;

	/// owned reference
	std::vector<CGI_Ammunition *> m_vecpAmmo;

//	float m_fAccel, m_fBoostAccel, m_fBrakeAccel;

	double m_dLastFireTime;

	CBEC_EnemyShipExtraData() : m_dLastFireTime(0.0), m_bInUse(false)/*, m_pShip(NULL)*/ {}

//	CPseudoAircraftSimulator& GetPseudoSimulator() { return m_pShip->PseudoSimulator(); }

	void Release();
};


class CBE_EnemyShip : public CBE_Enemy
{
private:

	enum PrivateParams
	{
		NUM_INIT_ENEMY_AIRCRAFTS = 16
	};

	CBaseEntityHandle m_Missile;

	/// holds extra information for each copy entity
	std::vector<CBEC_EnemyShipExtraData> m_vecExtraData;

	std::string m_strShipItemName;
	std::vector<std::string> m_vecWeaponItemName;
	std::vector<std::string> m_vecAmmoItemName;

private:

    void Fire( CCopyEntity* pCopyEnt, Vector3 vToTarget );

	/// updates the world pose and velocity of the entity
//	void UpdateScriptedMotionPath( CCopyEntity* pCopyEnt );

	void AddExtraData();

	void virtual OnDestroyed( CCopyEntity* pCopyEnt );

public:

	CBE_EnemyShip();
	virtual ~CBE_EnemyShip();

	virtual void Init();
	virtual void InitCopyEntity( CCopyEntity* pCopyEnt );

	virtual void Act(CCopyEntity* pCopyEnt);	// behavior in a frame

	virtual void UpdatePhysics( CCopyEntity *pCopyEnt, float dt );

	// void Draw(CCopyEntity* pCopyEnt); 
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_ENEMYSHIP; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

//	void SearchManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
//	void AttackManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
//	void ManeuverUnderAttack(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
	

	inline int GetNewExtraDataIndex();

	inline CBEC_EnemyShipExtraData& GetExtraData( CCopyEntity *pCopyEnt ) { return m_vecExtraData[pCopyEnt->iExtraDataIndex]; }

};


inline int CBE_EnemyShip::GetNewExtraDataIndex()
{
	size_t i, num_extra_data = m_vecExtraData.size();
	for( i=0; i<num_extra_data; i++ )
	{
		if( !m_vecExtraData[i].m_bInUse )
			return (int)i;
	}

	// add new extra data
	AddExtraData();

	return (int)num_extra_data;
}


#endif  /*  __BE_EnemyShip_H__  */
