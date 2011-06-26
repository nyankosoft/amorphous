#ifndef  __BillboardAnimationAppBase_HPP__
#define  __BillboardAnimationAppBase_HPP__


#include "gds/App/GameApplicationBase.hpp"
#include "gds/Task/StageViewerGameTask.hpp"
#include "gds/Task/GameTaskFactoryBase.hpp"


enum ShadowAppTaskID
{
//	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_BASIC_PHYSICS,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CBillboardAnimationAppTask : public CStageViewerGameTask
{
	void OnTriggerPulled();

public:

	CBillboardAnimationAppTask();

	~CBillboardAnimationAppTask() {}

	void HandleInput( const SInputData& input );
};


class CBillboardAnimationAppGUITask : public CGUIGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

	std::string m_StageScriptToLoad;

public:

	CBillboardAnimationAppGUITask();

	~CBillboardAnimationAppGUITask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class CBillboardAnimationAppTaskFactory : public CGameTaskFactoryBase
{
public:

	CGameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_BASIC_PHYSICS:
			return new CBillboardAnimationAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new CBillboardAnimationAppGUITask();
		default:
			return CGameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CBillboardAnimationAppBase : public CGameApplicationBase
{
public:

	CBillboardAnimationAppBase();

	virtual ~CBillboardAnimationAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "BillboardAnimation"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	CGameTaskFactoryBase *CreateGameTaskFactory() const { return new CBillboardAnimationAppTaskFactory(); }

	void Release();
};


#endif		/*  __BillboardAnimationAppBase_HPP__  */
