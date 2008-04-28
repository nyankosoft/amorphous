
#include "GameTask_ShopFG.h"
#include "EventHandler_MainMenuFG.h"	// needed to derive event hander from CEventHandler_Base
#include "../UI/all.h"


#include "GameCommon/AircraftCapsDisplay.h"
#include "GameCommon/GameItemShop.h"

#include "Stage/Stage.h"
#include "Stage/PlayerInfo.h"
//#include "Stage/CopyEntity.h"
//#include "Stage/BE_GeneralEntity.h"

#include "Item/GI_Aircraft.h"
#include "Item/GI_Ammunition.h"
#include "Item/GI_Weapon.h"
#include "Item/GI_MissileLauncher.h"

#include "GameInput/InputHub.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/Font.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"

#include "GameEvent/ScriptManager.h"
#include "GameEvent/PyModules.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"
#include "Support/Macro.h"

#include <direct.h>
#include <assert.h>


class CInputHandler_Debug : public CInputHandler_Dialog
{
//	CInputHandler_Dialog *m_pUIInputHandler;

	CGameTask_ShopFG* m_pTask;

public:
//	CInputHandler_Debug() m_pUIInputHandler(new CInputHandler_Dialog) : {}
//	~CInputHandler_Debug() { SafeDelete(m_pUIInputHandler); }

	CInputHandler_Debug( CGameTask_ShopFG *pTask, CGM_DialogManagerSharedPtr pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pTask(pTask) {}

	CInputHandler_Debug( CGameTask_ShopFG *pTask, CGM_DialogManager *pDialogManager )
		: CInputHandler_Dialog(pDialogManager), m_pTask(pTask) {}

	~CInputHandler_Debug() {}

	virtual void ProcessInput(SInputData& input)
	{
		CInputHandler_Dialog::ProcessInput( input );

		switch( input.iGICode )
		{
		case GIC_F5:
			if( input.iType == ITYPE_KEY_PRESSED )
			{
				m_pTask->InitStage();
				m_pTask->OnAircraftChanged( 0 );
			}
	//		else if( input.iType == ITYPE_KEY_RELEASED ) {}
			break;
		}
	}

//	CInputHandler_Dialog *GetUIInputHandler() { return m_pUIInputHandler; }
};


class CEventHandler_ShopFG : public CEventHandler_Base
{
	/// task that processes events (borrowed reference)
	CGameTask_ShopFG *m_pTask;

public:

	CEventHandler_ShopFG( CGameTask_ShopFG *pTask ) : m_pTask(pTask) {}

	virtual ~CEventHandler_ShopFG() {}

	virtual void HandleEvent( CGM_Event &event )
	{
		CEventHandler_Base::HandleEvent( event );

		if( !event.pControl )
			return;


		switch( event.pControl->GetID() )
		{
		case ID_TIS_ITEMTYPE_SELECT_DIALOG:
            if( event.Type == CGM_Event::DIALOG_CLOSED )
                m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK ); // root dialog was closed
			break;

		default:
			break;
		}
	}

};



class ItemListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CGameTask_ShopFG* m_pTask;
	int m_ItemType;

public:
	ItemListBoxEventHandler( CGameTask_ShopFG* task, int item_type )
		: m_pTask(task), m_ItemType(item_type) {}

	virtual ~ItemListBoxEventHandler() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item )
	{
//		MsgBoxFmt( "task: %d, button control for ammo name: %d", m_pTask, m_pTask->GetWeaponItemButton(m_ItemType) );
//		MsgBoxFmt( "updating ammo name: %d, %s -> %s",
//			m_ItemType, m_pTask->GetWeaponItemButton(m_ItemType)->GetText().c_str(), item.text.c_str() );
//		m_pTask->OnItemSelected( m_ItemType, ((CGameItem *)(item.pUserData))->GetName()/*item.text*/ );
		m_pTask->OnItemSelected( m_ItemType, item.GetText() );
	}

	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item ) {}
};


