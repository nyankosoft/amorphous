
#include "GameTask_Stage.h"
#include "GameTask_StageSelect.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Font.h"
#include "3DCommon/GraphicsEffectManager.h"
#include "3DCommon/LogOutput_OnScreen.h"

#include "Stage/Stage.h"
#include "Stage/EntitySet.h"
#include "Stage/ScreenEffectManager.h"
#include "Stage/PlayerInfo.h"

#include "Stage/DebugWindow.h"

#include "GameCommon/Timer.h"

#include "GameInput/InputHub.h"
#include "Stage/Input/InputHandler_PlayerShip.h"
//#include "GameInput/SystemInputHandler_Debug.h"

#include "GameTextSystem/TextMessageManager.h"

#include "Item/WeaponSystem.h"

#include "Sound/SoundManager.h"

#include "Support/macro.h"
#include "Support/memory_helpers.h"
#include "Support/Profile.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Log/StateLog.h"
#include "Support/Log/DefaultLog.h"
#include "Support/DebugOutput.h"


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


unsigned int CGameTask_Stage::ms_FadeInTime = 500;

CGameTask_Stage::CGameTask_Stage()
{
	if( !g_pStage )
	{
		InitAnimatedGraphicsManager();

		// set animated graphics manager here
		// since init routines of the scripts use graphics manager
		SetAnimatedGraphicsManagerForScript();

		if( 0 < GetGlobalStageScriptFilename().length() )
		{
            LoadStage( GetGlobalStageScriptFilename() );
		}
		else
		{
			PrintLog( "CGameTask_Stage::CGameTask_Stage() - no global stage has been specified" );
			return;
		}
	}

	g_pStage->ResumeTimer();

///	PLAYERINFO.SetInputHandlerForPlayerShip();

	m_pFont = new CFont;
	m_pFont->InitFont( "‚l‚r ƒSƒVƒbƒN", 8, 16);

	SetDefaultFadeInTimeMS( ms_FadeInTime );

	m_pOnScreenLog = new CLogOutput_OnScreen( "‚l‚r ƒSƒVƒbƒN", 6, 12, 16, 95 );
	g_Log.AddLogOutput( m_pOnScreenLog );

	DebugOutput.AddDebugItem( "log",	new CDebugItem_Log(m_pOnScreenLog) );
//	DebugOutput.SetBackgroundRect( C2DRect( D3DXVECTOR2(8,8), D3DXVECTOR2(320, 480), 0x80000000 ) );


/*
//	m_pDebugOutput = new CDebugOutput( font name );
	m_pDebugOutput = new CDebugOutput( "", 6, 12 );
	m_pDebugOutput->AddDebugItem( new CDebugItem_Log( m_pOnScreenLog ) );
	m_pDebugOutput->AddDebugItem( new CDebugItem_Profile() );
//	m_pDebugOutput->AddDebugItem( new CDebugItem_StateLog( StateLog::PlayerLogOffset,	StateLog::NumPlayerLogs ) );
//	m_pDebugOutput->AddDebugItem( new CDebugItem_StateLog( StateLog::EnemyLogOffset,	StateLog::NumEnemyLogs ) );
*/
}


