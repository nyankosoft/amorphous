#include "GameTask_ControlCustomizerFG.h"
#include "EventHandler_MainMenuFG.h"	// needed to derive event hander from CEventHandler_Base

#include "Task/GameTask_Stage.h"	// extern dec of g_pStage

#include "Stage/Stage.h"
#include "Stage/PlayerInfo.h"
//#include "Stage/CopyEntity.h"
//#include "Stage/BE_GeneralEntity.h"
#include "Stage/ScreenEffectManager.h"

#include "GameInput/InputHub.h"
#include "GameInput/InputHandler.h"
#include "GameInput/GICodeTitle.h"
#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"
#include "3DCommon/Font.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DMisc.h"
#include "3DCommon/RenderTaskProcessor.h"

#include "GameEvent/ScriptManager.h"
#include "GameEvent/PyModules.h"

#include "App/ApplicationBase.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"
#include "Support/macro.h"

#include "../DefaultKeybindFG.h"
#include "../UI/all.h"

using namespace std;


struct ActionDescFG
{
	int code;
	string title;

	int GICode[8];

	string desc;	///< brief description

	ActionDescFG( int _code, const string& _title )
		:
	code(_code), title(_title) {}
};


ActionDescFG g_ActionDescFG[] = {
//	ActionDescFG( ACTION_ATK_FIRE0,					"Fire(Missile)" ),
	ActionDescFG( ACTION_ATK_FIRE,					"Fire(Missile)" ),
	ActionDescFG( ACTION_ATK_FIRE1,					"Fire(Gun)" ),
	ActionDescFG( ACTION_MISC_CYCLE_WEAPON,			"Change Weapon" ),
	ActionDescFG( ACTION_MISC_CYCLE_TARGET_FOCUS,	"Change Target" ),
	ActionDescFG( ACTION_MISC_CYCLE_VIEWPOINTS,		"Change Viewpoints" ),
	ActionDescFG( ACTION_MISC_HOLD_RADAR,			"Global Radar" ),
//	ActionDescFG( ACTION_MOV_ACCEL,					"Accel" ),
	ActionDescFG( ACTION_MOV_BOOST,					"Accel" ),
	ActionDescFG( ACTION_MOV_BRAKE,					"Brake" ),
	ActionDescFG( ACTION_MOV_YAW_ACCEL_POS,			"Yaw Right" ),
	ActionDescFG( ACTION_MOV_YAW_ACCEL_NEG,			"Yaw Left" ),
	ActionDescFG( ACTION_MOV_PITCH_ACCEL_NEG,		"Pitch" ),
	ActionDescFG( ACTION_MOV_ROLL_ACCEL_NEG,		"Roll" ),
	ActionDescFG( ACTION_MOV_LOOK_UP,				"Pitch(View)" ),
	ActionDescFG( ACTION_MOV_LOOK_RIGHT,			"Heading(View)" ),
	ActionDescFG( ACTION_QMENU_OPEN,				"Pause" )
};


void LoadActionDescJp()
{
	ActionDescFG* action = g_ActionDescFG;
//	action.desc = ""; //ACTION_ATK_FIRE0,
	action[ 0].desc = "兵器1 発射（たいていはミサイル）"; //ACTION_ATK_FIRE,
	action[ 1].desc = "機銃（未実装）";	//ACTION_ATK_FIRE1,
	action[ 2].desc = "兵器を変更";		//ACTION_MISC_CYCLE_WEAPON,
	action[ 3].desc = "ターゲット変更";	//ACTION_MISC_CYCLE_TARGET_FOCUS,
	action[ 4].desc = "視点変更";		//ACTION_MISC_CYCLE_VIEWPOINTS,
	action[ 5].desc = "広域レーダー（押してる間ON）"; //ACTION_MISC_HOLD_RADAR,
//	action[].desc = ""; //ACTION_MOV_ACCEL,
	action[ 6].desc = "加速";			//ACTION_MOV_BOOST,
	action[ 7].desc = "減速";			//ACTION_MOV_BRAKE,
	action[ 8].desc = "右に旋廻";		//ACTION_MOV_YAW_ACCEL_POS,
	action[ 9].desc = "左に旋廻";		//ACTION_MOV_YAW_ACCEL_NEG,
	action[10].desc = "上下に旋廻";		//ACTION_MOV_PITCH_ACCEL_NEG,
	action[11].desc = "ロール";			//ACTION_MOV_ROLL_ACCEL_NEG,
	action[12].desc = "視点を上下する";	//ACTION_MOV_LOOK_UP,
	action[13].desc = "視点を左右に振る"; //ACTION_MOV_LOOK_RIGHT,
	action[14].desc = "？？？";			//ACTION_QMENU_OPEN,
};