CGameTask_ShopFG::CGameTask_ShopFG()
:
m_pItemDisplay(NULL),
m_pCameraEntity(NULL),
m_pCamera(NULL)
{
	m_CurrentAircraftIndex = 0;
	m_SubMenu = SM_AIRCRAFT_SELECT;
	m_NextSubMenu = SM_INVALID;

	m_pMoneyLeft = NULL;

	size_t i;
	for( i=0; i<NUM_ITEM_TYPES; i++ )
	{
		m_apItemButton[i] = NULL;
		m_apItemListBox[i] = NULL;
		m_apAmmoDisplay[i] = NULL;
	}


//	const vector<CGameItem *>& vecpPlayerItem = PLAYERINFO.GetItemList();

	//m_vecpPlayerAircraft.reserve( 64 );

	//size_t num_items = vecpPlayerItem.size();
	//for( i=0; i<num_items; i++ )
	//{
	//	if( vecpPlayerItem[i]->GetArchiveObjectID() == CGameItem::ID_AIRCRAFT )
	//		m_vecpPlayerAircraft.push_back( (CGI_Aircraft *)vecpPlayerItem[i] );
	//}

	// create dialog menu for selecting stages
	InitMenu();

//	if( 0 < g_FlightGameParams.DefaultAircraftName.length() )
//	{
////		SelectDefaultAircraft();
//		return;
//	}

	// load stage for aircraft select task
	InitStage();

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

	m_vecpShop.push_back( new CGameItemShop("shop") );
//	m_vecpShop[0]->AddItem( ItemDatabaseManager.GetItem( "condor", 1 ) );
//	m_vecpShop[0]->AddItem( ItemDatabaseManager.GetItem( "goshawk", 1 ) );
//	m_vecpShop[0]->AddItem( ItemDatabaseManager.GetItem( "kinghawk", 1 ) );

	CScriptManager event_mgr;
	event_mgr.AddModule( "PlayerInfo",	g_PyModulePlayerMethod );
	event_mgr.AddModule( "Shop",		g_PyModuleShopMethod );
	event_mgr.LoadScriptArchiveFile( "Script/shop.bin" );
	event_mgr.Update();

	UpdateItemListBox();

	UpdatePlayerMoneyLeftDisplay();

//	OnAmmoTypeFocusChanged( AMMO_BULLET );

//	MessageBox( NULL, "load stage", "message", MB_OK );
}


CGameTask_ShopFG::~CGameTask_ShopFG()
{
//	INPUTHUB.PopInputHandler();
//	SafeDelete( m_pInputHandler );

	m_pStage.reset();

	SafeDelete( m_pCamera );

	SafeDeleteVector( m_vecpShop );
}


void CGameTask_ShopFG::InitStage()
{
/*	SafeDelete( m_pStage );

	m_pStage = new CStage;

	m_pStage->Initialize( "Script/tsk_as.bin" );

	// get entities - item display entity & scripted camera entity
	m_pItemDisplay	= m_pStage->GetEntitySet()->GetEntityByName( "aircraft_display" );
//	m_apAmmoDisplay[AMMO_MISSILE]	= m_pStage->GetEntitySet()->GetEntityByName( "weapon_display0" );
	m_apAmmoDisplay[AMMO_MISSILE]	= m_pStage->GetEntitySet()->GetEntityByName( "ammo_display0" );
	m_apAmmoDisplay[AMMO_SPW]		= m_pStage->GetEntitySet()->GetEntityByName( "ammo_display1" );
	m_pCameraEntity	= m_pStage->GetEntitySet()->GetEntityByName( "cam" );

	m_pStage->GetEntitySet()->SetCameraEntity( m_pCameraEntity );

	m_apAmmoDisplay[AMMO_MISSILE]->SetPosition( Vector3( 2.0f, 1.0f, 13.5f ) );
	m_apAmmoDisplay[AMMO_SPW]->SetPosition( Vector3(-2.0f, 1.0f, 13.5f ) );*/
}


