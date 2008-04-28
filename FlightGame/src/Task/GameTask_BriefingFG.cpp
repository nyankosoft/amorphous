
#include "GameTask_BriefingFG.h"
#include "EventHandler_MainMenuFG.h"
#include "GameCommon/AircraftCapsDisplay.h"

#include "Stage/Stage.h"
#include "Stage/StageLoader.h"
#include "Stage/EntitySet.h"
#include "Stage/StaticGeometryBase.h"
#include "Stage/StaticGeometryFG.h"
#include "Stage/BaseEntityManager.h"
#include "Stage/PlayerInfo.h"
#include "Stage/CopyEntity.h"
#include "Stage/BE_GeneralEntity.h"
#include "Stage/ScreenEffectManager.h"

#include "GameInput/InputHub.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/Font.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/3DGameMath.h"
#include "Support/memory_helpers.h"

#include "App/ApplicationBase.h"

#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"

#include "../FlightGameGlobalParams.h"
#include "../UI/all.h"
#include "../EntityGroups.h"

#include <assert.h>



static string gs_BriefingStageScript;


void SetBriefingStageScriptFilename( const string& filename )
{
	gs_BriefingStageScript = filename;
}


inline static const string& GetBriefingStageScriptFilename()
{
	return gs_BriefingStageScript;
}


class CInputHandler_BriefingFG : public CInputHandler_Dialog
{
//	CInputHandler_Dialog *m_pUIInputHandler;

	CGameTask_BriefingFG* m_pTask;

public:

	CInputHandler_BriefingFG( CGameTask_BriefingFG* pTask, CGM_DialogManager *pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pTask(pTask) {}

	~CInputHandler_BriefingFG() {}

	virtual void ProcessInput(SInputData& input)
	{
		CInputHandler_Dialog::ProcessInput( input );

		switch( input.iGICode )
		{
		case GIC_RIGHT:
			if( input.iType == ITYPE_KEY_PRESSED )
			{
//				m_pTask->OnUnitFocusShifted();
			}
//			else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;

		case GIC_ENTER:
			if( input.iType == ITYPE_KEY_PRESSED )
			{
				m_pTask->OnEnterPressed();
			}
//			else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;
		}
	}
};



class CEventHandler_Briefing : public CGM_EventHandlerBase
{
	CGameTask_BriefingFG *m_pTask;

public:

	CEventHandler_Briefing( CGameTask_BriefingFG *pTask ) : m_pTask(pTask) {}
	virtual ~CEventHandler_Briefing() {}

	void HandleEvent( CGM_Event &event )
	{
//		CGM_EventHandlerBase::HandleEvent( event );

		if( !event.pControl )
			return;

		switch( event.pControl->GetID() )
		{
/*		case ID_ISM_OPEN_SYSTEM_MENU:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->SetRequest( CGameTask_InStageMenuFG::SYSMENU_TASK_REQUESTED );
			break;*/

		case ID_BR_NEXT:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTransitionToNextTask();
			break;

		case ID_BR_BACK:
		default:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );
			break;
		}
	}
};


static CCamera *g_pCamera = NULL;


