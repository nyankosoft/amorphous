#include "GameTask_MainMenu.hpp"
#include "App/GameApplicationBase.hpp"
#include "App/GameWindowManager.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/Font/BuiltinFonts.hpp"
#include "Graphics/GraphicsElementManager.hpp"
#include "GUI.hpp"
#include "Input/InputHub.hpp"
#include "Stage/Stage.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


#define CONFIG_STATE_RETURN_TO_GAME	0
#define CONFIG_STATE_IN_MENU		1

static int gs_ConfigState = CONFIG_STATE_IN_MENU;

#define CREATE_DEBUG_SUBMENU

static int s_ButtonHeight = 32;

enum eCaptionID
{
	ID_CAP_LOAD = 0,
	ID_CAP_SAVE,
	ID_CAP_CONTINUE,
	ID_CAP_CONTROL,
	ID_CAP_SETTINGS,
	ID_CAP_EXIT,
	NUM_CAPTION_IDS
};

CGameTask::eGameTask CGameTask_MainMenu::ms_NextTaskID;


CGameTask_MainMenu::CGameTask_MainMenu()
{
//	m_pFlowCaptionRenderRoutine = NULL;

	CGameTask_MainMenu::SetNextTaskID( CGameTask::ID_INVALID );

	InitMenu();

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_pDialogManager );

	int input_handler_index = 1;
	if( InputHub().GetInputHandler(input_handler_index) )
		InputHub().GetInputHandler(input_handler_index)->AddChild( m_pInputHandler );
	else
		InputHub().PushInputHandler( input_handler_index, m_pInputHandler );
}


CGameTask_MainMenu::~CGameTask_MainMenu()
{
	InputHub().RemoveInputHandler( 1, m_pInputHandler );

	SafeDelete( m_pInputHandler );
//	SafeDelete( m_pFlowCaptionRenderRoutine );
}


//void LoadCaptionText( vector<string>& strCaption );
#define array_size(fixed_array)	sizeof(fixed_array) / sizeof(fixed_array[0])


CGM_Dialog *CGameTask_MainMenu::CreateControlMenu()
{
	string button_title[] = { "INPUT", "GAMEPAD", "KEYBOARD", "MOUSE" };
	CGM_DialogDesc dlg_desc;
	CGM_SubDialogButtonDesc subdlg_button;
	CGM_SubDialogButton *apSubDlgButton[array_size(button_title)];

	// create the sub menu
	dlg_desc.Rect.SetPositionLTWH( 140, 10, 120, 200 );
	dlg_desc.strTitle = "CONTROL";
	CGM_Dialog *pDialog = m_pDialogManager->AddDialog( dlg_desc );

	for( int i=0; i<array_size(button_title); i++ )
	{
		apSubDlgButton[i] = pDialog->AddSubDialogButton(
			0,
			RectLTWH( 10, 32 + (s_ButtonHeight+8) * i, 100, s_ButtonHeight ),
			button_title[i] );
	}

//	apSubDlgButton[0]->SetSubDialog( CreateInputDeviceSelectDialog() );
//	apSubDlgButton[1]->SetSubDialog( GamepadBindDialog() );

	// create input sub-dialog, which decides input device for the game
	CGM_Dialog *pInputDevDlg = m_pDialogManager->AddDialog( 0, RectLTWH( 280, 10, 186, 170 ), "INPUT" );

	string input_dev_title[] = { "GAMEPAD", "KEYBOARD", "KBD.+MOUSE" };
	for( int i=0; i<array_size(input_dev_title); i++ )
	{
		pInputDevDlg->AddRadioButton(
			0/*INPUT_DEV_ID*/,
			RectLTWH( 8, 32 + (s_ButtonHeight+8) * i, 164, s_ButtonHeight ),
			0,
			input_dev_title[i] );
	}
	apSubDlgButton[0]->SetSubDialog( pInputDevDlg );

	return pDialog;
}


CGM_Dialog *CGameTask_MainMenu::CreateOptionMenu()
{
	string button_title[] = { "VIDEO", "SOUND" };
	CGM_DialogDesc dlg_desc;
	CGM_SubDialogButtonDesc subdlg_button;
	CGM_SubDialogButton *apSubDlgButton[array_size(button_title)];

	// create the sub menu
	dlg_desc.Rect.SetPositionLTWH( 140, 10, 120, 110 );
	dlg_desc.strTitle = "OPTION";
	CGM_Dialog *pDialog = m_pDialogManager->AddDialog( dlg_desc );

	for( int i=0; i<array_size(button_title); i++ )
	{
		subdlg_button.strText = button_title[i];
		subdlg_button.Rect.SetPositionLTWH( 10, 32 + (s_ButtonHeight+8) * i, 100, s_ButtonHeight/*24*/ );

		apSubDlgButton[i] = (CGM_SubDialogButton *)pDialog->AddControl( &subdlg_button );
	}

	apSubDlgButton[0]->SetSubDialog( CreateVideoOptionMenu() );
//	apSubDlgButton[1]->SetSubDialog( CreateSoundOptionMenu() );

	return pDialog;
}


