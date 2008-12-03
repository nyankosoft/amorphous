#include "ApplicationBase.h"

#include "Support/Timer.h"
#include "Support/memory_helpers.h"
#include "Support/Profile.h"
#include "Support/Log/DefaultLog.h"
#include "Support/DebugOutput.h"
#include "Support/BitmapImage.h"
#include "Support/MiscAux.h"
#include "Support/msgbox.h"
#include "3DCommon/GraphicsResourceManager.h"
#include "3DCommon/AsyncResourceLoader.h"
#include "3DCommon/LogOutput_OnScreen.h"
#include "GameInput/DirectInputMouse.h"
#include "GameInput/DIKeyboard.h"
#include "GameInput/DirectInputGamepad.h"
#include "GameInput/InputHub.h"
#include "Sound/SoundManager.h"
#include "Physics/PhysicsEngine.h"

#include "GameCommon/MTRand.h"
#include "GameCommon/GlobalInputHandler.h"
#include "GameCommon/GlobalParams.h"
#include "GameCommon/GameStageFrameworkGlobalParams.h"
#include "GameCommon/MouseCursor.h"
#include "GameEvent/ScriptArchive.h"
#include "Task/GameTask.h"
#include "Task/GameTaskManager.h"
#include "Task/GameTaskFactoryBase.h"
#include "Item/ItemDatabaseManager.h"
#include "Stage/BaseEntityManager.h"
#include "Stage/SurfaceMaterialManager.h"
#include "App/GameWindowManager_Win32.h"

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

		BaseEntityManager.UpdateDatabase( "../resources/entity/BaseEntity.txt" );
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


int CApplicationBase::GetStartTaskID() const
{
	return CGameTask::ID_INVALID;
}


CGameTaskFactoryBase *CApplicationBase::CreateGameTaskFactory() const
{
	return new CGameTaskFactoryBase();
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
	GraphicsResourceManager().ReleaseSingleton();
}


void CApplicationBase::InitDebugItems()
{
	const string font_name = "‚l‚r ƒSƒVƒbƒN";

	// set debug items and output to the screen
	// (graphics component)
	GlobalDebugOutput.Init( font_name, 6, 12 );

	DebugOutput.AddDebugItem( "perf", new CDebugItem_Profile() );

	m_pOnScreenLog = new CLogOutput_ScrolledTextBuffer( font_name, 6, 12, 16, 95 );
	g_Log.AddLogOutput( m_pOnScreenLog );

	DebugOutput.AddDebugItem( "log",  new CDebugItem_Log(m_pOnScreenLog) );

	DebugOutput.AddDebugItem( "graphics_resource_manager", new CDebugItem_GraphicsResourceManager() );

	DebugOutput.AddDebugItem( "sound_manager", new CDebugItem_SoundManager() );

	DebugOutput.SetTopLeftPos( Vector2(16,32) );

//	DebugOutput.SetBackgroundColor( 0x80000000 );


/*
//	m_pDebugOutput = new CDebugOutput( font name );
	m_pDebugOutput = new CDebugOutput( "", 6, 12 );
	m_pDebugOutput->AddDebugItem( new CDebugItem_Log( m_pOnScreenLog ) );
	m_pDebugOutput->AddDebugItem( new CDebugItem_Profile() );
//	m_pDebugOutput->AddDebugItem( new CDebugItem_StateLog( StateLog::PlayerLogOffset,	StateLog::NumPlayerLogs ) );
//	m_pDebugOutput->AddDebugItem( new CDebugItem_StateLog( StateLog::EnemyLogOffset,	StateLog::NumEnemyLogs ) );
*/
}


void CApplicationBase::ReleaseDebugItems()
{
	// delete debug output
	//  - it uses the borrowed reference of screen overlay log (m_pOnScreenLog)
//	SafeDelete( m_pDebugOutput );

	DebugOutput.ReleaseDebugItem( "log" );

	g_Log.RemoveLogOutput( m_pOnScreenLog );
	SafeDelete( m_pOnScreenLog );

	DebugOutput.ReleaseDebugItem( "perf" );
	GlobalDebugOutput.Release();
}


