#include "ApplicationBase.h"

#include "App/GameWindowManager_Win32.h"

#include "GameInput/DirectInputMouse.h"
#include "GameInput/DIKeyboard.h"
#include "GameInput/DirectInputGamepad.h"
#include "GameInput/InputHub.h"

#include "GameCommon/Timer.h"
#include "GameCommon/MTRand.h"
#include "GameCommon/GlobalInputHandler.h"
#include "GameCommon/GlobalParams.h"
#include "GameCommon/GameStageFrameworkGlobalParams.h"
#include "GameCommon/MouseCursor.h"

#include "3DCommon/fps.h"
#include "Task/GameTaskManager.h"

#include "Sound/GameSoundManager.h"

#include "GameEvent/ScriptArchive.h"

#include "Item/ItemDatabaseManager.h"

#include "Stage/BaseEntityManager.h"
#include "Stage/SurfaceMaterialManager.h"

#include "Support/memory_helpers.h"
#include "Support/Profile.h"
#include "Support/Log/DefaultLog.h"
#include "Support/DebugOutput.h"

#include <direct.h>
#include <windows.h>


// ================================ global variables ================================

CApplicationBase *g_pAppBase = NULL;

CDirectInputMouse *g_pDIMouse = NULL;


void UpdateBaseEntityDatabase()
{
	static bool s_bBaseEntityDatabase_Initialized = false;
	static bool s_AlwaysUpdateDatabaseBeforeStage = false;

	if( !s_bBaseEntityDatabase_Initialized || s_AlwaysUpdateDatabaseBeforeStage )
	{
		CScopeLog sl( "- Initializing & updating the base entity database" );

		BaseEntityManager.UpdateDatabase( "./resources/entity/BaseEntity.txt" );
		BaseEntityManager.OpenDatabase( "./System/BaseEntity.bin" );
		s_bBaseEntityDatabase_Initialized = true;
	}
}


//========================================================================================
// CApplicationBase
//========================================================================================


int CApplicationBase::ms_DefaultSleepTimeMS = 3;


CApplicationBase::CApplicationBase()
{
	m_pTaskManager = NULL;

	g_pDIMouse = NULL;
	m_pDIKeyboard = NULL;
	m_pDIGamepad = NULL;
}


CApplicationBase::~CApplicationBase()
{
	Release();
}


void CApplicationBase::Release()
{
	SafeDelete( m_pTaskManager );

	SafeDelete( g_pDIMouse );
	SafeDelete( m_pDIKeyboard );
	SafeDelete( m_pDIGamepad );

	INPUTHUB.PopInputHandler( 3 );
	SafeDelete( m_pGlobalInputHandler );

	// release any singleton class that inherits CGraphicsComponent
	CGraphicsResourceManager::ReleaseSingleton();

}