CGM_Dialog *CGameTask_MainMenu::CreateVideoOptionMenu()
{
	string button_title[] = { "RESOLUTION", "EFFECT" };
	CGM_DialogDesc dlg_desc;
	CGM_SubDialogButtonDesc subdlg_button;
	CGM_SubDialogButton *apSubDlgButton[array_size(button_title)];

	// create the sub menu
	dlg_desc.Rect.SetPositionLTWH( 280, 10, 120, 110 );
	dlg_desc.strTitle = "VIDEO";
	CGM_Dialog *pDialog = m_pDialogManager->AddDialog( dlg_desc );

	for( int i=0; i<array_size(button_title); i++ )
	{
		subdlg_button.strText = button_title[i];
		subdlg_button.Rect.SetPositionLTWH( 10, 10 + (s_ButtonHeight+8) * i, 100, s_ButtonHeight );

		apSubDlgButton[i] = (CGM_SubDialogButton *)pDialog->AddControl( &subdlg_button );
	}

	apSubDlgButton[1]->SetSubDialog( CreateScreenResolutionSelectMenu() );
//	apSubDlgButton[0]->SetSubDialog( CreateVideoOptionMenu() );

	return pDialog;
}


CGM_Dialog *CGameTask_MainMenu::CreateScreenResolutionSelectMenu()
{
	// --- video option sub dialog
	CGM_Dialog *apVideoOptSubDlg[2];
	int btn_width = 100;
	int btn_wmargin = 8;
	CGM_DialogDesc subdlg_desc[2];
/*	dlg_desc.SetDefault();
	dlg_desc.Rect = RectLTWH( 50, 80, btn_width + btn_wmargin*2, (24 + 8) * 6 + 8 + 16);
	dlg_desc.bRootDialog = false;
	dlg_desc.pBackgroundRectElement = &rect_element; 
	dlg_desc.pEventHandler = NULL;	// holds only the subdialog buttons for each video option
	CGM_Dialog *pVideoOptionDlg = m_pDialogManager->AddDialog( dlg_desc );
*/
	// --- resolution & effect option sub dialog
	string video_opt_text[2]= { "RESOLUTION", "EFFECT" };
	int video_opt_id[2]		= { ID_UI_VOP_RESOLUTION, ID_UI_VOP_EFFECT };
//	int dlg_btn_x = btn_wmargin;
//	int dlg_btn_y = btn_wmargin;
	int dlg_x = 50 + btn_width + btn_wmargin*2 + 4;
	int dlg_y = 80;
	int i;
	for( i=0; i<1; i++ )
	{
		subdlg_desc[i].SetDefault();
		subdlg_desc[i].Rect = RectLTWH( dlg_x, dlg_y, 240, 320 );
		subdlg_desc[i].bRootDialog = false;
		subdlg_desc[i].strTitle = video_opt_text[i];
//		subdlg_desc[i].pEventHandler = ResolutionChangeEventHandler;

		apVideoOptSubDlg[i] = m_pDialogManager->AddDialog( subdlg_desc[i] );
/*
		pVideoOptionDlg->AddSubDialogButton(
			video_opt_id[i],
			RectLTWH( btn_wmargin, btn_wmargin, btn_width, 24 ),
			video_opt_text[i],
			apVideoOptSubDlg[i]
			);*/
	}

	string res_title[4];
	int res_id[4];
	res_title[0] = " 640x480";	res_id[0] = ID_UI_RESOLUTION_640X480;
	res_title[1] = " 800x600";	res_id[1] = ID_UI_RESOLUTION_800X600;
	res_title[2] = "1024x768";	res_id[2] = ID_UI_RESOLUTION_1024X768;
	res_title[3] = "1280x960";	res_id[3] = ID_UI_RESOLUTION_1280X960;
	for( i=0; i<4; i++ )
	{
		// add control to the resolution setting dlg
		apVideoOptSubDlg[0]->AddRadioButton(
			res_id[i],
			RectLTWH( btn_wmargin, btn_wmargin + i * 32, btn_width, 24 ),
			0, /* button group for resolution radio buttons */
			res_title[i]
			);	// add control to the resolution setting dlg
	}

	// 'full screen' check box
	apVideoOptSubDlg[0]->AddCheckBox( ID_UI_RESOLUTION_FULLSCREEN,	RectLTWH( btn_wmargin, btn_wmargin + i * 32, 100, 24 ), "full screen", false );
	i++;

	// 'apply changes' button
	apVideoOptSubDlg[0]->AddButton( ID_UI_RESOLUTION_APPLY,			RectLTWH( btn_wmargin, btn_wmargin + i * 32, 100, 24 ), "apply changes" );
	i++;

	return apVideoOptSubDlg[0];
}


