#ifndef  __FG_AIAircraftEntity_H__
#define  __FG_AIAircraftEntity_H__


#include "Stage/fwd.h"
#include "Stage/CopyEntity.h"
#include "Stage/EntityMotionPathRequest.h"
#include "Item/GI_Aircraft.h"
#include "Item/GI_Ammunition.h"
#include "Item/GI_Weapon.h"
#include "fwd.h"


class CFG_AIAircraftEntityDesc;


class CFG_AIAircraftEntity : public CCopyEntity
{
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

	CEntityGroupHandle m_ProjectileEntityGroup;

	CEntityGroupHandle m_TargetFilter;

	CBaseEntityHandle m_Missile;

	// variables that hold states

	CCopyEntity *m_pCurrentTarget;
//	CCopyEntityHandle m_CurrentTarget

	/// holds when the current target was set
	double m_TimeOfLastTargetUpdate;

	float m_fTargetSensorElapsedTime;

	float m_fTargetSensoringInterval;

	double m_dLastFireTime;

private:

	CPseudoAircraftSimulator& GetPseudoSimulator() { return m_pAircraft->PseudoSimulator(); }

	void UpdateTarget();

	void SelectTarget();

	void SetTarget( CCopyEntity *pTargetEntity, double current_time );

	void Fire( Vector3 vToTarget );

public:

	enum eTargetingPolicy
	{
		TP_ALWAYS_TARGET_PLAYER,
//		TP_CHOOSE_RANDOMLY_FROM_TARGET_GROUP,
		TP_SELECT_ANY_TARGET, ///< choose a target randomly from the target group
		TP_SELECT_SURFACE_TARGET,
		TP_SELECT_AIR_TARGET,
		NUM_TARGETING_POLICIES
	};

	enum eActionPolicy
	{
		AP_ENGAGE_AT_WILL,
		AP_COVER,
		NUM_ACTION_POLICIES
	};

	CFG_AIAircraftEntity();

	virtual ~CFG_AIAircraftEntity();

//	virtual void Init( CCopyEntityDesc& desc );

//	void Init( CFG_AIAircraftEntityDesc& desc );

	virtual void Update( float dt );

	virtual void UpdatePhysics( float dt );

	virtual void HandleMessage( SGameMessage& msg );

	virtual void TerminateDerived();

	friend class CFG_AIAircraftBaseEntity;
};


#endif		/*  __FG_AIAircraftEntity_H__  */