class CInputHandler_ControlCustomizer : public CInputHandler
{
	CGameTask_ControlCustomizerFG* m_pTask;

	void SendAssignmentRequest( int gi_code );

public:

	CInputHandler_ControlCustomizer( CGameTask_ControlCustomizerFG* pTask )
		: m_pTask(pTask) {}

	virtual ~CInputHandler_ControlCustomizer() {}

	void ProcessInput(SInputData& input);

};


void CInputHandler_ControlCustomizer::SendAssignmentRequest( int gi_code )
{
	int input_device_index;
	if( IsGamepadInputCode(gi_code) )		input_device_index = 0;
	else if( IsKeyboardInputCode(gi_code) )	input_device_index = 1;
	else if( IsMouseInputCode(gi_code) )	input_device_index = 2;
	else									input_device_index = -1;

	m_pTask->AssignKey( gi_code, input_device_index );
}


void CInputHandler_ControlCustomizer::ProcessInput( SInputData& input )
{
	int gi_code = input.iGICode;

	float fParam = input.fParam1;

	if( IsGamepadAnalogAxisInputCode( gi_code ) )
	{
		// gamepad analog axis - discard slight shift as non-input
//		if( fabsf(input.fParam1) < 0.9f )
		if( fabsf(input.fParam1) < 850.0f )
			return;
	}
	else if( IsMouseMoveInputCode( gi_code ) )
	{
		return;
	}

	if( input.iType == ITYPE_KEY_PRESSED )
		SendAssignmentRequest(gi_code);
}



class CEH_ControlCustomizerFG : public CEventHandler_Base
{
	/// task that processes events (borrowed reference)
	CGameTask_ControlCustomizerFG *m_pTask;

public:

	CEH_ControlCustomizerFG( CGameTask_ControlCustomizerFG *pTask ) : m_pTask(pTask) {}

	virtual ~CEH_ControlCustomizerFG() {}

	virtual void HandleEvent( CGM_Event &event )
	{
		CEventHandler_Base::HandleEvent( event );

		if( !event.pControl )
			return;

		switch( event.pControl->GetID() )
		{
		case ID_TCC_DLG_ROOT_INPUTDEVICE_SELECT:
			if( event.Type == CGM_Event::DIALOG_CLOSED )
			{
				if( g_pStage )
				{
					// control customizer is launched from in-stage game menu
					// - no fade effect
					m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK, 0, 0, 0 );
				}
				else
				{
					m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );
				}
			}
			break;
		default:
			break;
		}
	}

};



class CEH_ControlList : public CGM_ListBoxEventHandler
{
	CGameTask_ControlCustomizerFG* m_pTask;

public:

	CEH_ControlList( CGameTask_ControlCustomizerFG* task )
		: m_pTask(task) {}

	virtual ~CEH_ControlList() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item )
	{
		m_pTask->OnActionSelected( item );
	}

	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item ) {}
};


static CCamera *g_pCamera = NULL;



CGameTask_ControlCustomizerFG::CGameTask_ControlCustomizerFG()
:
m_pDialogInputHandler(NULL),
m_pKeyBindInputHandler(NULL)
{
	m_CurrentActionIndex = 0;//g_ActionDescFG[0].code;

	size_t i;
	for( i=0; i<NUM_INPUT_DEVICE_TYPES; i++ )
	{
//		m_apItemButton[i] = NULL;
		m_apItemListBox[i] = NULL;
	}


	// TODO: do not load default key binds when the player has loaded saved data
	// 13:43 2007/07/14
//	ONCE( SetDefaultKeyBindFG() );


	// create dialog menu for selecting stages
	InitMenu();

	// load stage for aircraft select task
//	InitStage();

/*
	CScriptManager event_mgr;
	event_mgr.AddModule( "PlayerInfo",	g_PyModulePlayerMethod );
	event_mgr.AddModule( "Shop",		g_PyModuleShopMethod );
	event_mgr.LoadScriptArchiveFile( "Script/shop.bin" );
	event_mgr.Update();
*/
//	UpdateItemListBox();
}


CGameTask_ControlCustomizerFG::~CGameTask_ControlCustomizerFG()
{
//	INPUTHUB.PopInputHandler();
	SafeDelete( m_pDialogInputHandler );
	SafeDelete( m_pKeyBindInputHandler );

//	SafeDelete( m_pStage );

	SafeDelete( g_pCamera );
}


