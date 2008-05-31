
#include "GameTask_AircraftSelect.h"
#include "AircraftSelectEventHandler.h"
#include "GameCommon/AircraftCapsDisplay.h"
#include "GameTaskFactoryFG.h"

#include "Stage/Stage.h"
#include "Stage/StageLoader.h"
#include "Stage/BaseEntityManager.h"
#include "Stage/EntityRenderManager.h"
#include "Stage/PlayerInfo.h"
#include "Stage/CopyEntity.h"
#include "Stage/BE_GeneralEntity.h"
#include "Stage/ScreenEffectManager.h"

#include "Item/GI_Aircraft.h"
#include "Item/GI_Ammunition.h"
#include "Item/GI_Weapon.h"
#include "Item/GI_MissileLauncher.h"

#include "GameInput/InputHub.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/font.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"

#include "3DCommon/RenderTaskProcessor.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"

#include "../FlightGameGlobalParams.h"
#include "../UI/all.h"

#include <direct.h>
#include <assert.h>


class CInputHandler_Debug : public CInputHandler_Dialog
{
//	CInputHandler_Dialog *m_pUIInputHandler;

	CGameTask_AircraftSelect* m_pAircraftSelectTask;

public:
//	CInputHandler_Debug() m_pUIInputHandler(new CInputHandler_Dialog) : {}
//	~CInputHandler_Debug() { SafeDelete(m_pUIInputHandler); }

	CInputHandler_Debug( CGameTask_AircraftSelect* pTask, CGM_DialogManagerSharedPtr pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pAircraftSelectTask(pTask) {}

	CInputHandler_Debug( CGameTask_AircraftSelect* pTask, CGM_DialogManager *pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pAircraftSelectTask(pTask) {}

	~CInputHandler_Debug() {}

	virtual void ProcessInput(SInputData& input)
	{
		CInputHandler_Dialog::ProcessInput( input );

		switch( input.iGICode )
		{
		case GIC_F5:
			if( input.iType == ITYPE_KEY_PRESSED )
			{
				m_pAircraftSelectTask->InitStage();
				m_pAircraftSelectTask->OnAircraftChanged( 0 );
			}
	//		else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;
		}
	}

//	CInputHandler_Dialog *GetUIInputHandler() { return m_pUIInputHandler; }
};


class AmmoListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CGameTask_AircraftSelect* m_pTask;
	int m_AmmoType;

public:
	AmmoListBoxEventHandler( CGameTask_AircraftSelect* task, int ammo_type)
		: m_pTask(task), m_AmmoType(ammo_type) {}
	virtual ~AmmoListBoxEventHandler() {}
	virtual void OnItemSelected( CGM_ListBoxItem& item )
	{
//		MsgBoxFmt( "task: %d, button control for ammo name: %d", m_pTask, m_pTask->GetWeaponItemButton(m_AmmoType) );
//		MsgBoxFmt( "updating ammo name: %d, %s -> %s",
//			m_AmmoType, m_pTask->GetWeaponItemButton(m_AmmoType)->GetText().c_str(), item.text.c_str() );
		m_pTask->OnAmmunitionSelected( m_AmmoType, ((CGI_Ammunition *)(item.pUserData))->GetName()/*item.text*/ );
	}
	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item ) {}
};


void SupplyAircraftItemsToPlayer()
{
	if( !PLAYERINFO.SupplyItem( "condor", 1 ) )
		return;

	PLAYERINFO.SupplyItem( "goshawk", 1 );
	PLAYERINFO.SupplyItem( "kinghawk", 1 );
	PLAYERINFO.SupplyItem( "dolphin", 1 );

//	found a default aircraft in the database - this is a flight game test app
//	supply the player aircraft with some basic equipments

	PLAYERINFO.SupplyItem( "20mmMG",	1 );
	PLAYERINFO.SupplyItem( "SML",		1 );
	PLAYERINFO.SupplyItem( "MTML4",		1 );

	PLAYERINFO.SupplyItem( "Missile", 50 );
	PLAYERINFO.SupplyItem( "Missile2", 50 );
	PLAYERINFO.SupplyItem( "Missile3", 50 );
	PLAYERINFO.SupplyItem( "BlackShark", 50 );

	PLAYERINFO.SupplyItem( "AXM",		100 );
	PLAYERINFO.SupplyItem( "AXM2",		100 );
	PLAYERINFO.SupplyItem( "AXM3",		100 );
	PLAYERINFO.SupplyItem( "AXM4",		100 );
	PLAYERINFO.SupplyItem( "XMAA",		100 );
	PLAYERINFO.SupplyItem( "XLAA",		100 );
	PLAYERINFO.SupplyItem( "UGB",		100 );
	PLAYERINFO.SupplyItem( "UGBL",		100 );
}


