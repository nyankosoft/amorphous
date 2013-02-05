#ifndef  __ShadowsAppBase_HPP__
#define  __ShadowsAppBase_HPP__


#include "gds/App/GameApplicationBase.hpp"
#include "gds/Task/StageViewerGameTask.hpp"
#include "gds/Task/GameTaskFactoryBase.hpp"


enum ShadowAppTaskID
{
	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_SHADOWS_STAGE_SELECT,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CShadowAppTask : public CStageViewerGameTask
{
public:

	CShadowAppTask();

	~CShadowAppTask() {}

};

class CShadowAppStageSelectTask : public CGUIGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

	std::string m_StageScriptToLoad;

public:

	CShadowAppStageSelectTask();

	~CShadowAppStageSelectTask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class CShadowAppTaskFactory : public CGameTaskFactoryBase
{
public:

	CGameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_SHADOWS_TEST_STAGE:
			return new CShadowAppTask();
		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
			return new CShadowAppStageSelectTask();
		default:
			return CGameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CShadowsAppBase : public CGameApplicationBase
{
public:

	CShadowsAppBase();

	virtual ~CShadowsAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "Shadows"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	CGameTaskFactoryBase *CreateGameTaskFactory() const { return new CShadowAppTaskFactory(); }

	void Release();
};


#endif		/*  __ShadowsAppBase_HPP__  */
