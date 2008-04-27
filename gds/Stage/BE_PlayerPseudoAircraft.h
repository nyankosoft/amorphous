#ifndef  __BE_PLAYERPSEUDOAIRCRAFT_H__
#define  __BE_PLAYERPSEUDOAIRCRAFT_H__

#include <Stage/BE_Player.h>
#include <Stage/BaseEntityHandle.h>
#include <Stage/CopyEntity.h>
#include <Stage/GameMessage.h>

#include "Support/FixedVector.h"

#include "GameCommon/PseudoAircraftSimulator.h"
#include "Item/GI_Aircraft.h"

#include "BEC_PseudoAircraft.h"


class CPlayerVisionMode
{
public:
	enum eMode
	{
		Normal,
		NightVision,
		Thermal,
		NumVisionTypes
	};
};


class HUD_TargetInfo
{
public:
	enum param { MAX_TITLE_LENGTH = 20 };
	enum type_flag
	{
		TGT_AIR		= (1 << 0),
		TGT_SURFACE	= (1 << 1),
		PLAYER		= (1 << 2),
		ENEMY		= (1 << 3),
		ALLY		= (1 << 4),
		MISSILE		= (1 << 5),
		NEUTRAL		= (1 << 6),
		FOCUSED		= (1 << 7),
		LOCKED_ON	= (1 << 8),
		NOT_LOCKABLE= (1 << 9),
	};

	HUD_TargetInfo() {}

	HUD_TargetInfo( const Vector3& pos, char* _title, int _type )
		: position(pos), title(_title), type(_type) {}

	Vector3 position;

	Vector3 direction;

	float radius;

	char *title;

	int type;
};


class RadarInfo
{
	std::vector<HUD_TargetInfo> m_vecTargetInfo;

	std::vector<int> m_vecVisibleTargetIndex;

	std::vector<int> m_vecLocalTargetIndex;

	int m_FocusedTargetIndex;

public:

	RadarInfo() : m_FocusedTargetIndex(-1) {}
	~RadarInfo() {}

	const std::vector<HUD_TargetInfo>& GetAllTargetInfo() const { return m_vecTargetInfo; }

	size_t GetNumLocalTargets() const { return m_vecLocalTargetIndex.size(); }

	const HUD_TargetInfo& GetLocalTarget( int index ) const { return m_vecTargetInfo[ m_vecLocalTargetIndex[index] ]; }

	size_t GetNumVisibleTargets() const { return m_vecVisibleTargetIndex.size(); }

	const HUD_TargetInfo& GetVisibleTarget( int index ) const { return m_vecTargetInfo[ m_vecVisibleTargetIndex[index] ]; }

	inline void ClearTargetInfo();

	const HUD_TargetInfo *GetFocusedTarget() const { return 0 <= m_FocusedTargetIndex ? &m_vecTargetInfo[m_FocusedTargetIndex] : NULL; }

//	void Update( float frametime );

	friend class CBE_PlayerPseudoAircraft;
	friend class HUD_PlayerAircraft;
};


inline void RadarInfo::ClearTargetInfo()
{
	m_vecTargetInfo.resize(0);
	m_vecVisibleTargetIndex.resize(0);
	m_vecLocalTargetIndex.resize(0);

	m_FocusedTargetIndex = -1;
}


class CBEC_ExtraBaseEntity : public IArchiveObjectBase
{
public:
	Matrix34 InitLocalPose;
	CBaseEntityHandle Handle;

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & Handle & InitLocalPose;
	}
};


/**
 * holds a copy entity of aircraft for a single player
 */
class CBE_PlayerPseudoAircraft : public CBE_Player
{
private:

	enum eViewPointType
	{
		FIRST_PERSON_VIEW = 0,
		THIRD_PERSON_VIEW,
		NUM_VIEWPOINTS
	};

	enum param
	{
		NUM_MAX_FOCUS_CANDIDATES = 12
	};

