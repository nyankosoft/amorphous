
#include "GameTask_Settings.h"
#include "GameTask_Stage.h"
#include "App/ApplicationBase.h"

#include "Graphics/Direct3D9.h"
#include "Graphics/2DRect.h"
#include "Graphics/font.h"

#include "Stage/stage.h"
//#include "Stage/EntitySet.h"
#include "Stage/PlayerInfo.h"
#include "Stage/BE_PlayerShip.h"
//#include "Stage/ScreenEffectManager.h"
//#include "Stage/ScreenEffectManager.h"

#include "UI/ui.h"

#include "GameInput/InputHub.h"
#include "GameInput/InputHandler_Dialog.h"
#include "GameInput/DirectInputMouse.h"

#include "JigLib/PhysicsManager_Visualizer.h"

//#include "Sound/GameSoundManager.h"

#include "Support/memory_helpers.h"
//#include "Support/Profile.h"

#define CONFIG_STATE_RETURN_TO_GAME	0
#define CONFIG_STATE_IN_MENU		1

static int gs_ConfigState = CONFIG_STATE_IN_MENU;


#define ID_UI_JL_CONTACT_POINTS		1000
#define ID_UI_JL_CONTACT_NORMALS	1001
#define ID_UI_JL_AABB				1002
#define ID_UI_JL_VELOCITY			1003
#define ID_UI_JL_ANGULAR_VELOCITY	1004

#define ID_UI_CONFIG_CONTROL	10
#define ID_UI_CONFIG_JIGLIB	11
#define ID_UI_CONFIG_SOUND		12
#define ID_UI_CONFIG_VIDEO		13

#define ID_UI_EXIT			2000


void Config_JigLib( eGameMenuEventType event, int iControlID, CGM_Control* pControl )
{
	CPhysicsManager_Visualizer *pVisualizer = g_pStage->GetPhysicsVisualizer();
	CGM_CheckBox *pBox;

	switch( iControlID )
	{
	case ID_UI_JL_CONTACT_POINTS:
		if( event == GM_EVENT_CHECKBOX_CHANGED )
		{
			pBox = (CGM_CheckBox *)pControl;
			if( pBox->IsChecked() )
				pVisualizer->RaiseRenderStateFlag( JL_VIS_CONTACT_POINTS );
			else
				pVisualizer->ClearRenderStateFlag( JL_VIS_CONTACT_POINTS );
		}
		break;

	case ID_UI_JL_CONTACT_NORMALS:
		if( event == GM_EVENT_CHECKBOX_CHANGED )
		{
			pBox = (CGM_CheckBox *)pControl;
			if( pBox->IsChecked() )
				pVisualizer->RaiseRenderStateFlag( JL_VIS_CONTACT_NORMALS );
			else
				pVisualizer->ClearRenderStateFlag( JL_VIS_CONTACT_NORMALS );
		}
		break;

	case ID_UI_JL_AABB:
		if( event == GM_EVENT_CHECKBOX_CHANGED )
		{
			pBox = (CGM_CheckBox *)pControl;
			if( pBox->IsChecked() )
				pVisualizer->RaiseRenderStateFlag( JL_VIS_AABB );
			else
				pVisualizer->ClearRenderStateFlag( JL_VIS_AABB );
		}
		break;

	case ID_UI_JL_VELOCITY:
		if( event == GM_EVENT_CHECKBOX_CHANGED )
		{
			pBox = (CGM_CheckBox *)pControl;
			if( pBox->IsChecked() )
				pVisualizer->RaiseRenderStateFlag( JL_VIS_VELOCITY );
			else
				pVisualizer->ClearRenderStateFlag( JL_VIS_VELOCITY );
		}
		break;

	case ID_UI_JL_ANGULAR_VELOCITY:
		if( event == GM_EVENT_CHECKBOX_CHANGED )
		{
			pBox = (CGM_CheckBox *)pControl;
			if( pBox->IsChecked() )
				pVisualizer->RaiseRenderStateFlag( ID_UI_JL_ANGULAR_VELOCITY );
			else
				pVisualizer->ClearRenderStateFlag( ID_UI_JL_ANGULAR_VELOCITY );
		}
		break;
	}
}