bool CApplicationBase::InitBase()
{
	LOG_FUNCTION_SCOPE();

	CGameStageFrameworkGlobalParams gsf_params;
	bool loaded = gsf_params.LoadFromTextFile( "../resources/gsf_params" );
	if( loaded )
	{
		// update binary file
		gsf_params.SaveToFile( "./System/gsf.bin" );
	}

	gsf_params.LoadFromFile( "./System/gsf.bin" );
	gsf_params.UpdateParams();

	// update material file
	CSurfaceMaterialManager surf_mat_mgr;
	if( surf_mat_mgr.LoadFromTextFile( "../resources/material.rd" ) )
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
	SoundManager().Init( global_params.AudioLibraryName );
	SoundManager().LoadSoundsFromList( "./Sound/SoundList.lst" );

//	LOG_PRINT( " - Initialized the sound manager" );

	bool phys_init = physics::PhysicsEngine().Init();

	if( phys_init )
		LOG_PRINT( " Initialization of the physics engine: " + string(phys_init ? "[  OK  ]" : "[FAILED]") );

	// update & load the item database
//	ItemDatabaseManager().Update( "..." );
	CItemDatabaseManager::Get()->Update( "../resources/item/item.mkx" );
	CItemDatabaseManager::Get()->LoadFromFile( "./Item/item.gid" );

	LOG_PRINT( " - Loaded the item database." );

	try
	{
		UpdateScriptArchives( "../resources/scripts", "Script/" );
	}
	catch( exception& e )
	{
		g_Log.Print( WL_WARNING, "exception: %s", e.what() );
	}

	m_pGlobalInputHandler = new CGlobalInputHandler;
	INPUTHUB.PushInputHandler( 3, m_pGlobalInputHandler );

	CGameTask::AddTaskNameToTaskIDMap( "Stage",             CGameTask::ID_STAGE );
	CGameTask::AddTaskNameToTaskIDMap( "GlobalStageLoader", CGameTask::ID_GLOBALSTAGELOADER );

	InitDebugItems();

	// start the timer
//	GlobalTimer.Start();

	// Call the init routine defined by the user
	bool res = Init();

	if( !res )
		LOG_PRINT_WARNING( " Init() of derived class failed." );

	// update the binary file of base entity database
	// - Called after Init() since derived classes register base entity factory and (class name : id) maps in Init() above
	UpdateBaseEntityDatabase();

	return true;
}


bool CApplicationBase::InitTaskManager()
{
	const std::string start_task_name = GetStartTaskName();

	if( 0 < start_task_name.length() )
	{
		m_pTaskManager = new CGameTaskManager( CreateGameTaskFactory(), start_task_name );
	}
	else
	{
		const int start_task_id = GetStartTaskID();
		if( start_task_id != CGameTask::ID_INVALID )
			m_pTaskManager = new CGameTaskManager( CreateGameTaskFactory(), start_task_id );
		else
		{
			LOG_PRINT_WARNING( " No start task was specified by the user." );

		}
	}

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
//	MSGBOX_FUNCTION_SCOPE();

	// timer resolution for timeGetTime()
	timeBeginPeriod( APPBASE_TIMER_RESOLUTION );

	if( !InitBase() )
		return;

	if( !InitTaskManager() )
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
			// update the timer
			GlobalTimer().UpdateFrameTime();

			frametime = GlobalTimer().GetFrameTime();

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

			AsyncResourceLoader().ProcessGraphicsDeviceRequests();

			Sleep( ms_DefaultSleepTimeMS );
//			PERIODICAL( 2, Sleep(5) );
		}
	}

	// Release graphics resources before the graphics device is released

	ReleaseDebugItems();

	CGameTask::ReleaseAnimatedGraphicsManager();
	MouseCursor.ReleaseGraphicsResources();

//	MessageBox( NULL, "exit the main loop", "msg", MB_OK );

	timeEndPeriod( APPBASE_TIMER_RESOLUTION );
}


inline void GDS_FreeImageErrorHandler( FREE_IMAGE_FORMAT fif, const char *message )
{
	if( fif != FIF_UNKNOWN )
	{
		g_Log.Print( "Free Image: %s Format", FreeImage_GetFormatFromFIF(fif) );
	}

	g_Log.Print( "Free Image: %s", message );
}


inline void SetFreeImageErrorHandler()
{
	// mutex - lock

	ONCE( FreeImage_SetOutputMessage(GDS_FreeImageErrorHandler) );
}


void CApplicationBase::Run()
{
//	MSGBOX_FUNCTION_SCOPE();

	// set log output device
//	boost::filesystem::complete( "./debug" );
	CLogOutput_HTML html_log( "./debug/log_" + string(GetBuildInfo()) + ".html" );
	g_Log.AddLogOutput( &html_log );

	SetFreeImageErrorHandler();

	try
	{
		Execute();
	}
	catch( exception& e )
	{
		g_Log.Print( WL_ERROR, "exception: %s", e.what() );
	}
}