CGM_Dialog *CGameTask_ShopFG::CreateAircraftSelectRootMenu()
{/*
	// creates a root dialog that stores aircraft select sub-dialog
	// & controls for aircraft caps display
	CGM_Dialog *pRootNullDlg = m_apDialogManager[SM_AIRCRAFT_SELECT]->AddRootDialog( 0, SRect(0,0,0,0), "" );
	CGM_SubDialogButton *pSubDlgButton = pRootNullDlg->AddSubDialogButton( 0, SRect(0,0,0,0), "" );

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
	pSubDlgButton->OpenDialog();

	return pAircraftSelectDialog;*/
	return NULL;
}


CGM_Dialog *CGameTask_ShopFG::CreateAircraftSelectDialog()
{
	// aircraft select submenu
/*
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


void CGameTask_ShopFG::UpdateItemListBox()
{
	if( m_vecpShop.size() == 0 )
		return;

	size_t i, num = m_vecpShop[0]->GetNumItems();
	for( i=0; i<num; i++ )
	{
		const CGameItem *pItem = m_vecpShop[0]->GetItem((int)i);

		m_apItemListBox[0]->AddItem( pItem->GetName(), NULL );
	}
}


CGM_Dialog *CGameTask_ShopFG::CreateItemSelectDialog()
{
	CGM_Dialog *pItemTypeSelectDialog = m_apDialogManager[0]->AddRootDialog(
		ID_TIS_ITEMTYPE_SELECT_DIALOG,
		RectLTWH( 50, 50, 150, 150 ),
		"SHOP" );

	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_ShopFG(this) );
	pItemTypeSelectDialog->SetEventHandler( pEventHandler );

	CGM_SubDialogButtonDesc sdlg_btn_desc;
	string item_title[3]    = { "AIRCRAFT",         "GUN",              "ITEM" };
	int subdlg_button_id[3] = { ID_TIS_SPW,         ID_TIS_GUN,         ID_TIS_MISSILE };
	int listbox_id[3]       = { ID_TIS_LISTBOX_SPW, ID_TIS_LISTBOX_GUN, ID_TIS_LISTBOX_MISSILE };
	int top_margin = 20;
	for( int j=0; j<numof(item_title); j++ )
	{
//		pItemTypeSelectDialog->AddStatic( 0, RectLTWH( 10, 10 + 32 * j, 48, 28 ), ammo_title[j] );

		CGM_SubDialogButton *pSubDlgButton = pItemTypeSelectDialog->AddSubDialogButton(
			subdlg_button_id[j]/*0*/,
			RectLTWH( 24, top_margin + 32 * j, 96, 28 ),
			item_title[j] );

		m_apItemButton[j] = pSubDlgButton;

//		MsgBoxFmt( "sub-dlg button created: %d", GetWeaponItemButton(j) );

		SRect sub_dlg_rect = RectLTWH(210,50,150,250);
		CGM_Dialog *pSubDlg = m_apDialogManager[0]->AddDialog(
			0,
			sub_dlg_rect,
			item_title[j] );

		// set event handler for the class
		CGM_DialogEventHandlerSharedPtr pEventHandler( new CEventHandler_ShopFG(this) );
		pSubDlg->SetEventHandler( pEventHandler );

		pSubDlgButton->SetSubDialog( pSubDlg );

		const int item_text_height = 28;
		CGM_ListBoxDesc box_desc;
		box_desc.Style = CGM_ListBox::CLOSE_DIALOG_ON_ITEM_SELECTION;
		box_desc.Rect.SetPositionLTWH( 0, 0, sub_dlg_rect.GetWidth(), sub_dlg_rect.GetHeight() );
		box_desc.Rect.Inflate( -5, -5 );
		box_desc.Rect.top += item_text_height;
		box_desc.ID = listbox_id[j];
		box_desc.nTextHeight = item_text_height;
		m_apItemListBox[j] = (CGM_ListBox *)pSubDlg->AddControl( &box_desc );

		CGM_ListBoxEventHandlerSharedPtr pItemListBoxEventHandler( new ItemListBoxEventHandler( this, j ) );
		m_apItemListBox[j]->SetEventHandler( pItemListBoxEventHandler );
	}

//	pItemTypeSelectDialog->AddButton( ID_TIS_ITEMSELECT_OK, RectLTWH( 50, 10 + 32 * j, 48, 28 ), "OK" );

	return pItemTypeSelectDialog;
}


