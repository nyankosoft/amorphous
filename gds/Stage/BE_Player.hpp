#ifndef  __BE_PLAYER_H__
#define  __BE_PLAYER_H__


#include "BE_PhysicsBaseEntity.hpp"
#include "BaseEntityHandle.hpp"
#include "CopyEntity.hpp"
#include "GameMessage.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Input/InputHandler.hpp"
#include "gds/GameCommon/3DActionCode.hpp"
#include "gds/GameCommon/CombatRecord.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/Sound/SoundHandle.hpp"


namespace amorphous
{

class InputHandler_PlayerBase;
class HUD_PlayerBase;
class CShockWaveCameraEffect;


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

	SPlayerEntityAction() : ActionCode(-1), SecondaryActionCode(-1), type(ITYPE_KEY_PRESSED), fParam(0) {}
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
 - Such information is stored in CSinglePlayerInfo singleton class,
   and accessed dynamically or linked as borrowed reference
 */
class CBE_Player : public CBE_PhysicsBaseEntity
{
protected:

	U32 m_adwLastActionInputTime[NUM_ACTION_CODES];

	CCopyEntity* m_pPlayerCopyEntity;

	InputHandler_PlayerBase *m_pInputHandler;

	/// ENTITY_GROUP_MIN is set by default
	EntityGroupHandle m_ProjectileEntityGroup;

	float m_fBoost;

	/// sound names played when the player gets shot
	SoundHandle m_aDamageSound[NUM_DAMAGE_TYPES][PLYAER_DAMAGESOUND_VARIATION];

	/// view from the player
	Camera m_Camera;

	Matrix34 m_CameraLocalPose;

	Matrix34 m_PrevCameraPose;

	BaseEntityHandle m_LaserDot;
	BaseEntityHandle m_HeadLight;

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

	virtual InputHandler_PlayerBase *CreatePlayerInputHandler() { return NULL; }

	virtual void OnDestroyingEnemyEntity( GameMessage& msg );

public:

	CBE_Player();

	virtual ~CBE_Player();

	virtual void Init();
	virtual void InitCopyEntity(CCopyEntity* pCopyEnt);

	CCopyEntity* GetPlayerCopyEntity() { return m_pPlayerCopyEntity; }

	virtual void Act(CCopyEntity* pCopyEnt);

	virtual void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	virtual void OnEntityDestroyed(CCopyEntity* pCopyEnt);

	virtual void RenderStage(CCopyEntity* pCopyEnt);

	virtual void CreateRenderTasks(CCopyEntity* pCopyEnt);

	virtual void Draw(CCopyEntity *pCopyEnt);

	virtual void UpdateCamera(CCopyEntity* pCopyEnt);
	Camera *GetCamera() { return &m_Camera; }

	virtual void Move( CCopyEntity *pCopyEnt );

	/// process input for various operations of the player entity
	virtual bool HandleInput( SPlayerEntityAction& input );

	virtual HUD_PlayerBase *GetHUD() { return NULL; }

	void PlayerDead(CCopyEntity* pCopyEnt);

	inline float GetCurrentLife();

	void SetCommandMenuUI( CGM_DialogManagerSharedPtr pDlgMgr,
		                   std::vector<int>& vecRootDialogID,
		                   CGM_DialogInputHandlerSharedPtr pDlgInputHandler = CGM_DialogInputHandlerSharedPtr() );

	inline void SetTrigger( int iTriggerState );
	inline void UnlockTriggerSafety( bool bUnlock );

	void LaserAimingDevice( bool bLaserDot );

	void ToggleHeadLight();

	CombatRecord& CombatRecord() { return m_CombatRecord; }

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual void Serialize( IArchive& ar, const unsigned int version );

	// static member variable(s)

	static int ms_InputHandlerIndex;
};


// ================================ inline implementations ================================ 

inline float CBE_Player::GetCurrentLife()
{
	if( m_pPlayerCopyEntity )
		return m_pPlayerCopyEntity->fLife;
	else
		return 0;
}

} // namespace amorphous



#endif		/*  __BE_PLAYER_H__  */
