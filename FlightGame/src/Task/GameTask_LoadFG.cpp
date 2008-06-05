
#include "GameTask_LoadFG.h"
#include "EventHandler_MainMenuFG.h"

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

/*
class CInputHandler_SaveFG : public CInputHandler_Dialog
{
//	CInputHandler_Dialog *m_pUIInputHandler;

	CGameTask_LoadFG* m_pTask;

public:

	CInputHandler_SaveFG( CGameTask_LoadFG* pTask, CGM_DialogManager *pDialogManager )
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
				m_pTask->OnEnterPressed();
			}
//			else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;
		}
	}
};
*/


class CEventHandler_Load : public CGM_EventHandlerBase
{
	CGameTask_LoadFG *m_pTask;

public:

	CEventHandler_Load( CGameTask_LoadFG *pTask ) : m_pTask(pTask) {}
	virtual ~CEventHandler_Load() {}

	void HandleEvent( CGM_Event &event )
	{
		if( event.Type == CGM_Event::DIALOG_CLOSED
		 && m_pTask->GetCurrentSubMenu() == CGameTask_LoadFG::SM_SELECT_SAVESLOT )
			m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );

//		CGM_EventHandlerBase::HandleEvent( event );

		if( !event.pControl )
			return;
/*
		id = event.pControl->GetID();
		if( ID_UI_LOAD_BUTTON(0) <= id && id < ID_UI_LOAD_BUTTON(8) )
			m_pTask->SaveCurrentState( id - ID_UI_LOAD_BUTTON(0) );
*/
		switch( event.pControl->GetID() )
		{
/*		case ID_UI_LOAD_BUTTON:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTransitionToNextTask();
			break;
*/
		case ID_UI_LOAD_CONFIRM_YES:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
			{
				m_pTask->LoadSavedState();
				m_pTask->SetNextSubMenu( CGameTask_LoadFG::SM_SELECT_SAVESLOT );
			}
			break;

		case ID_UI_LOAD_CONFIRM_NO:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->SetNextSubMenu( CGameTask_LoadFG::SM_SELECT_SAVESLOT );
			break;

		case ID_UI_LOAD_PREVPAGE:
		case ID_UI_LOAD_NEXTPAGE:
		default:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
				m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );
			break;
		}
	}
};


class CLoadDataListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CGameTask_LoadFG* m_pTask;
	int m_ItemType;

public:
	CLoadDataListBoxEventHandler( CGameTask_LoadFG* task, int item_type )
		: m_pTask(task), m_ItemType(item_type) {}

	virtual ~CLoadDataListBoxEventHandler() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item )
	{
        m_pTask->SetSaveDataSlotFocus( item.UserDataID );

		string filename = SaveDataManager.GetSaveDataFilename( item.UserDataID );
		if( fnop::file_exists(filename) )
		{
			// slot is already used - confirm if the user want it to be overwritten
			m_pTask->SetNextSubMenu( CGameTask_LoadFG::SM_CONFIRM_LOAD );
		}
		else
		{
			// empty save slot - just write the data
            m_pTask->LoadSavedState( item.UserDataID );
		}
	}

	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item ) {}
};


CGameTask_LoadFG::CGameTask_LoadFG()
{
	m_SubMenu = SM_SELECT_SAVESLOT;
	m_NextSubMenu = SM_INVALID;

	// create dialog menu for selecting stages
	InitMenu();

	// load stage for aircraft select task
//	InitStage();

//	if( /* load default data */ )
//	{
//		...
//	}

}


CGameTask_LoadFG::~CGameTask_LoadFG()
{
//	INPUTHUB.SetInputHandler( NULL );
//	SafeDelete( m_pInputHandler );

}

/*
CGM_Dialog *CGameTask_LoadFG::CreateRootMenu()
{
	return NULL;
}*/


