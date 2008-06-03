
#include "GameTask_MainMenuFG.h"
#include "GameTask_BriefingFG.h"
#include "EventHandler_MainMenuFG.h"
#include "Task/GameTask_Stage.h"
#include "GameCommon/AircraftCapsDisplay.h"

#include "Stage/Stage.h"
#include "Stage/StageLoader.h"
#include "Stage/BaseEntityManager.h"
#include "Stage/PlayerInfo.h"
#include "Stage/CopyEntity.h"
#include "Stage/BE_GeneralEntity.h"
#include "Stage/ScreenEffectManager.h"

#include "Stage/StageGraph.h"

#include "GameInput/InputHub.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/Font.h"
#include "3DCommon/TextureFont.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/2DFrameRect.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/GraphicsElementManager.h"
#include "3DMath/misc.hpp"

#include "App/ApplicationBase.h"

#include "Support.h"

#include "../UI/FG_StdControlRendererManager.h"
#include "../FlightGameGlobalParams.h"
#include "../DefaultKeybindFG.h"

using namespace boost;


class CInputHandler_DebugMainMenuFG : public CInputHandler_Dialog
{
//	CInputHandler_Dialog *m_pUIInputHandler;

	CGameTask_MainMenuFG* m_pTask;

public:
//	CInputHandler_DebugMainMenuFG() m_pUIInputHandler(new CInputHandler_Dialog) : {}
//	~CInputHandler_DebugMainMenuFG() { SafeDelete(m_pUIInputHandler); }

	CInputHandler_DebugMainMenuFG( CGameTask_MainMenuFG* pTask, CGM_DialogManagerSharedPtr pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pTask(pTask) {}

	CInputHandler_DebugMainMenuFG( CGameTask_MainMenuFG* pTask, CGM_DialogManager *pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pTask(pTask) {}
	~CInputHandler_DebugMainMenuFG() {}

	virtual void ProcessInput( SInputData& input )
	{
		CInputHandler_Dialog::ProcessInput( input );

		switch( input.iGICode )
		{
		case GIC_F5:
			if( input.iType == ITYPE_KEY_PRESSED )
			{
//				m_pAircraftSelectTask->InitStage();
//				m_pAircraftSelectTask->OnAircraftChanged( 0 );
			}
			break;
		}
	}
};


//class CGameTask_MainMenuFG;


class CListBoxEventHandler_StageSelect : public CGM_ListBoxEventHandler
{
	CGameTask_MainMenuFG* m_pTask;

public:

	CListBoxEventHandler_StageSelect( CGameTask_MainMenuFG* task )
		: m_pTask(task) {}
	virtual ~CListBoxEventHandler_StageSelect() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item )
	{
		m_pTask->OnStageSelected( item.GetText() );
	}

	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item ) {}
};


CGameTask_MainMenuFG::CGameTask_MainMenuFG()
:
m_pCameraEntity(NULL)
{
//	m_SubMenu = SM_AIRCRAFT_SELECT;
//	m_NextSubMenu = SM_INVALID;

	size_t i;
	for( i=0; i<NUM_LISTBOXES; i++ )
	{
		m_apListBox[i] = NULL;
	}


	// create dialog menu for selecting stages
	InitMenu();

	// load stage for aircraft select task
//	InitStage();

	m_CamPosition.current = Vector3(0,0,0);
	m_CamPosition.target = Vector3(0,0,0);
	m_CamPosition.vel = Vector3(0,0,0);
	m_CamPosition.smooth_time = 0.20f;

	m_CamOrient.current = Matrix33Identity();
	m_CamOrient.target = Matrix33Identity();
	m_CamOrient.vel = Matrix33Identity();
	m_CamOrient.smooth_time = 0.25f;

	// load global params file
	g_FlightGameParams.LoadFromFile( "debug/FlightGameGlobalParams.txt" );

//	OnAircraftChanged( 0 );
//	OnAmmoTypeFocusChanged( AMMO_BULLET );

	StageNodeDesc desc;
	if( 0 < g_FlightGameParams.StageName.length() )
	{
		desc.name = g_FlightGameParams.StageName;
		desc.script_filename = g_FlightGameParams.StageScriptFilename;
	}
	else
	{
		desc.name = "0. forest";
		desc.script_filename = "Script/stg_forest.bin";
//		desc.name = "test - canyon";
//		desc.script_filename = "Script/stg_canyon.bin";
//		desc.name = "0. rocky";
//		desc.script_filename = "Script/stg_rocky.bin";
	}
	StageGraph.AddStageNode( desc );

	// update available stages
	m_vecpStageNode.resize( 0 );
	StageGraph.GetAvailableStages( m_vecpStageNode );

	// TODO: Do not initialize keybind when the game has been loaded from a saved data
	ONCE( SetDefaultKeyBindFG() );
}