CGM_Dialog *CGameTask_MainMenu::CreateSoundOptionMenu()
{
	return NULL;
}


#define NUM_SUB_DIALOGS		6

enum root_items
{
	ITM_RETURN,
	ITM_SAVE,
	ITM_LOAD,
	ITM_CONTROL,
	ITM_OPTION, 
	ITM_EXIT,
	ITM_DEBUG,
	NUM_ROOTMENU_ITEMS
};


void CGameTask_MainMenu::CreateMenu()
{
	CGM_DialogDesc dlg_desc;
	CGM_ButtonDesc btn_desc;
//	CGM_Button *apButton[8];
	int btn = 0;

	// create the root menu
	dlg_desc.Rect.SetPositionLTWH( 10, 10, 120, 320 );
	dlg_desc.ID = ID_UI_DLG_ROOT;
	dlg_desc.bRootDialog = true;
//	dlg_desc.pEventHandler = &m_RootMenuEventHandler;
	dlg_desc.pEventHandler = CGM_DialogEventHandlerSharedPtr( new CMainMenuEventHandler );

	CGM_Dialog *pRootDialog = m_pDialogManager->AddDialog( dlg_desc );

	// create root menu items
	string button_title[NUM_ROOTMENU_ITEMS] = { "RETURN", "SAVE", "LOAD", "CONTROL", "OPTION", "EXIT", "DEBUG" };

	CGM_SubDialogButton *apSubDlgButton[NUM_SUB_DIALOGS];
	CGM_SubDialogButtonDesc subdlg_button;

	btn_desc.ID = ID_UI_RESUME_GAME;
	btn_desc.strText = button_title[btn];
	btn_desc.Rect.SetPositionLTWH( 10, 10 + (32+8) * btn, 100, 32 );
	pRootDialog->AddControl( &btn_desc );
	btn++;

	const int btn_height = s_ButtonHeight;
	int i;
	for( i=0; i<NUM_SUB_DIALOGS-1; i++, btn++ )
	{
		subdlg_button.strText = button_title[btn];
		subdlg_button.Rect.SetPositionLTWH( 10, 10 + (btn_height+8) * btn, 100, btn_height );

		apSubDlgButton[i] = (CGM_SubDialogButton *)pRootDialog->AddControl( &subdlg_button );
	}

//	apSubDlgButton[ITM_SAVE]->SetSubDialog( CreateSaveMenu() );
//	apSubDlgButton[ITM_LOAD]->SetSubDialog( CreateLoadMenu() );
	apSubDlgButton[ITM_CONTROL-1]->SetSubDialog( CreateControlMenu() );
	apSubDlgButton[ITM_OPTION-1]->SetSubDialog( CreateOptionMenu() );

	SRect dlg_rect, btn_rect1, btn_rect2;
	dlg_rect.SetPositionLTWH( 400 - 90, 300 - 60, 180, 120 );
	btn_rect1.SetPositionLTWH( 30,       65, 50, 30 );
	btn_rect2.SetPositionLTWH( 30+50+20, 65, 50, 30 );
	CGM_Dialog *pExitDialog = Create2ChoiceDialog( m_pDialogManager, false,
		"Exit", dlg_rect,
		ID_UI_EXIT,	"Yes",	btn_rect1,
		0,			"No",	btn_rect2 );

	CGM_DialogEventHandlerSharedPtr pEventHandler( new CMainMenuEventHandler() );
	pExitDialog->SetEventHandler( pEventHandler );

	apSubDlgButton[ITM_EXIT-1]->SetSubDialog( pExitDialog );

#ifdef CREATE_DEBUG_SUBMENU
	subdlg_button.strText = button_title[btn];
	subdlg_button.Rect.SetPositionLTWH( 10, 10 + (32+8) * btn + 10, 100, 32 );
	apSubDlgButton[i] = (CGM_SubDialogButton *)pRootDialog->AddControl( &subdlg_button );
	apSubDlgButton[i]->SetSubDialog( CreateDebugMenu() );
#endif /* CREATE_DEBUG_SUBMENU */
}


