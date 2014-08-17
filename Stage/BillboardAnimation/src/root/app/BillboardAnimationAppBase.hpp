#ifndef  __BillboardAnimationAppBase_HPP__
#define  __BillboardAnimationAppBase_HPP__


#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Task/StageViewerGameTask.hpp"
#include "amorphous/Task/GameTaskFactoryBase.hpp"

using namespace amorphous;


enum ShadowAppTaskID
{
	GAMETASK_ID_BILLBOARD_ANIMATION,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class BillboardAnimationAppTask : public StageViewerGameTask
{
	void OnTriggerPulled();

public:

	BillboardAnimationAppTask();

	~BillboardAnimationAppTask() {}

	void HandleInput( const InputData& input );
};


class BillboardAnimationAppGUITask : public GUIGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

	std::string m_StageScriptToLoad;

public:

	BillboardAnimationAppGUITask();

	~BillboardAnimationAppGUITask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class BillboardAnimationAppTaskFactory : public GameTaskFactoryBase
{
public:

	GameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BILLBOARD_ANIMATION:
			return new BillboardAnimationAppTask();
		default:
			return GameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class BillboardAnimationAppBase : public GameApplicationBase
{
public:

	BillboardAnimationAppBase();

	virtual ~BillboardAnimationAppBase();

//	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "BillboardAnimation"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	GameTaskFactoryBase *CreateGameTaskFactory() const { return new BillboardAnimationAppTaskFactory(); }

	void Release();
};


#endif		/*  __BillboardAnimationAppBase_HPP__  */
