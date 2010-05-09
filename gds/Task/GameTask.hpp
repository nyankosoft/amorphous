#ifndef  __GameTask_H__
#define  __GameTask_H__

// base class for game task classes


#include "../base.hpp"
#include "Support/Timer.hpp"
#include "Input/fwd.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Support/SafeDelete.hpp"

#include "Graphics/RenderTask.hpp"

#include <map>


class CGameTask : public CGraphicsComponent
{
	/// Started in CGameTask::CGameTask()
	CTimer m_Timer;

	/// set to true when the Render() is called for the first time
	bool m_Rendered;

	U32 m_RenderStartTimeMS;

	/// the time when the fade-out time was started
	U32 m_FadeoutStartTimeMS;

	/// turned on when the task transition was started
	/// - prevent the multiple executions of task transition initiation routine in CGameTask::ProcessTaskTransitionRequest()
	bool m_bTaskTransitionStarted;

	unsigned long m_TaskTransitionTimeMS;

	/// Flag to hold the task change request.
	/// When this variable is anything other than ID_INVALID
	/// task is already started transition to the next task.
	/// FrameMove() may be called if the task is in fadeout phase
	/// but no input should be processed during the transition
	int m_RequestedNextTaskID;

	int m_RequestedNextTaskPriority;

	/// Any task transition request with priority lower than this threshold is blocked.
	/// - default: 0 (=any request with priority 0 or higher is accepted)
	int m_TaskTransitionMinimumAllowedPriority;

	int m_PrevTaskID;

	/// time to spend on fade-in effect
	unsigned int m_FadeinTimeMS;

	/// time to spend on fade-out effect
	unsigned int m_FadeoutTimeMS;

	/// default fadeout & fadein time
	/// - used when they are not specified with RequestTaskTransition() call
	unsigned int m_DefaultFadeinTimeMS;
	unsigned int m_DefaultFadeoutTimeMS;

	/// Show / hide the mouse cursor. True by default
	/// - If true, the system simply calls MouseCursor.Draw() at the end of the renderering
	bool m_bShowMouseCursor;

	boost::shared_ptr<CGraphicsElementGroup> m_pMouseCursorElement;

	bool m_bIsAppExitRequested;

	/// shared by all the game tasks
	static CAnimatedGraphicsManager *ms_pAnimatedGraphicsManager;

	/// holds mappings from task name string to task id
	/// used to change tasks from script
	static std::map<std::string,int> ms_TaskNameStringToTaskID;

	/// temporarily hold fade in time for next task during a transition
	/// default fade in time is used when this is set to -1
	static int ms_FadeinTimeForNextTaskMS;

protected:

	CInputHandler *m_pInputHandler;

protected:

	const CTimer& GetTaskTimer() const { return m_Timer; }

	bool IsInTransitionToNextTask() const { return GetRequestedNextTaskID() != CGameTask::ID_INVALID; }

	virtual void ProcessTaskTransitionRequest();

public:

	CGameTask();

	virtual ~CGameTask();

	static void AddTaskNameToTaskIDMap( std::string task_name, int task_id ) { ms_TaskNameStringToTaskID[task_name] = task_id; }

	/**
	   must be called from FrameMove() of derived classes

		int ret = CGameTask::FrameMove(dt);
		if( ret != ID_INVALID )
			return ret;
	 */
	virtual int FrameMove( float dt );

	inline virtual void Render();

	virtual void HandleInput( const SInputData& input ) {}

	virtual int GetInputHandlerIndex() const;

	/// called from game task manager.
	/// calls Render() of derived class
	void RenderBase();

	virtual void CreateRenderTasks();

	inline void RenderFadeEffect();

	int GetRequestedNextTaskID() const { return m_RequestedNextTaskID; }

	/// start fade-out effect.
	/// the task is terminated after it fades out completely
	inline void StartFadeout();

	/// Override this in your CGameTask subclasses if there is anything
	/// that needs to be done before a certain task transition.
	/// When you do that, call CGameTask::RequestTaskTransition() in the overridden
	/// virtual function
	/// \param fade_out_time [in] time spent on fade out [sec].
	///        Default fade out time is used when a negative value is set
	///        or the argument is omitted.
	/// \param fade_in_time [in] time spent on fade in when the next task started [sec].
	virtual void RequestTaskTransition( int next_task_id,
										int priority = 0,
										float delay_in_sec = 0,
		                                float fade_out_time = -1.0f,
										float fade_in_time = -1.0f );