CGameTask_BriefingFG::CGameTask_BriefingFG()
:
m_pCamera(NULL)
{
	m_CurrentFocusedUnit = 0;

//	m_SubMenu = SM_AIRCRAFT_SELECT;
//	m_NextSubMenu = SM_INVALID;

/*	size_t i;
	for( i=0; i<NUM_AMMO_TYPES; i++ )
	{
		m_apItemButton[i] = NULL;
		m_apItemListBox[i] = NULL;
	}
*/

	if( g_FlightGameParams.SkipBriefing )
	{
		RequestTransitionToNextTask();
		CGameTask::SetFadeInTimeMS(0);
		CGameTask::SetFadeOutTimeMS(0);

		return;
	}

	// create dialog menu for selecting stages
	InitMenu();

	// load stage for aircraft select task
	InitStage();

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


CGameTask_BriefingFG::~CGameTask_BriefingFG()
{
//	INPUTHUB.PopInputHandler();
//	SafeDelete( m_pInputHandler );

	m_pStage.reset();

	SafeDelete( g_pCamera );
}


void CGameTask_BriefingFG::InitStage()
{
//	SafeDelete( m_pStage );
//	m_pStage = new CStage;

/*	m_pStage->Initialize( "Script/tsk_br.bin" ); */
//	m_pStage->Initialize( gs_BriefingStageScript );

	m_pStage.reset();

	CStageLoader stage_loader;
	m_pStage = stage_loader.LoadStage( gs_BriefingStageScript );

	// get entities - item display entity & scripted camera entity
	m_pWorldMapDisplay	= m_pStage->GetEntitySet()->GetEntityByName( "ao_display" );
	m_pCamera	= m_pStage->GetEntitySet()->GetEntityByName( "cam" );


	CStaticGeometryBase *pSG = m_pStage->GetStaticGeometry();
	if( !pSG )
	{
		g_Log.Print( "CGameTask_BriefingFG::InitStage() - no static geometry in stage", WL_ERROR );
		return;
	}
	else if( pSG->GetType() != CStaticGeometryBase::TYPE_FLIGHTGAME )
	{
		g_Log.Print( "CGameTask_BriefingFG::InitStage() - static geometry type is not TYPE_FLIGHTGAME", WL_ERROR );
		return;
	}

	CStaticGeometryFG* pMap = (CStaticGeometryFG *)pSG;

	vector<CCopyEntity *> vecpEntity;
	AABB3 aabb = pMap->GetAABB();
	aabb.vMax.y = 30000.0f;
	COverlapTestAABB aabb_test( aabb, &vecpEntity, CE_GROUP_GENERAL );
	m_pStage->GetEntitySet()->GetOverlappingEntities( aabb_test );

	float fMiniatureScale = 0.01f;

	size_t i, num_entities = vecpEntity.size();
	for( i=0; i<num_entities; i++ )
	{
		if( !IsValidEntity( vecpEntity[i] ) )
			continue;
		
		if( vecpEntity[i]->pBaseEntity->GetNameString().find( "IconDisplay_" ) == 0 )
		{
			CCopyEntity* pEntity = vecpEntity[i];
			pEntity->Position() = pEntity->Position() * fMiniatureScale;
			m_vecpIconEntity.push_back( pEntity );
		}
	}

//	pMap->SetRenderTechnique( "SGMiniatureFG" );

	m_pStage->GetEntitySet()->SetCameraEntity( m_pCamera );
}


CGM_Dialog *CGameTask_BriefingFG::CreateRootMenu()
{
	// creates a root dialog that stores aircraft select sub-dialog
	// & controls for aircraft caps display
	CGM_Dialog *pRootDlg = m_apDialogManager[0]->AddRootDialog( 0, RectLTWH(80,120,100,160), "" );
	CGM_Button *pButton0 = pRootDlg->AddButton( 0, SRect(5, 32, 80, 24), "Select Aircraft" );
	CGM_Button *pButton1 = pRootDlg->AddButton( 0, SRect(5, 64, 80, 24), "Back" );
//	CGM_Button *pButton2 = pRootDlg->AddButton( 0, SRect(5, 96, 80, 24), "Save" );
//	CGM_Button *pButton3 = pRootDlg->AddButton( 0, SRect(5,128, 80, 24), "Title" );

	return pRootDlg;
}


CGM_Dialog *CGameTask_BriefingFG::CreateAircraftSelectDialog()
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


void CGameTask_BriefingFG::InitMenu()
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CFG_StdControlRendererManager /* CUIRender_BriefingFG */ );
	m_pUIRendererManager = pRenderMgr;

	// add font to UI render manager
	CGameTaskFG::LoadFonts( m_pUIRendererManager );

	m_apDialogManager[0]	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

/*	m_apDialogManager[SM_ITEMS_SELECT]		= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager );
	m_apDialogManager[SM_CONFIRM]			= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager );
*/
	CGM_Dialog *apDialog[1];

	// confirmation dialog box
	apDialog[0]
		= Create2ChoiceDialog( m_apDialogManager[0], true,
		"proceed", SRect( 400-80, 450-60, 400+80, 450+30 ),
		0,	"yes",	RectLTWH( 15, 60, 60, 25 ),
		0,	"no",	RectLTWH( 85, 60, 60, 25 ) );
//		0, "You're gonna go up with this one?", SRect( 400-70, 300-40, 400+70, 300-15 ) );

/*
	// submenu for aircraft select
	apDialog[SM_AIRCRAFT_SELECT] = CreateAircraftSelectRootMenu();

	// submenu for ammunition select
	apDialog[SM_ITEMS_SELECT] = CreateAmmoSelectRootMenu();
*/
	// set input handler for dialog menu
//	m_pInputHandler = new CInputHandler_Debug( this, m_apDialogManager[0] );
	m_pInputHandler = new CInputHandler_BriefingFG( this, m_apDialogManager[0].get() );
	INPUTHUB.PushInputHandler( m_pInputHandler );

	// set scale for the current resolution
	int i;
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	for( i=0; i<1; i++ )
	{
		m_apDialogManager[i]->ChangeScale( scale_factor );
	}
}


