
#include "GameTaskFG.h"
#include "../UI/FG_StdControlRenderer.h"
#include "3DCommon/GraphicsElementManager.h"
#include "3DCommon/FontBase.h"
#include "UI.h"
#include "Support/Log/DefaultLog.h"

using namespace Graphics;


/*

// rearrage the layout of UI controls
void UpdatePosOfUIControls()
{
	m_pDialogManager->GetControl(ID_TT_DLG_EXIT)->SetTopLeftPos(GetConformationDialogTopLeftPos());
	m_pDialogManager->GetControl(ID_TT_CONTROL_AT_BOTTOM_RIGHT)->SetTopLeftPos();
}
*/

CGM_Dialog *CGameTaskFG::FG_CreateYesNoDialogBox( CGM_DialogManagerSharedPtr pDialogManager,
									 bool is_root_dlg, int dlg_id, const std::string& title,
									 const std::string& text, int id_yes, int id_no )
{
	int dw = 320;
	int dh = 180;
	SRect dlg_rect = RectAtCenter( dw, dh );
	dlg_rect.Offset( 0, -60 );
	CGM_Dialog *pDlgBox = Create2ChoiceDialog( pDialogManager, is_root_dlg,
		dlg_id, title, dlg_rect,
		id_yes,	"YES",	RectLTWH(  30, 60, 120, 50 ),
		id_no,	"NO",	RectLTWH( 170, 60, 120, 50 ) );

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
//	string filename = "Texture/HGGE_16x8_256.dds";
	string filename = "./Texture/BitstreamVeraSansMono_Bold.dds";
	if( !pGraphicsElementMgr->LoadFont( 0, filename,        CFontBase::FONTTYPE_TEXTURE, 24, 48 ) )
		LOG_PRINT_ERROR( "failed to register font: " + filename );

	if( !pGraphicsElementMgr->LoadFont( 1, filename,        CFontBase::FONTTYPE_TEXTURE, 20, 40 ) )
		LOG_PRINT_ERROR( "failed to register font: " + filename );

	if( !pGraphicsElementMgr->LoadFont( 2, filename,        CFontBase::FONTTYPE_TEXTURE, 16, 24 ) )
		LOG_PRINT_ERROR( "failed to register font: " + filename );

	string font_name = "‚l‚r ƒSƒVƒbƒN";
	if( !pGraphicsElementMgr->LoadFont( 3, font_name, CFontBase::FONTTYPE_NORMAL, 20, 40 ) )
		LOG_PRINT_ERROR( "failed to register font: " + font_name );
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