	int m_Viewpoint;

	/// one of the aircraft items owned by the player
	/// borrowed reference
	CGI_Aircraft* m_pAircraft;

	CBEC_AircraftNozzleFlames m_NozzleFlames;

	SCDVariable<Matrix33> m_CameraOrient;	///< used in third-person view mode

	SCDVariable<float> m_CamHeading, m_CamPitch;

	std::vector<Matrix34> m_vecWeaponSlotLocalPose;

	std::vector<CCopyEntity *> m_vecpVisibleEntity;

	RadarInfo m_RadarInfo;

	struct FocusCandidate
	{
		CCopyEntity *pEntity;
		float score;

		FocusCandidate() : pEntity(NULL), score(0.0f) {}
		FocusCandidate( CCopyEntity *entity, float _score ) : pEntity(entity), score(_score) {}
	};

	TCFixedVector<FocusCandidate, NUM_MAX_FOCUS_CANDIDATES> m_vecFocusCandidate;

	int m_CurrentTargetFocusIndex;

	CCopyEntity* m_pFocusedTarget;

	int m_State;

	int m_MissionState;

	Matrix34 m_WorldPoseOnMissionFailed;

	enum eExtraBaseEntity
	{
		EBE_SMOKE_TRAIL,
		EBE_FLAME_TRAIL,
		EBE_EXPLOSION,
		NUM_EXTRA_BASE_ENTITIES
	};

	CBEC_ExtraBaseEntity m_aExtraBaseEntity[NUM_EXTRA_BASE_ENTITIES];

	int m_VisionMode;

private:

	bool SetAircraft();

	void UpdateObjectsInSight( CCopyEntity* pCopyEnt );

	/**
	 * updates candidates which can be focused on
	 */
	void UpdateFocusCandidateTargets( const vector<CCopyEntity *>& vecpEntityBuffer );

	/**
	 * focus candidates are updated together
	 */
	void UpdateRadarInfo( CCopyEntity* pCopyEnt );

	void CreateNozzleFlames( CCopyEntity* pCopyEnt );

	inline void UpdateNozzleFlamesVisibility();

	int GetVisionMode() const { return m_VisionMode; }

//	void EnableNightVision( bool enable );

//	void ToggleNightVision();

public:

	CBE_PlayerPseudoAircraft();

	~CBE_PlayerPseudoAircraft();

	virtual void Init();

	virtual void InitCopyEntity(CCopyEntity* pCopyEnt);

	virtual void Act(CCopyEntity* pCopyEnt);

	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	virtual void OnEntityDestroyed(CCopyEntity* pCopyEnt);

	virtual void Draw( CCopyEntity *pCopyEnt );

	void Move( CCopyEntity *pCopyEnt );

	void UpdateCamera(CCopyEntity* pCopyEnt);

	virtual void UpdatePhysics( CCopyEntity *pCopyEnt, float dt );

//	void PlayerDead(CCopyEntity* pCopyEnt);

//	void AdaptToNewScreenSize() {}
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
	virtual void ReleaseGraphicsResources();

	virtual unsigned int GetArchiveObjectID() const { return BE_PLAYERPSEUDOAIRCRAFT; }

	virtual void Serialize( IArchive& ar, const unsigned int version );


	/// updates bone transformations for aircraft animation (e.g. flaps, gears)
	void UpdateRenderStates();

	CPseudoAircraftSimulator& PseudoSimulator() { return m_pAircraft->PseudoSimulator(); }

	CGI_Aircraft *GetAircraft() { return m_pAircraft; }

	bool HandleInput( SPlayerEntityAction& input );

//	inline void SetAccel( float accel );

	/// returns true if all the aircraft is on the ground
	inline bool IsOnGround();

	inline float GetCameraHeading() const { return m_CamHeading.current; }
	inline float GetCameraPitch() const { return m_CamPitch.current; }

	inline void CycleViewpoint();

	inline void CycleTargetFocus();