void CGameTask_ShopFG::InitMenu()
{
//	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CAircraftCapsDisplay() );
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CFG_StdControlRendererManager() );
	m_pUIRendererManager = pRenderMgr;

	m_apDialogManager[0]	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );

	CGameTaskFG::DoCommonInit( m_apDialogManager[0] );

	CGM_Dialog *apDialog[NUM_SUB_MENUS];

	// confirmation dialog box
//	apDialog[SM_CONFIRM]
//		= Create2ChoiceDialog( m_apDialogManager[SM_CONFIRM], true,
//		"confirm", SRect( 400-80, 300-60, 400+80, 300+30 ),
//		ID_TIS_CONFIRM_OK,		"yes",	RectLTWH( 15, 60, 60, 25 ),
//		ID_TIS_CONFIRM_CANCEL,	"no",	RectLTWH( 85, 60, 60, 25 )/*,
//		0, "You're gonna go up with this one?", SRect( 400-70, 300-40, 400+70, 300-15 )*/ );

	// submenu for ammunition select
	apDialog[0] = CreateItemSelectDialog();

	m_pMoneyLeft = apDialog[0]->AddStatic( 0, RectLTWH( 50, 500, 120, 32 ), "" );

	// set input handler for dialog menu
//	m_pInputHandler = new CInputHandler_Dialog( m_apDialogManager[m_SubMenu] );
	m_pInputHandler = new CInputHandler_Debug( this, m_apDialogManager[m_SubMenu] );
	INPUTHUB.PushInputHandler( m_pInputHandler );

	// set scale for the current resolution
	int i;
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	for( i=0; i<1; i++ )
	{
		m_apDialogManager[i]->ChangeScale( scale_factor );
	}
	// open root dialog
	m_apDialogManager[0]->OpenRootDialog( ID_TIS_ITEMTYPE_SELECT_DIALOG );
}


int CGameTask_ShopFG::FrameMove( float dt )
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

	// render UI graphics
	if( m_apDialogManager[0].get() )
		m_apDialogManager[0]->Update( dt );

	if( m_pStage )
		m_pStage->Update( dt );

//	if( m_pUIRendererManager )
//		m_pUIRendererManager->Update( dt );

	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_ShopFG::UpdateCamera( float dt )
{
	if( !m_pCamera )
	{
		m_pCamera = new CCamera;
//		m_pCamera->SetPosition( Vector3(5.0f, 4.5f, -6.0f) );
//		m_pCamera->SetOrientation( Matrix33RotationY(-2.5f) );
		m_pCamera->SetPosition( Vector3(0.0f, 4.5f, 0.0f) );
	}

	static float s_Time = 0;
	s_Time += dt;
//	Vector3 vDir = Matrix33RotationY( s_Time * 0.3f ) * Vector3(0,0,1);
//	m_pCamera->SetPosition( Vector3(0.0f,4.2f,0.0f) + vDir * 18.0f );
//	m_pCamera->SetOrientation( Matrix33( Vec3Cross( Vector3(0,1,0), (-vDir) ), Vector3(0,1,0), -vDir ) );

	Vector3 vLookAtPos = Vector3(0,3,0);
	float dist = 20.0f;
	switch( m_SubMenu )
	{
/*	case SM_AIRCRAFT_SELECT:
		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
		dist = 18.0f;
		break;

	case SM_ITEMS_SELECT:
		if( m_apAmmoDisplay[m_CurrentItemType] )
		{
			vLookAtPos = m_apAmmoDisplay[m_CurrentAmmoType]->Position() + Vector3(0.0f,0.5f,0.0f);
			dist = 6.0f;
		}
		break;

	case SM_CONFIRM:
		vLookAtPos = Vector3(0.0f,4.2f,0.0f);
		dist = 25.0f;
		break;
*/
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


void CGameTask_ShopFG::Render()
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
/*			pEffectMgr->RaiseEffectFlag( ScreenEffect::PostProcessEffects );

			pEffectMgr->SetMonochromeColorOffset( -0.3f, 0.1f, -0.3f );
			pEffectMgr->RaiseEffectFlag( ScreenEffect::MonochromeColor );

			pEffectMgr->RaiseEffectFlag( ScreenEffect::GLARE );
			pEffectMgr->SetGlareLuminanceThreshold( 0.5f );
*/
//			pEffectMgr->RaiseEffectFlag( ScreenEffect::NOISE );

//			pEffectMgr->RaiseEffectFlag( ScreenEffect::PSEUDO_BLUR );
//			pEffectMgr->SetBlurEffect( 4.0f );
		}

		m_pStage->Render( *m_pCamera );
	}

	// render stage select dialog
	if( m_apDialogManager[m_SubMenu] )
        m_apDialogManager[m_SubMenu]->Render();

	// draw cursor
	DrawMouseCursor();
}