CGameTask_MainMenuFG::~CGameTask_MainMenuFG()
{
//	INPUTHUB.PopInputHandler();
//	SafeDelete( m_pInputHandler );

	m_pStage.reset();
}


void CGameTask_MainMenuFG::InitStage()
{
//	SafeDelete( m_pStage );
//	m_pStage = new CStage;
//	m_pStage->Initialize( "Script/tsk_mm.bin" );

	m_pStage.reset();

	CStageLoader stage_loader;
	m_pStage = stage_loader.LoadStage( "Script/tsk_mm.bin" );

	// get entities - item display entity & scripted camera entity
	m_pWorldMapDisplay	= m_pStage->GetEntitySet()->GetEntityByName( "worldmap_display" );
	m_pCameraEntity	= m_pStage->GetEntitySet()->GetEntityByName( "cam" );

	m_pStage->GetEntitySet()->SetCameraEntity( m_pCameraEntity );
}


CGM_Dialog *CGameTask_MainMenuFG::CreateRootMenu()
{
	// creates a root dialog that stores aircraft select sub-dialog
	// & controls for aircraft caps display
	CGM_Dialog *pRootDlg = m_apDialogManager[0]->AddRootDialog( ID_TMM_DLG_ROOT, RectLTWH(80,120,120,(int)(120 * GOLDEN_RATIO)), "" );
	CGM_Button *pButton0 = pRootDlg->AddButton( ID_TMM_NEXT,	RectLTWH(5, 32, 80, 28), "NEXT" );
//	CGM_Button *pButton1 = pRootDlg->AddButton( 0, RectLTWH(5, 64, 80, 24), "Select Stage" );
	CGM_SubDialogButton *pSubDlgButton = pRootDlg->AddSubDialogButton( 0, RectLTWH(5, 64, 80, 28), "STAGES" );
	CGM_Button *pButton2 = pRootDlg->AddButton( ID_TMM_SHOP,	RectLTWH(5, 96, 80, 28), "SHOP" );
	CGM_Button *pButton3 = pRootDlg->AddButton( ID_TMM_SAVE,	RectLTWH(5,128, 80, 28), "SAVE" );
	CGM_Button *pButton4 = pRootDlg->AddButton( ID_TMM_TITLE,	RectLTWH(5,160, 80, 28), "TITLE" );

	CGM_Dialog *pStageSelectDlg = CreateStageSelectMenu();
	pSubDlgButton->SetSubDialog( pStageSelectDlg );

	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_MainMenuFG(this) );
	pRootDlg->SetEventHandler( pEventHandler );

	// open root dialog
	m_apDialogManager[0]->OpenRootDialog( ID_TMM_DLG_ROOT );

	return pRootDlg;
}


CGM_Dialog *CGameTask_MainMenuFG::CreateStageSelectMenu()
{
	// subdialog & listbox for stage select
	SRect sdlg_rect = RectLTWH(100,130,200,324);

	// create a sub-dialog
	CGM_Dialog *pStageSelectDlg = m_apDialogManager[0]->AddDialog( 0, sdlg_rect, "Stages" );

	// create a list box
	const int item_text_height = 28;
	CGM_ListBoxDesc box_desc;
	box_desc.Style = CGM_ListBox::CLOSE_DIALOG_ON_ITEM_SELECTION;
	box_desc.Rect.SetPositionLTWH( 0, 0, sdlg_rect.GetWidth(), sdlg_rect.GetHeight() );
	box_desc.Rect.Inflate( -5, -5 );
	box_desc.Rect.top += item_text_height;
	box_desc.ID = 0;//listbox_id[j];
	box_desc.nTextHeight = item_text_height;
	m_apListBox[0] = (CGM_ListBox *)pStageSelectDlg->AddControl( &box_desc );

	// create list box items
	const int NumStgs = 4;
	char stage_name[NumStgs][32] = {
		"stg_forest.bin",
		"stg_rocky.bin",
		"stg_canyon.bin",
		"stg_arctic.bin"
	};

	size_t i, num_stages = (size_t)NumStgs;
	for( i=0; i<num_stages; i++ )
	{
		m_vecStage.push_back( StageInfo( stage_name[i], stage_name[i] ) );
		m_apListBox[0]->AddItem( stage_name[i], NULL );
	}

	CGM_ListBoxEventHandlerSharedPtr pListBoxEventHandler( new CListBoxEventHandler_StageSelect(this) );
	m_apListBox[0]->SetEventHandler( pListBoxEventHandler );

	return pStageSelectDlg;
}