void CGameTask_BriefingFG::RequestTransitionToNextTask()
{
	RequestTaskTransition( ID_AIRCRAFT_SELECT );
}


int CGameTask_BriefingFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	// render UI graphics
	if( m_apDialogManager[0].get() )
		m_apDialogManager[0]->Update( dt );

//	if( m_NextSubMenu != SM_INVALID )
//	{
//		m_pInputHandler->SetDialogManager( m_apDialogManager[m_NextSubMenu] );
//		m_SubMenu = m_NextSubMenu;
//		m_NextSubMenu = SM_INVALID;
//	}

	if( m_pStage.get() )
		m_pStage->Update( dt );

//	m_pUIRendererManager->Update( dt );

	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_BriefingFG::UpdateCamera( float dt )
{
	if( !g_pCamera )
	{
		g_pCamera = new CCamera;
		g_pCamera->SetPosition( Vector3(0.0f, 10.0f, 0.0f) );
		g_pCamera->SetOrientation( Matrix33Identity() );
		g_pCamera->SetNearClip( 1.0f );
		g_pCamera->SetFarClip( 50000.0f );
	}

	static float s_Time = 0;
	s_Time += dt;
	Vector3 vLookAtPos = Vector3(0,10,0);
	float dist = 30.0f;

	if( m_CurrentFocusedUnit < (int)m_vecpIconEntity.size() )
	{
		CCopyEntity* pFocusedEntity = m_vecpIconEntity[m_CurrentFocusedUnit];
		if( IsValidEntity(pFocusedEntity) )
			vLookAtPos = pFocusedEntity->Position();
	}

//	switch( m_SubMenu )
//	{
//	case SM_AIRCRAFT_SELECT:
//		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
//		dist = 18.0f;
//		break;
//
//	case SM_INVALID:
//	default:
//		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
//		dist = 24.0f;
//		break;
//	}

	Vector3 vOutDir = Matrix33RotationY( s_Time * 0.2f ) * Vector3(0,0,1);
	Vector3 vCamDir = vOutDir * dist + Vector3( 0, 15, 0 );	// dir to camera
//	m_CamPosition.target = vLookAtPos + vDir * dist + Vector3( 0, 15, 0 );
	m_CamPosition.target = vLookAtPos + vCamDir;
//	m_CamOrient.target = Matrix33( Vec3Cross( Vector3(0,1,0), (-vDir) ), Vector3(0,1,0), -vDir );
	Vec3Normalize( vCamDir, vCamDir );
	m_CamOrient.target = CreateOrientFromFwdDir( -vCamDir );

	g_pCamera->SetPosition( m_CamPosition.current );
	g_pCamera->SetOrientation( m_CamOrient.current );

	m_CamPosition.Update( dt );
	m_CamOrient.Update( dt );

}


void CGameTask_BriefingFG::Render()
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

	// render the stage (aircraft, missile, etc.)
	if( m_pStage )
	{
		CScreenEffectManager* pEffectMgr = m_pStage->GetScreenEffectManager();
		if( pEffectMgr )
		{
			pEffectMgr->SetEffectFlag( 0 );
/*			pEffectMgr->RaiseEffectFlag( ScreenEffect::PostProcessEffects );

			pEffectMgr->RaiseEffectFlag( ScreenEffect::GLARE );
			pEffectMgr->SetGlareLuminanceThreshold( 0.5f );
*/
		}

		m_pStage->Render( *g_pCamera );
	}

	// render stage select dialog
	if( m_apDialogManager[0] )
        m_apDialogManager[0]->Render();

	// draw cursor
	DrawMouseCursor();

	// render fade-out effect if the task is starting / terminating
	RenderFadeEffect();

	// end the scene
    pd3dDevice->EndScene();

    // present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


void CGameTask_BriefingFG::ReleaseGraphicsResources()
{
}


void CGameTask_BriefingFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


void CGameTask_BriefingFG::OnUnitFocusChanged( int index )
{
//	if( m_State == STATE_UNIT_VIEW )
//	{
//	}
}


void CGameTask_BriefingFG::OnAmmunitionSelected( int ammo_type, const string& ammo_name )
{
}


void CGameTask_BriefingFG::OnEnterPressed()
{
/*	switch( m_Stage )
	{
	case STATE_UNIT_VIEW:
		break;
	case STATE_BRIEFING:
		break;
	default:
		return;
	}*/

	RequestTransitionToNextTask();
//	SetRequest( NEXT_TASK_REQUESTED );
}

/*
void CGameTask_BriefingFG::OnAircraftSelected()
{
}*/


void CGameTask_BriefingFG::SetNextSubMenu( int next_submenu_id )
{
	m_NextSubMenu = next_submenu_id;
}
