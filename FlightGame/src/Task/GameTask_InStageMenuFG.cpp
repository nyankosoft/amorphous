
#include "GameTask_InStageMenuFG.h"
#include "Task/GameTask_Stage.h"
#include "../UI/all.h"

#include "Stage/Stage.h"
//#include "Stage/CopyEntity.h"
//#include "Stage/BE_GeneralEntity.h"

#include "GameInput/InputHub.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/font.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/RenderTaskProcessor.h"
#include "Support/memory_helpers.h"

#include "App/ApplicationBase.h"
#include "App/GameWindowManager_Win32.h"

#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"

#include <direct.h>
#include <assert.h>


void CEventHandler_InStageMenuFG::HandleEvent( CGM_Event &event )
{
	if( !event.pControl )
		return;

	if( event.Type == CGM_Event::DIALOG_CLOSED
	 && event.pControl->GetID() == ID_ISM_DLG_ROOT
	 && m_pTask->GetRequestedNextTaskID() == CGameTask::ID_INVALID )
	{
		// immediately return to the stage task (no fade in / fade out time)
		m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK, 0, 0 ,0 );
	}

	switch( event.pControl->GetID() )
	{
	case ID_ISM_RETRY_YES:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			// TODO: restore player's state and re-start the stage
		}
		break;

	case ID_ISM_QUIT_YES:
		// TODO: restore player's state and leave the stage

		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			g_pStage.reset();

			m_pTask->RequestTaskTransition( CGameTaskFG::ID_MAINMENU_FG );

/*			//========== destroy stage object before ending application ==========
			SafeDelete( g_pStage );
			//====================================================================
*/		}
		break;

	case ID_ISM_RETURN_TO_STAGE:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
			m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK, 0, 0, 0 );
		break;

	case ID_ISM_OPEN_SYSTEM_MENU:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
			m_pTask->RequestTaskTransition( CGameTask::ID_MAIN_MENU );
		break;

	case ID_ISM_CUSTOMIZE_CONTROLS:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
//			m_pTask->RequestTaskTransition( CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
			m_pTask->RequestTaskTransition( CGameTaskFG::ID_CONTROLCUSTOMIZER_FG, 0, 0, 0 );	// no fade in / fade out effect
		break;

	default:
		break;
	}
}


CGM_Dialog *CGameTask_InStageMenuFG::CreateRootMenu()
{
	// creates a root dialog that stores aircraft select sub-dialog
	// & controls for aircraft caps display
	CGM_Dialog *pRootDlg = m_pDialogManager->AddRootDialog( ID_ISM_DLG_ROOT, RectLTWH(20,20,120,200), "" );

	CGM_Dialog *apDialog[2];

	// confirmation dialog box
	apDialog[0] = FG_CreateYesNoDialogBox( m_pDialogManager, false,
		0, "RETRY", "", ID_ISM_RETRY_YES, ID_ISM_RETRY_NO );

	apDialog[1] = FG_CreateYesNoDialogBox( m_pDialogManager, false,
		0, "QUIT", "", ID_ISM_QUIT_YES, ID_ISM_QUIT_NO );

/*		= Create2ChoiceDialog( m_pDialogManager, false,
		"RETRY", SRect( 400-80, 300-60, 400+80, 300+30 ),
		ID_ISM_RETRY_YES,	"YES",	RectLTWH( 15, 60, 60, 25 ),
		ID_ISM_RETRY_NO,	"NO",	RectLTWH( 85, 60, 60, 25 ) );

	apDialog[1]
		= Create2ChoiceDialog( m_pDialogManager, false,
		"QUIT", SRect( 400-80, 300-60, 400+80, 300+30 ),
		ID_ISM_QUIT_YES,"YES",	RectLTWH( 15, 60, 60, 25 ),
		ID_ISM_QUIT_NO,	"NO",	RectLTWH( 85, 60, 60, 25 ) );
*/
	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_InStageMenuFG(this) );
	apDialog[0]->SetEventHandler( pEventHandler );
	apDialog[1]->SetEventHandler( pEventHandler );

	pRootDlg->AddButton( ID_ISM_RETURN_TO_STAGE,	RectLTWH( 10, 10 + 40*0, 100, 32 ), "RETURN" );
	pRootDlg->AddButton( ID_ISM_OPEN_SYSTEM_MENU,	RectLTWH( 10, 10 + 40*1, 100, 32 ), "SYSTEM" );
	pRootDlg->AddButton( ID_ISM_CUSTOMIZE_CONTROLS,	RectLTWH( 10, 10 + 40*2, 100, 32 ), "CONTROLS" );
	pRootDlg->AddSubDialogButton(                0, RectLTWH( 10, 10 + 40*3, 100, 32 ), "RETRY", apDialog[0] );
	pRootDlg->AddSubDialogButton(                0, RectLTWH( 10, 10 + 40*4, 100, 32 ), "QUIT",  apDialog[1] );

	pRootDlg->SetEventHandler( pEventHandler );
	return pRootDlg;
}