	void SetVisionMode( int vision_mode );

	const RadarInfo& GetRadarInfo() const { return m_RadarInfo; }

	void SetAircraftState( int aircraft_state ) { m_State = aircraft_state; }
	int GetAircraftState() const { return m_State; }

	enum ePseudoAircraftState
	{
		STATE_NORMAL,
		STATE_LOCKED_ON,
		STATE_MISSILE_APPROACHING,
		NUM_STATES
	};

	void SetMissionState( int mission_state ) { m_MissionState = mission_state; }
	int GetMissionState() const { return m_MissionState; }

	enum eMissionState
	{
		MSTATE_NOT_IN_MISSION,
		MSTATE_IN_MISSION,
		MSTATE_MISSION_ACCOMPLISHED,
//		MSTATE_MISSION_FAILED,
		MSTATE_CRASHED,               ///< crashed - mission failed
		MSTATE_KIA,                   ///< kill in action. e.g.) shot down - mission failed
		MSTATE_TIMEUP,                ///< time up - mission failed
		NUM_MISSION_STATES
	};

	const Matrix34& GetWorldPoseOnMissionFailed() const { return m_WorldPoseOnMissionFailed; }

//	const std::vector<CCopyEntity *>& GetVisibleEntity() const { return m_vecpVisibleEntity; }

//	void ToggleHeadLight();

};


/*
void InitPlayerAircraft()
{
	// let the player select one from the available planes
	CGI_Aircraft* pAircraft = SelectAircraft();

	CWeaponSystem& weapon_system = pAircraft->WeaponSystem();

	weapon_system.GetWeaponSlot(0).Mount( "20mmMG" );
	weapon_system.GetWeaponSlot(1).Mount( "SML" );	// standard missile launcher
//	weapon_system.GetWeaponSlot(2).Mount( "SPW" );

	m_pAircraftBaseEntity->SetAircraft( pAircraft );
}
*/


// ================================ inline implementations ================================ 

/*
inline void CBE_PlayerPseudoAircraft::SetAccel( float accel )
{
	m_AircraftSimulator.SetAccel( accel );
}
*/

inline bool CBE_PlayerPseudoAircraft::IsOnGround()
{
	return false;
}


inline void CBE_PlayerPseudoAircraft::CycleViewpoint()
{
	m_Viewpoint = ( m_Viewpoint + 1 ) % NUM_VIEWPOINTS;

	UpdateNozzleFlamesVisibility();
}


inline void CBE_PlayerPseudoAircraft::UpdateNozzleFlamesVisibility()
{
	int nozzle_frame_entity_offset = m_pLaserDotEntity ? 1 : 0;

	if( m_Viewpoint == THIRD_PERSON_VIEW )
		m_NozzleFlames.ShowNozzleFlames( m_pPlayerCopyEntity, nozzle_frame_entity_offset, m_pAircraft );
	else
		m_NozzleFlames.HideNozzleFlames( m_pPlayerCopyEntity, nozzle_frame_entity_offset, m_pAircraft );
}


inline void CBE_PlayerPseudoAircraft::CycleTargetFocus()
{
	if( m_vecFocusCandidate.size() == 0 )
		return;

//	if( !m_pFocusedTarget || !m_pFocusedTarget->inuse )

	int num_focus_candidates = (int)m_vecFocusCandidate.size();
	m_CurrentTargetFocusIndex = ( m_CurrentTargetFocusIndex + 1 ) % num_focus_candidates;

	m_pFocusedTarget = m_vecFocusCandidate[m_CurrentTargetFocusIndex].pEntity;
}

/*
inline void CBE_PlayerPseudoAircraft::ToggleNightVision()
{
	m_bNightVisionEnabled = !m_bNightVisionEnabled;
    EnableNightVision( m_bNightVisionEnabled );
}
*/

#endif		/*  __BE_PLAYERPSEUDOAIRCRAFT_H__  */
