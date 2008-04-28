#include "GameTask_StageFG.h"
#include "FG_CmdMenuEventHandler.h"

#include "UI/ui.h"     /// ui files of GameStageFramework
#include "../UI/FG_CmdMenuControlRenderer.h" /// ui files of FlightGame
#include "../UI/FG_CmdMenuControlRendererManager.h" /// ui files of FlightGame

#include "3DCommon/Direct3D9.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Font.h"
#include "3DCommon/GraphicsEffectManager.h"
#include "3DCommon/RenderTaskProcessor.h"
#include "3DCommon/LogOutput_OnScreen.h"

#include "Stage/Stage.h"
#include "Stage/EntitySet.h"
#include "Stage/ScreenEffectManager.h"
#include "Stage/PlayerInfo.h"
#include "Stage/BE_PlayerPseudoAircraft.h"

#include "Stage/DebugWindow.h"

#include "GameCommon/Timer.h"

#include "GameInput/InputHub.h"

#include "GameTextSystem/TextMessageManager.h"

#include "Sound/GameSoundManager.h"

#include "Support/macro.h"
#include "Support/memory_helpers.h"
#include "Support/Profile.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Log/StateLog.h"
#include "Support/Log/DefaultLog.h"
#include "Support/DebugOutput.h"


void CreatePlayerCommandMenu( CBE_PlayerPseudoAircraft *pPlayerAircraft,
							  CGM_DialogManagerSharedPtr& pCmdMenuDlgMgr )
{
	CGM_ControlRendererManagerSharedPtr pRendererMgr( new CFG_CmdMenuControlRendererManager );
	pCmdMenuDlgMgr = CGM_DialogManagerSharedPtr( new CGM_DialogManager( pRendererMgr ) );

	CGM_Dialog *apDlg[5];
	SRect dlg_rect = RectLTWH( 15, 200, (int)(120 * GOLDEN_RATIO), 120 );
	apDlg[0] = pCmdMenuDlgMgr->AddRootDialog( UIID_CMD_DLG_CMD_ROOT, dlg_rect, "command" );
	apDlg[1] = pCmdMenuDlgMgr->AddRootDialog( UIID_CMD_DLG_WPN_ROOT, dlg_rect, "weapon" );

	apDlg[2] = pCmdMenuDlgMgr->AddRootDialog( UIID_CMD_DLG_MSC_ROOT, dlg_rect, "MISC" );
	apDlg[3] = pCmdMenuDlgMgr->AddRootDialog( UIID_CMD_DLG_VIS_ROOT, dlg_rect, "VISIONS" );
	apDlg[4] = pCmdMenuDlgMgr->AddRootDialog( UIID_CMD_DLG_SMT_ROOT, dlg_rect, "SUB MONITOR" );

	int title_h = 24;
	int mx = 5;
	int my = 5;
	int y,w,h;
	int id = 0;
	int i, num_allies_plus_one = 4;
	string name_str[4] = { "ALL", "ALLY0", "ALLY1", "ALLY2" };
	for( i=0; i<num_allies_plus_one; i++ )
	{
		y = 24 + i * 40;
		h = 20;
		apDlg[0]->AddStatic( id, RectLTWH( mx, y, 60, h ), name_str[i] );
		apDlg[1]->AddStatic( id, RectLTWH( mx, y, 60, h ), name_str[i] );

		y += 20;
		w = 8 * 6;
		apDlg[0]->AddRadioButton( id, RectLTWH( mx + w * 0, y, w, h ), i, "AIR" );
		apDlg[0]->AddRadioButton( id, RectLTWH( mx + w * 1, y, w, h ), i, "GRND" );
		apDlg[0]->AddRadioButton( id, RectLTWH( mx + w * 2, y, w, h ), i, "CVR" );
		apDlg[0]->AddRadioButton( id, RectLTWH( mx + w * 3, y, w, h ), i, "FREE" );

		apDlg[1]->AddRadioButton( id, RectLTWH( mx + w * 2, y, w, h ), i, "USU." );
		apDlg[1]->AddRadioButton( id, RectLTWH( mx + w * 3, y, w, h ), i, "SPW" );
		apDlg[1]->AddRadioButton( id, RectLTWH( mx + w * 3, y, w, h ), i, "CNSV" );
	}

	mx = 5;
	h = 20;
	w = dlg_rect.GetWidth() - mx * 2;

	// items
	apDlg[2]->AddCheckBox( id, RectLTWH( mx, my + title_h + 0 * h, w, h ), "Gear" );
	apDlg[2]->AddCheckBox( id, RectLTWH( mx, my + title_h + 1 * h, w, h ), "Call URAV" );
	apDlg[2]->AddCheckBox( id, RectLTWH( mx, my + title_h + 2 * h, w, h ), "Optical Cmflg" );

	// visions
	int vis_btn_group = 0;
	vector<string> vecVisionTitle;
	vecVisionTitle.push_back( "Normal" );
	vecVisionTitle.push_back( "Night Vision" );
	vecVisionTitle.push_back( "Thermal" );
	int rbt_id[] = { UIID_CMD_RBT_VIS_NORMAL, UIID_CMD_RBT_VIS_NIGHTVISION, UIID_CMD_RBT_VIS_THERMAL };
	for( i=0; i<(int)vecVisionTitle.size(); i++ )
		apDlg[3]->AddRadioButton( rbt_id[i], RectLTWH( mx, my + title_h + i * h, w, h ), vis_btn_group, vecVisionTitle[i] );

	((CGM_RadioButton *)apDlg[3]->GetControl(UIID_CMD_RBT_VIS_NORMAL))->SetChecked( true );

	// sub monitor
	int sub_mon_btn_group = 1;
	vecVisionTitle.resize( 0 );
	vecVisionTitle.push_back( "None" );
	vecVisionTitle.push_back( "Auto" );
	vecVisionTitle.push_back( "Rear" );
	vecVisionTitle.push_back( "Target" );
	for( i=0; i<(int)vecVisionTitle.size(); i++ )
		apDlg[4]->AddRadioButton( id, RectLTWH( mx, my + title_h + i * h, w, h ), vis_btn_group, vecVisionTitle[i] );

//	pCmdMenuDlgMgr->OpenRootDialog( UIID_CMD_DLG_VIS_ROOT );

	apDlg[2]->SetNextDialogsLR( apDlg[4], apDlg[3] );
	apDlg[3]->SetNextDialogsLR( apDlg[2], apDlg[4] );
	apDlg[4]->SetNextDialogsLR( apDlg[3], apDlg[2] );


	// set event handlers
	CGM_DialogEventHandlerSharedPtr pEventHandler( new CFG_CmdMenuEventHandler( pPlayerAircraft ) );
	apDlg[3]->SetEventHandler( pEventHandler );
}