CGM_Dialog *CGameTask_LoadFG::CreateLoadConfirmationDialog()
{
/*	CGM_Dialog *pSaveSlotsDialog;

	CGM_DialogDesc dlg_desc;
	dlg_desc.Rect = SRect( 10, 10, 50, 200 );
	dlg_desc.bRootDialog = true;
//	dlg_desc.bRootDialog = false;
	pSaveSlotsDialog = m_apDialogManager[SM_CONFIRM_LOAD]->AddDialog( dlg_desc );
*/
	size_t i, num_buttons = 1;//8;
	CGM_ButtonDesc btn_desc;
	CGM_Dialog* pConfirmDlg;
	for( i=0; i<num_buttons; i++ )
	{
		pConfirmDlg =
			FG_CreateYesNoDialogBox( m_apDialogManager[SM_CONFIRM_LOAD],
			true, 0, "", "Load this data?",
			ID_UI_LOAD_CONFIRM_YES, ID_UI_LOAD_CONFIRM_NO );
/*
			Create2ChoiceDialog( m_apDialogManager[SM_CONFIRM_LOAD], true,
			"load this data?", SRect( 400-80, 450-60, 400+80, 450+30 ),
			ID_UI_LOAD_CONFIRM_YES,	"yes",	RectLTWH( 15, 60, 60, 25 ),
			ID_UI_LOAD_CONFIRM_NO,	"no",	RectLTWH( 85, 60, 60, 25 ) );*/
//		0, "OVERWRITE?", SRect( 400-70, 300-40, 400+70, 300-15 ) );

//		btn_desc.ID = ID_UI_LOAD_BUTTON(0) + i;
//		btn_desc.Rect.SetPositionLTWH( 10, 10 + i * 20, 220, 80 );
//		pSaveSlotsDialog->AddSubDialogButton( &btn_desc );
//		pConfirmDlg ),
	}

//	return pSaveSlotsDialog;
	return pConfirmDlg;
}


void CGameTask_LoadFG::InitMenu()
{
//	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CUIRender_SaveLoadFG ); // error LNK2001
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CGM_StdControlRendererManager );
	m_pUIRendererManager = pRenderMgr;

	InitFonts();

	// create dialog managers
	m_apDialogManager[SM_SELECT_SAVESLOT]	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );
	m_apDialogManager[SM_CONFIRM_LOAD]		= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	// create event handlers
	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_Load(this) );
//	m_apEventHandler[SM_CONFIRM_LOAD]	= new CEventHandler_ConfirmLoad( this );


	CGM_Dialog *apDialog[2];
	apDialog[SM_SELECT_SAVESLOT] = CreateSaveDataSlotListBox();
	apDialog[SM_CONFIRM_LOAD]    = CreateLoadConfirmationDialog();

	// set event handlers to dialogs
	apDialog[SM_SELECT_SAVESLOT]->SetEventHandler( pEventHandler );
	apDialog[SM_CONFIRM_LOAD]->SetEventHandler( pEventHandler );

	CGM_ListBoxEventHandlerSharedPtr pListBoxEventHandler( new CLoadDataListBoxEventHandler( this, 0 ) );
	m_pSaveSlotListBox->SetEventHandler( pListBoxEventHandler );

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_apDialogManager[0] );
	INPUTHUB.SetInputHandler( m_pInputHandler );

	// set scale for the current resolution
	int i;
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	for( i=0; i<NUM_SUB_MENUS; i++ )
	{
		m_apDialogManager[i]->ChangeScale( scale_factor );
	}
	// register preview entries to list box
	CreateSaveDataPreviewInfoTable();
}


int CGameTask_LoadFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( m_NextSubMenu != SM_INVALID )
	{
		((CInputHandler_Dialog *)m_pInputHandler)->SetDialogManager( m_apDialogManager[m_NextSubMenu] );
		m_SubMenu = m_NextSubMenu;
		m_NextSubMenu = SM_INVALID;
	}

//	if( m_pStage )
//		m_pStage->Update( dt );

	// render UI graphics
	for( int i=0; i<2; i++ )
	{
		if( m_apDialogManager[0].get() )
			m_apDialogManager[0]->Update( dt );
	}

//	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_LoadFG::RenderTaskMenu()
{
	// always render save slots
	m_apDialogManager[SM_SELECT_SAVESLOT]->Render();

	if( m_SubMenu == SM_CONFIRM_LOAD )
		m_apDialogManager[SM_CONFIRM_LOAD]->Render();
}


void CGameTask_LoadFG::ReleaseGraphicsResources()
{
}


void CGameTask_LoadFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


bool CGameTask_LoadFG::LoadSavedState( int index )
{
	if( index < 0 )
		index = m_CurrentFocusedSlotIndex;

	if( index < 0 )
		return false;

	SaveDataManager.LoadSavedStateFromFile( index );

	UpdatePreviewEntry( index );

	return true;
}

/*
void CGameTask_LoadFG::OnEnterPressed()
{
	RequestTransitionToNextTask();
}
*/

