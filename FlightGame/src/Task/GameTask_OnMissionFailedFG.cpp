
#include "GameTask_OnMissionFailedFG.h"
#include "Task/GameTask_Stage.h"
#include "../UI/all.h"

#include "Stage/Stage.h"
#include "Stage/PlayerInfo.h"
#include "Stage/BE_PlayerPseudoAircraft.h"
#include "Stage/CopyEntityDesc.h"
#include "Stage/CopyEntity.h"
#include "Stage/EntityMotionPathRequest.h"
#include "Stage/BE_ScriptedCamera.h"

#include "GameInput/InputHub.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/Font.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/RenderTaskProcessor.h"
#include "GameCommon/MTRand.h"

#include "App/GameWindowManager_Win32.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"

#include <direct.h>
#include <assert.h>


void CEventHandler_OnMissionFailedFG::HandleEvent( CGM_Event &event )
{
	if( !event.pControl )
		return;

//	if( event.Type == CGM_Event::DIALOG_CLOSED )
//	{
//		m_pTask->RequestTaskTransition( CGameTaskFG::ID_PREVTASK );
//	}

	switch( event.pControl->GetID() )
	{
	case ID_OMF_RETRY_YES:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			// TODO: restore player's state and re-start the stage
			m_pTask->RequestTaskTransition( CGameTask::ID_GLOBALSTAGELOADER );
		}
		break;

	case ID_OMF_QUIT_YES:
		// TODO: restore player's state and leave the stage

		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			//========== destroy stage object before ending application ==========
			g_pStage.reset();
			//====================================================================

			// PLAYERINFO.Release() must be called after the stage has been released
			// since PLAYERINFO.Release() release all the items and CBE_PlayerPseudoAircraft
			// has borrowed reference of a player's aircraft
			PLAYERINFO.Release();

			PostMessage( GAMEWINDOWMANAGER.GetWindowHandle(), WM_QUIT, 0, 0 );
		}
		break;

//	case ID_OMF_OPEN_SYSTEM_MENU:
//		if( event.Type == CGM_Event::BUTTON_CLICKED )
//			m_pTask->SetRequest( CGameTask_OnMissionFailedFG::SYSMENU_TASK_REQUESTED );
//		break;

	default:
		break;
	}
}


CGM_Dialog *CGameTask_OnMissionFailedFG::CreateRootMenu()
{
	CGM_Dialog *pRootDlg = m_pDialogManager->AddRootDialog(
		ID_OMF_DLG_ROOT,
		RectLTWH( 500, 420, (int)(100 * GOLDEN_RATIO), 100 ),
		"MISSION FAILED",
		CGM_Dialog::STYLE_ALWAYS_OPEN );

	CGM_Dialog *apDialog[2];

	// confirmation dialog box
	apDialog[0]
		= Create2ChoiceDialog( m_pDialogManager, false,
		"RETRY", SRect( 400-80, 300-60, 400+80, 300+30 ),
		ID_OMF_RETRY_YES,	"YES",	RectLTWH( 15, 60, 60, 25 ),
		ID_OMF_RETRY_NO,	"NO",	RectLTWH( 85, 60, 60, 25 ) );

	apDialog[1]
		= Create2ChoiceDialog( m_pDialogManager, false,
		"QUIT", SRect( 400-80, 300-60, 400+80, 300+30 ),
		ID_OMF_QUIT_YES,"YES",	RectLTWH( 15, 60, 60, 25 ),
		ID_OMF_QUIT_NO,	"NO",	RectLTWH( 85, 60, 60, 25 ) );

	// customize confirm dialogs - no sliding in / out
	for( int i=0; i<2; i++ )
	{
		CFG_StdDialogRenderer *pDlgRenderer = dynamic_cast<CFG_StdDialogRenderer *>(apDialog[i]->GetRenderer());
		if( pDlgRenderer )
			pDlgRenderer->SetSlideInOutAmount( Vector2(0,0), Vector2(0,0) );
	}

	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_OnMissionFailedFG(this) );
	apDialog[0]->SetEventHandler( pEventHandler );
	apDialog[1]->SetEventHandler( pEventHandler );

	int top_margin = 20;
	pRootDlg->AddSubDialogButton( 0, RectLTWH( 10, top_margin + 32*0, 100, 32 ), "RETRY", apDialog[0] );
	pRootDlg->AddSubDialogButton( 0, RectLTWH( 10, top_margin + 32*1, 100, 32 ), "QUIT", apDialog[1] );

	// set the event handler to the root dialog as well
	pRootDlg->SetEventHandler( pEventHandler );

	return pRootDlg;
}

/*
CGM_Dialog *CGameTask_OnMissionFailedFG::CreateSubDialog0()
{
	// create subdialog
	CGM_Dialog *pSubDialog = NULL;

	return pSubDialog;
}
*/