string GetControlTitle( int action_index, int gi_code )
{
	string gi_code_title = ( 0 <= gi_code ) ? g_GeneralInputCodeTitle[gi_code] : "";

	ActionDescFG& ActionDesc = g_ActionDescFG[action_index];

	string spaces = "                         ";

	string title
		= ActionDesc.title
		+ spaces.substr( 0, 20 - ActionDesc.title.length())
		+ gi_code_title;

	return title;
}


void CGameTask_ControlCustomizerFG::OnActionSelected( CGM_ListBoxItem& item )
{
	INPUTHUB.PushInputHandler( m_pKeyBindInputHandler );

	SetState( STATE_ASSIGNING_KEY );
}


void CGameTask_ControlCustomizerFG::ClearPreviousGICode( int gi_code, int input_device_index )
{
	int i, num_actions = sizeof(g_ActionDescFG) / sizeof(ActionDescFG);
	for( i=0; i<num_actions; i++ )
	{
		ActionDescFG& action = g_ActionDescFG[i];

		if( action.GICode[input_device_index * 2] == gi_code )
		{
			action.GICode[input_device_index * 2] = -1;

			CGM_ListBoxItem* pItem = m_apItemListBox[input_device_index]->GetItem( i );
			pItem->SetText( GetControlTitle( i, -1 ) );
			pItem->SetDesc( action.desc );

			return;
		}
	}
}

bool IsValidInputForAction( int gi_code, int action_code )
{
	if( GIC_GPD_BUTTON_00 <= gi_code && gi_code <= GIC_GPD_BUTTON_11
	 && ( action_code == ACTION_MOV_PITCH_ACCEL_NEG || action_code == ACTION_MOV_ROLL_ACCEL_NEG ) )
	{
		// don't assign button input to pitch / roll control
		return false;
	}

	return true;
}


void CGameTask_ControlCustomizerFG::AssignKey( int gi_code, int input_device_index )
{
	if( input_device_index < 0 )
		return;

	m_CurrentActionIndex = m_apItemListBox[input_device_index]->GetSelectedItemIndex();

	ActionDescFG& ActionDesc = g_ActionDescFG[m_CurrentActionIndex];

	// return to the normal input handler
//	INPUTHUB.SetInputHandler( m_pDialogInputHandler );
	INPUTHUB.PopInputHandler();

	if( IsValidInputForAction( gi_code, ActionDesc.code ) )
	{
//		m_PlayerKeyBind[gi_code] = m_CurrentAction;
//		m_PlayerKeyBind.Assgin( gi_code, ActionDesc.code );
		PLAYERINFO.KeyBind().Assign( gi_code, ActionDesc.code );

		ClearPreviousGICode( gi_code, input_device_index );

		ActionDesc.GICode[input_device_index * 2] = gi_code;

		// update UI control
		CGM_ListBoxItem *pItem = m_apItemListBox[input_device_index]->GetItem( m_CurrentActionIndex );

//		pItem->text = ActionDesc.action_title + string(" : ") + g_GeneralInputCodeTitle[gi_code];
		pItem->SetText( GetControlTitle( m_CurrentActionIndex, gi_code ) );
	}

	SetState( STATE_WAITING_INPUT );

}