CGameTask_AircraftSelect::CGameTask_AircraftSelect()
:
m_pAircraftDisplay(NULL),
m_pCameraEntity(NULL),
m_pCamera(NULL)
{
	// set the default aircraft items
	// 19:18 2007/05/27 moved to script
//	SupplyAircraftItemsToPlayer();

	m_CurrentAircraftIndex = 0;
	m_CurrentAmmoType = 0;
	m_SubMenu = SM_INVALID;
	m_NextSubMenu = SM_INVALID;

	size_t i;
	for( i=0; i<NUM_SUB_MENUS; i++ )
	{
		m_apRootDialog[i] = NULL;
	}

	for( i=0; i<NUM_AMMO_TYPES; i++ )
	{
		m_apItemButton[i] = NULL;
		m_apItemListBox[i] = NULL;
		m_apAmmoDisplay[i] = NULL;
	}


	// create a list of aircrafts currently owned by the player
	const vector<CGameItem *>& vecpPlayerItem = PLAYERINFO.GetItemList();

	m_vecpPlayerAircraft.reserve( 64 );

	size_t num_items = vecpPlayerItem.size();
	for( i=0; i<num_items; i++ )
	{
		if( vecpPlayerItem[i]->GetArchiveObjectID() == CGameItem::ID_AIRCRAFT )
			m_vecpPlayerAircraft.push_back( (CGI_Aircraft *)vecpPlayerItem[i] );
	}

	m_vecpBullet.reserve( 16 );
	m_vecpMissile.reserve( 32 );
	m_vecpSpAmmo.reserve( 32 );

	// create UI menus
	InitMenu();

	if( 0 < g_FlightGameParams.DefaultAircraftName.length() )
	{
		SelectDefaultAircraft();
		CGameTask::SetDefaultFadeInTimeMS(0);
		CGameTask::SetDefaultFadeOutTimeMS(0);
		return;
	}

	// load stage for aircraft select task
	InitStage();

	// set init sub menu
	// - open the root dialog
	SetNextSubMenu( SM_AIRCRAFT_SELECT );
	ChangeSubMenu();

	OnAircraftChanged( 0 );

//	m_CameraPose.current = Matrix34Identity();
//	m_CameraPose.target = Matrix34Identity();
//	m_CameraPose.smooth_time = 0.1f;

	m_CamPosition.current = Vector3(0,0,0);
	m_CamPosition.target = Vector3(0,0,0);
	m_CamPosition.vel = Vector3(0,0,0);
	m_CamPosition.smooth_time = 0.20f;

	m_CamOrient.current = Matrix33Identity();
	m_CamOrient.target = Matrix33Identity();
	m_CamOrient.vel = Matrix33Identity();
	m_CamOrient.smooth_time = 0.25f;


	OnAmmoTypeFocusChanged( AMMO_BULLET );

//	MessageBox( NULL, "load stage", "message", MB_OK );
}


CGameTask_AircraftSelect::~CGameTask_AircraftSelect()
{
//	INPUTHUB.PopInputHandler();
//	SafeDelete( m_pInputHandler );

	m_pStage.reset();

	SafeDelete( m_pCamera );
}


void CGameTask_AircraftSelect::InitStage()
{
	m_pStage.reset();

	CStageLoader stage_loader;
	m_pStage = stage_loader.LoadStage( "Script/tsk_as.bin" );

	// get entities - item display entity & scripted camera entity
	m_pAircraftDisplay	= m_pStage->GetEntitySet()->GetEntityByName( "aircraft_display" );
//	m_apAmmoDisplay[AMMO_MISSILE]	= m_pStage->GetEntitySet()->GetEntityByName( "weapon_display0" );
	m_apAmmoDisplay[AMMO_MISSILE]	= m_pStage->GetEntitySet()->GetEntityByName( "ammo_display0" );
	m_apAmmoDisplay[AMMO_SPW]		= m_pStage->GetEntitySet()->GetEntityByName( "ammo_display1" );
	m_pCameraEntity	= m_pStage->GetEntitySet()->GetEntityByName( "cam" );

	m_pStage->GetEntitySet()->SetCameraEntity( m_pCameraEntity );

	if( m_apAmmoDisplay[AMMO_MISSILE] )
		m_apAmmoDisplay[AMMO_MISSILE]->SetPosition( Vector3( 2.0f, 1.0f, 13.5f ) );

	if( m_apAmmoDisplay[AMMO_SPW] )
		m_apAmmoDisplay[AMMO_SPW]->SetPosition( Vector3(-2.0f, 1.0f, 13.5f ) );

	// enable shadow
//	m_pStage->GetEntitySet()->GetRenderManager()->EnableShadowMap();
}