void CGameTask_OnMissionFailedFG::InitMenu()
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CFG_StdControlRendererManager );
	m_pUIRendererManager= pRenderMgr;

	// add font to UI render manager
//	CGameTaskFG::LoadFonts( m_pUIRendererManager );

	m_pDialogManager = CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	CGameTaskFG::DoCommonInit( m_pDialogManager );

	// submenu for aircraft select
	CGM_Dialog *pRootDialog = CreateRootMenu();

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_pDialogManager );
//	m_pInputHandler = new CInputHandler_Debug( this, m_pDialogManager );
	INPUTHUB.PushInputHandler( m_pInputHandler );

	// set scale for the current resolution
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	m_pDialogManager->ChangeScale( scale_factor );
	m_pDialogManager->OpenRootDialog( ID_OMF_DLG_ROOT );
}


CGameTask_OnMissionFailedFG::CGameTask_OnMissionFailedFG()
{
	// create dialog menu for selecting stages
	InitMenu();

	// set init sub menu
	m_SubMenu = SM_SELECT1;
	m_NextSubMenu = SM_INVALID;

	SetDefaultFadeInTimeMS( 0 );

	if( g_pStage )
		g_pStage->PauseTimer();

	// set mission failure type
	CBaseEntity *pPlayerBaseEntity = PLAYERINFO.GetCurrentPlayerBaseEntity();
	if( pPlayerBaseEntity
	 && pPlayerBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT )
//	 && PLAYERINFO.GetCurrentPlayerEntity() )
	{
		CBE_PlayerPseudoAircraft *pAircraftBaseEntity
			= (CBE_PlayerPseudoAircraft *)pPlayerBaseEntity;

		switch( pAircraftBaseEntity->GetMissionState() )
		{
		case CBE_PlayerPseudoAircraft::MSTATE_TIMEUP:	m_MissionFailedType = MFAILED_TIMEUP;	break;
		case CBE_PlayerPseudoAircraft::MSTATE_CRASHED:	m_MissionFailedType = MFAILED_CRASHED;	break;
		case CBE_PlayerPseudoAircraft::MSTATE_KIA:		m_MissionFailedType = MFAILED_KIA;		break;
		default:
			m_MissionFailedType = MFAILED_INVALID;
			break;
		}

		CreateCamerasAndCameraController( pAircraftBaseEntity );
	}

	if( g_pStage )
		g_pStage->ResumeTimer();
}


CGameTask_OnMissionFailedFG::~CGameTask_OnMissionFailedFG()
{
//	INPUTHUB.PopInputHandler();
//	SafeDelete( m_pInputHandler );

}


