#include "GameApplicationBase.hpp"

#include <boost/filesystem.hpp>
#include <boost/exception/get_error_info.hpp>
#include "amorphous/Support/MiscAux.hpp"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Support/single_instance.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Support/DebugOutput.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/FreeTypeAux.hpp"
#include "amorphous/XML/XMLDocumentBase.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Graphics/AsyncResourceLoader.hpp"
#include "amorphous/Graphics/LogOutput_OnScreen.hpp"
#include "amorphous/Graphics/DebugInfo_GraphicsResourceManager.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Input/InputDeviceInitializer.hpp"
#include "amorphous/Input/DebugInfo_InputDevice.hpp"
#include "amorphous/Sound/SoundManager.hpp"
#include "amorphous/Sound/DebugInfo_SoundManager.hpp"
#include "amorphous/Physics/PhysicsEngine.hpp"

#include "amorphous/GameCommon/GlobalInputHandler.hpp"
#include "amorphous/GameCommon/GlobalParams.hpp"
#include "amorphous/GameCommon/GameStageFrameworkGlobalParams.hpp"
#include "amorphous/GameCommon/MouseCursor.hpp"
#include "amorphous/Script/ScriptArchive.hpp"
#include "amorphous/Task/GameTask.hpp"
#include "amorphous/Task/GameTaskManager.hpp"
#include "amorphous/Task/GameTaskFactoryBase.hpp"
#include "amorphous/Task/StdInputDeviceStateCallback.hpp"
#include "amorphous/Item/ItemDatabaseManager.hpp"
#include "amorphous/Stage/BaseEntityManager.hpp"
#include "amorphous/Stage/SurfaceMaterialManager.hpp"
#include "amorphous/App/GameWindowManager.hpp"


