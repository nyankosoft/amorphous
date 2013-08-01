#ifndef  __PlanarReflectionAppBase_HPP__
#define  __PlanarReflectionAppBase_HPP__


#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Task/StageViewerGameTask.hpp"
#include "amorphous/Task/GameTaskFactoryBase.hpp"

using namespace amorphous;


enum ShadowAppTaskID
{
//	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_BASIC_PHYSICS,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CPlanarReflectionAppTask : public StageViewerGameTask
{
public:

	CPlanarReflectionAppTask();

	~CPlanarReflectionAppTask() {}

	void HandleInput( const InputData& input );
};


class CPlanarReflectionAppGUITask : public GUIGameTask
{
	std::string m_StageScriptToLoad;

public:

	CPlanarReflectionAppGUITask();

	~CPlanarReflectionAppGUITask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class CPlanarReflectionAppTaskFactory : public GameTaskFactoryBase
{
public:

	GameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BASIC_PHYSICS:
			return new CPlanarReflectionAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new CPlanarReflectionAppGUITask();
		default:
			return GameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CPlanarReflectionAppBase : public CGameApplicationBase
{
public:

	CPlanarReflectionAppBase();

	virtual ~CPlanarReflectionAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "PlanarReflection"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	GameTaskFactoryBase *CreateGameTaskFactory() const { return new CPlanarReflectionAppTaskFactory(); }

	void Release();
};


#endif		/*  __PlanarReflectionAppBase_HPP__  */