void CGameTask_OnMissionFailedFG::CreateCamerasAndCameraController( CBE_PlayerPseudoAircraft *pAircraftBaseEntity )
{
	if( !g_pStage )
		return;

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( "CameraController" );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = "cam_controller_for_mission_failed_task";

	CCopyEntity* pCameraController = g_pStage->CreateEntity( desc );

	if( !IsValidEntity(pCameraController) )
	{
		g_Log.Print( "%s - unable to create camera controller", __FUNCTION__ );
		return;
	}

	baseentity_handle.SetBaseEntityName( "ScriptedCamera" );
	desc.SetDefault();
	desc.pParent = pCameraController;
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = "cam_for_mission_failed_task";

	CCopyEntity* pCameraEntity = g_pStage->CreateEntity( desc );

	if( !pCameraEntity )
	{
		g_Log.Print( "%s - unable to create camera entity", __FUNCTION__ );
		return;
	}


	// set motion for scripted camera
	EntityMotionPathRequest motion_path;
	CScriptCameraKeyFrames cam_keyframes;

	motion_path.MotionPathType = CBEC_MotionPath::MPTYPE_WAIT_INFINITELY_AT_END_POSE;

	float start_time = (float)g_pStage->GetElapsedTime();
	float end_time   = start_time + 1000.0f; // TODO: use infinite loop or wait infinitely at end keyframe
	Matrix34 pose_on_mission_failed = pAircraftBaseEntity->GetWorldPoseOnMissionFailed();
	float default_fov = 3.141592f / 3.0f;
	switch( m_MissionFailedType )
	{
	case MFAILED_CRASHED:
		{
			Vector3& vCrashedPos = pose_on_mission_failed.vPosition;
			Matrix34 cam_pose = Matrix34( vCrashedPos + Vector3( 500, 500, 0 ), Matrix33Identity() );
			motion_path.vecKeyPose.push_back( KeyPose( start_time, cam_pose ) );
			motion_path.vecKeyPose.push_back( KeyPose( end_time,   cam_pose ) );

			cam_keyframes.Camera.FocusTarget.AddKeyFrame( start_time, CameraTargetHolder( vCrashedPos ) );
			cam_keyframes.Camera.FocusTarget.AddKeyFrame( end_time,   CameraTargetHolder( vCrashedPos ) );

			cam_keyframes.Camera.farclip.AddKeyFrame( start_time, 40000.0f );
			cam_keyframes.Camera.farclip.AddKeyFrame( end_time,   40000.0f );

			cam_keyframes.Camera.fov.AddKeyFrame( start_time,         default_fov );
			cam_keyframes.Camera.fov.AddKeyFrame( start_time + 10.0f, default_fov + 0.25f );
			cam_keyframes.Camera.fov.AddKeyFrame( end_time,           default_fov + 0.25f );
		}
		break;

	case MFAILED_KIA:
		{
			CCopyEntity *pPlayerEntity = pAircraftBaseEntity->GetPlayerCopyEntity();

			Vector3& vCrashedPos = pose_on_mission_failed.vPosition;
			Vector3 vCamPos;

			// set target to look at
			if( IsValidEntity(pPlayerEntity) )
			{
				vCamPos = vCrashedPos + Vector3( 0, 40, 0 )
					    + pPlayerEntity->GetWorldPose().matOrient.GetColumn(2) * 80.0f
					    + pPlayerEntity->GetWorldPose().matOrient.GetColumn(0) * RangedRand( -10.0f, 10.0f );

				cam_keyframes.Camera.FocusTarget.AddKeyFrame( start_time, CameraTargetHolder( pPlayerEntity ) );
				cam_keyframes.Camera.FocusTarget.AddKeyFrame( end_time,   CameraTargetHolder( pPlayerEntity ) );
			}
			else
			{
				vCamPos = vCrashedPos + Vector3( 0, 40, 0 );
				cam_keyframes.Camera.FocusTarget.AddKeyFrame( start_time, CameraTargetHolder( vCrashedPos ) );
				cam_keyframes.Camera.FocusTarget.AddKeyFrame( end_time,   CameraTargetHolder( vCrashedPos ) );
			}

			Matrix34 cam_pose = Matrix34( vCamPos, Matrix33Identity() );
			motion_path.vecKeyPose.push_back( KeyPose( start_time, cam_pose ) );
			motion_path.vecKeyPose.push_back( KeyPose( end_time,   cam_pose ) );

			cam_keyframes.Camera.farclip.AddKeyFrame( start_time, 40000.0f );
			cam_keyframes.Camera.farclip.AddKeyFrame( end_time,   40000.0f );

			cam_keyframes.Camera.fov.AddKeyFrame( start_time,         default_fov );
			cam_keyframes.Camera.fov.AddKeyFrame( end_time,           default_fov );
		}
		break;

	case MFAILED_TIMEUP:
		break;
	default:
		break;
	}

	// set motion path (pos & orientation)
	SGameMessage msg( GM_SET_MOTION_PATH );
	msg.pUserData = &motion_path;

	SendGameMessageTo( msg, pCameraEntity );

	// set camera-related effects
	// camera property: fov, nearclip, farclip, etc.
	// effect property: blur, glare, etc.
	msg = SGameMessage( GM_SET_SCRIPTCAMERAKEYFRAMES );
	msg.pUserData = &cam_keyframes;

	SendGameMessageTo( msg, pCameraEntity );

	CameraParam default_cam_param;
	default_cam_param.nearclip = 5.0f;

	// set default params
	msg = SGameMessage( GM_SET_DEFAULT_CAMERA_PARAM );
	msg.pUserData = &default_cam_param;
	SendGameMessageTo( msg, pCameraEntity );

}


int CGameTask_OnMissionFailedFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( m_NextSubMenu != SM_INVALID )
	{
		m_SubMenu = m_NextSubMenu;
		m_NextSubMenu = SM_INVALID;
	}

	m_pDialogManager->Update( dt );

	if( g_pStage )
		g_pStage->Update( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_OnMissionFailedFG::RenderStage()
{
	if( !g_pStage )
		return;

	// render the main stage
	switch( m_MissionFailedType )
	{
	case MFAILED_KIA:
		g_pStage->Render();
		break;
	case MFAILED_TIMEUP:
		g_pStage->Render();
		break;
	case MFAILED_CRASHED:
		g_pStage->Render();
		break;
	}
}


void CGameTask_OnMissionFailedFG::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	RenderStage();

//	C2DRect rect( 0, 0, GetScreenWidth(), GetScreenHeight(), 0xA0000000 );
//	rect.Draw();

	// render ui
	m_pDialogManager->Render();

	// draw cursor
	DrawMouseCursor();
}


void CGameTask_OnMissionFailedFG::CreateRenderTasks()
{
	// create and register render tasks necessary to render the stage
	// by the current camera entity
	if( g_pStage )
		g_pStage->CreateRenderTasks();

	RenderTaskProcessor.AddRenderTask( new CGameTaskRenderTask( this ) );
}


void CGameTask_OnMissionFailedFG::ReleaseGraphicsResources()
{
}


void CGameTask_OnMissionFailedFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	InitMenu();
}
