
#include "GameTaskFG.h"
#include "../UI/FG_StdControlRenderer.h"
#include "3DCommon/GraphicsElementManager.h"
#include "3DCommon/FontBase.h"
#include "UI.h"
#include "Support/Log/DefaultLog.h"

using namespace Graphics;


static SPoint GetReferenceScreenCenterPosition() { return SPoint(800,600)  * 0.5f; }


/*
static int GetReferenceScreenWidth() { return 800; }
static int GetReferenceScreenHeight() { return 600; }


// rearrage the layout of UI controls
void UpdatePosOfUIControls()
{
	m_pDialogManager->GetControl(ID_TT_DLG_EXIT)->SetTopLeftPos(GetConformationDialogTopLeftPos());
	m_pDialogManager->GetControl(ID_TT_CONTROL_AT_BOTTOM_RIGHT)->SetTopLeftPos();
}


SPoint GetConformationDialogTopLeftPos( int w, int h )
{
	int cx = GetReferenceScreenWidth() / 2;
	int cy = GetReferenceScreenHeight() / 2;
	return SPoint( cx-w/2, cy-h/2 );
}

*/

/*
CreateConfirmationDialog()
{
	int dw = 200;
	int dh = 80;
	SPoint pos = GetConformationDialogTopLeftPos( dw, dh );

	CGM_Dialog *pExitDialog
		= Create2ChoiceDialog( m_pDialogManager, false,
		ID_TT_DLG_EXIT, "Exit Game", RectLTWH( pos.x, pos.y, dw, dh ),
		ID_TT_EXIT_YES,	"YES",	     RectLTWH( 20,     40, 60, 24 ),
		ID_TT_EXIT_NO,	"NO",	     RectLTWH( dw/2+20, 40, 60, 24 ),
		0, ""/"Are you sure?"/, SRect( 20, 16, 20+120, 16+20 ) );
	pExitDialog->SetEventHandler( pEventHandler );
}
*/
CGM_Dialog *FG_CreateYesNoDialogBox( CGM_DialogManagerSharedPtr pDialogManager,
									 bool is_root_dlg, int dlg_id, const std::string& title,
									 const std::string& text, int id_yes, int id_no )
{
	int dw = 160;
	int dh = 90;
	SPoint cp = GetReferenceScreenCenterPosition();
	cp.y -= 30;
	CGM_Dialog *pDlgBox = Create2ChoiceDialog( pDialogManager, is_root_dlg,
		dlg_id, title, RectCWH( cp, dw, dh ),
		id_yes,	"YES",	RectLTWH( 15, 60, 60, 25 ),
		id_no,	"NO",	RectLTWH( 85, 60, 60, 25 ) );

	if( !pDlgBox )
		return NULL;

	// customize confirm dialog renderers - no sliding in / out effect
	CFG_StdDialogRenderer *pDlgRenderer = dynamic_cast<CFG_StdDialogRenderer *>(pDlgBox->GetRenderer());
	if( pDlgRenderer )
		pDlgRenderer->SetSlideInOutAmount( Vector2(0,0), Vector2(0,0) );

	return pDlgBox;
}


void CGameTaskFG::LoadFonts( CGM_ControlRendererManagerSharedPtr pControlRenedererMgr )
{
	CGraphicsElementManager *pGraphicsElementMgr = pControlRenedererMgr->GetGraphicsElementManager();

	// add font to UI render manager
//	string filename = "Texture\\HGGE_16x8_256.dds";
	string filename = "Texture\\BitstreamVeraSansMono_Bold.dds";
	if( !pGraphicsElementMgr->LoadFont( 0, filename,        CFontBase::FONTTYPE_TEXTURE, 12, 24 ) )
		g_Log.Print( "failed to register font: %s", filename.c_str() );

	if( !pGraphicsElementMgr->LoadFont( 1, filename,        CFontBase::FONTTYPE_TEXTURE, 10, 20 ) )
		g_Log.Print( "failed to register font: %s", filename.c_str() );

	if( !pGraphicsElementMgr->LoadFont( 2, filename,        CFontBase::FONTTYPE_TEXTURE,  8, 12 ) )
		g_Log.Print( "failed to register font: %s", filename.c_str() );

	if( !pGraphicsElementMgr->LoadFont( 3, "‚l‚r ƒSƒVƒbƒN", CFontBase::FONTTYPE_NORMAL, 10, 20 ) )
		g_Log.Print( "failed to register font: %s", filename.c_str() );
}



void CGameTaskFG::SetSounds( CGM_DialogManagerSharedPtr pDialogManager )
{
	CGM_GlobalSoundPlayerSharedPtr pSoundPlayer = pDialogManager->GetDefaultSoundPlayer();

	pSoundPlayer->SetSoundOnFocusShifted(                       "ui_focus_shifted" );
	pSoundPlayer->SetSoundOnDialogClosedByCancelInput(          "ui_dlg_canceled" );
	pSoundPlayer->SetSoundOnDialogClosedByListBoxItemSelection( "ui_selected" );
//	pSoundPlayer->SetSoundOnDialogClosedByDialogSwitching(      "ui_" );
//	pSoundPlayer->SetSoundOnDialogClosedByDialogCloseButton(    "ui_" );
//	pSoundPlayer->SetSoundOnDialogClosed(                       "ui_dlg_closed" );
//	pSoundPlayer->SetSoundOnDialogAttemptedToClose(             "ui_dlg_cannot_close" );
	pSoundPlayer->SetSoundOnButtonPressed(                      "ui_selected" );
//	pSoundPlayer->SetSoundOnButtonReleased(                     "ui_button_released" );
	pSoundPlayer->SetSoundOnSubDialogButtonPressed(             "ui_selected" );
//	pSoundPlayer->SetSoundOnSubDialogButtonReleased(            "" );
//	pSoundPlayer->SetSoundOnDialogCloseButtonPressed(           "" );
//	pSoundPlayer->SetSoundOnDialogCloseButtonReleased(          "" );
//	pSoundPlayer->SetSoundOnCheckBoxChecked(                    "ui_checked" );
//	pSoundPlayer->SetSoundOnCheckBoxCheckCleared(               "ui_unchecked" );
//	pSoundPlayer->SetSoundOnRadioButtonChecked(                 "ui_checked" );
//	pSoundPlayer->SetSoundOnRadioButtonCheckCleared(            "ui_unchecked" );
	pSoundPlayer->SetSoundOnItemFocusShifted(                   "ui_focus_shifted" );
	pSoundPlayer->SetSoundOnItemSelected(                       "ui_selected" );
}


void CGameTaskFG::DoCommonInit( CGM_DialogManagerSharedPtr pDialogManager )
{
	LoadFonts( pDialogManager->GetControlRendererManagerSharedPtr() );

	SetSounds( pDialogManager );
}