CGM_Dialog *CGameTask_AircraftSelect::CreateAircraftSelectRootMenu()
{
	CGM_ControlRendererSharedPtr null_renderer = CGM_ControlRendererSharedPtr( new CGM_ControlRenderer() );
	// creates a root dialog that stores aircraft select sub-dialog
	// & controls for aircraft caps display
	CGM_Dialog *pRootNullDlg = m_pDialogManager->AddRootDialog( ID_TAS_DLG_ROOT_AIRCRAFTSELECT, SRect(0,0,0,0), "", 0, null_renderer );
	CGM_SubDialogButton *pSubDlgButton = pRootNullDlg->AddSubDialogButton( ID_TAS_SDB_AIRCRAFTSELECT, SRect(0,0,0,0), "" );

	m_apRootDialog[SM_AIRCRAFT_SELECT] = pRootNullDlg;

	CGM_Dialog* pAircraftSelectDialog = CreateAircraftSelectDialog();
	pSubDlgButton->SetSubDialog( pAircraftSelectDialog );

	// background rect for name & desc
	pRootNullDlg->AddStatic( 0, RectLTWH( 270, 416, 200,  32 ), "bg:" );
	pRootNullDlg->AddStatic( 0, RectLTWH( 270, 452, 480, 100 ), "bg:" );

	m_Caps.pName = pRootNullDlg->AddStatic( CAircraftCaps::CID_CAPS_NAME, RectLTWH( 280, 420, 200,  24 ), "Name" );
	m_Caps.pDesc = pRootNullDlg->AddStatic( CAircraftCaps::CID_CAPS_DESC, RectLTWH( 280, 452, 420, 100 ), "Desc\n-\n-" );
//	MsgBoxFmt( "added static control: %x", m_Caps.pDesc );

	int i;
	int id_offset = CAircraftCaps::CID_PERF_OFFSET;
	for( i=0; i<CAircraftCaps::GetNumPerfs(); i++ )
	{
		pRootNullDlg->AddStatic( id_offset + i, RectLTWH( 80, 415 + i*24, 150, 24 ), g_AircraftCapsText[i] );
	}

	// aircraft select sug-dialog is always open
	// it is used as a root dialog
//	pSubDlgButton->OpenDialog();

	return pAircraftSelectDialog;
}


CGM_Dialog *CGameTask_AircraftSelect::CreateAircraftSelectDialog()
{
	// aircraft select submenu

	CGM_Dialog *pAircraftSelectDialog;

	if( /* style == untitled_buttons */ false )
	{
		CGM_DialogDesc dlg_desc;
		dlg_desc.ID = ID_TAS_DLG_AIRCRAFTSELECT;
		dlg_desc.Rect = SRect( 10, 10, 50, 200 );
		dlg_desc.bRootDialog = false;
		pAircraftSelectDialog = m_pDialogManager->AddDialog( dlg_desc );

		size_t i, num_buttons = m_vecpPlayerAircraft.size() < 8 ? m_vecpPlayerAircraft.size() : 8;
		CGM_ButtonDesc btn_desc;
		for( i=0; i<num_buttons; i++ )
		{
			btn_desc.ID = AIRCRAFT_BUTTON_ID_OFFSET + (int)i;
			btn_desc.Rect.SetPositionLTWH( 10, 10 + (int)i * 20, 15, 15 );
			pAircraftSelectDialog->AddControl( &btn_desc );
		}
	}
	else
	{
		SRect dlg_rect = RectLTWH( 10, 10, 180, (int)(180 * GOLDEN_RATIO) );
		pAircraftSelectDialog = m_pDialogManager->AddDialog(
			ID_TAS_DLG_AIRCRAFTSELECT,
			dlg_rect,
			"AIRCRAFT"
			);

		// listbox
		SRect listbox_rect = dlg_rect;
		listbox_rect.bottom = listbox_rect.top + 20;
		CGM_ListBox *pListBox = pAircraftSelectDialog->AddListBox( ID_TAS_LBX_AIRCRAFTSELECT, listbox_rect, "", 
			CGM_ListBox::CLOSE_DIALOG_ON_ITEM_SELECTION, 20 );

		for( size_t i=0; i<m_vecpPlayerAircraft.size(); i++ )
		{
			pListBox->AddItem( m_vecpPlayerAircraft[i]->GetName() );
		}

		CGM_ListBoxEventHandlerSharedPtr pEventHandler( new TAS_AircraftSelectListBoxEventHandler( this ) );
		pListBox->SetEventHandler( pEventHandler );
	}

	return pAircraftSelectDialog;
}


CGM_Dialog *CGameTask_AircraftSelect::CreateAmmoSelectRootMenu()
{
	CGM_ControlRendererSharedPtr null_renderer = CGM_ControlRendererSharedPtr( new CGM_ControlRenderer() );
	// creates a root dialog that stores aircraft select sub-dialog
	// & controls for aircraft caps display
	CGM_Dialog *pRootNullDlg = m_pDialogManager->AddRootDialog( ID_TAS_DLG_ROOT_AMMOSELECT, SRect(0,0,0,0), "", 0, null_renderer );
	CGM_SubDialogButton *pSubDlgButton = pRootNullDlg->AddSubDialogButton( ID_TAS_SDB_AMMOSELECT, SRect(0,0,0,0), "" );

	CGM_Dialog* pAmmoSelectDialog = CreateAmmoSelectDialog();
	pSubDlgButton->SetSubDialog( pAmmoSelectDialog );

	m_apRootDialog[SM_ITEMS_SELECT] = pRootNullDlg;

	// background rect for name & desc
	pRootNullDlg->AddStatic( 0, RectLTWH( 270, 416, 200,  32 ), "bg:" );
	pRootNullDlg->AddStatic( 0, RectLTWH( 270, 452, 480, 100 ), "bg:" );

	// add static for ammo name & desc
	// controls will be registered to m_pUIRendererManager
	pRootNullDlg->AddStatic( 0, RectLTWH( 280, 420, 200,  24 ), "AmmoName" );
	pRootNullDlg->AddStatic( 0, RectLTWH( 280, 452, 420, 100 ), "AmmoDesc\n\n" );
//	MsgBoxFmt( "added static control: %x", m_Caps.pDesc );

/*	int i;
	int id_offset = CAircraftCaps::CID_PERF_SPEED;
	for( i=0; i<CAircraftCapsDisplay::NUM_PERFS; i++ )
	{
		pRootNullDlg->AddStatic( id_offset + i, RectLTWH( 80, 415 + i*24, 150, 24 ), g_AircraftCapsText[i] );
	}
*/

	return pAmmoSelectDialog;
}


