
#include "GameTask.h"
#include "App/ApplicationBase.h"
#include "GameInput/InputHub.h"
#include "GameInput/InputHandler.h"
#include "GameInput/DirectInputMouse.h"
#include "GameCommon/MouseCursor.h"
#include "3DCommon/GraphicsEffectManager.h"
#include "3DCommon/PyModule_GraphicsElement.h"

#include "3DCommon/RenderTaskProcessor.h"


using namespace std;

CAnimatedGraphicsManager *CGameTask::ms_pAnimatedGraphicsManager = NULL;
 
std::map<std::string,int> CGameTask::ms_TaskNameStringToTaskID;

int CGameTask::ms_FadeinTimeForNextTaskMS = -1;


CGameTask::CGameTask()
:
m_RequestedNextTaskID(ID_INVALID),
//m_NextTaskID(ID_INVALID),
m_bTaskTransitionStarted(false),
m_bIsAppExitRequested(false),
m_pInputHandler(NULL),
m_FadeoutStartTimeMS(0),
m_RenderStartTimeMS(0),
m_DefaultFadeinTimeMS(800),
m_DefaultFadeoutTimeMS(800),
m_FadeoutTimeMS(0),
m_Rendered(false)//,
//ms_pAnimatedGraphicsManager(NULL)
{
	m_Timer.Start();

	if( 0 <= ms_FadeinTimeForNextTaskMS )
	{
		/// the previous task set the fade in time for this task
		/// - use it and reset the static variable
		m_FadeinTimeMS = (unsigned int)ms_FadeinTimeForNextTaskMS;
		ms_FadeinTimeForNextTaskMS = -1;
	}
	else
		m_FadeinTimeMS = m_DefaultFadeinTimeMS;
}


CGameTask::~CGameTask()
{
	SafeDelete( m_pInputHandler );

//	SafeDelete( ms_pAnimatedGraphicsManager );
}


void CGameTask::ProcessTaskTransitionRequest()
{
	if( GetRequestedNextTaskID() != ID_INVALID 
	 && m_TaskTransitionTimeMS <= m_Timer.GetTimeMS()
	 && !m_bTaskTransitionStarted )
	{
		// a task transition has been requested
		// and the fade-out effect should be starting

		StartFadeout();

		// Don't accept input after this point
		if( INPUTHUB.GetInputHandler() == m_pInputHandler )
			INPUTHUB.PopInputHandler();

		// raise the flag to ensure this routine is executed only once
		m_bTaskTransitionStarted = true;
	}
}

/*
 * must be called in Render() of derived class
 * after all the objects are rendered
 *
 * template for Render() of derived class

	// do the render routine of the base class
	CGameTask::Render(dt);

	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	// begin the scene
	pd3dDevice->BeginScene();

	// set render states

	// set camera

	// render scene

	// render UI

	// render fade-out effect if the task is starting / terminating
	RenderFadeEffect();

	// end the scene
	pd3dDevice->EndScene();

	// present the backbuffer contents to the display
	pd3dDevice->Present( NULL, NULL, NULL, NULL );

*/
void CGameTask::RenderFadeEffect()
{
	// calc alpha
	int alpha = 0;
	if( m_Timer.GetTimeMS() - m_RenderStartTimeMS < m_FadeinTimeMS )
	{
		alpha = 255 * ( m_FadeinTimeMS - (m_Timer.GetTimeMS() - m_RenderStartTimeMS) ) / m_FadeinTimeMS;
	}
	else if( 0 < m_FadeoutStartTimeMS && 0 < m_FadeoutTimeMS )
	{
		alpha = 255 * (m_Timer.GetTimeMS() - m_FadeoutStartTimeMS) / m_FadeoutTimeMS;
	}

	Limit( alpha, 0, 255 );

	if( 0 < alpha )
	{
		C2DRect rect;
		rect.SetColor( alpha << 24 );
		int w,h;
		GetViewportSize( w, h );
		rect.SetPositionLTRB( 0, 0, w, h );
		rect.Draw();
	}
}