CGameTask_StageFG::CGameTask_StageFG()
{
	if( !g_pStage )
	{
		LOG_PRINT_ERROR( "Global stage is not loaded. Cannot initialize player aircraft command menu" );
		return;
	}

//	CGM_ControlRendererManagerSharedPtr pCmdMenuRenderMgr( new CUIRenderMgr_CommandMenuFG );
//	CGM_DialogManager *pCmdMenu = new CGM_DialogManager( pCmdMenuRenderMgr );
//	CInputHandler_Dialog *pInputHnadler = new CInputHandler_Dialog( pCmdMenu );

	CBE_Player *pPlayerBaseEntity = PLAYERINFO.GetCurrentPlayerBaseEntity();
	if( pPlayerBaseEntity
	 && pPlayerBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT )
	{
		CGM_DialogManagerSharedPtr pCmdMenuDlgMgr;
		CreatePlayerCommandMenu( (CBE_PlayerPseudoAircraft *)pPlayerBaseEntity, pCmdMenuDlgMgr );

		vector<int> vecRootDlgID;
		vecRootDlgID.push_back( UIID_CMD_DLG_VIS_ROOT );
		pPlayerBaseEntity->SetCommandMenuUI( pCmdMenuDlgMgr, vecRootDlgID );
	}
	else
	{
		LOG_PRINT_ERROR( "Player base entity is not loaded. Cannot initialize player aircraft command menu" );
	}

//	if( !GetAnimatedGraphicsManager() )
//		InitAnimatedGraphicsManager();
}


CGameTask_StageFG::~CGameTask_StageFG()
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


void CGameTask_StageFG::UpdateStage( float dt )
{
	Matrix34 cam_pose;
	CCamera *pCamera;

	// get the pose of the current camera
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
			cam_pose.Identity();
	}

	// update listener for sound manager
	GAMESOUNDMANAGER.UpdateListenerPose( cam_pose ); 

	CCopyEntity *pCameraEntity = g_pStage->GetEntitySet()->GetCameraEntity();
	if( pCameraEntity )
		GAMESOUNDMANAGER.UpdateListenerVelocity( pCameraEntity->Velocity() ); 

	GAMESOUNDMANAGER.CommitDeferredSettings();

	GetAnimatedGraphicsManager()->UpdateEffects( dt );
	SetAnimatedGraphicsManagerForScript();

	ONCE( g_Log.Print( "CGameTask_StageFG::UpdateStage() - updating the stage g_pStage: 0x%x", g_pStage ) );

	// update the world and progress by one frame
	ProfileBegin( "Stage Update" );
	g_pStage->Update( dt );
	ProfileEnd( "Stage Update" );

	ONCE( g_Log.Print( "CGameTask_StageFG::UpdateStage() - updated stage" ) );

	RemoveAnimatedGraphicsManagerForScript();
}


