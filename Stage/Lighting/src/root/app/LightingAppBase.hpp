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


class CLightingAppTask : public StageViewerGameTask
{
	void DisplayEntityPositions( GraphicsElementAnimationManager& animated_graphics_manager );

public:

	CLightingAppTask();

	~CLightingAppTask() {}

	int FrameMove( float dt );
};


class CLightingAppTaskFactory : public GameTaskFactoryBase
{
public:

	GameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_LIGHTING:
			return new CLightingAppTask();
		default:
			return GameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CLightingAppBase : public GameApplicationBase
{
public:

	CLightingAppBase();

	virtual ~CLightingAppBase();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "Lighting"; }

	int GetStartTaskID() const;

	GameTaskFactoryBase *CreateGameTaskFactory() const { return new CLightingAppTaskFactory(); }
};


#endif		/*  __LightingAppBase_HPP__  */
