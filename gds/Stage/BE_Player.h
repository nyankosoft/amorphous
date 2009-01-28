#ifndef  __BE_PLAYER_H__
#define  __BE_PLAYER_H__

#include "../base.h"
#include "BE_PhysicsBaseEntity.h"
#include "BaseEntityHandle.h"
#include "CopyEntity.h"
#include "GameMessage.h"
#include "Graphics/Camera.h"
#include "Graphics/fwd.h"

#include "GameInput/3DActionCode.h"

#include "GameCommon/CombatRecord.h"
#include "../UI/fwd.h" // Need to use the relative path to include the UI/fwd.h of GameStageFramework. Otherwise the DefaultKeyBindFG.cpp of the FlightGame project include UI/fwd.h of its project

#include "Sound/SoundHandle.h"

class CShockWaveCameraEffect;
class HUD_PlayerBase;


enum eThrustDirection
{
	THRUST_FORWARD = 0,
	THRUST_BACKWARD,
	THRUST_RIGHT,
	THRUST_LEFT,
	THRUST_UP,
	THRUST_DOWN
};


struct SPlayerEntityAction
{
	int ActionCode;
	int SecondaryActionCode;
	int type;
	float fParam;

	enum eInputType
	{
		KEY_PRESSED = 0,
		KEY_RELEASED,
	};

	SPlayerEntityAction() : ActionCode(-1), SecondaryActionCode(-1), type(KEY_PRESSED), fParam(0) {}
};


//class CBEP_MouseInputParams
class CBEP_AnalogInputParams
{
public:
	float m_fSensitivity_X;
	float m_fSensitivity_Y;
	float m_fAxisDirection_Y;		/// 1: no invert / -1: invert
};


#define PLYAER_DAMAGESOUND_VARIATION 6


/**
 Stores the state of the player in a stage
 - player base entity is the physical representation & status
   of a player in a stage. It does not store permanent information
   about the player data 
   - e.g.) items owned by the player, levels, etc.
 - Such information is stored in PLAYERINFO singleton class,
   and accessed dynamically or linked as borrowed reference
 */
class CBE_Player : public CBE_PhysicsBaseEntity
{
protected:

	U32 m_adwLastActionInputTime[NUM_ACTION_CODES];

	CCopyEntity* m_pPlayerCopyEntity;

	/// ENTITY_GROUP_MIN is set by default
	CEntityGroupHandle m_ProjectileEntityGroup;

	float m_fBoost;

	/// sound names played when the player gets shot
	CSoundHandle m_aDamageSound[NUM_DAMAGE_TYPES][PLYAER_DAMAGESOUND_VARIATION];

	/// view from the player
	CCamera m_Camera;

	Matrix34 m_CameraLocalPose;

	Matrix34 m_PrevCameraPose;

	CBaseEntityHandle m_LaserDot;
	CBaseEntityHandle m_HeadLight;

	CCopyEntity *m_pLaserDotEntity;
	CCopyEntity *m_pHeadLightEntity;
	bool m_bHeadLightOn;

	CShockWaveCameraEffect *m_pShockWaveEffect;

	Vector3 m_vLocalMuzzlePos;
	Matrix34 m_WorldMuzzlePose;

	CombatRecord m_CombatRecord;

	/// Shared reference of UI manager 
	/// - for operations / commands available to player in stage
	///   e.g.)
	///   - select and use items
	///   - issue orders to NPCs
	CGM_DialogManagerSharedPtr m_pCmdMenuDialogManager;

	/// owned reference
	CGM_DialogInputHandlerSharedPtr m_pCmdMenuInputHandler;

	std::vector<int> m_vecCmdMenuRootDialogID;

protected:

	void InitDamageSound();

	/// returns the camera position modified by the camera related effect,
	/// and requests screen effects to screen effect manager if necessary
	void UpdateCameraEffect( Vector3& vNewCameraPos, CCopyEntity* pCopyEnt );

public:

	CBE_Player();

	virtual ~CBE_Player();

	virtual void Init();
	virtual void InitCopyEntity(CCopyEntity* pCopyEnt);

	CCopyEntity* GetPlayerCopyEntity() { return m_pPlayerCopyEntity; }

	virtual void Act(CCopyEntity* pCopyEnt);

	virtual void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	virtual void OnEntityDestroyed(CCopyEntity* pCopyEnt);

	virtual void RenderStage(CCopyEntity* pCopyEnt);

	virtual void CreateRenderTasks(CCopyEntity* pCopyEnt);

	virtual void Draw(CCopyEntity *pCopyEnt);

	virtual void UpdateCamera(CCopyEntity* pCopyEnt);
	CCamera *GetCamera() { return &m_Camera; }

	virtual void Move( CCopyEntity *pCopyEnt );

	/// process input for various operations of the player entity
	virtual bool HandleInput( SPlayerEntityAction& input );

	virtual HUD_PlayerBase *GetHUD() { return NULL; }

	void PlayerDead(CCopyEntity* pCopyEnt);

	inline float GetCurrentLife();

	void SetCommandMenuUI( CGM_DialogManagerSharedPtr pDlgMgr,
		                   std::vector<int>& vecRootDialogID,
		                   CGM_DialogInputHandlerSharedPtr pDlgInputHandler = CGM_DialogInputHandlerSharedPtr() );


//	void AdaptToNewScreenSize() {}
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	inline void SetTrigger( int iTriggerState );
	inline void UnlockTriggerSafety( bool bUnlock );

	void LaserAimingDevice( bool bLaserDot );

	void ToggleHeadLight();

	CombatRecord& CombatRecord() { return m_CombatRecord; }

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


// ================================ inline implementations ================================ 

inline float CBE_Player::GetCurrentLife()
{
	if( m_pPlayerCopyEntity )
		return m_pPlayerCopyEntity->fLife;
	else
		return 0;
}


#endif		/*  __BE_PLAYER_H__  */
