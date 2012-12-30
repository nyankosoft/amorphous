#ifndef  __GameTask_AsyncStageLoader_H__
#define  __GameTask_AsyncStageLoader_H__


#include "GameTask.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/Stage/fwd.hpp"
#include "gds/Stage/StageLoader.hpp"
#include "gds/Support/Timer.hpp"


namespace amorphous
{

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
		STATE_LOADED_STAGE_AND_CHANGING_TASKS,
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

	/// for debugging. Set this to false to Load the stage synchronously
	bool m_LoadStageAsyncronously;

protected:

	int GetState() const { return m_State; }

//	void LoadStageAsync();

public:

	CGameTask_AsyncStageLoader();
	virtual ~CGameTask_AsyncStageLoader();

	virtual int FrameMove( float dt );
	virtual void Render();

	static void SetNextTaskName( const std::string& next_task_name ) { ms_strNextTaskName = next_task_name; }

	static void SetStageScriptName( const std::string& script_name ) { ms_strStageScripName = script_name; }

	void AsyncLoadThreadMain();
};

/*
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
*/
} // namespace amorphous



#endif  /*  __GameTask_AsyncStageLoader_H__  */
