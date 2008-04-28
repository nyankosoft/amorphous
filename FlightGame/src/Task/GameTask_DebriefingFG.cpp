
#include "GameTask_DebriefingFG.h"
#include "EventHandler_MainMenuFG.h"
#include "../UI/all.h"


#include "Task/GameTask_Stage.h"

#include "Stage/Stage.h"
#include "Stage/StageLoader.h"
#include "Stage/BaseEntityManager.h"
#include "Stage/PlayerInfo.h"
#include "Stage/CopyEntity.h"
#include "Stage/BE_GeneralEntity.h"
#include "Stage/BE_Player.h"
#include "Stage/ScreenEffectManager.h"

#include "GameInput/InputHub.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/Font.h"
#include "3DCommon/TextureFont.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"
#include "Support/memory_helpers.h"

#include "App/ApplicationBase.h"

#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"

#include <direct.h>
#include <assert.h>



class CKillReport
{
public:
	string name;
	string invidual_name;
	float score;
};

class CCombatRecord
{
public:
	int m_NumMisslesFired;
	int m_NumMisslesHit;
	int m_NumBulletsFired;
	int m_NumBulletsHit;
	vector<CKillReport> m_vecKillReport;
};


class CEventHandler_Debriefing : public CGM_EventHandlerBase
{
	CGameTask_DebriefingFG *m_pTask;

public:

	CEventHandler_Debriefing( CGameTask_DebriefingFG *pTask ) : m_pTask(pTask) {}
	virtual ~CEventHandler_Debriefing() {}

	void HandleEvent( CGM_Event &event )
	{
//		CGM_EventHandlerBase::HandleEvent( event );

		if( !event.pControl )
			return;

		if( event.Type == CGM_Event::DIALOG_CLOSED )
		{
//			m_pTask->SetRequest( CGameTask_DebriefingFG::NEXT_TASK_REQUESTED );
			m_pTask->RequestTransitionToNextTask();
			return;
		}

		switch( event.pControl->GetID() )
		{
	/*	case ID_ISM_RETRY_YES:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
			{
				// TODO: restore player's state and re-start the stage
			}
			break;

		case ID_ISM_RETURN_TO_STAGE:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->SetRequest( CGameTask_InStageMenuFG::PREV_TASK_REQUESTED );
			break;

		case ID_ISM_OPEN_SYSTEM_MENU:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->SetRequest( CGameTask_InStageMenuFG::SYSMENU_TASK_REQUESTED );
			break;*/

		case ID_DB_NEXT:
		default:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
//				m_pTask->SetRequest( CGameTask_DebriefingFG::NEXT_TASK_REQUESTED );
				m_pTask->RequestTransitionToNextTask();
			break;
		}
	}
};




static CCamera *g_pCamera = NULL;


void CGameTask_DebriefingFG::InitStage()
{
/*	SafeDelete( m_pStage );

	m_pStage = new CStage;

	m_pStage->Initialize( "Script/tsk_db.bin" );

	// get entities - item display entity & scripted camera entity
	m_pWorldMapDisplay	= m_pStage->GetEntitySet()->GetEntityByName( "worldmap_display" );
	m_pCamera	= m_pStage->GetEntitySet()->GetEntityByName( "cam" );

	m_pStage->GetEntitySet()->SetCameraEntity( m_pCamera );*/
}


CGM_Dialog *CGameTask_DebriefingFG::CreateRootMenu()
{
	// creates a root dialog that stores aircraft select sub-dialog
	// & controls for aircraft caps display
	CGM_Dialog *pRootDlg = m_apDialogManager[0]->AddRootDialog( 0, RectLTWH(80,120,100,160), "" );
	CGM_Button *pButton0 = pRootDlg->AddButton( 0, SRect(5, 32, 80, 24), "NEXT" );
	CGM_Button *pButton1 = pRootDlg->AddButton( 0, SRect(5, 64, 80, 24), "Back" );
	CGM_Button *pButton2 = pRootDlg->AddButton( 0, SRect(5, 96, 80, 24), "Save" );
	CGM_Button *pButton3 = pRootDlg->AddButton( 0, SRect(5,128, 80, 24), "Title" );


	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_Debriefing(this) );
	pRootDlg->SetEventHandler( pEventHandler );

	return pRootDlg;
}