CGameTask_Stage::~CGameTask_Stage()
{
	SafeDelete( m_pFont );

	// delete debug output
	//  - it uses the borrowed reference of screen overlay log (m_pOnScreenLog)
//	SafeDelete( m_pDebugOutput );

	DebugOutput.ReleaseDebugItem( "log" );

	g_Log.RemoveLogOutput( m_pOnScreenLog );
	SafeDelete( m_pOnScreenLog );

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

	Matrix34 cam_pose;
	CCamera *pCamera;

	ProfileBegin( "Main Loop" );

//	ONCE( g_Log.Print( "CGameTask_Stage::FrameMove() - stage state: %d", g_pStage->GetState() ) );

	int state = STATE_PLAYER_IN_STAGE;//this->GetState();

	switch( state )
	{
	case STATE_PLAYER_IN_STAGE:
		{
			pCamera = g_pStage->GetCurrentCamera();
			if( pCamera )
			{
				pCamera->GetPose( cam_pose );
			}
			else
			{
				CCopyEntity* pEntity = g_pStage->GetEntitySet()->GetCameraEntity();
				if( pEntity )
					cam_pose = pEntity->GetWorldPose();
				else
				{
					cam_pose.Identity();
				}
			}

			// update listener for sound manager
			SoundManager().SetListenerPose( cam_pose ); 

			CCopyEntity *pCameraEntity = g_pStage->GetEntitySet()->GetCameraEntity();
			if( pCameraEntity )
				SoundManager().SetListenerVelocity( pCameraEntity->Velocity() ); 

			SoundManager().CommitDeferredSettings();

			GetAnimatedGraphicsManager()->UpdateEffects( dt );
			SetAnimatedGraphicsManagerForScript();

			// update the world and progress by one frame
			g_pStage->Update( dt );

			RemoveAnimatedGraphicsManagerForScript();
		}
		break;

	case STATE_PLAYER_LEFT_STAGE:
		ReleaseAnimatedGraphicsManager();
		return CGameTask::ID_STAGE_SELECT;
/*
	case CStage::MISSION_FAILED:
		ReleaseAnimatedGraphicsManager();
//		return CGameTaskFG::ID_ON_MISSIONFAILED;

	case CStage::MISSION_ACCOMPLISHED:
		ReleaseAnimatedGraphicsManager();
//		return CGameTaskFG::ID_MAINMENU;
*/
	default:
		ONCE( g_Log.Print( WL_WARNING, "CGameTask_Stage::FrameMove() - unsupported stage state" ) );
		return CGameTask::ID_INVALID;

	}

	// update player info (includes update routines for items)
	PLAYERINFO.Update( dt );

	ONCE( g_Log.Print( "CGameTask_Stage::FrameMove() - updated player info" ) );

	// update text message
	// 0:40 2007/05/06 - moved to CStage::Update()
//	GetTextMessageManager().Update( dt );

	if( PLAYERINFO.GetTaskRequest() != CGameTask::ID_INVALID )
	{
		g_pStage->PauseTimer();

//		CGameTask::eGameTask next_task = PLAYERINFO.GetTaskRequest();
		int next_task = PLAYERINFO.GetTaskRequest();
		PLAYERINFO.RequestTaskChange( CGameTask::ID_INVALID );
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

	// do the render routine of the base class
	CGameTask::Render();

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
///	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255,0,255,255), 1.0f, 0 );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// set the camera and draw the scene
	if( g_pStage )
		g_pStage->Render();

	if( GetAnimatedGraphicsManager() )
        GetAnimatedGraphicsManager()->GetGraphicsElementManager()->Render();

	// render text messages
//	GetTextMessageManager().Render();
	if( g_pStage && g_pStage->GetTextMessageManager() )
        g_pStage->GetTextMessageManager()->Render();

	// need to finish profile before calling DrawDebugInfo()
	ProfileEnd( "Main Loop" );
	ProfileDumpOutputToBuffer();

	// display fps and other performance information
	pd3dDevice->BeginScene();
	DrawDebugInfo();
    pd3dDevice->EndScene();

	pd3dDevice->BeginScene();
	RenderFadeEffect();
    pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


void CGameTask_Stage::DrawDebugInfo()
{
	// render background rect
//	D3DXVECTOR2 v2d, v2end;
	D3DXVECTOR2 v2d = D3DXVECTOR2(8,8);
//	v2end = v2d + D3DXVECTOR2(320, 480);
//	C2DRect rect = C2DRect( v2d, v2end, 0x88000000 );

//	rect.Draw();

	DebugOutput.Render();

	// display fps
	CFontBase *pFont = m_pFont;
	if( !pFont )
		return;

//	float fLetterHeight = (float)pFont->GetFontHeight();
	v2d = D3DXVECTOR2(16, 8);

///	StateLog.Update( 0, "FPS: " + to_string(TIMER.GetFPS()) );
//	StateLog.Update( 1, "AVE. FPS: " + to_string(FPS.GetAverageFPS()) );
	string fps_text = "FPS: " + to_string(TIMER.GetFPS());

	DWORD color = ( TIMER.GetFPS() < 40.0f ) ? 0xFFFF0000 /* fps low - red */ : 0xFF00FF00; /* green */

	pFont->DrawText( fps_text.c_str(), v2d, color );

	// display ohter debug info
	// e.g.) log, profile results, input device status, entity positions / velocities, etc.
/*	if( m_pDebugOutput )
	{
		m_pDebugOutput->Render();
	}
*/
}


void CGameTask_Stage::ReleaseGraphicsResources()
{
	m_pFont->Release();
}


void CGameTask_Stage::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	m_pFont->Reload(); 
}
