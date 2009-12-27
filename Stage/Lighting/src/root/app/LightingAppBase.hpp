#ifndef  __LightingAppBase_HPP__
#define  __LightingAppBase_HPP__


#include "App/ApplicationBase.hpp"
#include "Task/StageViewerGameTask.hpp"
#include "Task/GameTaskFactoryBase.hpp"


enum ShadowAppTaskID
{
//	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_BASIC_PHYSICS,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CLightingAppTask : public CStageViewerGameTask
{
public:

	CLightingAppTask();

	~CLightingAppTask() {}

};

class CLightingAppGUITask : public CGUIGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

	std::string m_StageScriptToLoad;

public:

	CLightingAppGUITask();

	~CLightingAppGUITask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class CLightingAppTaskFactory : public CGameTaskFactoryBase
{
public:

	CGameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BASIC_PHYSICS:
			return new CLightingAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new CLightingAppGUITask();
		default:
			return CGameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CLightingAppBase : public CApplicationBase
{
public:

	CLightingAppBase();

	virtual ~CLightingAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "Lighting"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	CGameTaskFactoryBase *CreateGameTaskFactory() const { return new CLightingAppTaskFactory(); }

	void Release();
};


#endif		/*  __LightingAppBase_HPP__  */
