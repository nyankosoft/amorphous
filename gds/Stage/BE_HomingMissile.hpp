#ifndef __BE_HOMINGMISSILE_H__
#define __BE_HOMINGMISSILE_H__

#include "CopyEntity.hpp"
#include "BE_Explosive.hpp"
#include "BaseEntityHandle.hpp"
#include "gds/Graphics/Camera.hpp"


namespace amorphous
{


inline short& MissileState( CCopyEntity* pCopyEnt ) { return pCopyEnt->s1; }
inline float& MissileIgnitionTimer(CCopyEntity* pCopyEnt) { return pCopyEnt->f5; }


class CBE_HomingMissile : public CBE_Explosive
{
private:

	enum flag
	{
		DONT_LOSE_TARGET_EVEN_IF_OBSTRUCTED = (1 << 0),
//		 = (1 << 1),
//		 = (1 << 2),
//		 = (1 << 3),
	};

	/// true if the missile has its own sensor and capable of spotting & locking on targets
	bool m_bSensor;

	CCamera m_InitSensorCamera;

	float m_fValidSensorAngle;
	float m_fMaxSensorRange;

	/// maximum distance the missile can travel
	float m_fMaximumRange;

	float m_fAcceleration;

	float m_fMaxSpeed;

	/// how often the missle checks the ray to the target & corrects the course to follow a target
	/// XXX set -1 to disable sensing. This will prevent the missile to lose target even if it is obstructed
	/// XXX by another object.
	float m_fSensoringInterval;

	float m_fTurnSpeed;			///< how quickly the missile can turn to follow a target [rad/sec]

	/// missile explodes if the distance to the target is below this threshold
	/// even if not actually hitting the target.
	/// zero by default (i.e. no proximity sensor)
	float m_fProximityThresholdDistSq;

	int m_MissileFlag;

	CBaseEntityHandle m_SmokeTrail;
	CBaseEntityHandle m_FlameTrail;
	CBaseEntityHandle m_Light;


	void FindTarget(CCopyEntity* pCopyEnt);

	void MissileMove( CCopyEntity* pCopyEnt, float frametime );

	void UpdateMissile( CCopyEntity* pCopyEnt, float frametime );

	inline void CorrectCourse( CCopyEntity* pCopyEnt, float frametime );

public:

	CBE_HomingMissile();
	~CBE_HomingMissile() {}

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);

	void UpdatePhysics( CCopyEntity *pCopyEnt, float dt );

	// void Draw(CCopyEntity* pCopyEnt); {}
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_HOMINGMISSILE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	void Ignite( CCopyEntity* pCopyEnt );

	enum eMissileState
	{
		MS_LOADED,
		MS_WAITING_IGNITION,
		MS_IGNITED,
		MS_DETONATING, ///< set when the missile hit something. CBE_Explosive::Explode() is called at the next turn
		MS_DETONATED,
		NUM_MISSILE_STATES
	};

};
} // namespace amorphous



#endif  /*  __BE_HOMINGMISSILE_H__  */