namespace amorphous
{

using std::string;


// ================================ global variables ================================

GameApplicationBase *g_pGameAppBase = NULL;

DirectInputMouse *m_pMouse = NULL;


void UpdateBaseEntityDatabase()
{
	static bool s_bBaseEntityDatabase_Initialized = false;
	static bool s_AlwaysUpdateDatabaseBeforeStage = false;

	if( !s_bBaseEntityDatabase_Initialized || s_AlwaysUpdateDatabaseBeforeStage )
	{
		CScopeLog sl( "- Initializing & updating the base entity database" );

		string default_db_filepath = "./System/BaseEntity.bin";
		GetBaseEntityManager().UpdateDatabase( "../resources/entity/BaseEntity.txt", default_db_filepath );
		GetBaseEntityManager().OpenDatabase( default_db_filepath );
		s_bBaseEntityDatabase_Initialized = true;
	}
}


//========================================================================================
// GameApplicationBase
//========================================================================================


int GameApplicationBase::ms_DefaultSleepTimeMS = 3;


GameApplicationBase::GameApplicationBase()
{
//	m_pDIGamepad = NULL;

	m_UseDefaultMouse = false;
	m_UseDefaultKeyboard = false;
}


GameApplicationBase::~GameApplicationBase()
{
	Release();
}


int GameApplicationBase::GetStartTaskID() const
{
	return GameTask::ID_INVALID;
}


GameTaskFactoryBase *GameApplicationBase::CreateGameTaskFactory() const
{
	return new GameTaskFactoryBase();
}


void GameApplicationBase::Release()
{
	m_pTaskManager.reset();
	GameTask::SetMouseInputDevice( shared_ptr<MouseInputDevice>() );

	m_pMouse.reset();
	m_pDIKeyboard.reset();
//	SafeDelete( m_pDIGamepad );

	GetInputHub().RemoveInputHandler( 0, m_pGlobalInputHandler.get() );
	m_pGlobalInputHandler.reset();

	// release any singleton class that inherits GraphicsComponent
	GetGraphicsResourceManager().ReleaseSingleton();
}


void GameApplicationBase::InitDebugItems()
{
	const string font_name = "BuiltinFont::BitstreamVeraSansMono-Bold-256";//"DotumChe";

	// set debug items and output to the screen
	// (graphics component)
	GlobalDebugOutput.Init( font_name, 6, 14 );

	DebugOutput.AddDebugItem( "perf", new DebugInfo_Profile() );

	m_pOnScreenLog.reset( new LogOutput_ScrolledTextBuffer( font_name, 6, 12, 16, 95 ) );
	GlobalLog().AddLogOutput( m_pOnScreenLog.get() );

	DebugOutput.AddDebugItem( "log",                       new DebugInfo_Log(m_pOnScreenLog.get()) );
	DebugOutput.AddDebugItem( "graphics_resource_manager", new DebugInfo_GraphicsResourceManager() );
	DebugOutput.AddDebugItem( "sound_manager",             new DebugInfo_SoundManager() );
	DebugOutput.AddDebugItem( "input_device",              new DebugInfo_InputDevice() );

//	DebugOutput.SetTopLeftPos( Vector2(16,32) );

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


void GameApplicationBase::ReleaseDebugItems()
{
	// delete debug output
	//  - it uses the borrowed reference of screen overlay log (m_pOnScreenLog)
//	SafeDelete( m_pDebugOutput );

	DebugOutput.ReleaseDebugItem( "log" );

	GlobalLog().RemoveLogOutput( m_pOnScreenLog.get() );
	m_pOnScreenLog.reset();

	DebugOutput.ReleaseDebugItem( "perf" );
	GlobalDebugOutput.Release();
}


bool GameApplicationBase::CreateDefaultMouseCursor( GraphicsElementManager& mgr )
{
	GetMouseCursor().CreateDefault( mgr );

	return true;
}


bool GameApplicationBase::InitBase()
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
	GlobalParams global_params;
	global_params.LoadFromFile( "config" );

	// init graphics library (Direct3D/OpenGL)
	SelectGraphicsLibrary( global_params.GraphicsLibraryName );

	// create the main game window
	// Direct3D is initialized in this function
	GameWindow::ScreenMode mode
		= global_params.FullScreen ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
	GetGameWindowManager().CreateGameWindow( global_params.ScreenWidth, global_params.ScreenHeight, mode, GetApplicationTitle() );
//	GetGameWindowManager().CreateGameWindow( 800, 600, GameWindow::WINDOWED /*GameWindow::FULLSCREEN*/ );

	if( 0 <= global_params.WindowLeftPos && 0 <= global_params.WindowTopPos )
		GetGameWindowManager().SetWindowLeftTopCornerPosition( global_params.WindowLeftPos, global_params.WindowTopPos );

	// create DirectInput mouse device
	m_pMouse.reset( CreateMouseInputDevice() );
	m_pMouse->Init();

	GameTask::SetMouseInputDevice( m_pMouse );

	LOG_PRINT( " - Initialized the direct input mouse device." );

	// initialize keyboard (DirectInput)
	m_pDIKeyboard.reset( CreateKeyboardInputDevice() );
	m_pDIKeyboard->Init();

//	LOG_PRINT( " - Initialized the direct input keyboard device." );

/*	m_pDIGamepad = new DirectInputGamepad;
	Result::Name r = m_pDIGamepad->Init();
	if( r != Result::SUCCESS )
		SafeDelete( m_pDIGamepad );*/

	DIInputDeviceMonitor().start_thread();

//	LOG_PRINT( " - Initialized the input devices." );


	//
	// Sound Module Initialization
	//

	bool sound_module_init = GetSoundManager().Init( global_params.AudioLibraryName );

	LOG_PRINT( " Sound module initialization: " + string(sound_module_init ? "[  OK  ]" : "[FAILED]") );

	GetSoundManager().LoadSoundsFromList( "./Sound/SoundList.lst" );


	//
	// Physics Engine Initialization
	//

	bool phys_init = physics::PhysicsEngine().Init();

	LOG_PRINT( " Physics engine initialization: " + string(phys_init ? "[  OK  ]" : "[FAILED]") );

	// update & load the item database
//	GetItemDatabaseManager().Update( "..." );
	GetItemDatabaseManager().Update( "../resources/items/items.xml", "./Item/item.gid" );
	GetItemDatabaseManager().LoadFromFile( "./Item/item.gid" );

	LOG_PRINT( " - Loaded the item database." );


	//
	// Log information on other libraries
	//

	LogFreeTypeLibraryVersion();

	try
	{
		UpdateScriptArchives( "../resources/scripts", "Script/" );
	}
	catch( std::exception& e )
	{
		GlobalLog().Print( WL_WARNING, "std::exception: %s", e.what() );
	}
	catch( boost::exception& e )
	{
		const int * const line       = get_error_info<throw_line>(e);
		const char * const* file     = get_error_info<throw_file>(e);
		const char * const* function = get_error_info<throw_function>(e);
		printf( "exception: at %s (%s, L%d)\n", *function, *file, *line );
	}

	m_pGlobalInputHandler.reset( new GlobalInputHandler );
	GetInputHub().PushInputHandler( 0, m_pGlobalInputHandler.get() );

	GameTask::AddTaskNameToTaskIDMap( "Stage",             GameTask::ID_STAGE );
	GameTask::AddTaskNameToTaskIDMap( "GlobalStageLoader", GameTask::ID_GLOBALSTAGELOADER );

	// Create a graphics effect manager used by all game tasks
	GameTask::InitAnimatedGraphicsManager();

	if( GameTask::GetAnimatedGraphicsManager()
	 && GameTask::GetAnimatedGraphicsManager()->GetGraphicsElementManager() )
	{
		CreateDefaultMouseCursor( *(GameTask::GetAnimatedGraphicsManager()->GetGraphicsElementManager()) );
	}

	shared_ptr<CStdInputDeviceStateCallback> pDeviceStateCallback( new CStdInputDeviceStateCallback(GameTask::GetAnimatedGraphicsManager()) );
	pDeviceStateCallback->Init();
	DIInputDeviceMonitor().RegisterCallback( pDeviceStateCallback );
	

	InitDebugItems();

	// load and update global params of the framework
	GameStageFrameworkGlobalParams gsf_params;
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


bool GameApplicationBase::InitTaskManager()
{
	const std::string start_task_name = GetStartTaskName();

	if( 0 < start_task_name.length() )
	{
		m_pTaskManager.reset( new GameTaskManager( CreateGameTaskFactory(), start_task_name ) );
	}
	else
	{
		const int start_task_id = GetStartTaskID();
		if( start_task_id != GameTask::ID_INVALID )
			m_pTaskManager.reset( new GameTaskManager( CreateGameTaskFactory(), start_task_id ) );
		else
		{
			LOG_PRINT_WARNING( " No start task was specified by the user." );

		}
	}

	return true;
}


void GameApplicationBase::AcquireInputDevices()
{
	if( m_pDIKeyboard )
		m_pDIKeyboard->Acquire();

	if( m_pMouse && GetGameWindowManager().IsMouseCursorInClientArea() )
		m_pMouse->AcquireMouse();

	DIInputDeviceMonitor().AcquireInputDevices();
//	if( m_pDIGamepad )
//		m_pDIGamepad->Acquire();
}


void GameApplicationBase::UpdateFrame()
{
	// update the timer
	GlobalTimer().UpdateFrameTime();

	const float frametime = GlobalTimer().GetFrameTime();

	if( m_pMouse )
	{
		m_pMouse->UpdateScreenSize(
			GetGameWindowManager().GetScreenWidth(),
			GetGameWindowManager().GetScreenHeight() );

//		float scale
//			= (float)GetGameWindowManager().GetScreenWidth()
//			/ (float)GraphicsComponent::REFERENCE_SCREEN_WIDTH;

		// MouseCursor() uses non-scaled screen coordinates
		GetMouseCursor().UpdateCursorPosition(
			(int)( m_pMouse->GetCurrentPositionX() ),
			(int)( m_pMouse->GetCurrentPositionY() ) );
	}

	/// Update input

	// Create input device object if a new gamdpad has been plugged to the computer
	DIInputDeviceMonitor().ProcessInputDeviceManagementRequest();

	// Send input data to the active input handlers
	GetInputDeviceHub().SendInputToInputHandlers();

	/// Update the task, stage, entities and all the other subsystems

	// Do the current task
	m_pTaskManager->Update( frametime );


	/// Render

	m_pTaskManager->Render();


	GetAsyncResourceLoader().ProcessGraphicsDeviceRequests();

	ProfileDumpOutputToBuffer();

	if( m_pTaskManager->GetCurrentTask()
	 && m_pTaskManager->GetCurrentTask()->IsAppExitRequested() )
	{
		RequestAppExit();
	}

	boost::this_thread::sleep( boost::posix_time::milliseconds(ms_DefaultSleepTimeMS) );
//	PERIODICAL( 2, Sleep(5) );
}


void GameApplicationBase::Execute()
{
	if( !InitBase() )
		return;

	if( !InitTaskManager() )
		return;

	// initialize profiler
	ProfileInit();

	// initialize rand generator
	InitRand( timeGetTime() );

	::amorphous::MainLoop( this );

	// Release graphics resources before the graphics device is released

	ReleaseDebugItems();

	DIInputDeviceMonitor().ExitThread();

	// release the ballback object before releasing GameTask::ms_pAnimatedGraphicsManager
	// by calling GameTask::ReleaseAnimatedGraphicsManager()
	DIInputDeviceMonitor().UnregisterCallback();

	GameTask::ReleaseAnimatedGraphicsManager();
}


void GameApplicationBase::Run()
{
	const char *app_id = GetUniqueID();
	if( app_id && 0 < strlen(app_id) && is_another_instance_running(app_id) )
		return;

	g_pGameAppBase = dynamic_cast<GameApplicationBase *>( ApplicationBase::GetInstance() );

	// initialize the XML module here and release at the end of this function
	// - Necessary when the xerces XML parser is used.
//	CXMLParserInitReleaseManager xml_module;

	// set log output device
//	boost::filesystem::complete( "./debug" );
	InitHTMLLog( "./debug/log_" + string(GetBuildInfo()) + ".html" );

	{
		// mutex - lock
		ONCE( InitFreeImage() );
	}

	try
	{
		Execute();
	}
	catch( std::exception& e )
	{
		GlobalLog().Print( WL_ERROR, "exception: %s", e.what() );
	}
	catch( boost::exception& e )
	{
		const int * const line       = get_error_info<throw_line>(e);
		const char * const* file     = get_error_info<throw_file>(e);
		const char * const* function = get_error_info<throw_function>(e);
		GlobalLog().Print( WL_ERROR, "exception: at %s (%s, L%d)", function, file, line );
	}

	GameTask::ReleaseAnimatedGraphicsManager();
}


} // namespace amorphous