void CGameTask_MainMenuFG::InitMenu()
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CFG_StdControlRendererManager );
	m_pUIRendererManager= pRenderMgr;

	m_apDialogManager[0]	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	CGameTaskFG::DoCommonInit( m_apDialogManager[0] );

	CGM_Dialog* pRootDialog = CreateRootMenu();

/*
	CGM_Dialog *apDialog[NUM_SUB_MENUS];

	// confirmation dialog box
	apDialog[SM_CONFIRM]
		= Create2ChoiceDialog( m_apDialogManager[SM_CONFIRM], true,
		"confirm", SRect( 400-80, 300-60, 400+80, 300+30 ),
		ID_TMM_CONFIRM_OK,		"yes",	RectLTWH( 15, 60, 60, 25 ),
		ID_TMM_CONFIRM_CANCEL,	"no",	RectLTWH( 85, 60, 60, 25 ) );
//		0, "You're gonna go up with this one?", SRect( 400-70, 300-40, 400+70, 300-15 ) );

*/

//	pDialog->SetEventHandler( m_apEventHandler[SM_AIRCRAFT_SELECT] );

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_DebugMainMenuFG( this, m_apDialogManager[0] );
	INPUTHUB.SetInputHandler( m_pInputHandler );

	// put some background image
	int bg_layer_index = 89;
	CGraphicsElementManager *pMgr = m_pUIRendererManager->GetGraphicsElementManager();
	CGE_Rect *pBGRect = pMgr->CreateRect( RectLTWH( 0, 0, 800, 600 ), SFloatRGBAColor(0.7f,0.7f,0.7f,0.7f), bg_layer_index );
	pMgr->LoadTexture( CFG_StdControlRendererManager::ID_TEX_BACKGROUNDTESTIMAGE, "Texture/bg_testimage.jpg" );
	pBGRect->SetTexture( CFG_StdControlRendererManager::ID_TEX_BACKGROUNDTESTIMAGE );

	// set scale for the current resolution
	float scale_factor = (float)GetScreenWidth() / 800.0f;
//	for( int i=0; i<1; i++ )
//	{
		m_apDialogManager[0]->ChangeScale( scale_factor );
//	}
}


const char *GetNextMissionStageScript() { return "Script/stg_forest.bin"; }


void CGameTask_MainMenuFG::ProcessTaskTransitionRequest()
{
	if( GetRequestedNextTaskID() == ID_BRIEFING_FG )
	{
		if( m_vecpStageNode.size() == 0 )
		{
			// no stage - cannot move on to the briefing task.
			// ignore the request
			RequestTaskTransition( ID_INVALID );
			return;
		}

		string script_filename = m_vecpStageNode[0]->GetStageScriptFilename();	// where should this be stored?
//		string script_filename = GetNextMissionStageScript();	// where should this be stored?
		SetGlobalStageScriptFilename( script_filename );

		string briefing_script = script_filename;
		fnop::append_to_body( briefing_script, "_br" );
		SetBriefingStageScriptFilename( briefing_script );

		// the rest of the job, i.e.
		// - set the m_NextTaskID and go into transition state
		// - pop input handler
		// will be done by CGameTask::ProcessTaskTransitionRequest()
	}
	else if( GetRequestedNextTaskID() == ID_AIRCRAFT_SELECT )
	{
		// player has directly selected a stage which has already been visited
		// - skip the briefing and let player select an aircraft
		// TODO: safe transition to stage task
		// task request may be overwritten after this
//		LoadStage( m_SelectedStageName );
//		SetBriefingStageScriptFilename( m_SelectedStageName );
		SetGlobalStageScriptFilename( m_SelectedStageName );
	}

	CGameTask::ProcessTaskTransitionRequest();
}


