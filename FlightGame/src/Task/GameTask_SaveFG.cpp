
#include "GameTask_SaveFG.h"
#include "EventHandler_MainMenuFG.h"

#include "Stage/PlayerInfo.h"
#include "App/ApplicationBase.h"

#include "GameInput/InputHub.h"
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


void CExtraSaveDataWriter::SaveTo( CBinaryDatabase<std::string>& db )
{
//	CSaveDataComponent::SaveTo( db );

	db.AddData( "(string)playtime[HH:MM]", PLAYERINFO.GetPlayTime().GetCurrentPlayTimeString( CPlayTime::FMT_HHMM ) );

	int money = PLAYERINFO.GetMoneyLeft();
	db.AddData( "(int)money", money );
}


/*
class CInputHandler_SaveFG : public CInputHandler_Dialog
{
	CGameTask_SaveFG* m_pTask;
public:
	CInputHandler_SaveFG( CGameTask_SaveFG* pTask, CGM_DialogManager *pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pTask(pTask) {}
	~CInputHandler_SaveFG() {}
	virtual void ProcessInput(SInputData& input)
	{
		CInputHandler_Dialog::ProcessInput( input );

		switch( input.iGICode )
		{
		case GIC_RIGHT:
			if( input.iType == ITYPE_KEY_PRESSED )
//				m_pTask->OnUnitFocusShifted();
//			else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;

		case GIC_ENTER:
			if( input.iType == ITYPE_KEY_PRESSED )
				m_pTask->OnEnterPressed();
//			else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;
		}
	}
};
*/


class CEventHandler_Save : public CGM_EventHandlerBase
{
	CGameTask_SaveFG *m_pTask;

public:

	CEventHandler_Save( CGameTask_SaveFG *pTask ) : m_pTask(pTask) {}
	virtual ~CEventHandler_Save() {}

	void HandleEvent( CGM_Event &event )
	{
		if( !event.pControl )
			return;

		int control_id = event.pControl->GetID();

		if( event.Type == CGM_Event::DIALOG_CLOSED
		 && control_id == UIID_SL_DLG_ROOT )
//		 && m_pTask->GetCurrentSubMenu() == CGameTask_SaveFG::SM_SELECT_SAVESLOT )
			m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );

//		CGM_EventHandlerBase::HandleEvent( event );
/*
		id = event.pControl->GetID();
		if( ID_UI_SAVE_BUTTON(0) <= id && id < ID_UI_SAVE_BUTTON(8) )
			m_pTask->SaveCurrentState( id - ID_UI_SAVE_BUTTON(0) );
*/
		switch( event.pControl->GetID() )
		{
/*		case ID_UI_SAVE_BUTTON:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTransitionToNextTask();
			break;
*/
		case ID_UI_SAVE_CONFIRM_OVERWRITE_YES:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
			{
				// user selected to overwrite the data - overwrite on the currently selected slot
				m_pTask->SaveCurrentState();
				m_pTask->SetNextSubMenu( CGameTask_SaveFG::SM_SELECT_SAVESLOT );
			}
			break;

		case ID_UI_SAVE_CONFIRM_OVERWRITE_NO:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->SetNextSubMenu( CGameTask_SaveFG::SM_SELECT_SAVESLOT );
			break;

		case ID_UI_SAVE_PREVPAGE:
		case ID_UI_SAVE_NEXTPAGE:
		default:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );
			break;
		}
	}
};


class CSaveDataListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CGameTask_SaveFG* m_pTask;
	int m_ItemType;

public:
	CSaveDataListBoxEventHandler( CGameTask_SaveFG* task, int item_type )
		: m_pTask(task), m_ItemType(item_type) {}

	virtual ~CSaveDataListBoxEventHandler() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item )
	{
        m_pTask->SetSaveDataSlotFocus( item.UserDataID );

		string filename = SaveDataManager.GetSaveDataFilename( item.UserDataID );
		if( fnop::file_exists(filename) )
		{
			// slot is already used - confirm if the user want it to be overwritten
			m_pTask->SetNextSubMenu( CGameTask_SaveFG::SM_CONFIRM_OVERWRITE );
		}
		else
		{
			// empty save slot - just write the data
            m_pTask->SaveCurrentState( item.UserDataID );
		}
	}

	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item ) {}
};


CGameTask_SaveFG::CGameTask_SaveFG()
{
	m_SubMenu = SM_SELECT_SAVESLOT;
	m_NextSubMenu = SM_INVALID;

	// create dialog menu for selecting stages
	InitMenu();

	SaveDataManager.AddComponent( &m_ExtraSaveDataWriter );

	// load stage for aircraft select task
//	InitStage();

//	if( /* load default data */ )
//	{
//		...
//	}
}


