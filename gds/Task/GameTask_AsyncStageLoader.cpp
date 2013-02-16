#include "GameTask_AsyncStageLoader.hpp"
#include "GameTask_Stage.hpp"
#include "Stage/Stage.hpp"

#include "Graphics/Font/BuiltinFonts.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Input/InputHub.hpp"
#include "GUI.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{


using namespace std;


string GameTask_AsyncStageLoader::ms_strNextTaskName;
string GameTask_AsyncStageLoader::ms_strStageScripName;


GameTask_AsyncStageLoader::GameTask_AsyncStageLoader()
:
m_State(STATE_STAGE_NOT_LOADED),
m_pStageLoader(NULL),
m_bTerminateAsyncLoadThread(false)
{
	m_LoadStageAsyncronously = false;

	m_bRendered = false;

	int w = 16 * GetScreenWidth() / 800;
	int h = w * 2;
	m_pFont = CreateDefaultBuiltinFontRawPtr();
	m_pFont->SetFontSize( w, h );

	m_fWaitTime = 5.0f;

	m_bLoadImmediately = false;

	// start loading immediately
//	if( m_bLoadImmediately )
//		this->LoadStageAsync();

}


GameTask_AsyncStageLoader::~GameTask_AsyncStageLoader()
{
//	m_AsyncStageLoader.Join();

	SafeDelete( m_pFont );

	SafeDelete( m_pStageLoader );

//	INPUTHUB.PopInputHandler();
}

/*
void GameTask_AsyncStageLoader::LoadStageAsync()
{
	if( m_State == STATE_LOADING_STAGE 
	 || m_State == STATE_LOADED_STAGE )
		return; // loading now or already loaded

	bool stage_loading_initiated = m_AsyncStageLoader.LoadStage( ms_strStageScripName );

///	bool stage_loading_initiated = true;
///	m_pStageLoader = new boost::thread( *this );

	if( stage_loading_initiated )
	{
		m_State = STATE_LOADING_STAGE;

		// loading the global stage

		InitAnimatedGraphicsManager();

		// set animated graphics manager here
		// since init routines of the scripts use graphics manager
		SetAnimatedGraphicsManagerForScript();
	}
	else
	{
		m_State = STATE_FAILED_TO_LOAD_STAGE;

		LOG_PRINT_ERROR( "cannot load stage: " + ms_strStageScripName );
//		return GameTask::ID_PREVTASK;
	}
}*/


int GameTask_AsyncStageLoader::FrameMove( float dt )
{
	int ret = GameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	switch( m_State )
	{
	case STATE_FAILED_TO_LOAD_STAGE:
		RequestTaskTransition( GameTask::ID_PREVTASK );
		return ID_INVALID;

	case STATE_LOADED_STAGE:
		if( m_fWaitTime < GetTaskTimer().GetTime() )
		{
			if( m_pStageLoader )
			{
				m_bTerminateAsyncLoadThread = true;
				m_pStageLoader->join();
				SafeDelete( m_pStageLoader );
			}

			m_State = STATE_LOADED_STAGE_AND_CHANGING_TASKS;

			// move on to the stage task
			GameTask::RequestTaskTransition( ms_strNextTaskName );
			return ID_INVALID;
		}
		break;

	case STATE_LOADING_STAGE:
		if( m_LoadStageAsyncronously )
		{
			if( m_AsyncStageLoader.IsStageLoaded() )
			{
				m_AsyncStageLoader.Join();
				m_State = STATE_LOADED_STAGE;
			}
		}
		break;

	default:
		break;
	}

//	if( !m_bRendered )
//		return GameTask::ID_INVALID;
	
//	if( m_bStageLoaded )
//		return GameTask::ID_INVALID;

	return GameTask::ID_INVALID;
}


void GameTask_AsyncStageLoader::Render()
{
	// render stage select dialog
//	m_pDialogManager->Render( dt );

	if( m_pFont )
	{
		C2DRect rect( 0,0,0,0, 0xFF000000 );
		rect.Draw();

		const string& script_name = ms_strStageScripName;
		int font_width = m_pFont->GetFontWidth();
		int font_height = m_pFont->GetFontHeight();
		int x = GetScreenWidth() / 2 - (int)script_name.length() * font_width / 2;
		int y = GetScreenHeight() / 2 - font_height / 2;
		m_pFont->SetFontColor( 0xFFFFFFFF );
		m_pFont->DrawText( script_name, x, y );
	}
}

/*
void GameTask_AsyncStageLoader::AsyncLoadThreadMain()
{
	if( !fnop::path_exists( ms_strStageScripName ) )
	{
		m_State = STATE_FAILED_TO_LOAD_STAGE;
		return; // invalid script name
	}

	m_State = STATE_LOADING_STAGE;

	if( !m_pStage )
	{
		StageLoader loader;
		m_pStage = loader.CreateStage();
	}

	bool loaded = m_pStage->Initialize( ms_strStageScripName );

	if( loaded )
	{
		InitAnimatedGraphicsManager();

		// set animated graphics manager here
		// since init routines of the scripts use graphics manager
		SetAnimatedGraphicsManagerForScript();

        m_State = STATE_LOADED_STAGE;
	}
	else
		m_State = STATE_FAILED_TO_LOAD_STAGE;

	while( !m_bTerminateAsyncLoadThread )
		Sleep(20);
}
*/


} // namespace amorphous
