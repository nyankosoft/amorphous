#ifndef  __BasicPhysicsAppBase_HPP__
#define  __BasicPhysicsAppBase_HPP__


#include "App/ApplicationBase.hpp"
#include "Task/StageViewerGameTask.hpp"
#include "Task/GameTaskFactoryBase.hpp"


enum ShadowAppTaskID
{
//	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_BASIC_PHYSICS,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CBasicPhysicsAppTask : public CStageViewerGameTask
{
public:

	CBasicPhysicsAppTask();

	~CBasicPhysicsAppTask() {}

};

class CBasicPhysicsAppGUITask : public CGUIGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

	std::string m_StageScriptToLoad;

public:

	CBasicPhysicsAppGUITask();

	~CBasicPhysicsAppGUITask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class CBasicPhysicsAppTaskFactory : public CGameTaskFactoryBase
{
public:

	CGameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BASIC_PHYSICS:
			return new CBasicPhysicsAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new CBasicPhysicsAppGUITask();
		default:
			return CGameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CBasicPhysicsAppBase : public CApplicationBase
{
public:

	CBasicPhysicsAppBase();

	virtual ~CBasicPhysicsAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "BasicPhysics"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	CGameTaskFactoryBase *CreateGameTaskFactory() const { return new CBasicPhysicsAppTaskFactory(); }

	void Release();
};


#endif		/*  __BasicPhysicsAppBase_HPP__  */
