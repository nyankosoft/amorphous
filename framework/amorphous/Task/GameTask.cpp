#include "GameTask.hpp"
#include "3DMath/MathMisc.hpp"
#include "App/GameApplicationBase.hpp"
#include "Input/InputHub.hpp"
#include "Input/InputHandler.hpp"
#include "Input/MouseInputDevice.hpp"
#include "GameCommon/MouseCursor.hpp"
#include "Graphics/GraphicsEffectManager.hpp"
#include "Graphics/PyModule_GraphicsElement.hpp"
#include "Graphics/RenderTaskProcessor.hpp"
#include "Graphics/Font/FontBase.hpp"
#include "Support/DebugOutput.hpp"
#include "Support/Profile.hpp"


namespace amorphous
{

using namespace std;

//draft
class CTaskTransitionRequest
{
public:
	int NextTaskID;
	int Priority;
};


// Static member variables

boost::shared_ptr<MouseInputDevice> GameTask::ms_pMouse;

GraphicsElementAnimationManager *GameTask::ms_pAnimatedGraphicsManager = NULL;

std::map<std::string,int> GameTask::ms_TaskNameStringToTaskID;

int GameTask::ms_FadeinTimeForNextTaskMS = -1;


/**
Creates an input handler that delegates input data to GameTask::HandleInput(),
which can be overriden by derived classes.
The input handler is pushed to the input handler stack of the index
returned by GameTask::GetInputHandlerIndex(), which, by default, returns
InputHub::MIN_USER_INPUT_HANDLER_INDEX, and can be overriden by derived classes.
*/
GameTask::GameTask()
:
m_RequestedNextTaskID(ID_INVALID),
m_bTaskTransitionStarted(false),
m_bIsAppExitRequested(false),
m_pInputHandler(NULL),
m_FadeoutStartTimeMS(0),
m_RenderStartTimeMS(0),
m_DefaultFadeinTimeMS(800),
m_DefaultFadeoutTimeMS(800),
m_FadeoutTimeMS(0),
m_Rendered(false),
m_bShowMouseCursor(true),
m_RequestedNextTaskPriority(0),
m_TaskTransitionMinimumAllowedPriority(0),
m_PrevTaskID(GameTask::ID_INVALID)
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

	// Push an input handler on the stack.
	m_pInputHandler = new CInputDataDelegate<GameTask>(this);
	GetInputHub().PushInputHandler( GetInputHandlerIndex(), m_pInputHandler );
}


GameTask::~GameTask()
{
	GetInputHub().RemoveInputHandler( m_pInputHandler );
	SafeDelete( m_pInputHandler );

//	SafeDelete( ms_pAnimatedGraphicsManager );
}


void GameTask::ProcessTaskTransitionRequest()
{
	if( GetRequestedNextTaskID() != ID_INVALID 
	 && m_TaskTransitionTimeMS <= m_Timer.GetTimeMS()
	 && !m_bTaskTransitionStarted )
	{
		if( m_RequestedNextTaskPriority < m_TaskTransitionMinimumAllowedPriority )
			return;

		// a task transition has been requested
		// and the fade-out effect should be starting

		StartFadeout();

		// Don't accept input after this point
		int input_handler_index = 1; // index of the input handlers registered by the framework
		if( GetInputHub().GetInputHandler(input_handler_index) == m_pInputHandler
		 && m_pInputHandler )
		{
			m_pInputHandler->SetActive( false );
		}

		// raise the flag to ensure this routine is executed only once
		m_bTaskTransitionStarted = true;
	}
}

/*
 * must be called in Render() of derived class
 * after all the objects are rendered
 *
 * template for Render() of derived class

	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	// set render states

	// set camera

	// render scene

	// render UI

*/
inline void GameTask::RenderFadeEffect()
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
		uint w,h;
		GraphicsDevice().GetViewportSize( w, h );
		rect.SetPositionLTRB( 0, 0, (int)w, (int)h );
		rect.Draw();
	}
}


void GameTask::RequestTaskTransitionMS( int next_task_id,
										 int priority,
									     int delay_in_ms,
		                                 int fade_out_time_in_ms,
									     int fade_in_time_in_ms )
{
	if( next_task_id == GameTask::ID_PREVTASK
	 && m_PrevTaskID == GameTask::ID_INVALID )
		return; // There is no previous task. The task stack is empty

	if( m_RequestedNextTaskPriority < m_TaskTransitionMinimumAllowedPriority )
		return;

	m_RequestedNextTaskID = next_task_id;

	m_RequestedNextTaskPriority = priority;

	if( fade_out_time_in_ms < 0 )
		m_FadeoutTimeMS = m_DefaultFadeoutTimeMS; // not specified or the user deliberately chose to use the default fade out time
	else
		m_FadeoutTimeMS = fade_out_time_in_ms;

	/// fade in time for the next task
	ms_FadeinTimeForNextTaskMS = fade_in_time_in_ms;

	m_TaskTransitionTimeMS = m_Timer.GetTimeMS() + (unsigned long)delay_in_ms;
}


void GameTask::RequestTaskTransitionMS( const std::string& next_task_name,
										 int priority,
		                                 int delay_in_ms,
		                                 int fade_out_time_in_ms,
									     int fade_in_time_in_ms )
{
	map<string,int>::iterator itr = ms_TaskNameStringToTaskID.find( next_task_name );

	if( itr != ms_TaskNameStringToTaskID.end() )
		RequestTaskTransitionMS( itr->second, priority, delay_in_ms, fade_out_time_in_ms, fade_in_time_in_ms );
}


