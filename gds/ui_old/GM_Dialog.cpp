
#include "GM_Dialog.h"

#include "GM_DialogDesc.h"
#include "GM_DialogManager.h"
#include "GM_ControlDescBase.h"
#include "GM_Static.h"
#include "GM_Button.h"
#include "GM_Slider.h"
#include "GM_SubDialogButton.h"

#ifdef UI_EXTENSION_EDIT
#include "GM_PaintBar.h"
#endif


#include "GameInput/InputHandler.h"


CGM_Control* CGM_Dialog::ms_pControlFocus   = NULL;      // The control which has focus
CGM_Control* CGM_Dialog::ms_pControlPressed = NULL;      // The control currently pressed


CGM_Dialog::CGM_Dialog(	CGM_DialogManager *pDialogManager, CGM_DialogDesc& desc )
{
	m_pDialogManager = pDialogManager;

	m_bIsOpen = false;

    m_bMinimized = false;

	m_bCaption = desc.bCaption;

    m_iCaptionHeight = desc.iCaptionHeight;

	m_bNonUserEvents = desc.bNonUserEvents;

    m_fPosX   = desc.fPosX;
	m_fPosY   = desc.fPosY;
	m_fWidth  = desc.fWidth;
	m_fHeight = desc.fHeight;

	if( desc.pBackgroundRectElement )
		m_BackgroundRect = *desc.pBackgroundRectElement;

	m_BackgroundRect.m_RectColor.m_CurrentColor = m_BackgroundRect.m_RectColor.m_aColor[GM_STATE_NORMAL];

	m_pEventHandler = desc.pEventHandler;	// Callback function to handle events

	m_pControlMouseOver = NULL;

	ms_pControlFocus = NULL;
	ms_pControlPressed = NULL;
}


CGM_Dialog::~CGM_Dialog()
{
}


bool CGM_Dialog::HandleInput( SInputData& input )
{
	// send input message to sub-dialogs first
	bool bHandled;
	int i;
	CGM_SubDialogButton *pSubDialogButton;
	for( i=0; i<m_vecpControl.size(); i++ )
	{
		if( m_vecpControl[i]->GetType() == CGM_Control::SUBDIALOGBUTTON )
		{
			pSubDialogButton = (CGM_SubDialogButton *)m_vecpControl[i];

			bHandled = pSubDialogButton->HandleInputInSubDialog( input );

			if( bHandled )
				return true;	// input has been processed in the sub-dialog
		}
	}

	SPoint pt;

	// handle mouse input
	switch( input.iGICode )
	{
	case GIC_MOUSE_BUTTON_L:
	case GIC_MOUSE_BUTTON_R:
	case GIC_MOUSE_BUTTON_M:
	case GIC_MOUSE_AXIS_X:
	case GIC_MOUSE_AXIS_Y:
    {
            // If not accepting mouse input, return false to indicate the message should still 
            // be handled by the application (usually to move the camera).
//            if( !m_bMouseInput )
//                return false;

//            POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
//            mousePoint.x -= m_x;            mousePoint.y -= m_y;

		pt = SPoint( input.GetParamH16(), input.GetParamL16() );

            // If caption is enabled, offset the Y coordinate by the negative of its height.
//            if( m_bCaption )
//                mousePoint.y -= m_nCaptionHeight;

			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if( ms_pControlFocus && 
				ms_pControlFocus->m_pDialog == this && 
				ms_pControlFocus->IsEnabled() )
			{
				if( ms_pControlFocus->HandleMouseInput( input ) )
					return true;
			}

			// Not yet handled, see if the mouse is over any controls
			CGM_Control* pControl = GetControlAtPoint( pt );
			if( pControl != NULL && pControl->IsEnabled() )
			{
				bHandled = pControl->HandleMouseInput( input );
				if( bHandled )
					return true;
			}
			else
			{
				// Mouse not over any controls in this dialog, if there was a control
				// which had focus it just lost it
				if( input.iGICode == GIC_MOUSE_BUTTON_L && 
					input.iType == ITYPE_KEY_PRESSED &&
					ms_pControlFocus && 
					ms_pControlFocus->m_pDialog == this )
				{
					ms_pControlFocus->OnFocusOut();
					ms_pControlFocus = NULL;
				}
			}

			// Still not handled, hand this off to the dialog. Return false to indicate the
			// message should still be handled by the application (usually to move the camera).
			switch( input.iGICode )
			{
				case GIC_MOUSE_AXIS_X:
				case GIC_MOUSE_AXIS_Y:
					OnMouseMove( pt );
					return false;
			}

			break;
		}
    }

	return false;
}


void CGM_Dialog::OnMouseMove( SPoint& pt )
{
    // Figure out which control the mouse is over now
    CGM_Control* pControl = GetControlAtPoint( pt );

    // If the mouse is still over the same control, nothing needs to be done
    if( pControl == m_pControlMouseOver )
        return;

    // Handle mouse leaving the old control
    if( m_pControlMouseOver )
        m_pControlMouseOver->OnMouseLeave();

    // Handle mouse entering the new control
    m_pControlMouseOver = pControl;
    if( pControl != NULL )
        m_pControlMouseOver->OnMouseEnter();

	m_pDialogManager->SetControlForCaption( pControl );
}


