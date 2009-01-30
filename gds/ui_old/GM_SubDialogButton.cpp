
#include "GM_SubDialogButton.hpp"

#include "GM_ControlDescBase.hpp"
#include "GM_Dialog.hpp"

#include "GameInput/InputHandler.hpp"



//========================================================================================
// CGM_SubDialogButton
//========================================================================================
CGM_SubDialogButton::CGM_SubDialogButton( CGM_Dialog *pDialog, CGM_SubDialogButtonDesc *pDesc )
: CGM_Button(pDialog, pDesc)
{
	m_pSubDialog = pDesc->pSubDialog;
}


bool CGM_SubDialogButton::HandleMouseInput( SInputData& input )
{

	if( m_pSubDialog && m_pSubDialog->IsOpen() )
	{
		bool bHandled = false;

		// send input to sub dialog
		bHandled = m_pSubDialog->HandleInput( input );

		if( bHandled )
			return true;	// input was processed in the sub dialog
	}

	// input was not handled in the child dialogs
	// see if it should be handled in this button

	if( !m_bEnabled || !m_bVisible )
        return false;

	SPoint pt = SPoint( input.GetParamH16(), input.GetParamL16() );
	switch( input.iGICode )
	{
	case GIC_MOUSE_BUTTON_L:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
            if( IsPointInside( pt ) )
            {
				// Pressed while inside the control
				m_bPressed = true;
//                SetCapture( DXUTGetHWND() );

                if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );

				if( !m_pSubDialog )
					return true;

 				if( !m_pSubDialog->IsOpen() )
				{
					// close other sug dialogs that are currently open
					// and belonging to the same parent dialog
					m_pDialog->CloseSubDialogs();

					// open the sub dialog of this button		
					m_pSubDialog->Open();
				}
				else
				{	// close the sug dialog of this button
					m_pSubDialog->Close();
				}

               return true;
            }
			else
			{
				// mouse not on control
				int stop = 1;
			}
		}
		break;
	}

	if( input.iType == ITYPE_KEY_RELEASED )
	{
		if( m_bPressed )
		{
			m_bPressed = false;
//                ReleaseCapture();

//                if( !m_pDialog->m_bKeyboardInput )
//                    m_pDialog->ClearFocus();

			// Button click
			if( IsPointInside( pt ) )
				m_pDialog->SendEvent( GM_EVENT_BUTTON_CLICKED, true, this );

			return true;
		}
	}

	return false;
}


bool CGM_SubDialogButton::HandleInputInSubDialog( SInputData& input )
{
	if( !m_pSubDialog || !m_pSubDialog->IsOpen() )
		return false;

	return m_pSubDialog->HandleInput( input );
}


void CGM_SubDialogButton::CloseSubDialogs()
{
	if( m_pSubDialog && m_pSubDialog->IsOpen() )
	{
		m_pSubDialog->Close();
	}
}


void CGM_SubDialogButton::Render( float fElapsedTime )
{
	if( m_pSubDialog && m_pSubDialog->IsOpen() )
		m_pSubDialog->Render( fElapsedTime );

	CGM_Button::Render( fElapsedTime );
}