bool CApplicationBase::Init()
{
	LOG_FUNCTION_SCOPE();

	CGameStageFrameworkGlobalParams gsf_params;
	bool loaded = gsf_params.LoadFromTextFile( "./resources/gsf_params" );
	if( loaded )
	{
		// update binary file
		gsf_params.SaveToFile( "./System/gsf.bin" );
	}

	gsf_params.LoadFromFile( "./System/gsf.bin" );
	gsf_params.UpdateParams();

	// update material file
	CSurfaceMaterialManager surf_mat_mgr;
	if( surf_mat_mgr.LoadFromTextFile( "./resources/material.rd" ) )
		surf_mat_mgr.SaveToFile( "./Stage/material.bin" );

	// load config file
	// - users who feel that in-game UI is cumbersome can edit this text file
	CGlobalParams global_params;
	global_params.LoadFromFile( "config" );

	// create the main game window
	// Direct3D is initialized in this function
	int mode = global_params.FullScreen ? SMD_FULLSCREEN : SMD_WINDOWED;
	GAMEWINDOWMANAGER.CreateGameWindow( global_params.ScreenWidth, global_params.ScreenHeight, mode, GetApplicationTitle() );
//	GAMEWINDOWMANAGER.CreateGameWindow( 800, 600, SMD_WINDOWED /*SMD_FULLSCREEN*/ );

	if( 0 <= global_params.WindowLeftPos && 0 <= global_params.WindowTopPos )
		GAMEWINDOWMANAGER.SetWindowLeftTopCornerPosition( global_params.WindowLeftPos, global_params.WindowTopPos );

	// create DirectInput mouse device
	g_pDIMouse = new CDirectInputMouse;
	g_pDIMouse->Init( GAMEWINDOWMANAGER.GetWindowHandle() );

	LOG_PRINT( " - Initialized the direct input mouse device." );

	// initialize keyboard (DirectInput)
	m_pDIKeyboard = new CDIKeyboard;
	m_pDIKeyboard->Init( GAMEWINDOWMANAGER.GetWindowHandle() );

//	LOG_PRINT( " - Initialized the direct input keyboard device." );

	m_pDIGamepad = new CDirectInputGamepad;
	HRESULT hr = m_pDIGamepad->Init( GAMEWINDOWMANAGER.GetWindowHandle() );
	if( FAILED(hr) )
		SafeDelete( m_pDIGamepad );

//	LOG_PRINT( " - Initialized the input devices." );

	// initialize sound
	GAMESOUNDMANAGER.Init( GAMEWINDOWMANAGER.GetWindowHandle() );
	GAMESOUNDMANAGER.LoadSoundsFromList( "./Sound/SoundList.lst" );

//	LOG_PRINT( " - Initialized the sound manager" );

	// update & load the item database
//	ItemDatabaseManager().Update( "..." );
	CItemDatabaseManager::Get()->Update( "./resources/item/item.mkx" );
	CItemDatabaseManager::Get()->LoadFromFile( "./Item/item.gid" );

	LOG_PRINT( " - Loaded the item database." );

	try
	{
		UpdateScriptArchives( "resources/script", "Script/" );
	}
	catch( exception& e )
	{
		g_Log.Print( WL_WARNING, "exception: %s", e.what() );
	}

	// update the binary file of base entity database
	UpdateBaseEntityDatabase();

	m_pGlobalInputHandler = new CGlobalInputHandler;
	INPUTHUB.PushInputHandler( 3, m_pGlobalInputHandler );

	// takes debug info and output to the screen
	// (graphics component)
	GlobalDebugOutput.Init( "�l�r �S�V�b�N", 6, 12 );
	DebugOutput.SetTopLeftPos( Vector2(16,32) );
	DebugOutput.AddDebugItem( "perf",	new CDebugItem_Profile() );

	// background rect is rendered by each debug item object
//	DebugOutput.SetBackgroundRect( C2DRect( D3DXVECTOR2(0,0), D3DXVECTOR2(0, 0), 0x00000000 ) );

	CGameTask::AddTaskNameToTaskIDMap( "Stage",             CGameTask::ID_STAGE );
	CGameTask::AddTaskNameToTaskIDMap( "GlobalStageLoader", CGameTask::ID_GLOBALSTAGELOADER );

	// start the timer
	TIMER.Start();

	return true;
}


#define APPBASE_TIMER_RESOLUTION	1


void CApplicationBase::AcquireInputDevices()
{
	if( m_pDIKeyboard )
		m_pDIKeyboard->Acquire();

	if( g_pDIMouse && GAMEWINDOWMANAGER.IsMouseCursorInClientArea() )
		g_pDIMouse->AcquireMouse();

	if( m_pDIGamepad )
		m_pDIGamepad->Acquire();
}


void CApplicationBase::Execute()
{
	// timer resolution for timeGetTime()
	timeBeginPeriod( APPBASE_TIMER_RESOLUTION );

	if( !InitTaskManager() )
		return;

	if( !Init() )
		return;

	// initialize profiler
	ProfileInit();

	// initialize rand generator
	InitRand( timeGetTime() );

    // Enter the message loop
	float frametime;
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			// update FPS timer (old)
			FPS.UpdateFPS();

			// update the timer
			TIMER.UpdateFrameTime();

			frametime = TIMER.GetFrameTime();

			// process input from input devices
			// input data are sent to an InputHandler currently set to the InputHub
			if( g_pDIMouse )
			{
				g_pDIMouse->UpdateInput();

				MouseCursor.UpdateCursorPosition(
					g_pDIMouse->GetCurrentPositionX(),
					g_pDIMouse->GetCurrentPositionY() );
			}

			if( m_pDIGamepad )
				m_pDIGamepad->UpdateInput();

			m_pDIKeyboard->ReadBufferedData();

			// do the current task
			m_pTaskManager->Update( frametime );

			m_pTaskManager->Render();

			Sleep( ms_DefaultSleepTimeMS );
//			PERIODICAL( 2, Sleep(5) );
		}
	}

	DebugOutput.ReleaseDebugItem( "perf" );
	GlobalDebugOutput.Release();
	CGameTask::ReleaseAnimatedGraphicsManager();
	MouseCursor.ReleaseGraphicsResources();

//	MessageBox( NULL, "exit the main loop", "msg", MB_OK );

	timeEndPeriod( APPBASE_TIMER_RESOLUTION );
}


void CApplicationBase::Run()
{
	// set log output device
	CLogOutput_HTML html_log( "debug/stage_base_app_log.html" );
	g_Log.AddLogOutput( &html_log );

	try
	{
		Execute();
	}
	catch( exception& e )
	{
		g_Log.Print( WL_ERROR, "exception: %s", e.what() );
	}
}