CGM_Control *CGM_Dialog::AddControl( CGM_ControlDesc *pControlDesc )
{
	CGM_Control *pNewControl = NULL;

	switch( pControlDesc->GetType() )
	{
	case CGM_Control::STATIC:
		pNewControl = new CGM_Static( this, (CGM_StaticDesc *)pControlDesc );
		break;

	case CGM_Control::BUTTON:
		pNewControl = new CGM_Button( this, (CGM_ButtonDesc *)pControlDesc );
		break;

	case CGM_Control::SUBDIALOGBUTTON:
		pNewControl = new CGM_SubDialogButton( this, (CGM_SubDialogButtonDesc *)pControlDesc );
		break;

	case CGM_Control::CHECKBOX:
		pNewControl = new CGM_CheckBox( this, (CGM_CheckBoxDesc *)pControlDesc );
		break;

	case CGM_Control::RADIOBUTTON:
		pNewControl = new CGM_RadioButton( this, (CGM_RadioButtonDesc *)pControlDesc );
		break;

	case CGM_Control::SLIDER:
		pNewControl = new CGM_Slider( this, (CGM_SliderDesc *)pControlDesc );
		break;

#ifdef UI_EXTENSION_EDIT
	case CGM_Control::PAINTBAR:
		pNewControl = new CGM_PaintBar( this, (CGM_PaintBarDesc *)pControlDesc );
		break;
#endif

	default:
		return NULL;	// invalid control id
	}

	// add to the list of controls
	m_vecpControl.push_back( pNewControl );

	return pNewControl;
}


void CGM_Dialog::SendEvent( eGameMenuEventType event, bool bTriggeredByUser, CGM_Control* pControl )
{
    // If no callback has been registered there's nowhere to send the event to
    if( m_pEventHandler == NULL )
        return;

    // Discard events triggered programatically if these types of events haven't been
    // enabled
    if( !bTriggeredByUser && !m_bNonUserEvents )
        return;

	(*m_pEventHandler)( event, pControl->GetID(), pControl );
}


void CGM_Dialog::RequestFocus( CGM_Control* pControl )
{
    if( ms_pControlFocus == pControl )
        return;

    if( !pControl->CanHaveFocus() )
        return;

    if( ms_pControlFocus )
        ms_pControlFocus->OnFocusOut();

    pControl->OnFocusIn();
    ms_pControlFocus = pControl;
}


bool CGM_Dialog::IsOpen()
{
	return m_bIsOpen;
}


void CGM_Dialog::Open()
{
	m_bIsOpen = true;
}


void CGM_Dialog::Close()
{
	CloseSubDialogs();

	m_bIsOpen = false;
}


void CGM_Dialog::CloseSubDialogs()
{
	CGM_SubDialogButton *pSubDialogButton;

	size_t i;
	for( i=0; i<m_vecpControl.size(); i++ )
	{
		if( m_vecpControl[i]->GetType() == CGM_Control::SUBDIALOGBUTTON )
		{
			pSubDialogButton = (CGM_SubDialogButton *)m_vecpControl[i];

			// all the child subdialogs will also be closed in this call
			pSubDialogButton->CloseSubDialogs();
		}
	}
	
}	


CGM_Control *CGM_Dialog::GetControlAtPoint( SPoint& pt )
{
	for( int i=0; i < m_vecpControl.size(); i++ )
	{
		if( m_vecpControl[i]->IsPointInside( pt ) )
			return m_vecpControl[i];
	}

	return NULL;
}


void CGM_Dialog::ClearRadioButtonGroup( int iButtonGroup )
{
    // Find all radio buttons with the given group number
	for( int i=0; i < m_vecpControl.size(); i++ )
	{
		CGM_Control *pControl = m_vecpControl[i];

        if( pControl->GetType() == CGM_Control::RADIOBUTTON )
        {
            CGM_RadioButton* pRadioButton = (CGM_RadioButton*) pControl;

            if( pRadioButton->GetButtonGroup() == iButtonGroup )
                pRadioButton->SetChecked( false, false );
        }
    }
}


void CGM_Dialog::ClearFocus()
{
    if( ms_pControlFocus )
    {
        ms_pControlFocus->OnFocusOut();
        ms_pControlFocus = NULL;
    }
}


// ====================== draw the boundary with wireframe for debug ======================

#include "3DCommon/Direct3D9.h"
#include "3DCommon/2DRect.h"


void CGM_Dialog::Render( float fElapsedTime )
{
//	float width = (float)GM_SCREEN_WIDTH;
	float width = 1.0f;
/*	D3DXVECTOR2 vTopLeft     = D3DXVECTOR2( width * m_fPosX, width * m_fPosY );
	D3DXVECTOR2 vBottomRight = D3DXVECTOR2( width * (m_fPosX + m_fWidth), width * (m_fPosY + m_fHeight) );
	C2DRect rect;
	rect.SetPosition( vTopLeft, vBottomRight );
	rect.SetColor( 0xFFA0A0A0 );
	rect.DrawWireframe();*/


  // experiment	
  if( m_BackgroundRect.m_pRenderRoutine )
  {
    m_BackgroundRect.m_pRenderRoutine->Render( fElapsedTime, m_BackgroundRect );
  }
  else
  {
	m_BackgroundRect.m_RectColor.Blend( GM_STATE_NORMAL, fElapsedTime, 0.0f );
	m_BackgroundRect.Draw( this );
  }


	size_t i;
	for( i=0; i<m_vecpControl.size(); i++ )
	{
		m_vecpControl[i]->Render( fElapsedTime );
	}
}


CFontBase *CGM_Dialog::GetFont( int iIndex )
{
	return m_pDialogManager->GetFont( iIndex );
}


LPDIRECT3DTEXTURE9 CGM_Dialog::GetTexture( int iIndex )
{
	return m_pDialogManager->GetTexture( iIndex );
}
