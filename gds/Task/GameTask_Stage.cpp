#include "GameTask_Stage.hpp"

#include "Support/macro.h"
#include "Support/memory_helpers.hpp"
#include "Support/Profile.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/DebugOutput.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Font/Font.hpp"
#include "Graphics/GraphicsEffectManager.hpp"
#include "Stage/Stage.hpp"
#include "Stage/StageLoader.hpp"
#include "Stage/EntitySet.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "Stage/PlayerInfo.hpp"
#include "Input/InputHub.hpp"
#include "Item/WeaponSystem.hpp"
#include "Sound/SoundManager.hpp"


CStageSharedPtr g_pStage;


static string gs_NextGlobalStageScript;


void SetGlobalStageScriptFilename( const string& filename )
{
	gs_NextGlobalStageScript = filename;
}


const string& GetGlobalStageScriptFilename()
{
	return gs_NextGlobalStageScript;
}


static int gs_iState = CGameTask_Stage::STAGE_NOT_LOADED;


void LoadStage( const string& strStageScriptFilename )
{
	// update the database of base entity before intitializing the stage
	// 070223 moved to ctor in CGameTask_TitleFG
//	UpdateBaseEntityDatabase();

	// old version that uses raw pointer for stage
//	SafeDelete( g_pStage );
//	g_pStage = new CStage;
//	bool bResult = g_pStage->Initialize( strStageScriptFilename );

	g_pStage.reset();

	CStageLoader stage_loader;
	g_pStage = stage_loader.LoadStage( strStageScriptFilename );

	bool bResult = false;
	if( g_pStage.get() )
		bResult = true;

	if( bResult )
	{
		gs_iState = CGameTask_Stage::STAGE_LOADED;
		LOG_PRINT( "loaded the stage: " + strStageScriptFilename );
	}
	else
	{
		gs_iState = CGameTask_Stage::STAGE_NOT_LOADED;
		LOG_PRINT_ERROR( "cannot load the stage: " + strStageScriptFilename );
	}

//	if( gs_iState != CGameTask_StageSelect::STAGE_LOADED )
//		LOG_PRINT_ERROR( "unable to load stage" + strStageScriptFilename

	// do not start the stage timer until the stage task is initiated
	g_pStage->PauseTimer();

//	LOG_PRINT( "stage loaded" );
}


unsigned int CGameTask_Stage::ms_FadeInTime = 500;

CGameTask_Stage::CGameTask_Stage()
{
	if( !g_pStage )
	{
		// Initialized at app startup in CApplicationBase::InitBase()
//		InitAnimatedGraphicsManager();

		// set animated graphics manager here
		// since init routines of the scripts use graphics manager
		SetAnimatedGraphicsManagerForScript();

		if( 0 < GetGlobalStageScriptFilename().length() )
		{
            LoadStage( GetGlobalStageScriptFilename() );
		}
		else
		{
			LOG_PRINT_WARNING( "No global stage has been specified." );
			return;
		}
	}

	g_pStage->ResumeTimer();

	SetDefaultFadeInTimeMS( ms_FadeInTime );
}


CGameTask_Stage::~CGameTask_Stage()
{
	ms_FadeInTime = 500;	// set the default fade-in time
}


int CGameTask_Stage::FrameMove( float dt )
{
	ONCE( g_Log.Print( "CGameTask_Stage::FrameMove()" ) );

	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( !g_pStage )
		return CGameTask::ID_INVALID;	// stage is not loaded

	ProfileBegin( "Main Loop" );

//	ONCE( g_Log.Print( "CGameTask_Stage::FrameMove() - stage state: %d", g_pStage->GetState() ) );

	int state = STATE_PLAYER_IN_STAGE;//this->GetState();

	switch( state )
	{
	case STATE_PLAYER_IN_STAGE:
		{
			g_pStage->UpdateListener();

//			GetAnimatedGraphicsManager()->UpdateEffects( dt );
			SetAnimatedGraphicsManagerForScript();

			// update the world and progress by one frame
			g_pStage->Update( dt );

			RemoveAnimatedGraphicsManagerForScript();
		}
		break;

	case STATE_PLAYER_LEFT_STAGE:
		return CGameTask::ID_STAGE_SELECT;
/*
	case CStage::MISSION_FAILED:
//		return CGameTaskFG::ID_ON_MISSIONFAILED;

	case CStage::MISSION_ACCOMPLISHED:
//		return CGameTaskFG::ID_MAINMENU;
*/
	default:
		ONCE( g_Log.Print( WL_WARNING, "CGameTask_Stage::FrameMove() - unsupported stage state" ) );
		return CGameTask::ID_INVALID;

	}

	// update player info (includes update routines for items)
	SinglePlayerInfo().Update( dt );

	ONCE( g_Log.Print( "CGameTask_Stage::FrameMove() - updated player info" ) );

	// update text message
	// 0:40 2007/05/06 - moved to CStage::Update()
//	GetTextMessageManager().Update( dt );

	if( SinglePlayerInfo().GetTaskRequest() != CGameTask::ID_INVALID )
	{
		g_pStage->PauseTimer();

//		CGameTask::eGameTask next_task = SinglePlayerInfo().GetTaskRequest();
		int next_task = SinglePlayerInfo().GetTaskRequest();
		SinglePlayerInfo().RequestTaskChange( CGameTask::ID_INVALID );
		return next_task;
	}
	else
	{
		return CGameTask::ID_INVALID;
	}
}


void CGameTask_Stage::Render()
{
	ONCE( g_Log.Print( "CGameTask_Stage::Render() - rendering the stage" ) );

	// set the camera and draw the scene
	if( g_pStage )
		g_pStage->Render();

	// render text messages
//	GetTextMessageManager().Render();
//	if( g_pStage && g_pStage->GetTextMessageManager() )
//		g_pStage->GetTextMessageManager()->Render();

	// need to finish profile before calling DrawDebugInfo()
	ProfileEnd( "Main Loop" );
//	ProfileDumpOutputToBuffer();
}
