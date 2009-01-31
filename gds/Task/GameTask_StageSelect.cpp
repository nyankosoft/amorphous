#include "GameTask_Stage.hpp"
#include "GameTask_StageSelect.hpp"

#include "Stage/Stage.hpp"
#include "Stage/StageLoader.hpp"
#include "Stage/PlayerInfo.hpp"

#include "Item/GI_Aircraft.hpp"
#include "Item/GI_Ammunition.hpp"
#include "Item/GI_Weapon.hpp"

#include "Input/InputHub.hpp"
#include "UI.hpp"
#include "UI/InputHandler_Dialog.hpp"
#include "UI/GM_StdControlRendererManager.hpp"
#include "Graphics/Font/Font.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/GraphicsElementManager.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "App/ApplicationBase.hpp"


using namespace std;


static int gs_iState = CGameTask_StageSelect::STAGE_NOT_LOADED;


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
		gs_iState = CGameTask_StageSelect::STAGE_LOADED;
		LOG_PRINT_ERROR( "loaded the stage: " + strStageScriptFilename );
	}
	else
	{
		gs_iState = CGameTask_StageSelect::STAGE_NOT_LOADED;
		LOG_PRINT_ERROR( "cannot load the stage: " + strStageScriptFilename );
	}

//	if( gs_iState != CGameTask_StageSelect::STAGE_LOADED )
//		LOG_PRINT_ERROR( "unable to load stage" + strStageScriptFilename

	// do not start the stage timer until the stage task is initiated
	g_pStage->PauseTimer();

//	LOG_PRINT( "stage loaded" );
}


void StageSelect_UIEventHandler( CGM_Event& event )
{
	// load stage from file and exit stage select task

	CGM_Button *pButton = (CGM_Button *)event.pControl;

	int i = event.pControl->GetID() - STG_BUTTON_ID_OFFSET;

	LoadStage( pButton->GetText() );
}


CGameTask_StageSelect::CGameTask_StageSelect()
:
m_pInputHandler(NULL),
m_iNumStages(0)
{
//	LOG_PRINT( NULL, "Initializing stage select task..." );

	gs_iState = STAGE_NOT_LOADED;


	// sets a default aircraft for the player
	if( !SinglePlayerInfo().GetAircraft() )
	{
///		SupplyAircraftItemsToPlayer();
		SinglePlayerInfo().SetAircraft( SinglePlayerInfo().GetItemByName<CGI_Aircraft>( "condor" ).get() );
		CGI_Aircraft *pPlayerAircraft = SinglePlayerInfo().GetAircraft();
		if( !pPlayerAircraft )
			return;
		pPlayerAircraft->WeaponSystem().GetWeaponSlot(1).MountWeapon( SinglePlayerInfo().GetItemByName<CGI_Weapon>("SML").get() );
	}

//	SetPlayerAircraft();

	FILE* fp = fopen("Stage\\StageFileSelector.txt", "r");
	if(!fp)
	{
		LOG_PRINT( "Stage list not found" );
		return;
	}
	int iNumStageFiles = 0;
	char acFilename[1024];
	while( fgets(acFilename, 1023, fp) && iNumStageFiles < NUM_MAX_STAGES )
	{
		if ( acFilename[strlen(acFilename) - 1] == '\n' )
			acFilename[strlen(acFilename) - 1] = '\0';

//		m_vecStageFile.push_back( acFilename );
		m_strStageFile[iNumStageFiles] = acFilename;

		iNumStageFiles++;
	}
	fclose(fp);

	m_iNumStages = iNumStageFiles;


	// create dialog menu for selecting stages
	InitMenu();


	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_pDialogManager );
	InputHub().PushInputHandler( m_pInputHandler );


//	MessageBox( NULL, "load stage", "message", MB_OK );

	// for now, just load the stage 0 and exit the task
	LoadStage( m_strStageFile[0] );
}


void CGameTask_StageSelect::InitMenu()
{
//	m_pUIRenderManager = new CGM_StdControlRendererManager;
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CGM_StdControlRendererManager );
	m_pUIRenderManager= pRenderMgr;

	m_pDialogManager = CGM_DialogManagerSharedPtr( new CGM_DialogManager( m_pUIRenderManager ) );

	CGM_DialogDesc dlg_desc;
	dlg_desc.Rect.SetPositionLTWH( 80 - 12, 300 - 8, 96 + 12 * 2, 32 * m_iNumStages + 8 );
	dlg_desc.bRootDialog = true;
	dlg_desc.pEventHandlerFn = StageSelect_UIEventHandler;

	CGM_Dialog *pDlg = m_pDialogManager->AddDialog( dlg_desc );

	// set font
//	FontDesc font_desc;
//	font_desc.strFontName = "ÇlÇr ÉSÉVÉbÉN";
//	font_desc.width  = 12;
//	font_desc.height = 24;
//	m_pUIRenderManager->AddFont( 0, font_desc );
	m_pUIRenderManager->GetGraphicsElementManager()->LoadFont( 0, "ÇlÇr ÉSÉVÉbÉN", CFontBase::FONTTYPE_NORMAL, 12, 24 );

	CGM_ButtonDesc btn_desc;
	int i;
	for( i=0; i<m_iNumStages; i++ )
	{
		btn_desc.Rect.SetPositionLTWH( 80, 300 + i * 32, 96, 32 );
		btn_desc.strText = m_strStageFile[i];
		btn_desc.ID = STG_BUTTON_ID_OFFSET + i;
		pDlg->AddControl( &btn_desc );
	}

	// set scale for the current resolution
//	float scale_factor = (float)GetScreenWidth() / 800.0f;
//	m_pDialogManager->ChangeScale( scale_factor );
}


CGameTask_StageSelect::~CGameTask_StageSelect()
{
	InputHub().PopInputHandler();
	SafeDelete( m_pInputHandler );
}


int CGameTask_StageSelect::FrameMove( float dt )
{
	if( gs_iState == STAGE_LOADED )
		return CGameTask::ID_STAGE;
	else
		return CGameTask::ID_INVALID;
}


void CGameTask_StageSelect::Render()
{
	// render stage select dialog
	m_pDialogManager->Render();

	// draw cursor
	int x,y;
	GetCurrentMousePosition( x, y );
	C2DRect rect( x-2, y-2, x+5, y+5 );
	rect.SetColor( 0xFFFF1010 );
	rect.Draw();
}


void CGameTask_StageSelect::ReleaseGraphicsResources()
{
}


void CGameTask_StageSelect::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	InitMenu();
}
