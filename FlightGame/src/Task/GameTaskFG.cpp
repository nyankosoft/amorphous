
#include "GameTaskFG.h"
#include "3DCommon/GraphicsElementManager.h"
#include "3DCommon/FontBase.h"
#include "UI/ui.h"
#include "Support/Log/DefaultLog.h"


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