int CGameTask_MainMenuFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	// render UI graphics
	if( m_apDialogManager[0].get() )
		m_apDialogManager[0]->Update( dt );

/*
	...
	else if( m_ReqState == STAGE_TASK_REQUESTED )
	{
		// TODO: safe transition to stage task
		// task request may be overwritten after this
//		LoadStage( m_SelectedStageName );
//		SetBriefingStageScriptFilename( m_SelectedStageName );
		SetGlobalStageScriptFilename( m_SelectedStageName );
		StartFadeout( ID_AIRCRAFT_SELECT );
		m_ReqState = NONE_REQUESTED;	// need to reset for fade-out routine to work properly
	}
	...

//	if( m_NextSubMenu != SM_INVALID )
//	{
//		m_pInputHandler->SetDialogManager( m_apDialogManager[m_NextSubMenu] );
//		m_SubMenu = m_NextSubMenu;
//		m_NextSubMenu = SM_INVALID;
//	}
*/
	if( m_pStage )
		m_pStage->Update( dt );

//	m_pUIRendererManager->Update( dt );

	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_MainMenuFG::UpdateCamera( float dt )
{
	if( !m_pCamera )
	{
		m_pCamera = shared_ptr<CCamera>( new CCamera() );
		m_pCamera->SetPosition( Vector3(0.0f, 5.0f, 0.0f) );
	}

	static float s_Time = 0;
	s_Time += dt;
	Vector3 vLookAtPos = Vector3(0,5,0);
	float dist = 10.0f;
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

	m_pCamera->SetPosition( m_CamPosition.current );
	m_pCamera->SetOrientation( m_CamOrient.current );

	m_CamPosition.Update( dt );
	m_CamOrient.Update( dt );

}


void CGameTask_MainMenuFG::Render()
{
	if( !m_pCamera )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	CScreenEffectManager *pEffectMgr;
	if( m_pStage
	 && (pEffectMgr = m_pStage->GetScreenEffectManager()) )
	{
/*		pEffectMgr->RaiseEffectFlag( ScreenEffect::PostProcessEffects );

		pEffectMgr->RaiseEffectFlag( ScreenEffect::GLARE );
		pEffectMgr->SetGlareLuminanceThreshold( 0.5f );
*/
	}

	// render the stage (aircraft, missile, etc.)
	if( m_pStage )
		m_pStage->Render( *m_pCamera.get() );

	// render stage select dialog
	m_apDialogManager[0]->Render();

	// render the structure of stage tree (test)
	RenderStageGraphSkeleton();
}


void CGameTask_MainMenuFG::ReleaseGraphicsResources()
{
}


void CGameTask_MainMenuFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


void CGameTask_MainMenuFG::OnStageSelected( const string& stage_name )
{
	RequestTaskTransition( ID_AIRCRAFT_SELECT );
	m_SelectedStageName = "Script/" + stage_name;
}

/*
void CGameTask_MainMenuFG::SetNextSubMenu( int next_submenu_id )
{
	m_NextSubMenu = next_submenu_id;
}
*/


void CGameTask_MainMenuFG::RenderStageGraphSkeleton_r( StageNode& node, int depth )
{
	// borrow font from ui render mgr
	CFontBase* pFont = m_pUIRendererManager->GetGraphicsElementManager()->GetFont(0);
	if( !pFont )
		return;

	C2DFrameRect frame_rect( 350, 80 + depth*32, 500, 104 + depth*32, 2 );

	if( node.GetState() == StageNode::AVAILABLE )
        frame_rect.SetColor( 0xFFFF1010 );
	else
        frame_rect.SetColor( 0xFFFFFFFF );

	frame_rect.Draw();

	SetRenderStatesForTextureFont( AlphaBlend::InvSrcAlpha );

	D3DXVECTOR2 pos = D3DXVECTOR2( 356, 82 + (float)depth*32 );
	pFont->DrawText( node.m_Name.c_str(), pos, 0xFFFFFFFF );

	int i, num = node.m_NumNextStages;
	for( i=0; i<num; i++ )
	{
		RenderStageGraphSkeleton_r( *(node.m_apNextStage[i]), depth + 1 );
	}
}


void CGameTask_MainMenuFG::RenderStageGraphSkeleton()
{
	StageNode* pRootNode = StageGraph.GetRootNode();

	if( pRootNode )
		RenderStageGraphSkeleton_r( *pRootNode, 0 );
}

