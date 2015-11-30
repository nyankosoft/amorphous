#ifndef  __LightingAppBase_HPP__
#define  __LightingAppBase_HPP__


#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Task/StageViewerGameTask.hpp"
#include "amorphous/Task/GameTaskFactoryBase.hpp"

using namespace amorphous;


enum ShadowAppTaskID
{
	GAMETASK_ID_LIGHTING,
	NUM_LIGHTING_APP_GAMETASK_IDS
};


class LightingAppTask : public StageViewerGameTask
{
	void DisplayEntityPositions( GraphicsElementAnimationManager& animated_graphics_manager );

public:

	LightingAppTask();

	~LightingAppTask() {}

	int FrameMove( float dt );
};


class LightingAppTaskFactory : public GameTaskFactoryBase
{
public:

	GameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_LIGHTING:
			return new LightingAppTask();
		default:
			return GameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class LightingAppBase : public GameApplicationBase
{
public:

	LightingAppBase();

	virtual ~LightingAppBase();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "Lighting"; }

	int GetStartTaskID() const;

	GameTaskFactoryBase *CreateGameTaskFactory() const { return new LightingAppTaskFactory(); }
};


#endif		/*  __LightingAppBase_HPP__  */
