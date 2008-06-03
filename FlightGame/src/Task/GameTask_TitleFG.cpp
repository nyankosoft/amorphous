
#include "GameTask_TitleFG.h"
#include "../UI/all.h"

#include "Task/GameTask_Stage.h"
#include "Stage/Stage.h"
#include "Stage/PlayerInfo.h"

#include "App/ApplicationBase.h"
#include "App/GameWindowManager_Win32.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Font.h"
#include "3DCommon/GraphicsElementManager.h"

#include "GameCommon/Timer.h"

#include "GameInput/InputHub.h"

#include "Sound/GameSoundManager.h"

#include "Support/memory_helpers.h"
#include "Support/Log/StateLog.h"
#include "Support/Log/DefaultLog.h"

#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"


#include "GameEvent/ScriptArchive.h"


enum id
{
	ID_TT_OK,
	ID_TT_CANCEL,
	ID_TT_START,
	ID_TT_CONTINUE,
	ID_TT_SYSTEM,
	ID_TT_EXIT,
	ID_TT_DLG_EXIT, ///< exit confirmation dialog
	ID_TT_EXIT_YES,
	ID_TT_EXIT_NO,
	NUM_TITLETASK_IDS
};


class TitleEventHandler : public CGM_EventHandlerBase
{
	CGameTask_TitleFG* m_pTask;

public:

	TitleEventHandler( CGameTask_TitleFG *pTask ) : m_pTask(pTask) {}
	virtual ~TitleEventHandler() {}

	void HandleEvent( CGM_Event &event )
	{
		if( !event.pControl )
			return;

		switch( event.pControl->GetID() )
		{
		case ID_TT_OK:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTransitionToNextTask();
			break;

/*		case ID_TT_CANCEL:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->SetRequest( CGameTask_TitleFG::EXIT_REQUESTED );
			break;
*/
		case ID_TT_SYSTEM:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
//				m_pTask->RequestTaskTransition( CGameTask::ID_MAIN_MENU );
				m_pTask->RequestTaskTransition( CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
			break;

		case ID_TT_CONTINUE:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTaskTransition( CGameTaskFG::ID_MAINMENU_FG );
			break;

		case ID_TT_EXIT_YES:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestAppExit();
			break;

		default:
			break;
		}
	}
};


CGameTask_TitleFG::CGameTask_TitleFG()
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CFG_StdControlRendererManager );
	m_pUIRendererManager= pRenderMgr;

    m_pDialogManager	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	CGameTaskFG::DoCommonInit( m_pDialogManager );

	CGM_DialogEventHandlerSharedPtr pEventHandler( new TitleEventHandler(this) );

	CGM_Dialog *pExitDialog =
		FG_CreateYesNoDialogBox(
		m_pDialogManager, false, ID_TT_DLG_EXIT, "Exit Game",
		"", ID_TT_EXIT_YES, ID_TT_EXIT_NO );

	pExitDialog->SetEventHandler( pEventHandler );

//	((CGM_Button *)(pExitDialog->GetControl(ID_TT_EXIT_YES)))->SetButtonEventHandler(  ); 

	CGM_Dialog *pTitleDialog = m_pDialogManager->AddRootDialog( UIID_TTL_DLG_ROOT, RectLTWH( 520, 320, 130, 200 ), "ROOT" );

	int top_margin = 20;
	pTitleDialog->AddButton(          ID_TT_START,    RectLTWH( 10, top_margin,        100, 32 ), "START" );
	pTitleDialog->AddButton(          ID_TT_CONTINUE, RectLTWH( 10, top_margin + 32*1, 100, 32 ), "CONTINUE" );
	pTitleDialog->AddButton(          ID_TT_SYSTEM,   RectLTWH( 10, top_margin + 32*2, 100, 32 ), "SYSTEM" );
	pTitleDialog->AddSubDialogButton( ID_TT_EXIT,     RectLTWH( 10, top_margin + 32*3, 100, 32 ), "EXIT", pExitDialog );

	pTitleDialog->SetEventHandler( pEventHandler );

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_pDialogManager );
	INPUTHUB.PushInputHandler( m_pInputHandler );

	// put some background image
	int bg_layer_index = 89;
	CGraphicsElementManager *pMgr = m_pUIRendererManager->GetGraphicsElementManager();
	CGE_Rect *pBGRect = pMgr->CreateRect( RectLTWH( 0, 0, 800, 600 ), SFloatRGBAColor(1,1,1,1), bg_layer_index );
	pMgr->LoadTexture( CFG_StdControlRendererManager::ID_TEX_BACKGROUNDTESTIMAGE, "Texture/bg_testimage.jpg" );
	pBGRect->SetTexture( CFG_StdControlRendererManager::ID_TEX_BACKGROUNDTESTIMAGE );

	float scale_factor = (float)GetScreenWidth() / 800.0f;
	m_pDialogManager->ChangeScale( scale_factor );
	m_pDialogManager->OpenRootDialog( UIID_TTL_DLG_ROOT );
}


CGameTask_TitleFG::~CGameTask_TitleFG()
{
//	INPUTHUB.PopInputHandler();
//	SafeDelete( m_pInputHandler );
}


int CGameTask_TitleFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( IsAppExitRequested() )
	{
		g_pStage.reset();
		PLAYERINFO.Release();
		PostMessage( GAMEWINDOWMANAGER.GetWindowHandle(), WM_QUIT, 0, 0 );
		return CGameTask::ID_INVALID;
	}

	if( m_pDialogManager.get() )
		m_pDialogManager->Update( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_TitleFG::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render stage select dialog
	m_pDialogManager->Render();
}


void CGameTask_TitleFG::ReleaseGraphicsResources()
{
}


void CGameTask_TitleFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}

