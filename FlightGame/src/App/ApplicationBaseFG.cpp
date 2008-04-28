
#include "ApplicationBaseFG.h"
#include "App/GameWindowManager_Win32.h"

#include "../Task/GameTaskFG.h"
#include "../Task/GameTaskFactoryFG.h"
#include "Task/GameTaskManager.h"
#include "Task/GameTask_GlobalStageLoader.h"

#include "GameEvent/ScriptManager.h"
#include "GameEvent/PyModules.h"

#include "../Stage/fwd.h"
#include "../Stage/FG_BaseEntityFactory.h"
#include "../Stage/FG_AIAircraftBaseEntity.h"
#include "Stage/BaseEntityManager.h"
#include "Stage/BaseEntityFactory.h"


#include "GameInput/DirectInputMouse.h"
#include "GameInput/DIKeyboard.h"
#include "GameInput/DirectInputGamepad.h"
#include "GameInput/InputHub.h"

#include "GameCommon/MouseCursor.h"
//#include "GameCommon/Timer.h"
//#include "GameCommon/MTRand.h"
//#include "GameCommon/GlobalInputHandler.h"
//#include "GameCommon/GlobalParams.h"


#include "Support/memory_helpers.h"
#include "Support/Profile.h"
#include "Support/Log/DefaultLog.h"
//#include "Support/.h"

/*
class CFG_BaseEntityFactory : public CBaseEntityFactory
{
public:

	CFG_BaseEntityFactory() {}

	virtual ~CFG_BaseEntityFactory() {}

//	void Init();

	virtual CBaseEntity *CreateUserDefinedBaseEntity( const unsigned id ) { return NULL; }


};
*/

//========================================================================================
// CApplicationBaseFG
//========================================================================================

CApplicationBaseFG::CApplicationBaseFG()
{
}


CApplicationBaseFG::~CApplicationBaseFG()
{
//	Release();
}

/*
void CApplicationBaseFG::Release()
{
}*/

bool CApplicationBaseFG::Init()
{
	// register base entity factory
	// - needs to be registered prior to CApplicationBase::Init()
	BaseEntityManager.RegisterBaseEntityFactory( CBaseEntityFactorySharedPtr( new CFG_BaseEntityFactory() ) );

	BaseEntityManager.AddBaseEntityClassNameToIDMap( "CFG_AIAircraftBaseEntity", CFG_BaseEntityID::BE_AIAIRCRAFTBASEENTITY );

	bool base_init = CApplicationBase::Init();
	if( !base_init )
		return false;

	// init by scripts
	CScriptManager script_mgr;
	script_mgr.AddModule( "PlayerInfo",	g_PyModulePlayerMethod );
	script_mgr.AddModule( "Shop",		g_PyModuleShopMethod );
	script_mgr.AddModule( "StageGraph",	g_PyModuleStageGraphMethod );
	script_mgr.LoadScriptArchiveFile( "Script/init.bin" );
	script_mgr.Update();

	SRect cursor_rect = SRect( 0, 0, 27, 27 ) * ((float)GAMEWINDOWMANAGER.GetScreenWidth()) / 800.0f;
	MouseCursor.Load( 0, cursor_rect, "Texture\\cursor.dds", 0xFFFFFFFF );

	// task-related settings
	// CGameTask_StageFG is used for managing global stage for FlightGame
	CGameTask_GlobalStageLoader::SetTaskForGlobalStage( "StageFG" );

	return true;
}


bool CApplicationBaseFG::InitTaskManager()
{
	CGameTask::AddTaskNameToTaskIDMap( "FG_Title",          CGameTaskFG::ID_TITLE_FG );
	CGameTask::AddTaskNameToTaskIDMap( "DebriefingFG",      CGameTaskFG::ID_DEBRIEFING_FG );
	CGameTask::AddTaskNameToTaskIDMap( "BriefingFG",        CGameTaskFG::ID_BRIEFING_FG );
	CGameTask::AddTaskNameToTaskIDMap( "AircraftSelect",    CGameTaskFG::ID_AIRCRAFT_SELECT );
	CGameTask::AddTaskNameToTaskIDMap( "StageFG",			CGameTaskFG::ID_STAGE_FG );
	CGameTask::AddTaskNameToTaskIDMap( "OnMissionFailed",   CGameTaskFG::ID_ON_MISSIONFAILED_FG );
	CGameTask::AddTaskNameToTaskIDMap( "ShopFG",            CGameTaskFG::ID_SHOP_FG );
	CGameTask::AddTaskNameToTaskIDMap( "MainMenuFG",        CGameTaskFG::ID_MAINMENU_FG );
	CGameTask::AddTaskNameToTaskIDMap( "InStageMenuFG",     CGameTaskFG::ID_INSTAGEMENU_FG );
	CGameTask::AddTaskNameToTaskIDMap( "ControlCustomizer", CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
	CGameTask::AddTaskNameToTaskIDMap( "AsyncStageLoader",  CGameTaskFG::ID_ASYNCSTAGELOADER_FG );

//	m_pTaskManager = new CGameTaskManager( CGameTaskFG::ID_TITLE_FG );
//	m_pTaskManager = new CGameTaskManager( CGameTaskFG::ID_STAGE_SELECT );
//	m_pTaskManager = new CGameTaskManager( CGameTaskFG::ID_AIRCRAFT_SELECT );
	m_pTaskManager = new CGameTaskManager( new CGameTaskFactoryFG, CGameTaskFG::ID_MAINMENU_FG );

	return true;
}

/*
void CApplicationBaseFG::Run()
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
*/
