
#include "GM_Button.h"

#include "GM_ControlDescBase.h"
#include "GM_Dialog.h"
#include "GM_ControlRenderer.h"
#include "GM_Input.h"
#include "GM_SoundPlayer.h"
#include "Support/SafeDelete.h"


//========================================================================================
// CGM_Button
//========================================================================================

CGM_Button::CGM_Button( CGM_Dialog *pDialog, CGM_ButtonDesc *pDesc )
: CGM_Static( pDialog, pDesc )
{
	m_bPressed = pDesc->bPressed;

//	m_pEventHandler = NULL;
//	m_pEventHandler = pDesc->pEventHandler;
}


CGM_Button::~CGM_Button()
{
	// the event handler is not deleted automatically
	// user is responsible for deleting it when the button is destroyed
}


// called from CGM_SubDialogButton::OnPressed()
void CGM_Button::OnPressed()
{
	if( m_pEventHandler )
		m_pEventHandler->OnPressed();

//	m_pDialog->SendEvent( CGM_Event::BUTTON_PRESSED, true, this );

	if( m_pRenderer )
		m_pRenderer->OnPressed();

	if( m_pSoundPlayer )
		m_pSoundPlayer->OnPressed();

//	m_pDialog->SendEvent( CGM_Event::BUTTON_PRESSED, true, this );
}


void CGM_Button::OnReleased()
{
	if( m_pEventHandler )
		m_pEventHandler->OnReleased();
//	else
//	{
	// clicked & released inside the button
	// - send the click notification event
	m_pDialog->SendEvent( CGM_Event::BUTTON_CLICKED, true, this );
//	}

	if( m_pRenderer )
		m_pRenderer->OnReleased();

	if( m_pSoundPlayer )
		m_pSoundPlayer->OnReleased();
}


bool CGM_Button::HandleMouseInput( CGM_InputData& input )
{
	if( !IsEnabled() || !IsVisible() )
        return false;

	SPoint pt = input.pos;
	switch( input.code )
	{
	case CGM_Input::MOUSE_BUTTON_L:
	    if( input.type == CGM_InputData::TYPE_PRESSED )
		{
	        if( IsPointInside( pt ) )
	        {
				// Pressed while inside the control
                m_bPressed = true;
//				SetCapture( DXUTGetHWND() );

				if( !HasFocus() /*&& m_pDialog->m_bKeyboardInput*/ )
					m_pDialog->RequestFocus( this );

				OnPressed();
                return true;
            }
        }
		else if( input.type == CGM_InputData::TYPE_RELEASED )
		{
            if( IsPressed() )
            {
                m_bPressed = false;
//				ReleaseCapture();

//                if( !m_pDialog->m_bKeyboardInput )
//                    m_pDialog->ClearFocus();

				// Button click
				if( IsPointInside( pt ) )
				{
					OnReleased();
				}

				return true;
			}
		}
		break;
	}
    
    return false;
}


bool CGM_Button::HandleKeyboardInput( CGM_InputData& input )
{
	SPoint pt = input.pos;
	switch( input.code )
	{
	case CGM_Input::OK:
	    if( input.type == CGM_InputData::TYPE_PRESSED )
		{
			// Pressed while inside the control
            m_bPressed = true;

//			if( !HasFocus() /*&& m_pDialog->m_bKeyboardInput*/ )
//				m_pDialog->RequestFocus( this );

			OnPressed();
            return true;
        }
		else if( input.type == CGM_InputData::TYPE_RELEASED )
		{
            if( IsPressed() )
            {
                m_bPressed = false;

//				if( !m_pDialog->m_bKeyboardInput )
//					m_pDialog->ClearFocus();

				OnReleased();

				return true;
			}
		}
		break;

/*	case GIC_UP:
	    if( input.type == CGM_InputData::TYPE_PRESSED )
			;
		break;

	case GIC_DOWN:
	    if( input.type == CGM_InputData::TYPE_PRESSED )
			;
		break;*/
	}
    
    return false;
}


void CGM_Button::OnFocusOut()
{
	CGM_Control::OnFocusOut();

	// if the button is marked as currenly being pressed
	// even if its focus is about to be taken away,
	// set it to 'not pressed'
	if( IsPressed() )
		m_bPressed = false;
}