void CGameTask::RequestTaskTransitionMS( int next_task_id,
									     int delay_in_ms,
		                                 int fade_out_time_in_ms,
									     int fade_in_time_in_ms )
{
	m_RequestedNextTaskID = next_task_id;

	if( fade_out_time_in_ms < 0 )
		m_FadeoutTimeMS = m_DefaultFadeoutTimeMS; // not specified or the user deliberately chose to use the default fade out time
	else
		m_FadeoutTimeMS = fade_out_time_in_ms;

	/// fade in time for the next task
	ms_FadeinTimeForNextTaskMS = fade_in_time_in_ms;

	m_TaskTransitionTimeMS = m_Timer.GetTimeMS() + (unsigned long)delay_in_ms;
}


void CGameTask::RequestTaskTransitionMS( const std::string& next_task_title,
		                                 int delay_in_ms,
		                                 int fade_out_time_in_ms,
									     int fade_in_time_in_ms )
{
	map<string,int>::iterator itr = ms_TaskNameStringToTaskID.find( next_task_title );

	if( itr != ms_TaskNameStringToTaskID.end() )
		RequestTaskTransitionMS( itr->second, delay_in_ms, fade_out_time_in_ms, fade_in_time_in_ms );
}


void CGameTask::RequestTaskTransition( int next_task_id,
									   float delay_in_sec,
		                               float fade_out_time_in_sec,
									   float fade_in_time_in_sec )
{
	RequestTaskTransitionMS( next_task_id,
		int(delay_in_sec * 1000.0f),
		int(fade_out_time_in_sec * 1000.0f),
		int(fade_in_time_in_sec * 1000.0f) );
}


void CGameTask::RequestTaskTransition( const std::string& next_task_title,
		                               float delay_in_sec,
		                               float fade_out_time_in_sec,
									   float fade_in_time_in_sec )
{
	RequestTaskTransitionMS( next_task_title,
		int(delay_in_sec * 1000.0f),
		int(fade_out_time_in_sec * 1000.0f),
		int(fade_in_time_in_sec * 1000.0f) );
}


int CGameTask::FrameMove( float dt )
{
	m_Timer.UpdateFrameTime();

	// if the task has received a reuqest to finish and translate
	// to another task, take appropriate actions
	// - go into fadeout mode
	// - pop input handler to ignore any further input
	ProcessTaskTransitionRequest();

	if( 0 < m_FadeoutStartTimeMS )
	{
		// fadeout effect has been started
		if( m_FadeoutTimeMS == 0 || m_FadeoutTimeMS < m_Timer.GetTimeMS() - m_FadeoutStartTimeMS )
		{
			// The user requested to change the task immediately
			// or the task had been in the fadeout phase
			// and the necessary fadeout time has passed
			// - switch to the next task
			return m_RequestedNextTaskID;
		}
	}

	return ID_INVALID;
}


void CGameTask::CreateRenderTasks()
{
	RenderTaskProcessor.AddRenderTask( new CGameTaskRenderTask( this ) );
}


void CGameTask::GetCurrentMousePosition( int& x, int& y )
{
	if( g_pDIMouse )
		g_pDIMouse->GetCurrentPosition( x, y );
}


void CGameTask::DrawMouseCursor()
{
	MouseCursor.Draw();
}


//
// static member functions
//

void CGameTask::InitAnimatedGraphicsManager()
{
	SafeDelete( ms_pAnimatedGraphicsManager );

	ms_pAnimatedGraphicsManager = new CAnimatedGraphicsManager();
}


void CGameTask::SetAnimatedGraphicsManagerForScript()
{
	if( !ms_pAnimatedGraphicsManager )
		return;

	::SetAnimatedGraphicsManagerForScript( GetAnimatedGraphicsManager() );
}


void CGameTask::RemoveAnimatedGraphicsManagerForScript()
{
	if( !ms_pAnimatedGraphicsManager )
		return;

	::RemoveAnimatedGraphicsManagerForScript();
}


void CGameTask::ReleaseAnimatedGraphicsManager()
{
	SafeDelete( ms_pAnimatedGraphicsManager );
}