int CGameTask_StageFG::FrameMove( float dt )
{
	PROFILE_FUNCTION();

	ONCE( g_Log.Print( "CGameTask_StageFG::FrameMove()" ) );

	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( !g_pStage )
	{
		ONCE( LOG_PRINT_ERROR( "global stage is not loaded." ) );
		return CGameTask::ID_INVALID;	// stage is not loaded
	}

	CBaseEntity *pPlayerBaseEntity = PLAYERINFO.GetCurrentPlayerBaseEntity();
	if( !IsInTransitionToNextTask()
	 && pPlayerBaseEntity
	 && pPlayerBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT 
	 && PLAYERINFO.GetCurrentPlayerEntity() )
	{
		CBE_PlayerPseudoAircraft *pAircraftBaseEntity
			= (CBE_PlayerPseudoAircraft *)pPlayerBaseEntity;

		int mission_state = pAircraftBaseEntity->GetMissionState();

		switch( mission_state )
		{
		case CBE_PlayerPseudoAircraft::MSTATE_MISSION_ACCOMPLISHED:
			break;

//		case CBE_PlayerPseudoAircraft::MSTATE_TIMEUP:
//			RequestTaskTransition( "OnMissionFailed", 0.0f, 0.0f, 0.0f );
//			break;

		case CBE_PlayerPseudoAircraft::MSTATE_CRASHED:
			RequestTaskTransition( "OnMissionFailed", 0.0f, 0.0f, 0.0f );
			break;

		case CBE_PlayerPseudoAircraft::MSTATE_KIA:
			RequestTaskTransition( "OnMissionFailed", 0.0f, 0.0f, 0.0f );
			break;

		case CBE_PlayerPseudoAircraft::MSTATE_NOT_IN_MISSION:
		case CBE_PlayerPseudoAircraft::MSTATE_IN_MISSION:
		default:
			break;
		}
	}

//	ProfileBegin( "Main Loop" );

//	ONCE( g_Log.Print( "CGameTask_StageFG::FrameMove() - stage state: %d", g_pStage->GetState() ) );

	// update stage and related components
	// - update stage
	//   - update entities
	//   - update script routines
	// - update listner pose for sound manager
	UpdateStage( dt );

	// update player info (includes update routines for items)
	PLAYERINFO.Update( dt );

	ONCE( g_Log.Print( "CGameTask_StageFG::FrameMove() - updated player info" ) );

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


void CGameTask_StageFG::Render()
{
	ONCE( g_Log.Print( "CGameTask_StageFG::Render() - rendering the stage" ) );

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
///	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255,0,255,255), 1.0f, 0 );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// set the camera and draw the scene
	if( g_pStage )
		g_pStage->Render();

	// render 2d elements on top of the scene
	if( GetAnimatedGraphicsManager() )
        GetAnimatedGraphicsManager()->GetGraphicsElementManager()->Render();

	// render text messages
//	HRESULT hr = pd3dDevice->BeginScene();

//	if( FAILED(hr) )
//		ONCE( LOG_PRINT_ERROR( "pd3dDevice->BeginScene() failed." ) );

	if( g_pStage && g_pStage->GetTextMessageManager() )
        g_pStage->GetTextMessageManager()->Render();
//	pd3dDevice->EndScene();

	// need to finish profile before calling DrawDebugInfo()
//	ProfileEnd( "Main Loop" );
	ProfileDumpOutputToBuffer();

	// display fps and other performance information
	DrawDebugInfo();
}


void CGameTask_StageFG::CreateRenderTasks()
{
	if( g_pStage
	 && g_pStage->GetScreenEffectManager() )
	{
		g_pStage->GetScreenEffectManager()->ClearEffectFlag( ScreenEffect::PostProcessEffects );	
	}

	if( g_pStage )
		g_pStage->CreateRenderTasks();

	RenderTaskProcessor.AddRenderTask( new CGameTaskRenderTask( this ) );
}


void CGameTask_StageFG::ReleaseGraphicsResources()
{
	CGameTask_Stage::ReleaseGraphicsResources();
}


void CGameTask_StageFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	CGameTask_Stage::LoadGraphicsResources( rParam );
}