/*
void CGM_Button::Render()
{
//	m_pRenderManager->RenderButton( *this );
}
*/

unsigned int CGM_Button::GetState() const
{
	if( IsPressed() )	return STATE_PRESSED;
	else return CGM_Control::GetState();
}


void CGM_Button::SetSoundPlayer( CGM_GlobalSoundPlayerSharedPtr pGlobalSoundPlayer )
{
	m_pSoundPlayer = pGlobalSoundPlayer->GetButtonSoundPlayer();
}


//========================================================================================
// CGM_CheckBox
//========================================================================================

CGM_CheckBox::CGM_CheckBox( CGM_Dialog *pDialog, CGM_CheckBoxDesc *pDesc )
: CGM_Button( pDialog, pDesc )
{
	m_bChecked = pDesc->bChecked;
}


void CGM_CheckBox::SetCheckedInternal( bool bChecked, bool bFromInput )
{
    m_bChecked = bChecked;

	if( m_bChecked )
		m_pDialog->SendEvent( CGM_Event::CHECKBOX_CHECKED, bFromInput, this );
	else
        m_pDialog->SendEvent( CGM_Event::CHECKBOX_CHECK_CLEARED, bFromInput, this );

	// update control renderer
	if( m_pRenderer )
	{
		if( m_bChecked )
			m_pRenderer->OnChecked();
		else
			m_pRenderer->OnCheckCleared();
	}
}

/*
void CGM_CheckBox::OnPressed()
{}
*/

void CGM_CheckBox::OnReleased()
{
	CGM_Button::OnReleased();

	// clicked & released inside the button
	// - send the click notification event
	SetCheckedInternal( !m_bChecked, true );
}

/*
commented out 19:55 12/4/2007

bool CGM_CheckBox::HandleMouseInput( CGM_InputData& input )
{
	if( !IsEnabled() || !IsVisible() )
		return false;

	if( CGM_Button::HandleMouseInput( input ) )
		return true;

	SPoint pt = input.pos;

    return false;
}
*/

/*
void CGM_CheckBox::Render()
{
//	m_pRenderManager->RenderCheckBox( *this );
}
*/

//========================================================================================
// CGM_RadioButton
//========================================================================================

CGM_RadioButton::CGM_RadioButton( CGM_Dialog *pDialog, CGM_RadioButtonDesc *pDesc )
: CGM_CheckBox( pDialog, pDesc )
{
	m_iButtonGroup = pDesc->iButtonGroup;
}


void CGM_RadioButton::SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput )
{
	if( bChecked && bClearGroup )
		m_pDialog->ClearRadioButtonGroup( m_iButtonGroup );

	m_bChecked = bChecked;

	if( m_bChecked )
        m_pDialog->SendEvent( CGM_Event::RADIOBUTTON_CHECKED, bFromInput, this );
	else
        m_pDialog->SendEvent( CGM_Event::RADIOBUTTON_CHECK_CLEARED, bFromInput, this );

	// update control renderer
	if( m_pRenderer )
	{
		if( m_bChecked )
			m_pRenderer->OnChecked();
		else
			m_pRenderer->OnCheckCleared();
	}
}

/*
void CGM_RadioButton::OnPressed()
{
}
*/

void CGM_RadioButton::OnReleased()
{
	CGM_Button::OnReleased();

	bool checked = true;
	bool clear_group = true;
//	SetCheckedInternal( checked, clear_group, false );
	SetCheckedInternal( checked, clear_group, true );

//	m_pDialog->ClearRadioButtonGroup( m_iButtonGroup );
//	m_bChecked = !m_bChecked;
	
//	m_pDialog->SendEvent( CGM_Event::RADIOBUTTON_CHANGED, true, this );
}

/*
commented out 19:55 12/4/2007

bool CGM_RadioButton::HandleMouseInput( CGM_InputData& input )
{
	if( !IsEnabled() || !IsVisible() )
		return false;

	if( CGM_Button::HandleMouseInput( input ) )
		return true;

    return false;
}
*/

/*
void CGM_RadioButton::Render()
{
//	m_pRenderManager->RenderRadioButton( *this );
}
*/
