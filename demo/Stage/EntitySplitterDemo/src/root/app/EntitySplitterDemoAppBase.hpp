#ifndef  __EntitySplitterDemoAppBase_HPP__
#define  __EntitySplitterDemoAppBase_HPP__


#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Stage/EntityHandle.hpp"
#include "amorphous/Task/StageViewerGameTask.hpp"
#include "amorphous/Task/GameTaskFactoryBase.hpp"

using namespace amorphous;


enum EntitySplitterDemoAppTaskID
{
	GAMETASK_ID_ENTITY_SPLITTER_APP,
	NUM_ENTITY_SPLITTER_APP_GAMETASK_IDS
};


class EntitySplitterDemoAppTask : public StageViewerGameTask
{
	EntityHandle<> m_SplitPlaneEntity;

	void OnTriggerPulled();

public:

	EntitySplitterDemoAppTask();

	~EntitySplitterDemoAppTask() {}

	int FrameMove( float dt );

	void HandleInput( const InputData& input );
};


class EntitySplitterDemoAppTaskFactory : public GameTaskFactoryBase
{
public:

	GameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_ENTITY_SPLITTER_APP:
			return new EntitySplitterDemoAppTask();
		default:
			return GameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class EntitySplitterDemoAppBase : public GameApplicationBase
{
public:

	EntitySplitterDemoAppBase();

	virtual ~EntitySplitterDemoAppBase();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "EntitySplitterDemo"; }

	int GetStartTaskID() const;

	GameTaskFactoryBase *CreateGameTaskFactory() const { return new EntitySplitterDemoAppTaskFactory(); }
};


#endif		/*  __EntitySplitterDemoAppBase_HPP__  */
