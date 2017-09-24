#include "GM_SubDialogButton.hpp"

#include "GM_ControlDescBase.hpp"
#include "GM_Dialog.hpp"
#include "GM_ControlRenderer.hpp"
#include "GM_SoundPlayer.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{


//========================================================================================
// CGM_SubDialogButton
//========================================================================================

CGM_SubDialogButton::CGM_SubDialogButton( CGM_Dialog *pDialog, CGM_SubDialogButtonDesc *pDesc )
: CGM_Button(pDialog, pDesc)
{
//	m_pSubDialog = pDesc->pSubDialog;

	// if the sub-dialog is handed with other control properties
	// link to it at the initialization phase
	SetSubDialog( pDesc->pSubDialog );
}


void CGM_SubDialogButton::OpenDialog()
{
	OnPressed();
	OnReleased();
}


void CGM_SubDialogButton::OnPressed()
{
	LOG_PRINT( m_strText );

//	if( !HasFocus() )
//	    m_pDialog->RequestFocus( this );

	// notify event handlers
	CGM_Button::OnPressed();

	if( !m_pSubDialog )
		return;
	
	if( !m_pSubDialog->IsOpen() )
	{
		// if the button currrently owns focus, release it
		// changed - focus is cleared after the button is released
		//           because clearing the focus before releasing the button will
		//           result in a failure to turn 'm_bPressed' to false
		// 061223 - 'm_bPressed' is set to false when the focus is cleared
//		if( HasFocus() )
//			m_pDialog->ClearFocus();

		// remember the control currently focused on,
		// which sould be this sub-dialog button
		m_pDialog->UpdateLastFocusedControl();

		// close other sug-dialogs that are currently open
		// and belonging to the same parent dialog
		m_pDialog->CloseSubDialogs();

		// open the sub dialog of this button		
		m_pSubDialog->Open();

		// release the focus since this sub-dialog button
		// should not handle focus-cycling any more
		// changed - focus cannot be cleared until the button is relealsed
//		if( HasFocus() )
//			m_pDialog->ClearFocus();
	}
	else
	{	// close the sub-dialog of this button
		m_pSubDialog->Close();
	}
}


void CGM_SubDialogButton::OnReleased()
{
	m_pDialog->SendEvent( CGM_Event::BUTTON_CLICKED, true, this );

	if( HasFocus() )
	{
		if( !m_pSubDialog )
			return;

		if( m_pSubDialog->IsOpen() )
		{
			m_pDialog->ClearFocus();
			m_pSubDialog->SetFocusOnLastFocusedControl();
		}
		else
		{
			// Sub-dialog button is pressed and released, but the dub-dialog is closed.
			// - probably the sub-dialog is closed before the sub-dialog button is released.
			//   Do not clear the focus in this case
		}
	}
}


bool CGM_SubDialogButton::HandleMouseInput( CGM_InputData& input )
{
	if( m_pSubDialog && m_pSubDialog->IsOpen() )
	{
		// sub-dialog is open - let it handle the input first
		bool bHandled = false;

		// send input to sub dialog
		bHandled = m_pSubDialog->HandleInput( input );

		if( bHandled )
			return true;	// input was processed in the sub dialog
	}

	// input was not handled in the child dialogs
	// see if it should be handled in this button

	if( CGM_Button::HandleMouseInput( input ) )
		return true;
		
	return false;
}


bool CGM_SubDialogButton::HandleInputInSubDialog( CGM_InputData& input )
{
	if( !m_pSubDialog || !m_pSubDialog->IsOpen() )
		return false;

	return m_pSubDialog->HandleInput( input );
}

/*
void CGM_SubDialogButton::OnFocusIn()
{
	CGM_Button::OnFocusIn();

	if( m_StyleFlag & OPEN_DIALOG_ON_FOCUSED )
		OpenDialog();
}
*/

void CGM_SubDialogButton::CloseSubDialogs()
{
	if( m_pSubDialog && m_pSubDialog->IsOpen() )
	{
		m_pSubDialog->Close();
	}
}


void CGM_SubDialogButton::OnSubDialogClosed()
{
	if( m_pDialog )
        m_pDialog->SetFocusOnLastFocusedControl();

//	if( m_pRenderer.get() )
//		m_pRenderer->OnSubDialogClosed();
}


bool CGM_SubDialogButton::IsSubDialogOpen() const
{
	return ( m_pSubDialog && m_pSubDialog->IsOpen() );
}


void CGM_SubDialogButton::SetSubDialog( CGM_Dialog* pDialog )
{
	m_pSubDialog = pDialog;
	if( m_pSubDialog )
		m_pSubDialog->SetOwnerButton( this );
}


unsigned int CGM_SubDialogButton::GetState() const
{
	if( m_pSubDialog && m_pSubDialog->IsOpen() )
		return STATE_SUBDIALOGOPEN;
	else
		return CGM_Control::GetState();
}
	

void CGM_SubDialogButton::SetDepth( int depth )
{
	CGM_ControlBase::SetDepth( depth );

	if( m_pSubDialog )
		m_pSubDialog->SetDepth( depth + 1 );
}


void CGM_SubDialogButton::UpdateGraphicsProperties()
{
	// first, update the child dialog and its controls
	if( m_pSubDialog )
		m_pSubDialog->UpdateGraphicsProperties();

	// then update this control
	CGM_ControlBase::UpdateGraphicsProperties();
}


int CGM_SubDialogButton::GetMaxDepth() const
{
	if( m_pSubDialog )
        return m_pSubDialog->GetMaxDepth();
	else
		return GetDepth();
}


void CGM_SubDialogButton::SetSoundPlayer( CGM_GlobalSoundPlayerSharedPtr pGlobalSoundPlayer )
{
	m_pSoundPlayer = pGlobalSoundPlayer->GetSubDialogButtonSoundPlayer();
}


} // namespace amorphous