CGameTask_Settings::CGameTask_Settings()
{
	int i;
	float fBaseWidth = 800.0f;
	float fWindowWidth = 800.0f;
	float fRatio = fWindowWidth / fBaseWidth;
	CGM_Dialog *apSubDialog[4] = { NULL, NULL, NULL, NULL };


	m_pDialogManager = new CGM_DialogManager;

	string strButton[7];
	strButton[0] = "Control";
	strButton[1] = "Jiggle";
	strButton[2] = "";
	strButton[3] = "Exit";

	strButton[4] = " contact points";
	strButton[5] = " contact normal";
	strButton[6] = " aabb";

	CGM_FontElement font_element;
	font_element.m_iFontIndex = 0;
	font_element.m_FontColor.m_aColor[GM_STATE_NORMAL   ].SetRGBA( 0.9f,  0.9f,  0.9f, 1.0f );
	font_element.m_FontColor.m_aColor[GM_STATE_MOUSEOVER].SetRGBA( 0.2f,  0.2f,  0.9f, 1.0f );
	font_element.m_FontColor.m_aColor[GM_STATE_PRESSED  ].SetRGBA( 0.25f, 0.25f, 1.0f, 1.0f );

	// jiglib config dialog
	CGM_DialogDesc dlg_desc;
	dlg_desc.fPosX   = ((float)(200)) * fRatio;
	dlg_desc.fPosY   = ((float)(100)) * fRatio;
	dlg_desc.fWidth  = ((float)(300)) * fRatio;
	dlg_desc.fHeight = ((float)(440)) * fRatio;
	dlg_desc.bRootDialog = false;
	apSubDialog[1] = m_pDialogManager->AddDialog( dlg_desc );

	// =============== controls for jiggle config dialog ===============
	// title
	CGM_StaticDesc static_desc;
	static_desc.strText = "Jiggle Physics Engine";
	static_desc.fPosX   = ((float)(220)) * fRatio;
	static_desc.fPosY   = ((float)(108)) * fRatio;
	static_desc.fWidth  = ((float)(250)) * fRatio;
	static_desc.fHeight = ((float)( 24)) * fRatio;
	apSubDialog[1]->AddControl( &static_desc );

	// SDK visualization controls
	CGM_CheckBoxDesc check_desc[8];
	check_desc[0].ID = ID_UI_JL_CONTACT_POINTS;
	check_desc[1].ID = ID_UI_JL_CONTACT_NORMALS;
	check_desc[2].ID = ID_UI_JL_AABB;
	for( i=0; i<3; i++ )
	{
		check_desc[i].fPosX   = ((float)(220)) * fRatio;
		check_desc[i].fPosY   = ((float)(145 * i * 20)) * fRatio;
		check_desc[i].fWidth  = ((float)(150)) * fRatio;
		check_desc[i].fHeight = ((float)(18)) * fRatio;
		check_desc[i].strText = strButton[4 + i];
		check_desc[i].pDefaultFontElement = &font_element;
		apSubDialog[1]->AddControl( &check_desc[i] );
	}


	// create root dialog at the bottom of the screen
	CGM_Dialog *pRootDialog;
	dlg_desc.SetDefault();
	dlg_desc.fPosX   = ((float)(42)) * fRatio;
	dlg_desc.fPosY   = ((float)(520)) * fRatio;
	dlg_desc.fWidth  = ((float)(8 + 96 * 4 + 16 * 3 + 8)) * fRatio;
	dlg_desc.fHeight = ((float)(40)) * fRatio;
	dlg_desc.bRootDialog = true;
	pRootDialog = m_pDialogManager->AddDialog( dlg_desc );

	// create buttons for the root dialog
	CGM_SubDialogButtonDesc subdlg_button[4];
	subdlg_button[0].ID = ID_UI_CONFIG_CONTROL;
	subdlg_button[1].ID = ID_UI_CONFIG_JIGLIB;
	subdlg_button[2].ID = 0;
	subdlg_button[3].ID = ID_UI_EXIT;

	for( i=0; i<4; i++ )
	{
		subdlg_button[i].pSubDialog = apSubDialog[i];

		subdlg_button[i].fPosX =   ((float)(50 + (96 + 16) * i)) * fRatio;
		subdlg_button[i].fPosY =   ((float)(528)) * fRatio;
		subdlg_button[i].fWidth =  ((float)(96)) * fRatio;
		subdlg_button[i].fHeight = ((float)(24)) * fRatio;

		subdlg_button[i].strText = strButton[i];
		subdlg_button[i].pDefaultFontElement = &font_element;
		pRootDialog->AddControl( &subdlg_button[i] );
	}

	// set font
	CGM_FontDesc font_desc;
	font_desc.height = 16;
	font_desc.width  = 8;
	font_desc.strFontName = "Arial";
	m_pDialogManager->AddFont( 0, font_desc );
//	CFont *pFont = new CFont;
//	pFont->InitFont( "Arial", 8, 16 );
//	m_pDialogManager->SetFont( 0, pFont );

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_pDialogManager );
	INPUTHUB.SetInputHandler( m_pInputHandler );
}


/*
	CGM_DialogDesc dialog_desc;

	int i, j;
	for( i=0; i<4; i++ )
	{
		dialog_desc.SetDefault();
		dialog_desc.fPosX   = ((float)(42 + (96 + 16) * i)) * fRatio;
		dialog_desc.fPosY   = ((float)(376)) * fRatio;
		dialog_desc.fWidth  = ((float)(96 + 16)) * fRatio;
		dialog_desc.fHeight = ((float)(136)) * fRatio;

		// create child dialog
		apSubDialog[i] = g_pDialogManager->AddDialog( dialog_desc );

		for( j=0; j<4; j++ )
		{
//			subdlg_button.SetDefault();
			subdlg_button[j].fPosX   = ((float)(50 + (96 + 16) * i)) * fRatio;
			subdlg_button[j].fWidth  = ((float)(96)) * fRatio;

			subdlg_button[j].fPosY   = ((float)(376 + 8 + (24 + 8) * j)) * fRatio;
			subdlg_button[j].fHeight = ((float)(24)) * fRatio;

			subdlg_button[j].ID = i * 10 * j;

			sprintf( str, "BUTTON%d%d", i, j );
			subdlg_button[j].strText = str;

			subdlg_button[j].pDefaultFontElement = &font_element;

			apSubDialog[i]->AddControl( &subdlg_button[j] );
		}
	}

*/


CGameTask_Settings::~CGameTask_Settings()
{
	INPUTHUB.SetInputHandler( NULL );
	SafeDelete( m_pInputHandler );
	SafeDelete( m_pDialogManager );
}


int CGameTask_Settings::FrameMove( float dt )
{
	return CGameTask::ID_INVALID;
}


void CGameTask_Settings::Render( float dt )
{
	LPDIRECT3DDEVICE9& pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	// Begin the scene
	pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render stage select dialog
	m_pDialogManager->Render( dt );

	// draw cursor
	int x,y;
	g_pDIMouse->GetCurrentPosition( x, y );
	C2DRect rect;
	rect.SetPosition( D3DXVECTOR2(x-2,y-2), D3DXVECTOR2(x+2,y+2) );
	rect.SetColor( 0xFFFF1010 );
	rect.Draw();

	// End the scene
    pd3dDevice->EndScene();
    // Present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );

}