#include "GameApplicationBase.hpp"

#include <boost/filesystem.hpp>
#include <boost/exception/get_error_info.hpp>
#include "Support.hpp"
#include "Support/MiscAux.hpp"
#include "Support/MTRand.hpp"
#include "XML/XMLDocumentLoader.hpp"
#include "Graphics/GraphicsResourceManager.hpp"
#include "Graphics/AsyncResourceLoader.hpp"
#include "Graphics/LogOutput_OnScreen.hpp"
#include "Input.hpp"
#include "Sound/SoundManager.hpp"
#include "Physics/PhysicsEngine.hpp"

#include "GameCommon/GlobalInputHandler.hpp"
#include "GameCommon/GlobalParams.hpp"
#include "GameCommon/GameStageFrameworkGlobalParams.hpp"
#include "GameCommon/MouseCursor.hpp"
#include "Script/ScriptArchive.hpp"
#include "Task/GameTask.hpp"
#include "Task/GameTaskManager.hpp"
#include "Task/GameTaskFactoryBase.hpp"
#include "Task/StdInputDeviceStateCallback.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "Stage/BaseEntityManager.hpp"
#include "Stage/SurfaceMaterialManager.hpp"
#include "App/GameWindowManager_Win32.hpp"

using namespace std;
using namespace boost;


// ================================ global variables ================================

CGameApplicationBase *g_pAppBase = NULL;

CDirectInputMouse *g_pDIMouse = NULL;


void UpdateBaseEntityDatabase()
{
	static bool s_bBaseEntityDatabase_Initialized = false;
	static bool s_AlwaysUpdateDatabaseBeforeStage = false;

	if( !s_bBaseEntityDatabase_Initialized || s_AlwaysUpdateDatabaseBeforeStage )
	{
		CScopeLog sl( "- Initializing & updating the base entity database" );

		BaseEntityManager().UpdateDatabase( "../resources/entity/BaseEntity.txt" );
		BaseEntityManager().OpenDatabase( "./System/BaseEntity.bin" );
		s_bBaseEntityDatabase_Initialized = true;
	}
}


//========================================================================================
// CGameApplicationBase
//========================================================================================


int CGameApplicationBase::ms_DefaultSleepTimeMS = 3;


CGameApplicationBase::CGameApplicationBase()
{
	m_pTaskManager = NULL;

	g_pDIMouse = NULL;
	m_pDIKeyboard = NULL;
//	m_pDIGamepad = NULL;

	m_UseDefaultMouse = false;
	m_UseDefaultKeyboard = false;
}


CGameApplicationBase::~CGameApplicationBase()
{
	Release();
}


int CGameApplicationBase::GetStartTaskID() const
{
	return CGameTask::ID_INVALID;
}


CGameTaskFactoryBase *CGameApplicationBase::CreateGameTaskFactory() const
{
	return new CGameTaskFactoryBase();
}


void CGameApplicationBase::Release()
{
	SafeDelete( m_pTaskManager );

	SafeDelete( g_pDIMouse );
	SafeDelete( m_pDIKeyboard );
//	SafeDelete( m_pDIGamepad );

	InputHub().PopInputHandler( 3 );
	SafeDelete( m_pGlobalInputHandler );

	// release any singleton class that inherits CGraphicsComponent
	GraphicsResourceManager().ReleaseSingleton();
}


