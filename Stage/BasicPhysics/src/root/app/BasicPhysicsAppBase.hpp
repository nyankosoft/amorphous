#ifndef  __BasicPhysicsAppBase_HPP__
#define  __BasicPhysicsAppBase_HPP__


#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Task/StageViewerGameTask.hpp"
#include "amorphous/Task/GameTaskFactoryBase.hpp"

using namespace amorphous;


enum ShadowAppTaskID
{
	GAMETASK_ID_BASIC_PHYSICS,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class BasicPhysicsAppTask : public StageViewerGameTask
{
public:

	BasicPhysicsAppTask();

	~BasicPhysicsAppTask() {}

};

class BasicPhysicsAppGUITask : public GUIGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

	std::string m_StageScriptToLoad;

public:

	BasicPhysicsAppGUITask();

	~BasicPhysicsAppGUITask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class BasicPhysicsAppTaskFactory : public GameTaskFactoryBase
{
public:

	GameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BASIC_PHYSICS:
			return new BasicPhysicsAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new BasicPhysicsAppGUITask();
		default:
			return GameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class BasicPhysicsAppBase : public GameApplicationBase
{
public:

	BasicPhysicsAppBase();

	virtual ~BasicPhysicsAppBase();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "BasicPhysics"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	GameTaskFactoryBase *CreateGameTaskFactory() const { return new BasicPhysicsAppTaskFactory(); }

	void Release();
};


#endif		/*  __BasicPhysicsAppBase_HPP__  */