CGM_Dialog *CGameTask_AircraftSelect::CreateAmmoSelectDialog()
{
	// aircraft select submenu
	CGM_Dialog *pAmmoSelectDialog;

	// item select submenu
	CGM_DialogDesc dlg_desc;
	dlg_desc.SetDefault();
	dlg_desc.ID = ID_TAS_DLG_AMMOSELECT;
	dlg_desc.Rect.SetPositionLTWH( 70, 70, 180, (int)(180 * GOLDEN_RATIO) );
//	dlg_desc.bRootDialog = true;
	dlg_desc.bRootDialog = false;
	dlg_desc.strTitle = "WEAPONS";
	pAmmoSelectDialog = m_pDialogManager->AddDialog( dlg_desc );

	string ammo_title[3] = { "GUN:", "MAIN:", "SPW:" };
	CGM_SubDialogButtonDesc sdlg_btn_desc;
	int subdlg_button_id[3] = { ID_TAS_GUN, ID_TAS_MISSILE, ID_TAS_SPW };
	int listbox_id[3] = { ID_TAS_LISTBOX_GUN, ID_TAS_LISTBOX_MISSILE, ID_TAS_LISTBOX_SPW };
	int top_margin = 20; // for list box placed on dialog
	int j;
	for( j=0; j<NUM_AMMO_TYPES; j++ )
	{
		// ammo title (static control)
		pAmmoSelectDialog->AddStatic( 0, RectLTWH( 10, 20 + 32 * j, 48, 28 ), ammo_title[j] );

		// sub-dialog button for ammo dialog and list box
		// - also used to display the name of the selected ammo
		sdlg_btn_desc.ID = subdlg_button_id[j];
		sdlg_btn_desc.strText = "";
		sdlg_btn_desc.Rect.SetPositionLTWH( 72, 20 + 32 * j, 72, 28 );
		CGM_SubDialogButton *pSubDlgButton = (CGM_SubDialogButton *)pAmmoSelectDialog->AddControl( &sdlg_btn_desc );

		m_apItemButton[j] = pSubDlgButton;

//		MsgBoxFmt( "sub-dlg button created: %d", GetWeaponItemButton(j) );

		// dialog and listbox for each type of ammunition
		CGM_DialogDesc sdlg_desc;
		sdlg_desc.strTitle = ammo_title[j];
		sdlg_desc.Rect.SetPositionLTWH( 210, 50, 160, (int)(160 * GOLDEN_RATIO) );

		// set event handler for the class
		sdlg_desc.pEventHandler
			= CGM_DialogEventHandlerSharedPtr( new TAS_ItemsSelectEventHandler(this) );

		CGM_Dialog *pSubDlg = m_pDialogManager->AddDialog( sdlg_desc );

		pSubDlgButton->SetSubDialog( pSubDlg );

		const int item_text_height = 28;
		CGM_ListBoxDesc box_desc;
		box_desc.Style = CGM_ListBox::CLOSE_DIALOG_ON_ITEM_SELECTION;
		box_desc.Rect = RectLTWH( 0, top_margin, sdlg_desc.Rect.GetWidth(), sdlg_desc.Rect.GetHeight() - top_margin );
		box_desc.Rect.Inflate( -5, -5 );
		box_desc.Rect.top += item_text_height;
		box_desc.ID = listbox_id[j];
		box_desc.nTextHeight = item_text_height;
		m_apItemListBox[j] = (CGM_ListBox *)pSubDlg->AddControl( &box_desc );

		CGM_ListBoxEventHandlerSharedPtr pListBoxEventHandler( new AmmoListBoxEventHandler( this, j ) );
		m_apItemListBox[j]->SetEventHandler( pListBoxEventHandler );
	}

	pAmmoSelectDialog->AddButton( ID_TAS_ITEMSELECT_OK, RectLTWH( 50, 10 + 32 * j, 48, 28 ), "OK" );

	return pAmmoSelectDialog;
}


