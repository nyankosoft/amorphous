#include "GM_DialogCloseButton.hpp"

#include "GM_ControlDescBase.hpp"
#include "GM_Dialog.hpp"
#include "GM_ControlRenderer.hpp"
#include "GM_SoundPlayer.hpp"


namespace amorphous
{


CGM_DialogCloseButton::CGM_DialogCloseButton( CGM_Dialog *pDialog, CGM_DialogCloseButtonDesc *pDesc )
: CGM_Button( pDialog, pDesc )
{
}


void CGM_DialogCloseButton::OnPressed()
{
	// send 'button pressed' events
	CGM_Button::OnPressed();
}


void CGM_DialogCloseButton::OnReleased()
{
	// clicked & released inside the button
	// - send the click notification event
//	m_pDialog->SendEvent( CGM_Event::BUTTON_CLICKED, true, this );

	// send an event if the control has an event handler
	CGM_Button::OnReleased();

	m_pDialog->Close();
}


void CGM_DialogCloseButton::SetSoundPlayer( CGM_GlobalSoundPlayerSharedPtr pGlobalSoundPlayer )
{
	m_pSoundPlayer = pGlobalSoundPlayer->GetDialogCloseButtonSoundPlayer();
}


} // namespace amorphous