	virtual void RequestTaskTransition( const std::string& next_task_name,
										int priority = 0,
		                                float delay_in_sec = 0,
		                                float fade_out_time_in_sec = -1.0f,
										float fade_in_time_in_sec = -1.0f );

	virtual void RequestTaskTransitionMS( int next_task_id,
										  int priority = 0,
										  int delay_in_ms = 0,
		                                  int fade_out_time_in_ms = -1,
										  int fade_in_time_in_ms = -1 );

	virtual void RequestTaskTransitionMS( const std::string& next_task_name,
										  int priority = 0,
		                                  int delay_in_ms = 0,
		                                  int fade_out_time_in_ms = -1,
										  int fade_in_time_in_ms = -1 );

	void SetTaskTransitionMinimumAllowedPriority( int priority ) { m_TaskTransitionMinimumAllowedPriority = priority; }

	/// If tasks are executed sequencially and you want to define one as a 'next task' of another,
	/// override this to call RequestTaskTransition() with a valid task id to establish
	/// a sequence of tasks.
	virtual void RequestTransitionToNextTask() { RequestTaskTransition(ID_INVALID); }

	/// Set by the task manager during the init phase
	void SetPrevTaskID( int prev_task_id ) { m_PrevTaskID = prev_task_id; }

	enum eGameTask
	{
		ID_PREVTASK = -2,
		ID_INVALID = -1,
		ID_MAIN_MENU = 0,
		ID_STAGE,
		ID_STAGE_SELECT,
		ID_GLOBALSTAGELOADER,
		ID_ASYNCSTAGELOADER,
		ID_GUI_TASK,
		ID_SINGLE_STAGE_TASK,
		ID_STAGE_VIEWER_TASK,
		USER_GAMETASK_ID_OFFSET
	};

	void RequestAppExit() { m_bIsAppExitRequested = true; }
	bool IsAppExitRequested() const { return m_bIsAppExitRequested; }

	void SetFadeInTimeMS( unsigned int time )	{ m_FadeinTimeMS = time;	if(time==0) m_FadeinTimeMS = 1; }
	void SetFadeOutTimeMS( unsigned int time )	{ m_FadeoutTimeMS = time;	if(time==0) m_FadeoutTimeMS = 1; }

	void SetDefaultFadeInTimeMS( unsigned int time )	{ m_DefaultFadeinTimeMS = time;	if(time==0) m_DefaultFadeinTimeMS = 1; }
	void SetDefaultFadeOutTimeMS( unsigned int time )	{ m_DefaultFadeoutTimeMS = time;	if(time==0) m_DefaultFadeoutTimeMS = 1; }

	void GetCurrentMousePosition( int& x, int& y );
	void DrawMouseCursor();

	void SetGraphicsElementGroupForMouseCursor( boost::shared_ptr<CGraphicsElementGroup> pElementGroup ) { m_pMouseCursorElement = pElementGroup; }

	static void InitAnimatedGraphicsManager();
	static CAnimatedGraphicsManager *GetAnimatedGraphicsManager() { return ms_pAnimatedGraphicsManager; }
	static void SetAnimatedGraphicsManagerForScript();
	static void RemoveAnimatedGraphicsManagerForScript();
	static void ReleaseAnimatedGraphicsManager();

	static int GetTaskIDFromTaskName( const std::string& task_name );

	virtual void ReleaseGraphicsResources() {}
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}
};


// ==================================== inline implementations ====================================


inline void CGameTask::Render()
{
	if( !m_Rendered )
	{
		m_Rendered = true;
		m_RenderStartTimeMS = m_Timer.GetTimeMS();
	}
}


inline void CGameTask::StartFadeout()
{
	m_FadeoutStartTimeMS = m_Timer.GetTimeMS();
}


class CGameTaskRenderTask : public CRenderTask
{
	CGameTask *m_pGameTask;

public:

	CGameTaskRenderTask( CGameTask *pGameTask )
		:
	m_pGameTask(pGameTask)
	{
//		m_TypeFlags |= DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE;
		m_TypeFlags |= RENDER_TO_BACKBUFFER;
	}

	void Render()
	{
		m_pGameTask->RenderBase();
 	}
};


#endif		/*  __GameTask_H__  */