void CGameTask_AircraftSelect::InitMenu()
{
///	CGM_ControlRendererManagerSharedPtr pRendererMgr( new CAircraftCapsDisplay );
///	m_pUIRendererManager = (CAircraftCapsDisplay *)pRendererMgr.get();
	m_pUIRendererManager = CGM_ControlRendererManagerSharedPtr( new CFG_StdControlRendererManager() );

	m_pDialogManager = CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	// add font to UI render manager
	// and load sounds
	CGameTaskFG::DoCommonInit( m_pDialogManager );

	CGM_DialogEventHandlerSharedPtr apEventHandler[3];
	apEventHandler[SM_AIRCRAFT_SELECT] = CGM_DialogEventHandlerSharedPtr( new TAS_AircraftSelectEventHandler(this) );
	apEventHandler[SM_ITEMS_SELECT]    = CGM_DialogEventHandlerSharedPtr( new TAS_ItemsSelectEventHandler(this) );
	apEventHandler[SM_CONFIRM]         = CGM_DialogEventHandlerSharedPtr( new TAS_ConfirmEventHandler(this) );

	// confirmation dialog box
	m_apRootDialog[SM_CONFIRM]
		= Create2ChoiceDialog( m_pDialogManager, true,
		ID_TAS_DLG_ROOT_CONFIRM, "confirm", SRect( 400-80, 300-60, 400+80, 300+30 ),
		ID_TAS_CONFIRM_OK,       "yes",     RectLTWH( 15, 60, 60, 25 ),
		ID_TAS_CONFIRM_CANCEL,   "no",      RectLTWH( 85, 60, 60, 25 )/*,
		0, "You're gonna go up with this one?", SRect( 400-70, 300-40, 400+70, 300-15 )*/ );

	CFG_StdDialogRenderer *pDlgRenderer
	= dynamic_cast<CFG_StdDialogRenderer *>(m_apRootDialog[SM_CONFIRM]->GetRenderer());
	if( pDlgRenderer )
	{
		pDlgRenderer->SetSlideInAmount( Vector2(0,0) );
		pDlgRenderer->SetSlideOutAmount( Vector2(0,0) );
	}

	// submenu for aircraft select
	CGM_Dialog *pAircraftSelectDlg = CreateAircraftSelectRootMenu();

	// submenu for ammunition select
	CGM_Dialog *pItemsSelectDlg = CreateAmmoSelectRootMenu();

	pAircraftSelectDlg->SetEventHandler( apEventHandler[SM_AIRCRAFT_SELECT] );
	pItemsSelectDlg->SetEventHandler( apEventHandler[SM_ITEMS_SELECT] );
	m_apRootDialog[SM_CONFIRM]->SetEventHandler( apEventHandler[SM_CONFIRM] );

	// set input handler for dialog menu
//	m_pInputHandler = new CInputHandler_Dialog( m_apDialogManager[m_SubMenu] );
	m_pInputHandler = new CInputHandler_Debug( this, m_pDialogManager );
	INPUTHUB.PushInputHandler( m_pInputHandler );

	// set scale for the current resolution
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	m_pDialogManager->ChangeScale( scale_factor );

//	m_pDialogManager->OpenRootDialog( m_apRootDialog[SM_AIRCRAFT_SELECT]->GetID() );
}


void CGameTask_AircraftSelect::SelectDefaultAircraft()
{
	const string& aircraft_name = g_FlightGameParams.DefaultAircraftName;

	m_vecpPlayerAircraft.resize(0);
	m_vecpPlayerAircraft.push_back( (CGI_Aircraft *)PLAYERINFO.GetItemByName( aircraft_name.c_str() ) );

	OnAircraftChanged(0);

	for( int i=0; i<NUM_AMMO_TYPES; i++ )
	{
		if( 0 < m_apItemListBox[i]->GetNumItems() )
///			m_apItemListBox[i]->SelectItem( 0 );
			m_apItemListBox[i]->SetItemSelectionFocus( 0 );
	}

	OnAircraftSelected();

	RequestTransitionToNextTask();
}


void CGameTask_AircraftSelect::ChangeSubMenu()
{
//	((CInputHandler_Dialog *)m_pInputHandler)->SetDialogManager( m_apDialogManager[m_NextSubMenu] );

	if( m_SubMenu != SM_INVALID )
	{
		// close the current dialog
		m_apRootDialog[m_SubMenu]->Close();
	}

	// open the root dialog for the new sub menu
	m_pDialogManager->OpenRootDialog( m_apRootDialog[m_NextSubMenu]->GetID() );

	if( m_NextSubMenu == SM_ITEMS_SELECT )
		((CGM_SubDialogButton *)m_pDialogManager->GetControl(ID_TAS_SDB_AMMOSELECT))->OpenDialog();
	else if( m_NextSubMenu == SM_AIRCRAFT_SELECT )
		((CGM_SubDialogButton *)m_pDialogManager->GetControl(ID_TAS_SDB_AIRCRAFTSELECT))->OpenDialog();

//	if( !pButton->IsSubDialogOpen() )
//		pButton->OpenDialog();

	m_SubMenu = m_NextSubMenu;
	m_NextSubMenu = SM_INVALID;
}