CGM_Dialog *CGameTask_ControlCustomizerFG::CreateKeyBindDialog()
{
	CGM_Dialog *pItemTypeSelectDialog = m_apDialogManager[0]->AddRootDialog(
		ID_TCC_DLG_ROOT_INPUTDEVICE_SELECT,
		RectAtLeftTop( 300, 300, 100, 100 ),
		"customize" );

	// create the event handler shared by all the dialogs related to the control customizer
	CGM_DialogEventHandlerSharedPtr pEventHandler( new CEH_ControlCustomizerFG(this) );

	pItemTypeSelectDialog->SetEventHandler( pEventHandler );

	CGM_SubDialogButtonDesc sdlg_btn_desc;
	string item_title[3] = { "GAMEPAD",          "KEYBOARD",         "MOUSE" };
	int subdlg_btn_id[3] = { ID_TCC_GAMEPAD,     ID_TCC_KEYBOARD,    ID_TCC_MOUSE };
	int listbox_id[3]    = { ID_TCC_LISTBOX_GPD, ID_TCC_LISTBOX_KBD, ID_TCC_LISTBOX_MSE };
	int top_margin = 40;
	for( int j=0; j<3/*NUM_INPUTDEVICE_TYPES*/; j++ )
	{
		CGM_SubDialogButton *pSubDlgButton = 
			pItemTypeSelectDialog->AddSubDialogButton(
			subdlg_btn_id[j],
			RectLTWH( 44, top_margin + 64 * j, 212, 56 ),
			item_title[j] );

//		m_apItemButton[j] = pSubDlgButton;

//		MsgBoxFmt( "sub-dlg button created: %d", GetWeaponItemButton(j) );

		SRect sub_dlg_rect = RectAtCenter(grof(640),640);
		CGM_Dialog *pSubDlg = m_apDialogManager[0]->AddDialog(
			0,
			sub_dlg_rect,
			item_title[j] );

		// set event handler for the class
		pSubDlg->SetEventHandler( pEventHandler );

		pSubDlgButton->SetSubDialog( pSubDlg );

		const int item_text_height = 54;
		CGM_ListBoxDesc box_desc;
		box_desc.Style = 0;//CGM_ListBox::CLOSE_DIALOG_ON_ITEM_SELECTION;
		box_desc.Rect.SetPositionLTWH( 0, 0, sub_dlg_rect.GetWidth(), sub_dlg_rect.GetHeight() );
		box_desc.Rect.Inflate( -12, -12 );
		box_desc.Rect.top += item_text_height;
		box_desc.ID = listbox_id[j];
		box_desc.nTextHeight = item_text_height;
		m_apItemListBox[j] = (CGM_ListBox *)pSubDlg->AddControl( &box_desc );

		CGM_ListBoxEventHandlerSharedPtr pListBoxEventHandler( new CEH_ControlList(this) );
		m_apItemListBox[j]->SetEventHandler( pListBoxEventHandler );
	}

/*
	// add key:action pairs
	string title;
	int num_actions = sizeof(g_ActionDescFG) / sizeof(ActionDescFG);
	for( int j=0; j<num_actions; j++ )
	{
		ActionDescFG& action = g_ActionDescFG[i];

//		title = GetControlTitle( i, PLAYERINFO.KeyBind().FindInputCode(action.code) );
		title = GetControlTitle( i, PLAYERINFO.KeyBind().FindGamepadInputCode(action.code) );

		m_apItemListBox[0]->AddItem( title, NULL );
	}
*/

	// load descriptions for controls
	LoadActionDescJp();

	// add key:action pairs
	int input_type[3] = { CKeyBind::GAMEPAD, CKeyBind::KEYBOARD, CKeyBind::MOUSE };
	int num_actions = sizeof(g_ActionDescFG) / sizeof(ActionDescFG);
	string title;
	int gi_code;
	for( int i=0; i<3; i++ )
	{
		for( int j=0; j<num_actions; j++ )
		{
			ActionDescFG& action = g_ActionDescFG[j];

			gi_code = PLAYERINFO.KeyBind().FindInputCode(action.code,input_type[i]);
			action.GICode[i * 2] = gi_code;

			title = GetControlTitle( j, gi_code );

			m_apItemListBox[i]->AddItem( title, NULL, 0, action.desc );
		}
	}

//	pItemTypeSelectDialog->AddButton( ID_TCC_ITEMSELECT_OK, RectLTWH( 50, 10 + 32 * j, 48, 28 ), "OK" );

	return pItemTypeSelectDialog;
}


void CGameTask_ControlCustomizerFG::InitMenu()
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CFG_StdControlRendererManager );
	m_pUIRendererManager = pRenderMgr;

	CGameTaskFG::LoadFonts( m_pUIRendererManager );

	m_apDialogManager[0]	= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager) );
	//m_apDialogManager[SM_ITEMS_SELECT]		= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager );
	//m_apDialogManager[SM_CONFIRM]			= CGM_DialogManagerSharedPtr( new CGM_DialogManager(m_pUIRendererManager );

	// add font to UI render manager
	// set sounds
	CGameTaskFG::DoCommonInit( m_apDialogManager[0] );

	CGM_Dialog *apDialog[NUM_SUB_MENUS];

	// confirmation dialog box
//	apDialog[SM_CONFIRM]
//		= Create2ChoiceDialog( m_apDialogManager[SM_CONFIRM], true,
//		"confirm", SRect( 400-80, 300-60, 400+80, 300+30 ),
//		ID_TCC_CONFIRM_OK,		"yes",	RectLTWH( 15, 60, 60, 25 ),
//		ID_TCC_CONFIRM_CANCEL,	"no",	RectLTWH( 85, 60, 60, 25 )/*,
//		0, "You're gonna go up with this one?", SRect( 400-70, 300-40, 400+70, 300-15 )*/ );

	apDialog[0] = CreateKeyBindDialog();