void GameTask::RequestTaskTransition( int next_task_id,
									   int priority,
									   float delay_in_sec,
		                               float fade_out_time_in_sec,
									   float fade_in_time_in_sec )
{
	RequestTaskTransitionMS( next_task_id,
		priority,
		int(delay_in_sec * 1000.0f),
		int(fade_out_time_in_sec * 1000.0f),
		int(fade_in_time_in_sec * 1000.0f) );
}


void GameTask::RequestTaskTransition( const std::string& next_task_name,
									   int priority,
		                               float delay_in_sec,
		                               float fade_out_time_in_sec,
									   float fade_in_time_in_sec )
{
	RequestTaskTransitionMS( next_task_name,
		priority,
		int(delay_in_sec * 1000.0f),
		int(fade_out_time_in_sec * 1000.0f),
		int(fade_in_time_in_sec * 1000.0f) );
}


/**
  1. Retrieves the current position of the mouse and update the position of the graphics element(s) for mouse cursor.
  2. Update animations
  3. Update task transition-related status
*/
int GameTask::FrameMove( float dt )
{
	PROFILE_FUNCTION();

	m_Timer.UpdateFrameTime();

	if( m_pMouseCursorElement )
	{
		if( ms_pMouse )
		{
			int x,y;
			ms_pMouse->GetCurrentPosition( x, y );
			float scale = (float)GetScreenWidth() / (float)REFERENCE_SCREEN_WIDTH;
			m_pMouseCursorElement->SetLocalTopLeftPos( Vector2((float)x,(float)y) * scale );
		}
	}

	if( GetAnimatedGraphicsManager() )
		GetAnimatedGraphicsManager()->UpdateEffects( dt );

	// if the task has received a reuqest to finish and translate
	// to another task, take appropriate actions
	// - go into fadeout mode
	// - disable the input handler to ignore any further input
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


int GameTask::GetInputHandlerIndex() const
{
	return InputHub::MIN_USER_INPUT_HANDLER_INDEX;
}


/**
 1. Clears the back buffer
 2. Call the Render() of derived class
 3. Render the graphics elements
 4. Render the mouse cursor (<- Isn't this created as graphics element(s)?)
 5. Render the fade-in/out effect
*/
void GameTask::RenderBase()
{
	PROFILE_FUNCTION();

//	SFloatRGBAColor bg_color = SFloatRGBAColor::Blue();
///	SFloatRGBAColor bg_color = SFloatRGBAColor::Green();
///	SFloatRGBAColor bg_color = SFloatRGBAColor::Aqua();
	SFloatRGBAColor bg_color = SFloatRGBAColor(0.3f,0.3f,0.3f,1.0f);

	Result::Name res = Result::SUCCESS;
	res = GraphicsDevice().SetClearColor( bg_color );
	res = GraphicsDevice().SetClearDepth( 1.0f );
	res = GraphicsDevice().Clear( BufferMask::COLOR | BufferMask::DEPTH );

	res = GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );
	res = GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,   false );

	// do the render routine of the base class
	GameTask::Render();

	// render routine of each derived class
	Render();

	if( GetAnimatedGraphicsManager() )
		GetAnimatedGraphicsManager()->GetGraphicsElementManager()->Render();

	// render the mouse cursor
	if( m_bShowMouseCursor )
		GetMouseCursor().Show();
	else
		GetMouseCursor().Hide();

	// render fade-out effect if the task is starting / terminating
	RenderFadeEffect();

	DebugOutput.Render();
}


void GameTask::CreateRenderTasks()
{
	RenderTaskProcessor.AddRenderTask( new GameTaskRenderTask( this ) );
}


void GameTask::ShowMouseCursor()
{
	GetMouseCursor().Show();
}


void GameTask::HideMouseCursor()
{
	GetMouseCursor().Hide();
}


void GameTask::GetCurrentMousePosition( int& x, int& y )
{
	if( ms_pMouse )
		ms_pMouse->GetCurrentPosition( x, y );
}


/// About mouse cursor
/// - Should use graphics element
///   - rationale: can support animation using effect manager
///     - animation is required to tell the user the state of system when it is, for example, loading something
/// - Things to remember when you use graphics elements
///   - You need to specify the coordinates in reference screen coordinates, which is
///     right now
///     - width: 1600
///     - height: [900,1280]
void GameTask::DrawMouseCursor()
{
//	GetMouseCursor().Draw();
}


//
// static member functions
//

void GameTask::InitAnimatedGraphicsManager()
{
	SafeDelete( ms_pAnimatedGraphicsManager );

	ms_pAnimatedGraphicsManager = new GraphicsElementAnimationManager();

	ms_pAnimatedGraphicsManager->GetGraphicsElementManager()->LoadFont(
		0,
		"BuiltinFont::BitstreamVeraSansMono-Bold-256",
		FontBase::FONTTYPE_TEXTURE,
		16, 32 // width & height
		);
}


void GameTask::SetAnimatedGraphicsManagerForScript()
{
	if( !ms_pAnimatedGraphicsManager )
		return;

	amorphous::SetAnimatedGraphicsManagerForScript( GetAnimatedGraphicsManager() );
}


void GameTask::RemoveAnimatedGraphicsManagerForScript()
{
	if( !ms_pAnimatedGraphicsManager )
		return;

	amorphous::RemoveAnimatedGraphicsManagerForScript();
}


void GameTask::ReleaseAnimatedGraphicsManager()
{
	SafeDelete( ms_pAnimatedGraphicsManager );
}


int GameTask::GetTaskIDFromTaskName( const std::string& task_name )
{
	map<string,int>::iterator itr = ms_TaskNameStringToTaskID.find( task_name );

	if( itr != ms_TaskNameStringToTaskID.end() )
		return itr->second;
	else
	{
		LOG_PRINT_ERROR( "An invalid task name: " + task_name );
		return GameTask::ID_INVALID;
	}
}


} // namespace amorphous