CGameTask_SaveFG::~CGameTask_SaveFG()
{
//	INPUTHUB.SetInputHandler( NULL );
//	SafeDelete( m_pInputHandler );

}


CGM_Dialog *CGameTask_SaveFG::CreateOerwriteConfirmationDialog()
{
	size_t i, num_buttons = 1;//8;
	CGM_ButtonDesc btn_desc;
	CGM_Dialog* pConfirmDlg;
	for( i=0; i<num_buttons; i++ )
	{
		pConfirmDlg =
			FG_CreateYesNoDialogBox( m_apDialogManager[0],
			true, ID_UI_DLG_SAVE_CONFIRM_OVERWRITE, "SAVE", "Overwrite?",
			ID_UI_SAVE_CONFIRM_OVERWRITE_YES, ID_UI_SAVE_CONFIRM_OVERWRITE_NO );
	}

	CFG_StdDialogRenderer *pDlgRenderer = dynamic_cast<CFG_StdDialogRenderer *>(pConfirmDlg->GetRenderer());
	if( pDlgRenderer )
		pDlgRenderer->SetSlideInOutAmount( Vector2(0,0), Vector2(0,0) );


	CGM_DialogEventHandlerSharedPtr pSelectSaveSlotEventHandler( new CEventHandler_Save( this ) );
	pConfirmDlg->SetEventHandler( pSelectSaveSlotEventHandler );

//	return pSaveSlotsDialog;
	return pConfirmDlg;
}


void CGameTask_SaveFG::InitMenu()
{
//	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CUIRender_SaveLoadFG ); // error LNK2001
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CFG_StdControlRendererManager );
	m_pUIRendererManager = pRenderMgr;

	InitFonts();

	// create dialog managers
	m_apDialogManager[0]	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	// create event handlers
//	m_apEventHandler[SM_CONFIRM_OVERWRITE]	= new CEventHandler_ConfirmOverwrite( this );


	CGM_Dialog *apDialog[2];
	apDialog[SM_SELECT_SAVESLOT]   = CreateSaveDataSlotListBox();
	apDialog[SM_CONFIRM_OVERWRITE] = CreateOerwriteConfirmationDialog();

	// set event handlers to dialogs
	CGM_DialogEventHandlerSharedPtr pSelectSaveSlotEventHandler( new CEventHandler_Save( this ) );
	apDialog[SM_SELECT_SAVESLOT]->SetEventHandler( pSelectSaveSlotEventHandler );
	apDialog[SM_CONFIRM_OVERWRITE]->SetEventHandler( pSelectSaveSlotEventHandler );

	CGM_ListBoxEventHandlerSharedPtr pListBoxEventHandler( new CSaveDataListBoxEventHandler( this, 0 ) );
	m_pSaveSlotListBox->SetEventHandler( pListBoxEventHandler );

	// set input handler for dialog menu
//	m_pInputHandler = new CInputHandler_SaveFG( this, m_apDialogManager[0] );
	m_pInputHandler = new CInputHandler_Dialog( m_apDialogManager[0] );
	INPUTHUB.SetInputHandler( m_pInputHandler );

	// set scale for the current resolution
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	m_apDialogManager[0]->ChangeScale( scale_factor );

	// register preview entries to list box
	CreateSaveDataPreviewInfoTable();

	// open the root dialog
	m_apDialogManager[0]->OpenRootDialog( UIID_SL_DLG_ROOT );
}


int CGameTask_SaveFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( m_NextSubMenu != SM_INVALID )
	{
		m_SubMenu = m_NextSubMenu;
		m_NextSubMenu = SM_INVALID;

		if( m_SubMenu == SM_CONFIRM_OVERWRITE )
			m_apDialogManager[0]->OpenRootDialog( ID_UI_DLG_SAVE_CONFIRM_OVERWRITE );
	}

//	if( m_pStage )
//		m_pStage->Update( dt );

	if( m_apDialogManager[0].get() )
		m_apDialogManager[0]->Update( dt );

//	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_SaveFG::RenderTaskMenu()
{
	// always render save slots
	m_apDialogManager[0]->Render();
}


void CGameTask_SaveFG::ReleaseGraphicsResources()
{
}


void CGameTask_SaveFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


bool CGameTask_SaveFG::SaveCurrentState( int index )
{
	if( index < 0 )
		index = m_CurrentFocusedSlotIndex;

	if( index < 0 )
		return false;

	SaveDataManager.SaveCurrentStateToFile( index );

	UpdatePreviewEntry( index );

	return true;
}


/*
void CGameTask_SaveFG::CreateSaveDataPreviewTable()
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