/*
CGM_Dialog *CGameTask_InStageMenuFG::CreateSubDialog0()
{
	// create sub-dialog
	CGM_Dialog *pSubDialog = NULL;

	return pSubDialog;
}
*/

void CGameTask_InStageMenuFG::InitMenu()
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr = CGM_ControlRendererManagerSharedPtr( new CFG_StdControlRendererManager );
	m_pUIRendererManager = pRenderMgr;

	m_pDialogManager = CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	// add fonts to UI renderer manager
	// set sounds for UI events (button pressed, focus shifted, etc.)
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

	m_pDialogManager->OpenRootDialog( ID_ISM_DLG_ROOT );
}


CGameTask_InStageMenuFG::CGameTask_InStageMenuFG()
{
/*	size_t i;
	for( i=0; i<; i++ )
		m_apItemListBox[i] = NULL;
*/

//	m_pStage = NULL;

	// create dialog menu for selecting stages
	InitMenu();

	SetFadeInTimeMS( 0 );	// always launch without fade in effect
}


CGameTask_InStageMenuFG::~CGameTask_InStageMenuFG()
{
//	INPUTHUB.PopInputHandler( NULL );
//	SafeDelete( m_pInputHandler );

//	SafeDelete( m_pStage );
}


int CGameTask_InStageMenuFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	m_pDialogManager->Update( dt );

//	if( m_pStage )
//		m_pStage->Update( dt );

//	m_pUIRendererManager->Update( dt );

	return CGameTask::ID_INVALID;
}

/*
void CGameTask_InStageMenuFG::UpdateCamera( float dt )
{
	switch( m_SubMenu )
	{
	case SM_SELECT:			break;
	case SM_ITEMS_SELECT:	break;
	case SM_CONFIRM:		break;
	case SM_INVALID:
	default:				break;
	}

}*/


void CGameTask_InStageMenuFG::Render()
{
//	if( !m_pCamera )		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	// Begin the scene
	pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render the main stage
	if( g_pStage )
		g_pStage->Render();

	C2DRect rect( 0, 0, GetScreenWidth(), GetScreenHeight(), 0xA0000000 );
	rect.Draw();

	// render ui
	m_pDialogManager->Render();
}


void CGameTask_InStageMenuFG::CreateRenderTasks()
{
	if( g_pStage )
		g_pStage->CreateRenderTasks();

	RenderTaskProcessor.AddRenderTask( new CGameTaskRenderTask( this ) );
}


void CGameTask_InStageMenuFG::ReleaseGraphicsResources()
{
}


void CGameTask_InStageMenuFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	InitMenu();
}

/*
void CGameTask_InStageMenuFG::SetNextSubMenu( int next_submenu_id )
{
	m_NextSubMenu = next_submenu_id;

	//if( m_NextSubMenu == SM_SELECT )
	//{
	//	CGM_SubDialogButton *pButton
	//		= (CGM_SubDialogButton *)m_pDialogManager->GetControl(  );

	//	if( !pButton->IsSubDialogOpen() )
 //           pButton->OpenDialog();
	//}
}*/