void CGameTask_ShopFG::ReleaseGraphicsResources()
{
}


void CGameTask_ShopFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	InitMenu();
}


void CGameTask_ShopFG::OnAmmoTypeFocusChanged( int ammo_type )
{
	//m_CurrentAmmoType = ammo_type;

	//CGM_ListBoxItem *pFocusedItem = m_apItemListBox[ammo_type]->GetSelectedItem();
	//if( pFocusedItem )
	//	UpdateAmmoDescDisplay( *(CGI_Ammunition *)(pFocusedItem->pUserData) );
}


void CGameTask_ShopFG::OnItemFocusChanged( int item_type )
{
//	UpdateItemDisplay( item_type );
}


void CGameTask_ShopFG::UpdatePlayerMoneyLeftDisplay()
{
	if( m_pMoneyLeft )
		m_pMoneyLeft->SetText( fmt_string( "Money: %09d", PLAYERINFO.GetMoneyLeft() ) );
}


void CGameTask_ShopFG::OnItemSelected( int item_type, const std::string& item_name )
{
	if( item_type != 0 )
		return;

	CGameItemShop &shop = *m_vecpShop[item_type];

	CPlayerCustomer player_customer;
	shop.PurchaseItem( player_customer, item_name );

/*	if( item.GetPrice() <= m_pCustomer->m_Money )
	{
		m_pCustomer->m_Money -= itme.GetPrice();
		m_pCustomer->AddItem();
	}
*/

	UpdatePlayerMoneyLeftDisplay();
}


void CGameTask_ShopFG::UpdateItemDescDisplay( CGameItem& item )
{
}


void CGameTask_ShopFG::UpdateAmmunitionDisplay( int ammo_type )
{
	//if( !m_apAmmoDisplay[ammo_type] )
	//	return;

	//CBE_GeneralEntity* pBaseEntity = (CBE_GeneralEntity *)(m_apAmmoDisplay[ammo_type]->pBaseEntity);

	//CGM_ListBoxItem *pFocusedItem = m_apItemListBox[ammo_type]->GetSelectedItem();
	//if( pFocusedItem )
	//{
	//	CGI_Ammunition& ammo = *(CGI_Ammunition *)(pFocusedItem->pUserData);

	//	pBaseEntity->MeshProperty().Release();
	//	pBaseEntity->MeshProperty() = SBE_MeshObjectProperty( ammo.GetMeshObjectHolder().m_str3DModelFileName );
	//	pBaseEntity->MeshProperty().LoadMeshObject();

	//	UpdateAmmoDescDisplay( ammo );
	//}

}


void CGameTask_ShopFG::OnAircraftChanged( int index )
{
}


void CGameTask_ShopFG::OnAircraftSelected()
{
}


void CGameTask_ShopFG::SetNextSubMenu( int next_submenu_id )
{
	//m_NextSubMenu = next_submenu_id;

	//if( m_NextSubMenu == SM_ITEMS_SELECT )
	//{
	//	CGM_SubDialogButton *pButton
	//		= (CGM_SubDialogButton *)m_apDialogManager[SM_ITEMS_SELECT]->GetControl( ID_TIS_ITEMSELECT_DIALOG_BUTTON );

	//	if( !pButton->IsSubDialogOpen() )
	//		pButton->OpenDialog();
	//}
}