int CGameTask_AircraftSelect::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;


	if( m_NextSubMenu != SM_INVALID )
	{
		ChangeSubMenu();
	}

	if( m_pStage )
		m_pStage->Update( dt );

	// render UI graphics
	if( m_pDialogManager.get() )
		m_pDialogManager->Update( dt );

	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_AircraftSelect::UpdateCamera( float dt )
{
	if( !m_pCamera )
	{
		m_pCamera = new CCamera( 3.141592f / 3.0f, 4.0f / 3.0f, 1000.0f, 0.1f );

//		m_pCamera->SetPosition( Vector3(5.0f, 4.5f, -6.0f) );
//		m_pCamera->SetOrientation( Matrix33RotationY(-2.5f) );
		m_pCamera->SetPosition( Vector3(0.0f, 4.5f, 0.0f) );
	}

	static float s_Time = 0;
	s_Time += dt;
//	m_pCamera->SetOrientation( Matrix33RotationY( s_Time * 0.5f ) );
/*	Vector3 vDir = Matrix33RotationY( s_Time * 0.3f ) * Vector3(0,0,1);
	m_pCamera->SetPosition( Vector3(0.0f,4.2f,0.0f) + vDir * 18.0f );
	m_pCamera->SetOrientation( Matrix33( Vec3Cross( Vector3(0,1,0), (-vDir) ), Vector3(0,1,0), -vDir ) );
*/
	Vector3 vLookAtPos = Vector3(0,3,0);
	float dist = 20.0f;
	switch( m_SubMenu )
	{
	case SM_AIRCRAFT_SELECT:
//		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
		vLookAtPos = Vector3(0.0f,7.2f,0.0f); // debug - check the envmapped surface
		dist = 18.0f;
		break;

	case SM_ITEMS_SELECT:
		if( m_apAmmoDisplay[m_CurrentAmmoType] )
		{
			vLookAtPos = m_apAmmoDisplay[m_CurrentAmmoType]->Position() + Vector3(0.0f,0.5f,0.0f);
			dist = 6.0f;
		}
		break;

	case SM_CONFIRM:
		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
		dist = 25.0f;
		break;

	case SM_INVALID:
	default:
		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
		dist = 24.0f;
		break;
	}

	Vector3 vDir = Matrix33RotationY( s_Time * 0.3f ) * Vector3(0,0,1);
	m_CamPosition.target = vLookAtPos + vDir * dist;
	m_CamOrient.target = Matrix33( Vec3Cross( Vector3(0,1,0), (-vDir) ), Vector3(0,1,0), -vDir );

//	m_pCamera->SetPose( m_CameraPose.current );
	m_pCamera->SetPosition( m_CamPosition.current );
	m_pCamera->SetOrientation( m_CamOrient.current );

//	m_CameraPose.Update( dt );
	m_CamPosition.Update( dt );
	m_CamOrient.Update( dt );

}


void CGameTask_AircraftSelect::Render()
{
	if( !m_pCamera )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render the stage (aircraft, missile, etc.)
	if( m_pStage )
	{
		CScreenEffectManager* pEffectMgr = m_pStage->GetScreenEffectManager();
		if( pEffectMgr )
		{
//			pEffectMgr->ClearEffectFlag( ScreenEffect::PostProcessEffects );
			pEffectMgr->RaiseEffectFlag( ScreenEffect::PostProcessEffects );

//			pEffectMgr->SetMotionBlurFactor( 0.5f );
//			pEffectMgr->RaiseEffectFlag( ScreenEffect::PseudoMotionBlur );

//			pEffectMgr->SetMonochromeColorOffset( -0.3f, 0.1f, -0.3f );
//			pEffectMgr->RaiseEffectFlag( ScreenEffect::MonochromeColor );

//			pEffectMgr->RaiseEffectFlag( ScreenEffect::Glare );
//			pEffectMgr->SetGlareLuminanceThreshold( 0.5f );

//			pEffectMgr->RaiseEffectFlag( ScreenEffect::NOISE );

//			pEffectMgr->RaiseEffectFlag( ScreenEffect::PSEUDO_BLUR );
//			pEffectMgr->SetBlurEffect( 4.0f );
		}

		m_pStage->Render( *m_pCamera );
	}

	// render stage select dialog
	if( m_pDialogManager )
        m_pDialogManager->Render();

	// draw cursor
	DrawMouseCursor();
}


void CGameTask_AircraftSelect::ReleaseGraphicsResources()
{
}


void CGameTask_AircraftSelect::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	InitMenu();
}


void CGameTask_AircraftSelect::OnAmmoTypeFocusChanged( int ammo_type )
{
	m_CurrentAmmoType = ammo_type;

	CGM_ListBoxItem *pFocusedItem = m_apItemListBox[ammo_type]->GetSelectedItem();
	if( pFocusedItem )
        UpdateAmmoDescDisplay( *(CGI_Ammunition *)(pFocusedItem->pUserData) );
}


void CGameTask_AircraftSelect::OnAmmoFocusChanged( int ammo_type )
{
	UpdateAmmunitionDisplay( ammo_type );
}