void CGameApplicationBase::InitDebugItems()
{
	const string font_name = "BuiltinFont::BitstreamVeraSansMono_Bold_256";//"DotumChe";

	// set debug items and output to the screen
	// (graphics component)
	GlobalDebugOutput.Init( font_name, 6, 14 );

	DebugOutput.AddDebugItem( "perf", new CDebugItem_Profile() );

	m_pOnScreenLog = new CLogOutput_ScrolledTextBuffer( font_name, 6, 12, 16, 95 );
	g_Log.AddLogOutput( m_pOnScreenLog );

	DebugOutput.AddDebugItem( "log",  new CDebugItem_Log(m_pOnScreenLog) );

	DebugOutput.AddDebugItem( "graphics_resource_manager", new CDebugItem_GraphicsResourceManager() );

	DebugOutput.AddDebugItem( "sound_manager", new CDebugItem_SoundManager() );

	DebugOutput.AddDebugItem( "input_device", new CDebugItem_InputDevice() );

	DebugOutput.SetTopLeftPos( Vector2(16,32) );

	DebugOutput.Hide();

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


void CGameApplicationBase::ReleaseDebugItems()
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


bool CGameApplicationBase::InitBase()
{
	LOG_FUNCTION_SCOPE();

	// create basic directories
	boost::filesystem::create_directory( "./System" );
	boost::filesystem::create_directory( "./Item" );
	boost::filesystem::create_directory( "./Script" );
	boost::filesystem::create_directory( "./SaveData" );

	// update material file
	CSurfaceMaterialManager surf_mat_mgr;
	if( surf_mat_mgr.LoadFromTextFile( "../resources/material.rd" ) )
		surf_mat_mgr.SaveToFile( "./Stage/material.bin" );

	// load config file
	// - users who feel that in-game UI is cumbersome can edit this text file
	CGlobalParams global_params;
	global_params.LoadFromFile( "config" );

	// init graphics library (Direct3D/OpenGL)
	SelectGraphicsLibrary( global_params.GraphicsLibraryName );

	// create the main game window
	// Direct3D is initialized in this function
	GameWindow::ScreenMode mode
		= global_params.FullScreen ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
	GameWindowManager().CreateGameWindow( global_params.ScreenWidth, global_params.ScreenHeight, mode, GetApplicationTitle() );
//	GameWindowManager().CreateGameWindow( 800, 600, GameWindow::WINDOWED /*GameWindow::FULLSCREEN*/ );

	if( 0 <= global_params.WindowLeftPos && 0 <= global_params.WindowTopPos )
		GameWindowManager().SetWindowLeftTopCornerPosition( global_params.WindowLeftPos, global_params.WindowTopPos );

	// create DirectInput mouse device
	g_pDIMouse = new CDirectInputMouse;
	g_pDIMouse->Init();

	LOG_PRINT( " - Initialized the direct input mouse device." );

	// initialize keyboard (DirectInput)
	m_pDIKeyboard = new CDIKeyboard;
	m_pDIKeyboard->Init();

//	LOG_PRINT( " - Initialized the direct input keyboard device." );

/*	m_pDIGamepad = new CDirectInputGamepad;
	Result::Name r = m_pDIGamepad->Init();
	if( r != Result::SUCCESS )
		SafeDelete( m_pDIGamepad );*/

	DIInputDeviceMonitor().start_thread();

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
	ItemDatabaseManager().Update( "../resources/items/items.xml", "./Item/item.gid" );
	ItemDatabaseManager().LoadFromFile( "./Item/item.gid" );

	LOG_PRINT( " - Loaded the item database." );

	try
	{
		UpdateScriptArchives( "../resources/scripts", "Script/" );
	}
	catch( std::exception& e )
	{
		g_Log.Print( WL_WARNING, "std::exception: %s", e.what() );
	}
	catch( boost::exception& e )
	{
		// compiled on VisualC++ 2005 Express, error on GCC 4.1.1
#ifdef _MSC_VER
		shared_ptr<throw_line::value_type const> pLine         = get_error_info<throw_line>(e);
		shared_ptr<throw_file::value_type const> pFile         = get_error_info<throw_file>(e);
		shared_ptr<throw_function::value_type const> pFunction = get_error_info<throw_function>(e);
		int line             = *pLine.get();
		const char *file     = *pFile.get();
		const char *function = *pFunction.get();
		g_Log.Print( WL_WARNING, "boost::exception: at %s (%s, L%d)\n", function, file, line );
#else
		// compiled on GCC 4.1.1, error on VisualC++ 2005 Express
		const int * const line      = get_error_info<throw_line>(e);
		const char * const* file     = get_error_info<throw_file>(e);
		const char * const* function = get_error_info<throw_function>(e);
		printf( "exception: at %s (%s, L%d)\n", *function, *file, *line );
#endif /* _MSC_VER */
	}

	m_pGlobalInputHandler = new CGlobalInputHandler;
	InputHub().PushInputHandler( 3, m_pGlobalInputHandler );

	CGameTask::AddTaskNameToTaskIDMap( "Stage",             CGameTask::ID_STAGE );
	CGameTask::AddTaskNameToTaskIDMap( "GlobalStageLoader", CGameTask::ID_GLOBALSTAGELOADER );

	// Create a graphics effect manager used by all game tasks
	CGameTask::InitAnimatedGraphicsManager();

	shared_ptr<CStdInputDeviceStateCallback> pDeviceStateCallback( new CStdInputDeviceStateCallback(CGameTask::GetAnimatedGraphicsManager()) );
	pDeviceStateCallback->Init();
	DIInputDeviceMonitor().RegisterCallback( pDeviceStateCallback );
	

	InitDebugItems();

	// load and update global params of the framework
	CGameStageFrameworkGlobalParams gsf_params;
	bool loaded = gsf_params.LoadFromTextFile( "../resources/gsf_params" );
	if( loaded )
	{
		// update binary file
		// - used in a release version of the software
		gsf_params.SaveToFile( "./System/gsf.bin" );
	}

	gsf_params.LoadFromFile( "./System/gsf.bin" );
	gsf_params.UpdateParams();

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


bool CGameApplicationBase::InitTaskManager()
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


void CGameApplicationBase::AcquireInputDevices()
{
	if( m_pDIKeyboard )
		m_pDIKeyboard->Acquire();

	if( g_pDIMouse && GameWindowManager().IsMouseCursorInClientArea() )
		g_pDIMouse->AcquireMouse();

	DIInputDeviceMonitor().AcquireInputDevices();
//	if( m_pDIGamepad )
//		m_pDIGamepad->Acquire();
}


void CGameApplicationBase::UpdateFrame()
{
	// update the timer
	GlobalTimer().UpdateFrameTime();

	const float frametime = GlobalTimer().GetFrameTime();

	if( g_pDIMouse )
	{
		g_pDIMouse->UpdateScreenSize(
			GameWindowManager().GetScreenWidth(),
			GameWindowManager().GetScreenHeight() );

//		float scale
//			= (float)GameWindowManager().GetScreenWidth()
//			/ (float)CGraphicsComponent::REFERENCE_SCREEN_WIDTH;

		// MouseCursor() uses non-scaled screen coordinates
		MouseCursor().UpdateCursorPosition(
			(int)( g_pDIMouse->GetCurrentPositionX() ),
			(int)( g_pDIMouse->GetCurrentPositionY() ) );
	}

	/// Update input

	// Create input device object if a new gamdpad has been plugged to the computer
	DIInputDeviceMonitor().ProcessInputDeviceManagementRequest();

	// Send input data to the active input handlers
	InputDeviceHub().SendInputToInputHandlers();

	/// Update the task, stage, entities and all the other subsystems

	// Do the current task
	m_pTaskManager->Update( frametime );


	/// Render

	m_pTaskManager->Render();


	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

	ProfileDumpOutputToBuffer();

	Sleep( ms_DefaultSleepTimeMS );
//	PERIODICAL( 2, Sleep(5) );
}


void CGameApplicationBase::Execute()
{
//	MSGBOX_FUNCTION_SCOPE();

	if( !InitBase() )
		return;

	if( !InitTaskManager() )
		return;

	// initialize profiler
	ProfileInit();

	// initialize rand generator
	InitRand( timeGetTime() );

	::MainLoop( this );

	// Release graphics resources before the graphics device is released

	ReleaseDebugItems();

	DIInputDeviceMonitor().ExitThread();

	// release the ballback object before releasing CGameTask::ms_pAnimatedGraphicsManager
	// by calling CGameTask::ReleaseAnimatedGraphicsManager()
	DIInputDeviceMonitor().UnregisterCallback();

	CGameTask::ReleaseAnimatedGraphicsManager();

//	MessageBox( NULL, "exit the main loop", "msg", MB_OK );
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


void CGameApplicationBase::Run()
{
//	MSGBOX_FUNCTION_SCOPE();

	// initialize the XML module here and release at the end of this function
	CXMLParserInitReleaseManager xml_module;

	// set log output device
//	boost::filesystem::complete( "./debug" );
	CLogOutput_HTML html_log( "./debug/log_" + string(GetBuildInfo()) + ".html" );
	g_Log.AddLogOutput( &html_log );

	SetFreeImageErrorHandler();

	try
	{
		Execute();
	}
	catch( std::exception& e )
	{
		g_Log.Print( WL_ERROR, "exception: %s", e.what() );
	}
	catch( boost::exception& e )
	{
		shared_ptr<throw_line::value_type const> pLine          = get_error_info<throw_line>(e);
		shared_ptr<throw_file::value_type const> pFile         = get_error_info<throw_file>(e);
		shared_ptr<throw_function::value_type const> pFunction = get_error_info<throw_function>(e);
		int line             = *pLine.get();
		const char *file     = *pFile.get();
		const char *function = *pFunction.get();
		g_Log.Print( WL_ERROR, "exception: at %s (%s, L%d)", function, file, line );
	}

	CGameTask::ReleaseAnimatedGraphicsManager();

	g_Log.RemoveLogOutput( &html_log );
}