CGM_Dialog *CGameTask_DebriefingFG::CreateAircraftSelectDialog()
{
/*	// aircraft select submenu
	CGM_Dialog *pAircraftSelectDialog;

	CGM_DialogDesc dlg_desc;
	dlg_desc.Rect = SRect( 10, 10, 50, 200 );
//	dlg_desc.bRootDialog = true;
	dlg_desc.bRootDialog = false;
	pAircraftSelectDialog = m_apDialogManager[SM_AIRCRAFT_SELECT]->AddDialog( dlg_desc );

	size_t i, num_buttons = m_vecpPlayerAircraft.size() < 8 ? m_vecpPlayerAircraft.size() : 8;
	CGM_ButtonDesc btn_desc;
	for( i=0; i<num_buttons; i++ )
	{
		btn_desc.ID = AIRCRAFT_BUTTON_ID_OFFSET + i;
		btn_desc.Rect.SetPositionLTWH( 10, 10 + i * 20, 15, 15 );
		pAircraftSelectDialog->AddControl( &btn_desc );
	}

	return pAircraftSelectDialog;*/
	return NULL;
}


CGM_Dialog *CGameTask_DebriefingFG::CreateAmmoSelectRootMenu()
{
	return NULL;
}


CGM_Dialog *CGameTask_DebriefingFG::CreateAmmoSelectDialog()
{
	return NULL;
}


void CGameTask_DebriefingFG::InitMenu()
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr = CGM_ControlRendererManagerSharedPtr( new CFG_StdControlRendererManager/* CUIRender_DebriefingFG */);
	m_pUIRendererManager = pRenderMgr;

	// add font to UI render manager
	CGameTaskFG::LoadFonts( m_pUIRendererManager );

	m_apDialogManager[0]	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	CGM_Dialog *apDialog[3];

	// confirmation dialog box
	apDialog[0]
		= Create2ChoiceDialog( m_apDialogManager[0], true,
		"confirm", SRect( 400-80, 300-60, 400+80, 300+30 ),
		ID_DB_NEXT,		"yes",	RectLTWH( 15, 60, 60, 25 ),
		ID_DB_NEXT,		"no",	RectLTWH( 85, 60, 60, 25 ) );
//		0, "You're gonna go up with this one?", SRect( 400-70, 300-40, 400+70, 300-15 ) );


	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_Debriefing(this) );
	apDialog[0]->SetEventHandler( pEventHandler );

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_apDialogManager[0] );
	INPUTHUB.PushInputHandler( m_pInputHandler );

	// set scale for the current resolution
	int i;
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	for( i=0; i<1; i++ )
	{
		m_apDialogManager[i]->ChangeScale( scale_factor );
	}
}


CGameTask_DebriefingFG::CGameTask_DebriefingFG()
:
m_pCamera(NULL)
{
	m_SubMenu = 0;
//	m_NextSubMenu = SM_INVALID;

	m_pFont = new CTextureFont( "Texture\\HGGE_16x8_256.dds", 16, 32 );

/*	size_t i;
	for( i=0; i<NUM_AMMO_TYPES; i++ )
	{
		m_apItemButton[i] = NULL;
		m_apItemListBox[i] = NULL;
	}
*/

	// create dialog menu for selecting stages
	InitMenu();

	// load stage for aircraft select task
	InitStage();

	GetCombatResults();

	m_CamPosition.current = Vector3(0,0,0);
	m_CamPosition.target = Vector3(0,0,0);
	m_CamPosition.vel = Vector3(0,0,0);
	m_CamPosition.smooth_time = 0.20f;

	m_CamOrient.current = Matrix33Identity();
	m_CamOrient.target = Matrix33Identity();
	m_CamOrient.vel = Matrix33Identity();
	m_CamOrient.smooth_time = 0.25f;


//	OnAmmoTypeFocusChanged( AMMO_BULLET );
}


CGameTask_DebriefingFG::~CGameTask_DebriefingFG()
{
//	INPUTHUB.PopInputHandler();
//	SafeDelete( m_pInputHandler );

	SafeDelete( m_pFont );

	m_pStage.reset();

	SafeDelete( g_pCamera );
}


void CGameTask_DebriefingFG::RequestTransitionToNextTask()
{
	RequestTaskTransition( ID_MAINMENU_FG );
}