void CGameTask_MainMenu::InitFont()
{
	// set font
/*	FontDesc font_desc;
//	font_desc.strFontName = "Arial";
//	font_desc.type = CGM_FontDesc::FONT_NORMAL;
	font_desc.strFontName = "Texture\\HGGE_16x8_256.dds";
	font_desc.type = FontDesc::TYPE_TEXTURE;
	font_desc.width = 12;
	font_desc.height = 24;
*/
///	if( !m_pUIRenderManager->AddFont( 0, font_desc ) )
	if( !m_pUIRenderManager->GetGraphicsElementManager()->LoadFont( 0, "Texture\\HGGE_16x8_256.dds", CFontBase::FONTTYPE_TEXTURE, 12, 24 )  )
		PrintLog( "cannot create a font. CGameTask_MainMenu::InitFont()" );
}


void CGameTask_MainMenu::InitMenu()
{
//	SafeDelete( m_pUIRenderManager );
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CGM_StdControlRendererManager );
	m_pUIRenderManager= pRenderMgr;

	m_pDialogManager.reset();
	m_pDialogManager = CGM_DialogManagerSharedPtr( new CGM_DialogManager( m_pUIRenderManager ) );

	// init font for the UI
	InitFont();

	CreateMenu();

	// init font for the background text
	InitFontForBackgroundText();

	m_pDialogManager->OpenRootDialog( ID_UI_DLG_ROOT );
}


CGM_Dialog *CGameTask_MainMenu::CreateDebugMenu()
{
//	string button_title[] = { "graphics", "physics", "sound" };

	// --- debug sub dialog
	CGM_DialogDesc dlg_desc;
	dlg_desc.Rect.SetPositionLTWH( 50 + 100 * 5 + 16 * 5, 300, 116, 24 * 4 + 8 * 5 );
	dlg_desc.strTitle = "DEBUG";
	dlg_desc.bRootDialog = false;
	CGM_Dialog *pDebugSubDlg = m_pDialogManager->AddDialog( dlg_desc );
	CGM_SubDialogButton *apDbgSubDlgBtn[4];

	int left = 8;
	int top = 8;
	apDbgSubDlgBtn[0] = pDebugSubDlg->AddSubDialogButton( ID_UI_DEBUG_RENDERING, RectLTWH( left, top, 100, 24 ), "Rendering" );
	apDbgSubDlgBtn[1] = pDebugSubDlg->AddSubDialogButton( ID_UI_DEBUG_ENTITY,    RectLTWH( left, top + 1*32, 100, 24 ), "Entity" );
	apDbgSubDlgBtn[2] = pDebugSubDlg->AddSubDialogButton( ID_UI_DEBUG_JIGLIB,    RectLTWH( left, top + 2*32, 100, 24 ), "Physics" );
	apDbgSubDlgBtn[3] = pDebugSubDlg->AddSubDialogButton( ID_UI_DEBUG_OTHERS,    RectLTWH( left, top + 3*32, 100, 24 ), "others" );

//	apDbgSubDlgBtn[0]->SetSubDialog( CreateDebugSubDialog() );
//	apDbgSubDlgBtn[1]->SetSubDialog( CreateDebugSubDialog() );
	apDbgSubDlgBtn[2]->SetSubDialog( CreatePhysicsDebugSubDialog() );
//	apDbgSubDlgBtn[3]->SetSubDialog( CreateDebugSubDialog() );

	return pDebugSubDlg;
}



