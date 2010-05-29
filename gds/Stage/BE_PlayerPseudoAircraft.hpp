#ifndef  __BE_PLAYERPSEUDOAIRCRAFT_H__
#define  __BE_PLAYERPSEUDOAIRCRAFT_H__

#include <Stage/BE_Player.hpp>
#include <Stage/BaseEntityHandle.hpp>
#include <Stage/CopyEntity.hpp>
#include <Stage/GameMessage.hpp>
#include <Stage/BEC_PseudoAircraft.hpp>
#include <Stage/HUD_PlayerAircraft.hpp>
#include <Stage/SubDisplay.hpp>

#include "Support/FixedVector.hpp"
#include "GameCommon/PseudoAircraftSimulator.hpp"
#include "Item/GI_Aircraft.hpp"
#include "Item/Radar.hpp"


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

//	RadarInfo m_RadarInfo;

	CSubDisplay m_SubDisplay;

	struct FocusCandidate
	{
		CEntityHandle<> entity;
		float score;

		FocusCandidate() : score(0.0f) {}
		FocusCandidate( CEntityHandle<> _entity, float _score ) : entity(_entity), score(_score) {}
	};

	TCFixedVector<FocusCandidate, NUM_MAX_FOCUS_CANDIDATES> m_vecFocusCandidate;

	int m_CurrentTargetFocusIndex;

	CEntityHandle<> m_FocusedTarget;

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

	HUD_PlayerAircraft *m_pPlayerAircraftHUD;

	boost::shared_ptr<CRadar> m_pShortRangeRadar;

	boost::shared_ptr<CRadar> m_pLongRangeRadar;

	enum TextMessageType
	{
		TM_DESTROYED_ENEMY,
		TM_FIRED_MISSILE,
		TM_RELEASED_BOMB,
		TM_CRASHED,
		NUM_TEXT_MESSAGE_TYPES,
	};

	std::vector<int> m_vecMessageIndex[NUM_TEXT_MESSAGE_TYPES];

	// static properties

	static std::map<int,int> ms_mapEntityTypeIDtoTargetTypeFlag;

	static std::map<int,int> ms_mapEntityGroupToTargetGroupFlag;

	static std::map<int,int> ms_mapEntityGroupToTargetTypeFlag;

	static std::vector<int> ms_vecFocusTargetEntityGroup;

private:

	void InitSubDisplay();

	bool SetAircraft();

	void UpdateObjectsInSight( CCopyEntity* pCopyEnt );

	inline bool IsFocusTargetEntity( int entity_group );

	/**
	 * updates candidates which can be focused on
	 */
	void UpdateFocusCandidateTargets( const std::vector<CCopyEntity *>& vecpEntityBuffer );

	/**
	 * focus candidates are updated together
	 */
	void UpdateRadarInfo( CCopyEntity* pCopyEnt, float dt );

	void CreateNozzleFlames( CCopyEntity* pCopyEnt );

	inline void UpdateNozzleFlamesVisibility();

	int GetVisionMode() const { return m_VisionMode; }

	HUD_PlayerBase *GetHUD() { return m_pPlayerAircraftHUD; }

	CInputHandler_PlayerBase *CreatePlayerInputHandler();

	void OnDestroyingEnemyEntity( SGameMessage& msg );

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

	void SetSubDisplayType( CSubDisplayType::Name type );

//	const CRadarInfo& GetRadarInfo() const { return m_RadarInfo; }

	boost::shared_ptr<CRadar> ShortRangeRadar() { return m_pShortRangeRadar; }

	boost::shared_ptr<CRadar> LongRangeRadar() { return m_pLongRangeRadar; }

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

	static std::map<int,int>& EntityTypeIDtoTargetTypeFlagMap() { return  ms_mapEntityTypeIDtoTargetTypeFlag; }

	static std::map<int,int>& EntityGroupToTargetGroupFlagMap() { return  ms_mapEntityGroupToTargetGroupFlag; }

	static std::map<int,int>& EntityGroupToTargetTypeFlagMap() { return  ms_mapEntityGroupToTargetTypeFlag; }

	static std::vector<int>& FocusTargetEntityGroup() { return ms_vecFocusTargetEntityGroup; }

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


inline bool CBE_PlayerPseudoAircraft::IsFocusTargetEntity( int entity_group )
{
	std::vector<int>::iterator itr
		= std::find( ms_vecFocusTargetEntityGroup.begin(),
		             ms_vecFocusTargetEntityGroup.end(),
			         entity_group );

	return ( itr != ms_vecFocusTargetEntityGroup.end() );
}


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

	m_FocusedTarget = m_vecFocusCandidate[m_CurrentTargetFocusIndex].entity;
}

/*
inline void CBE_PlayerPseudoAircraft::ToggleNightVision()
{
	m_bNightVisionEnabled = !m_bNightVisionEnabled;
    EnableNightVision( m_bNightVisionEnabled );
}
*/

#endif		/*  __BE_PLAYERPSEUDOAIRCRAFT_H__  */
