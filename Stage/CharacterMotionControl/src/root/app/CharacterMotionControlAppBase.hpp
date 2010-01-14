#ifndef  __CharacterMotionControlAppBase_HPP__
#define  __CharacterMotionControlAppBase_HPP__


#include <gds/App/ApplicationBase.hpp>
#include <gds/Task/StageViewerGameTask.hpp>
#include <gds/Task/GameTaskFactoryBase.hpp>
#include <gds/Stage.hpp>
#include <gds/GameCommon/KeyBind.hpp>


enum ShadowAppTaskID
{
//	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_BASIC_PHYSICS,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CCharacterEntity : public CCopyEntity
{
public:

	void Draw();
};


class CCharacterMotionControlAppTask : public CStageViewerGameTask
{
	boost::shared_ptr<CKeyBind> m_pKeyBind;

//	boost::shared_ptr<CCharacterMotionInputHandler> m_pInputHandler;

public:

	CCharacterMotionControlAppTask();

	~CCharacterMotionControlAppTask() {}

};

class CCharacterMotionControlAppGUITask : public CStageViewerGameTask
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


class CCharacterMotionControlAppTaskFactory : public CGameTaskFactoryBase
{
public:

	CGameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BASIC_PHYSICS:
			return new CCharacterMotionControlAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new CCharacterMotionControlAppGUITask();
		default:
			return CGameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CCharacterMotionControlAppBase : public CApplicationBase
{
public:

	CCharacterMotionControlAppBase();

	virtual ~CCharacterMotionControlAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "CharacterMotionControl"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	CGameTaskFactoryBase *CreateGameTaskFactory() const { return new CCharacterMotionControlAppTaskFactory(); }

	void Release();
};


#endif		/*  __CharacterMotionControlAppBase_HPP__  */
