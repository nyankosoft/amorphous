#ifndef  __CharacterMotionControlAppBase_HPP__
#define  __CharacterMotionControlAppBase_HPP__


#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Task/StageViewerGameTask.hpp"
#include "amorphous/Task/GameTaskFactoryBase.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/GameCommon/KeyBind.hpp"
#include "amorphous/GameCommon/ThirdPersonCameraController.hpp"
#include "amorphous/GameCommon/ThirdPersonMotionController.hpp"
#include "amorphous/Item/ItemEntity.hpp"
#include "amorphous/Item/SkeletalCharacter.hpp"
#include "amorphous/Graphics/DoubleConeScrollEffect.hpp"

using namespace amorphous;


enum ShadowAppTaskID
{
//	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_BASIC_PHYSICS,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CCharacterMotionControlAppTask : public StageViewerGameTask
{
	boost::shared_ptr<CKeyBind> m_pKeyBind;


	boost::shared_ptr< CInputDataDelegate<CCharacterMotionControlAppTask> > m_pInputHandler;

//	boost::shared_ptr<CCharacterMotionInputHandler> m_pInputHandler;

	EntityHandle<ItemEntity> m_CharacterItemEntity;

	std::vector< boost::shared_ptr<SkeletalCharacter> > m_pCharacterItems;

	boost::shared_ptr<CThirdPersonCameraController> m_pThirdPersonCameraController;

	boost::shared_ptr<CThirdPersonMotionController> m_pThirdPersonMotionController;

	DoubleConeScrollEffect m_ScrollEffect;

	Vector3 m_vPrevCamPos;

private:

	void UpdateThirdPersonCamera( float dt );

public:

	CCharacterMotionControlAppTask();

	~CCharacterMotionControlAppTask() {}

	int FrameMove( float dt );

	void Render();

	void HandleInput( InputData& input );
};

class CCharacterMotionControlAppGUITask : public StageViewerGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

//	std::string m_StageScriptToLoad;

public:

	CCharacterMotionControlAppGUITask();

	~CCharacterMotionControlAppGUITask() {}

	int FrameMove( float dt );

//	void LoadStage( const std::string& stage_script_name );
};


class CCharacterMotionControlAppTaskFactory : public GameTaskFactoryBase
{
public:

	GameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BASIC_PHYSICS:
			return new CCharacterMotionControlAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new CCharacterMotionControlAppGUITask();
		default:
			return GameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CCharacterMotionControlAppBase : public CGameApplicationBase
{
public:

	CCharacterMotionControlAppBase();

	virtual ~CCharacterMotionControlAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "CharacterMotionControl"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	GameTaskFactoryBase *CreateGameTaskFactory() const { return new CCharacterMotionControlAppTaskFactory(); }

	void Release();
};


#endif		/*  __CharacterMotionControlAppBase_HPP__  */
