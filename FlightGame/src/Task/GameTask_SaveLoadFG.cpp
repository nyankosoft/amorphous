
#include "GameTask_SaveLoadFG.h"
#include "EventHandler_MainMenuFG.h"

/*
#include "Stage/Stage.h"
#include "Stage/CopyEntity.h"
#include "Stage/BE_GeneralEntity.h"
#include "Stage/ScreenEffectManager.h"
*/

#include "Stage/PlayerInfo.h"
#include "App/ApplicationBase.h"

#include "GameInput/InputHub.h"
#include "GameCommon/SaveDataManager.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/Font.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/3DGameMath.h"
#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/fnop.h"

#include "../FlightGameGlobalParams.h"
#include "../UI/all.h"


class CInputHandler_SaveFG : public CInputHandler_Dialog
{
//	CInputHandler_Dialog *m_pUIInputHandler;

	CGameTask_SaveLoadFG* m_pTask;

public:

	CInputHandler_SaveFG( CGameTask_SaveLoadFG* pTask, CGM_DialogManager *pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pTask(pTask) {}

	~CInputHandler_SaveFG() {}

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
//				m_pTask->OnEnterPressed();
			}
//			else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;
		}
	}
};


CGameTask_SaveLoadFG::CGameTask_SaveLoadFG()
{
	// initialized in save / load task
//	m_SubMenu = SM_INVALID;
//	m_NextSubMenu = SM_INVALID;


	int i;
	for( i=0; i<1; i++ )
	{
//		m_apItemButton[i] = NULL;
		m_pSaveSlotListBox = NULL;
	}

	m_CurrentFocusedSlotIndex = -1;


	SaveDataManager.SetMaxNumSaveDataFiles( 8 );
	SaveDataManager.SetSaveDataDirectory( "./SaveData" );

	SaveDataManager.AddComponent( &PLAYERINFO );

	// create dialog menu for selecting stages
//	InitMenu();

	// load stage for aircraft select task
//	InitStage();

//	if( /* load default data */ )
//	{
//		...
//	}
}


CGameTask_SaveLoadFG::~CGameTask_SaveLoadFG()
{
//	INPUTHUB.SetInputHandler( NULL );
//	SafeDelete( m_pInputHandler );

	SaveDataManager.RemoveAllComponents();
}


CGM_Dialog *CGameTask_SaveLoadFG::CreateSaveDataSlotListBox()
{
	SRect dlg_rect = RectAtRightCenter( 720, 900, 25 );
	CGM_Dialog *pRootDlg = m_apDialogManager[0]->AddRootDialog( UIID_SL_DLG_ROOT, dlg_rect, "" );

	const int item_text_height = 56;
	CGM_ListBoxDesc listbox;
	listbox.Rect.SetPositionLTWH( 0, 0, dlg_rect.GetWidth(), dlg_rect.GetHeight() );
	listbox.Rect.Inflate( -5, -5 );
	listbox.Rect.top += item_text_height;
//	listbox.TextRect = SRect(0,0,300,450);
//	listbox.nSBWidth = 1;
//	listbox.nBorder = 1;
//	listbox.nMargin = 1;
	listbox.nTextHeight = item_text_height;
	listbox.Style = 0;

	m_pSaveSlotListBox = (CGM_ListBox *)pRootDlg->AddControl( &listbox );

	return pRootDlg;
}


void CGameTask_SaveLoadFG::InitFonts()
{
	// add font to UI render manager
	CGameTaskFG::LoadFonts( m_pUIRendererManager );
}

/*
void CGameTask_SaveLoadFG::InitMenu()
{
	m_pUIRendererManager = new CUIRender_SaveLoadFG;

	InitFonts();

	// set scale for the current resolution
	int i;
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	for( i=0; i<NUM_SUB_MENUS; i++ )
	{
		m_apDialogManager[i]->ChangeScale( scale_factor );
	}
}*/


void CGameTask_SaveLoadFG::Render()
{
	// do the render routine of the base class
//	CGameTask::Render(); done in CGameTask::RenderBase()!!!

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render the stage (aircraft, missile, etc.)
/*	if( m_pStage )
	{
		CScreenEffectManager* pEffectMgr = m_pStage->GetScreenEffectManager();
		if( pEffectMgr )
			pEffectMgr->SetEffectFlag( 0 );

		m_pStage->Render( *g_pCamera );
	}
*/
	// render stage select dialog
	m_apDialogManager[0]->Render();

	RenderTaskMenu();
}


void CGameTask_SaveLoadFG::ReleaseGraphicsResources()
{
}


void CGameTask_SaveLoadFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


bool CGameTask_SaveLoadFG::UpdatePreviewEntry( int index )
{
	SaveDataPreviewEntryFG& entry =  m_vecPreviewEntryTable[index];

	string filename = SaveDataManager.GetSaveDataFilename(index);
	
	if( fnop::file_exists(filename) )
	{
		// open the db and load data necessary to show the preview info
		CBinaryDatabase<std::string> db;
		db.Open( filename );

		db.GetData( "(string)playtime[HH:MM]", entry.m_TotalPlayTimeHHMM );
		db.GetData( "(int)money", entry.m_OwnedMoney );
		db.Close();

		entry.m_Title
			= fmt_string( "%d - $: %d / T: %s",
			index, entry.m_OwnedMoney, entry.m_TotalPlayTimeHHMM.c_str() );
	}
	else
	{
		entry.m_Title = fmt_string( "(empty)" );
	}

	// update the title of the list box item
	CGM_ListBoxItem *pItem = m_pSaveSlotListBox->GetItem(index);
	if( pItem )
		pItem->SetText( entry.m_Title );

	return true;
}


void CGameTask_SaveLoadFG::CreateSaveDataPreviewInfoTable()
{
	int i, num_savedatafiles = SaveDataManager.GetNumSaveDataFiles();
	int max_num_savedatafiles = SaveDataManager.GetMaxNumSaveDataFiles();

	m_vecPreviewEntryTable.resize( max_num_savedatafiles );

//	string filename;
//	string title, playtime;
//	int money = 0;

	for( i=0; i<max_num_savedatafiles; i++ )
	{
		UpdatePreviewEntry( i ); 

		m_pSaveSlotListBox->AddItem( m_vecPreviewEntryTable[i].m_Title, NULL, i );
	}

}


void CGameTask_SaveLoadFG::SetNextSubMenu( int next_submenu_id )
{
	m_NextSubMenu = next_submenu_id;
}


void CGameTask_SaveLoadFG::SetSaveDataSlotFocus( int index )
{
	m_CurrentFocusedSlotIndex = index;
}




/*
void CGameTask_SaveLoadFG::CreateSaveDataPreviewTable()
{
	int i, num = SaveDataManager.GetNumSaveDataFiles();
	for( i=0; i<num; i++ )
	{
		filename = [i];
		CBinaryDatabase<std::string> db( filename );

		db.GetData( "playtime", playtime );
		db.GetData( "money", money );
		db.Close();
	}
}*/


/*
void CGameTask_SaveLoadFG::InitStage()
{
	SafeDelete( m_pStage );

	m_pStage = new CStage;

//	m_pStage->Initialize( "Script/tsk_br.bin" );
	m_pStage->Initialize( gs_BriefingStageScript );

	// get entities - item display entity & scripted camera entity
	m_pWorldMapDisplay	= m_pStage->GetEntitySet()->GetEntityByName( "ao_display" );
	m_pCamera	= m_pStage->GetEntitySet()->GetEntityByName( "cam" );

	m_pStage->GetEntitySet()->SetCameraEntity( m_pCamera );
}
*/