int CGameTask_DebriefingFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( m_pStage.get() )
		m_pStage->Update( dt );

	// render UI graphics
	if( m_apDialogManager[0].get() )
		m_apDialogManager[0]->Update( dt );

	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


static vector<string> gs_TextBuffer;

void CGameTask_DebriefingFG::GetCombatResults()
{
	if( !g_pStage )
		return;

	CCopyEntity *pPlayerEntity = g_pStage->GetEntitySet()->GetEntityByName( "player" );

	if( !pPlayerEntity )
		return;

	CBE_Player *pBaseEntity = (CBE_Player *)(pPlayerEntity->pBaseEntity);

	CombatRecord& rec = pBaseEntity->CombatRecord();

	size_t i, num_kills = rec.m_vecKillReport.size();
	int total_score = 0;
	for( i=0; i<num_kills; i++ )
	{
		KillReport& rep = rec.m_vecKillReport[i];
		total_score += rep.score;
	}

	gs_TextBuffer.resize( 8 );

	gs_TextBuffer[0] = fmt_string( "enemy destroyed: %02d", num_kills );
	gs_TextBuffer[1] = fmt_string( "  ground targets destroyed:	--" );
	gs_TextBuffer[2] = fmt_string( "  air targets destroyed:	--" );
	gs_TextBuffer[3] = fmt_string( "total score:	%05d", total_score );
//	gs_TextBuffer[4] = fmt_string( "  hit ratio:	--" );
//	gs_TextBuffer[5] = fmt_string( "bullets fired:	--" );
//	gs_TextBuffer[6] = fmt_string( "  hit ratio:	--" );
}


void CGameTask_DebriefingFG::UpdateCamera( float dt )
{
	if( !g_pCamera )
	{
		g_pCamera = new CCamera;
		g_pCamera->SetPosition( Vector3(0.0f, 4.5f, 0.0f) );
	}

	static float s_Time = 0;
	s_Time += dt;
	Vector3 vLookAtPos = Vector3(0,3,0);
	float dist = 20.0f;
/*	switch( m_SubMenu )
	{
	case SM_AIRCRAFT_SELECT:
		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
		dist = 18.0f;
		break;

	case SM_INVALID:
	default:
		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
		dist = 24.0f;
		break;
	}
*/
	Vector3 vDir = Matrix33RotationY( s_Time * 0.3f ) * Vector3(0,0,1);
	m_CamPosition.target = vLookAtPos + vDir * dist;
	m_CamOrient.target = Matrix33( Vec3Cross( Vector3(0,1,0), (-vDir) ), Vector3(0,1,0), -vDir );

	g_pCamera->SetPosition( m_CamPosition.current );
	g_pCamera->SetOrientation( m_CamOrient.current );

	m_CamPosition.Update( dt );
	m_CamOrient.Update( dt );

}


void CGameTask_DebriefingFG::Render()
{
	// do the render routine of the base class
	CGameTask::Render();

	if( !g_pCamera )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	// Begin the scene
	pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	if( m_pStage.get() )
	{
		// render the stage (aircraft, missile, etc.)

		CScreenEffectManager* pEffectMgr = m_pStage->GetScreenEffectManager();
		if( pEffectMgr )
		{
/*			pEffectMgr->RaiseEffectFlag( ScreenEffect::PostProcessEffects );
			pEffectMgr->RaiseEffectFlag( ScreenEffect::GLARE );
			pEffectMgr->SetGlareLuminanceThreshold( 0.5f );
*/
		}

		m_pStage->Render( *g_pCamera );
	}

	// render stage select dialog
	m_apDialogManager[m_SubMenu]->Render();

	CFontBase *pFont = m_pFont;
	pFont->SetFontColor( 0xFFF0F0F0 );
	int i, num_rows = (int)gs_TextBuffer.size();
	for( i=0; i<num_rows; i++ )
		pFont->DrawText( gs_TextBuffer[i], 30, 64 + 32 * i );

	// draw cursor
	DrawMouseCursor();

	// render fade-out effect if the task is starting / terminating
	RenderFadeEffect();

	// end the scene
    pd3dDevice->EndScene();

    // present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


void CGameTask_DebriefingFG::ReleaseGraphicsResources()
{
}


void CGameTask_DebriefingFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


void CGameTask_DebriefingFG::SetNextSubMenu( int next_submenu_id )
{
	m_NextSubMenu = next_submenu_id;
}
