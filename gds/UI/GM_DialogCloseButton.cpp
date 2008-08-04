#include "GM_DialogCloseButton.h"

#include "GM_ControlDescBase.h"
#include "GM_Dialog.h"
#include "GM_ControlRenderer.h"
#include "GM_SoundPlayer.h"


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





/*
void CGM_DialogCloseButton::OnFocusOut()
{
	CGM_Control::OnFocusOut();

	// if the button is marked as currenly being pressed
	// even if its focus is about to be taken away,
	// set it to 'not pressed'
	if( IsPressed() )
		m_bPressed = false;
}*/

/**
void CGM_DialogCloseButton::Render()
{
	m_pRenderManager->RenderDialogCloseButton( *this );
}
*/

/*
unsigned int CGM_DialogCloseButton::GetState() const
{
	if( IsPressed() )	return STATE_PRESSED;
	else return CGM_Control::GetState();
}*/

