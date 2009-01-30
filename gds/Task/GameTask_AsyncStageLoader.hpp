#ifndef  __GameTask_AsyncStageLoader_H__
#define  __GameTask_AsyncStageLoader_H__


#include "GameTask.hpp"
#include "UI/fwd.hpp"
#include "Stage/fwd.hpp"
#include "Stage/StageLoader.hpp"
#include "Support/Timer.hpp"

class CFontBase;


class CGameTask_AsyncStageLoader : public CGameTask
{
protected:

	// handles the input to the player ship
//	CInputHandler *m_pInputHandler;
//	CGM_DialogManagerSharedPtr m_pDialogManager;
//	CGM_ControlRendererManagerSharedPtr m_pUIRenderManager;
//	TitleEventHandler* m_pEventHandler;

	enum eState
	{
		STATE_STAGE_NOT_LOADED,
		STATE_FAILED_TO_LOAD_STAGE,
		STATE_LOADED_STAGE,
		STATE_LOADING_STAGE,
		NUM_STATES
	};

	/// stores loaded stage
	CStageSharedPtr m_pStage;

	CFontBase *m_pFont;

	int m_State;

	bool m_bRendered;

	CASyncStageLoader m_AsyncStageLoader;

	float m_fWaitTime; /// [sec]

	bool m_bLoadImmediately;

	static std::string ms_strNextTaskName;

	static std::string ms_strStageScripName;

	boost::thread *m_pStageLoader;

	bool m_bTerminateAsyncLoadThread;

protected:

	int GetState() const { return m_State; }

//	void LoadStageAsync();

public:

	CGameTask_AsyncStageLoader();
	virtual ~CGameTask_AsyncStageLoader();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	static void SetNextTaskName( const std::string& next_task_name ) { ms_strNextTaskName = next_task_name; }

	static void SetStageScriptName( const std::string& script_name ) { ms_strStageScripName = script_name; }

	void AsyncLoadThreadMain();
};


class CAsyncLoadThreadStarter
{
	CGameTask_AsyncStageLoader *m_pAsyncLoaderTask;

public:

	CAsyncLoadThreadStarter( CGameTask_AsyncStageLoader* pTask ) : m_pAsyncLoaderTask(pTask) {}
	~CAsyncLoadThreadStarter() {}

	void operator()()
	{
		m_pAsyncLoaderTask->AsyncLoadThreadMain();
	}
};


#endif  /*  __GameTask_AsyncStageLoader_H__  */