CGM_Dialog *CGameTask_MainMenu::CreatePhysicsDebugSubDialog()
{
//	CGM_RadioButtonDesc radio_btn[8];
	int btn_width = 100, btn_wmargin = 8;

	// jiglib config dialog
	CGM_DialogDesc subdlg_desc;
	subdlg_desc.strTitle = "Jiggle Physics Engine";
	subdlg_desc.Rect.SetPositionLTWH( 200, 100, 300, 400 );
//	subdlg_desc.pEventHandler = Config_JigLib;
	subdlg_desc.bRootDialog = false;
	CGM_Dialog *pPhysDebugDlg = m_pDialogManager->AddDialog( subdlg_desc );

	// =============== controls for jiggle config dialog ===============

	// JigLib visualization controls
	bool phys_checked[4] = { false, false, false, false };
	int phys_id[4];
	string phys_title[4];
	phys_title[0] = " contact points";	phys_id[0] = ID_UI_PHYSSIM_CONTACT_POINTS;
	phys_title[1] = " contact normal";	phys_id[1] = ID_UI_PHYSSIM_CONTACT_NORMALS;
	phys_title[2] = " aabb";			phys_id[2] = ID_UI_PHYSSIM_AABB;

/*	if( g_pStage )
	{
//		CJL_PhysicsVisualizer *pVisualizer = g_pStage->GetPhysicsVisualizer();
//		phys_checked[0] = pVisualizer->GetRenderStateFlag() & JL_VIS_CONTACT_POINTS ? true : false;
//		phys_checked[1] = pVisualizer->GetRenderStateFlag() & JL_VIS_CONTACT_NORMALS ?  true : false;
//		phys_checked[2] = pVisualizer->GetRenderStateFlag() & JL_VIS_AABB ? true : false;
	}*/

	for( int i=0; i<3; i++ )
	{
		pPhysDebugDlg->AddCheckBox(
			phys_id[i],
			RectLTWH( 20, 45 + 32*i, 100, 32 ),
			phys_title[i],
			phys_checked[i]
			);
	}

	return pPhysDebugDlg;

//	vector<string> strCaptionText;

//	LoadCaptionText( strCaptionText );

/*
	// set font

	// set caption params
	CGM_CaptionParameters caption_params;
	caption_params.fPosX = 10 * fRatio;
	caption_params.fPosY = 10 * fRatio;
	caption_params.iFontIndex = 0;
	caption_params.FontColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );

	m_pFlowCaptionRenderRoutine = new CGM_FlowTextCaption( m_pDialogManager );
	caption_params.pCaptionRenderRoutine = m_pFlowCaptionRenderRoutine; 

	m_pDialogManager->SetCaptionParameters( caption_params );
	m_pDialogManager->EnableCaption( true );
*/
}


int CGameTask_MainMenu::FrameMove( float dt )
{
//	return CGameTask::ID_INVALID;
	return ms_NextTaskID;

}


void CGameTask_MainMenu::RenderQuickGuide()
{
	string strText[20];
	Vector2 vPos = Vector2(120, 50);

	strText[0] = "FPS Demo ver 0.xx";
	strText[1] = "---------------------";
	strText[2] = " E, D, S, F - Move";
	strText[3] = " SPACE - Up";
	strText[4] = " ALT(LEFT) - Down";
	strText[5] = " A, Q, G, V, Mouse Wheel - Weapon Change";
	strText[6] = " Right Click - Aim / Grab / Release";
	strText[7] = " Left Click - Fire / Shoot";
	strText[8] = " Screenshot - F12";

	int i;
	for( i=0; i<9; i++ )
	{
		m_pFont->DrawText( strText[i].c_str(), vPos, 0xFFA0A0A0 );
		vPos.y += 32;
	}
}


void CGameTask_MainMenu::Render()
{
	RenderQuickGuide();

	// render stage select dialog
	m_pDialogManager->Render();

//	MsgBox( "UI components has been rendered" );

	// draw cursor
	DrawMouseCursor();

//	MsgBox( "main menu UI has been rendered" );
}


void CGameTask_MainMenu::InitFontForBackgroundText()
{
	boost::shared_ptr<CTextureFont> pTexFont( new CTextureFont );
	pTexFont->InitFont( GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" ) );
	int w = 16 * GameWindowManager().GetScreenWidth() / 800;
	int h = w * 2;
	pTexFont->SetFontSize( w, h );
	
	m_pFont = pTexFont;
}


void CGameTask_MainMenu::ReleaseGraphicsResources()
{
//	m_pFont->Release();
}


void CGameTask_MainMenu::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	InitMenu();

//	m_pFont->Reload();

	// Since the screen size change is done from this main menu,
	// and the dialog manager has been destroyed and re-created
	// input handler also has to be updated.
	InputHub().RemoveInputHandler( m_pInputHandler );
	SafeDelete( m_pInputHandler );
	m_pInputHandler = new CInputHandler_Dialog( m_pDialogManager );
	InputHub().PushInputHandler( 1, m_pInputHandler );
}


void LoadCaptionText( vector<string>& strCaption )
{
	strCaption.resize( NUM_CAPTION_IDS );

	strCaption[ID_CAP_LOAD]		= "load game data";
	strCaption[ID_CAP_SAVE]		= "save current progress";
	strCaption[ID_CAP_CONTINUE]	= "return to the game";
	strCaption[ID_CAP_CONTROL]	= "customize key/mouse/gamepad controls";
	strCaption[ID_CAP_SETTINGS]	= "change display resolution";
	strCaption[ID_CAP_EXIT]		= "exit the game and return to windows";

//	strCaption[] = "";

}


} // namespace amorphous