void CGameTask_AircraftSelect::OnAmmunitionSelected( int ammo_type, const string& ammo_name )
{
	// update title of the button
	GetWeaponItemButton(ammo_type)->SetText( ammo_name );

	// load model for the selected ammunition
//	UpdateAmmunitionDisplay(ammo_type);
}


void CGameTask_AircraftSelect::UpdateAmmoDescDisplay( CGI_Ammunition& ammo )
{
//	if( m_pUIRendererManager )
//	{
//		m_pUIRendererManager->m_pAmmoName->SetText( ammo.GetName() );
//		m_pUIRendererManager->m_pAmmoDesc->SetText( ammo.GetDesc( /*GetCurrentLanguageID()*/ 0 ) );
//	}
}


void CGameTask_AircraftSelect::UpdateAmmunitionDisplay( int ammo_type )
{
	if( !m_apAmmoDisplay[ammo_type] )
		return;

	CBE_GeneralEntity* pBaseEntity = (CBE_GeneralEntity *)(m_apAmmoDisplay[ammo_type]->pBaseEntity);

	CGM_ListBoxItem *pFocusedItem = m_apItemListBox[ammo_type]->GetSelectedItem();
	if( pFocusedItem )
	{
		CGI_Ammunition& ammo = *(CGI_Ammunition *)(pFocusedItem->pUserData);

		pBaseEntity->MeshProperty().Release();
		pBaseEntity->MeshProperty() = CBE_MeshObjectProperty( ammo.GetMeshObjectContainer().m_MeshObjectHandle.filename );
		pBaseEntity->MeshProperty().LoadMeshObject();

		UpdateAmmoDescDisplay( ammo );
	}

}


void CGameTask_AircraftSelect::OnAircraftChanged( const std::string& aircraft_name )
{
	size_t i, num_aircrafts = m_vecpPlayerAircraft.size();
	for( i=0; i<num_aircrafts; i++ )
	{
		if( aircraft_name == m_vecpPlayerAircraft[i]->GetName() )
			OnAircraftChanged( (int)i );
	}
}


void CGameTask_AircraftSelect::OnAircraftChanged( int index )
{
	if( index < 0 || (int)m_vecpPlayerAircraft.size() <= index )
		return;	// invalid index

	if( m_pAircraftDisplay )
	{
		CBE_GeneralEntity* pBaseEntity = (CBE_GeneralEntity *)(m_pAircraftDisplay->pBaseEntity);

		pBaseEntity->MeshProperty().Release();
		pBaseEntity->MeshProperty().m_MeshObjectHandle
			= m_vecpPlayerAircraft[index]->GetMeshObjectContainer().m_MeshObjectHandle;
		pBaseEntity->MeshProperty().LoadMeshObject();
	}
	CGI_Aircraft& aircraft = *(m_vecpPlayerAircraft[index]);

	m_CurrentAircraftIndex = index;

	// update lists of ammo items that can be fired from the aircraft

	// gun
	const vector<CGameItem *>& vecpPlayerItem = PLAYERINFO.GetItemList();


	m_vecpBullet.resize( 0 );
	m_vecpMissile.resize( 0 );
	m_vecpSpAmmo.resize( 0 );

	// clear previous items in the item list boxes
	size_t i;
	for( i=0; i<NUM_AMMO_TYPES; i++ )
		m_apItemListBox[i]->RemoveAllItems();

	size_t num_items = vecpPlayerItem.size();
	int ammo_index, weapon_slot_index = 0;


	int listbox_offset = 0;//AMMO_MISSILE;
	vector<CGI_Ammunition *> *vecpAmmoList[3] = { &m_vecpBullet, &m_vecpMissile, &m_vecpSpAmmo };
	for( ammo_index=0; ammo_index<3; ammo_index++ )
	{
		weapon_slot_index = ammo_index;// + 1;
		string item_title;
		int payloads;
		for( i=0; i<num_items; i++ )
		{
			const CGameItem& item = *vecpPlayerItem[i];
			if( item.GetArchiveObjectID() != CGameItem::ID_AMMUNITION )
				continue;

			payloads = aircraft.GetPayloadForAmmunition( *(CGI_Ammunition *)&item, weapon_slot_index );
			if( payloads == 0 )
				continue;	// cannot carry this ammo

//			MsgBoxFmt( "adding an item to the listbox: %s", item.GetName().c_str() );

			vecpAmmoList[ammo_index]->push_back( (CGI_Ammunition *)&item );

			item_title = item.GetName();
			for( size_t j=0; j<7 - item.GetName().length(); j++ ) item_title += ".";
			item_title += fmt_string("%02d",payloads);
			m_apItemListBox[listbox_offset + ammo_index]->AddItem( item_title, (void *)&item );
		}
	}
/*
	for( i=0; i<num_items; i++ )
	{
		const CGameItem& item = *vecpPlayerItem[i];
		if( item.GetArchiveObjectID() == CGameItem::ID_AMMUNITION )
			aircraft.IsAmmoSupported( (CGI_Ammunition *)&item )
		{
			m_vecpBullet.push_back( (CGI_Ammunition *)&item );
		}
	}
*/

	// set default weapons names on each sub-dlg buton
	CGM_ListBoxItem* pFocusedItem;

	int ammo_type;
	for( ammo_type = 0; ammo_type<NUM_AMMO_TYPES; ammo_type++ )
	{
		pFocusedItem = m_apItemListBox[ammo_type]->GetSelectedItem();
		if( pFocusedItem )
			m_apItemButton[ammo_type]->SetText( ((CGI_Ammunition *)(pFocusedItem->pUserData))->GetName() );

		UpdateAmmunitionDisplay( ammo_type );
	}
/*	pFocusedItem = m_apItemListBox[AMMO_MISSILE]->GetSelectedItem();
	if( pFocusedItem )
		m_apItemButton[AMMO_MISSILE]->SetText( ((CGI_Ammunition *)(pFocusedItem->pUserData))->GetName() );

	pFocusedItem = m_apItemListBox[AMMO_SPW]->GetSelectedItem();
	if( pFocusedItem )
        m_apItemButton[AMMO_SPW]->SetText( ((CGI_Ammunition *)(pFocusedItem->pUserData))->GetName() );
*/
//	UpdateAmmunitionDisplay(AMMO_BULLET);
///	UpdateAmmunitionDisplay(AMMO_MISSILE);
///	UpdateAmmunitionDisplay(AMMO_SPW);

	m_Caps.pName->SetText( aircraft.GetName() );
	m_Caps.pDesc->SetText( aircraft.GetDesc(/*GetCurrentLanguageID()*/ 0 ) );
///	m_pUIRendererManager->GetRenderParam( m_Caps.pDesc->m_UserIndex ).m_Font.m_FontIndex = 3;//GetCurrentLanguageID()==0 ? 3 : 1;

///	m_pUIRendererManager->SetAircraft( &aircraft );
}