//	apDialog[SM_ITEMS_SELECT]->SetEventHandler( m_apEventHandler[SM_ITEMS_SELECT] );
//	apDialog[SM_CONFIRM]->SetEventHandler( m_apEventHandler[SM_CONFIRM] );

	// set input handler for dialog menu
	m_pDialogInputHandler = new CInputHandler_Dialog( m_apDialogManager[0] );
	INPUTHUB.PushInputHandler( m_pDialogInputHandler );

	m_pKeyBindInputHandler = new CInputHandler_ControlCustomizer(this);

	// set scale for the current resolution
/*	int i;
	float scale_factor = (float)GetScreenWidth() / 800.0f;
	for( i=0; i<1; i++ )
	{
		m_apDialogManager[i]->ChangeScale( scale_factor );
	}*/

	m_apDialogManager[0]->OpenRootDialog( ID_TCC_DLG_ROOT_INPUTDEVICE_SELECT );
}


void CGameTask_ControlCustomizerFG::ProcessTaskTransitionRequest()
{
	if( GetRequestedNextTaskID() != ID_INVALID )
	{
		// pop input handlers used by this task
		for( int i=0; i<2; i++ )
		{
			if( INPUTHUB.GetInputHandler() == m_pKeyBindInputHandler
			 || INPUTHUB.GetInputHandler() == m_pDialogInputHandler )
			{
				INPUTHUB.PopInputHandler();
			}
		}

	}

	CGameTask::ProcessTaskTransitionRequest();
}


int CGameTask_ControlCustomizerFG::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( m_apDialogManager[0].get() )
        m_apDialogManager[0]->Update( dt );

//	if( m_RequestedNextTaskID != ID_INVALID )
//	{
//		INPUTHUB.PopInputHandler();
//	}

/*	if( m_ReqState == STATE_ASSIGNING_KEY )
	{
		INPUTHUB.SetInputHandler( m_pKeyBindInputHandler );
	}
*/

/*

//	if( m_NextSubMenu != SM_INVALID )
//	{
//		m_pDialogInputHandler->SetDialogManager( m_apDialogManager[m_NextSubMenu] );
//		m_SubMenu = m_NextSubMenu;
//		m_NextSubMenu = SM_INVALID;
//	}
*/
//	if( m_pStage )
//		m_pStage->Update( dt );

//	UpdateCamera( dt );

	return CGameTask::ID_INVALID;
}


void CGameTask_ControlCustomizerFG::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render the stage (aircraft, missile, etc.)
	//if( m_pStage )
	//{
	//	m_pStage->Render( *g_pCamera );
	//}

	if( g_pStage )
	{
		// global stage is loaded - render on background
		g_pStage->Render();

		C2DRect rect = C2DRect( 0, 0, GetScreenWidth(), GetScreenHeight(), 0xA0000000 );
		rect.Draw();
	}

	// render stage select dialog
	if( m_apDialogManager[0] )
        m_apDialogManager[0]->Render();

	if( GetState() == STATE_ASSIGNING_KEY )
	{
		int w,h;
		GetViewportSize( w, h );
		C2DRect overlay_rect( 0,0,w,h );
		overlay_rect.SetColor( 0x50000000 );
		overlay_rect.Draw();
	}
}


void CGameTask_ControlCustomizerFG::CreateRenderTasks()
{
	if( g_pStage )
		g_pStage->CreateRenderTasks();

	RenderTaskProcessor.AddRenderTask( new CGameTaskRenderTask( this ) );
}


void CGameTask_ControlCustomizerFG::ReleaseGraphicsResources()
{
}


void CGameTask_ControlCustomizerFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	InitMenu();
}

/*
void CGameTask_ControlCustomizerFG::InitStage()
{
	SafeDelete( m_pStage );
	m_pStage = new CStage;
	m_pStage->Initialize( "Script/tsk_cc.bin" );

	// get entities - item display entity & scripted camera entity
	m_pItemDisplay	= m_pStage->GetEntitySet()->GetEntityByName( "aircraft_display" );
	m_apAmmoDisplay[AMMO_MISSILE]	= m_pStage->GetEntitySet()->GetEntityByName( "ammo_display0" );
	m_pCamera	= m_pStage->GetEntitySet()->GetEntityByName( "cam" );

	m_pStage->GetEntitySet()->SetCameraEntity( m_pCamera );

	m_apAmmoDisplay[AMMO_MISSILE]->SetPosition( Vector3( 2.0f, 1.0f, 13.5f ) );
	m_apAmmoDisplay[AMMO_SPW]->SetPosition( Vector3(-2.0f, 1.0f, 13.5f ) );
}
*/