void CGameTask_AircraftSelect::OnAircraftSelected()
{
	PLAYERINFO.SetAircraft( m_vecpPlayerAircraft[m_CurrentAircraftIndex] );

	CWeaponSystem& weapon_system = PLAYERINFO.GetAircraft()->WeaponSystem();

	weapon_system.GetWeaponSlot(0).MountWeapon( (CGI_Weapon *)PLAYERINFO.GetItemByName( "20mmMG" ) );

	weapon_system.GetWeaponSlot(1).MountWeapon( (CGI_MissileLauncher *)PLAYERINFO.GetItemByName( "SML" ) );	// standard missile launcher
	weapon_system.GetWeaponSlot(2).MountWeapon( (CGI_MissileLauncher *)PLAYERINFO.GetItemByName( "MTML4" ) );	// missile launcher that can simultaneously lock-on up to 4 targets

	// load ammunitions ( 0: bullets, 1: missiles, 2: special weapon ammo )
	for( int i=0; i<NUM_AMMO_TYPES; i++ )
	{
		CGM_ListBoxItem* pItem = m_apItemListBox[i]->GetSelectedItem();
		if( pItem && pItem->pUserData )
		{
			CGI_Ammunition *pAmmo = (CGI_Ammunition *)(pItem->pUserData);
			PLAYERINFO.SupplyItem( pAmmo->GetName(), PLAYERINFO.GetAircraft()->GetPayloadForAmmunition( *pAmmo, i ) );

			bool loaded = weapon_system.GetWeaponSlot(i).Load( pAmmo );
			if( !loaded )
				g_Log.Print( "CGameTask_AircraftSelect - cannot load the ammo '%s'"/* to weapon '%s'"*/,
				pAmmo->GetName().c_str()/*, weapon_system.GetWeaponSlot(1+i).pWeapon*/ );

			// load mesh object for the ammo since they are rendered with the aircraft model in the stage
			pAmmo->LoadMeshObject();
		}
	}

	// set the primary weapon slot first
	// - secondary weapon slot will not be selected if the primary weapon slot is set to 1
    weapon_system.SelectPrimaryWeapon( 1 );		// select missile as a default primary weapon
    weapon_system.SelectSecondaryWeapon( 0 );	// select gun as a secondary weapon

//	MsgBoxFmt( "PLAYERINFO.GetItemByName( \"SML\" ) - 0x%08x", PLAYERINFO.GetItemByName("SML") );
//	MsgBoxFmt( "(CGI_Weapon *)PLAYERINFO.GetItemByName( \"SML\" ) - 0x%08x", (CGI_Weapon *)PLAYERINFO.GetItemByName("SML") );
//	MsgBoxFmt( "(CGI_MissileLauncher *)PLAYERINFO.GetItemByName( \"SML\" ) - 0x%08x", (CGI_MissileLauncher *)PLAYERINFO.GetItemByName("SML") );
}


void CGameTask_AircraftSelect::SetNextSubMenu( int next_submenu_id )
{
	m_NextSubMenu = next_submenu_id;
}


void CGameTask_AircraftSelect::CreateRenderTasks()
{
	if( m_pStage )
		m_pStage->CreateStageRenderTasks( m_pCamera );

	RenderTaskProcessor.AddRenderTask( new CGameTaskRenderTask( this ) );
}

