#include "GM_Dialog.hpp"

#include "GM_DialogDesc.hpp"
#include "GM_DialogManager.hpp"
#include "GM_ControlDescBase.hpp"
#include "GM_ListBoxDesc.hpp"
#include "GM_ScrollBarDesc.hpp"
#include "GM_Static.hpp"
#include "GM_Button.hpp"
#include "GM_DialogCloseButton.hpp"
#include "GM_Slider.hpp"
#include "GM_ListBox.hpp"
#include "GM_ScrollBar.hpp"
#include "GM_SubDialogButton.hpp"
#include "GM_EventHandlerBase.hpp"
#include "GM_ControlRenderer.hpp"
#include "GM_ControlRendererManager.hpp"
#include "GM_Input.hpp"
#include "GM_SoundPlayer.hpp"

#ifdef UI_EXTENSION_EDIT
#include "GM_PaintBar.hpp"
#endif

#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"


//CGM_Control* CGM_Dialog::m_pDialogManager->ControlFocus()   = NULL;      // The control which has focus
//CGM_Control* CGM_Dialog::m_pDialogManager->ControlPressed() = NULL;      // The control currently pressed


CGM_Dialog::CGM_Dialog(	CGM_DialogManager *pDialogManager, CGM_DialogDesc& desc )
:
CGM_ControlBase( &desc )
{
	m_pDialogManager = pDialogManager;

	m_bIsOpen = false;

    m_bMinimized = false;

	m_bCaption = desc.bCaption;

    m_iCaptionHeight = desc.iCaptionHeight;

	m_bNonUserEvents = desc.bNonUserEvents;

	m_pEventHandler = desc.pEventHandler;

	m_pEventHandlerFn = desc.pEventHandlerFn;	// Callback function to handle events

	m_StyleFlag = desc.StyleFlag;

	for( int i=0; i<NUM_MAX_ADJACENT_DIALOGS; i++ )
	{
		m_apNextDialog[i] = desc.apNextDialog[i];
	}

	m_strTitle = desc.strTitle;

	m_pControlMouseOver = NULL;

	m_pLastFocusedControl = NULL;

	m_pOwnerButton = NULL;


//	m_pDialogManager->ControlFocus() = NULL;
//	m_pDialogManager->ControlPressed() = NULL;

	m_bRootDialog = desc.bRootDialog;

	m_vecpControl.reserve( 16 );
}


CGM_Dialog::~CGM_Dialog()
{
	SafeDeleteVector( m_vecpControl );
}


inline bool SendInputTo( CGM_InputData& input, CGM_Control* pControl )
{
	if( input.IsMouseInput() )
		return pControl->HandleMouseInput( input );	// send moust input
	else
		return pControl->HandleKeyboardInput( input );	// send keyboard & gamepad input
}


/// process input
/// \return true if the input has been used for some purpose
/// input is sent to control/dialog in the following order
/// - sub-dialogs that belong to the current dialog
/// - controls that belong to the current dialog
/// - the current dialog
bool CGM_Dialog::HandleInput( CGM_InputData& input )
{
	// If the dialog is minimized, don't send any messages to controls.
//	if( m_bMinimized )
//		return false;

	// send input message to sub-dialogs first
	bool bHandled;
	CGM_SubDialogButton *pSubDialogButton;

	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i < num_controls; i++ )
	{
		if( m_vecpControl[i]->GetType() == CGM_Control::SUBDIALOGBUTTON )
		{
			pSubDialogButton = (CGM_SubDialogButton *)m_vecpControl[i];

			bHandled = pSubDialogButton->HandleInputInSubDialog( input );

			if( bHandled )
				return true;	// input has been processed in the sub-dialog
		}
	}

	// If a control is in focus, it belongs to this dialog, and it's enabled, then give
	// it the first chance at handling the message.
	if( m_pDialogManager->ControlFocus() && 
		m_pDialogManager->ControlFocus()->m_pDialog == this && 
		m_pDialogManager->ControlFocus()->IsEnabled() )
	{
//				if( m_pDialogManager->ControlFocus()->HandleMouseInput( input ) )
		if( SendInputTo( input, m_pDialogManager->ControlFocus() ) )
			return true;
	}


	SPoint pt;

	// handle input
	switch( input.code )
	{
	case CGM_Input::MOUSE_BUTTON_L:
	case CGM_Input::MOUSE_BUTTON_R:
	case CGM_Input::MOUSE_BUTTON_M:
	case CGM_Input::MOUSE_AXIS_X:
	case CGM_Input::MOUSE_AXIS_Y:
		{
            // If not accepting mouse input, return false to indicate the message should still 
            // be handled by the application (usually to move the camera).
//            if( !m_bMouseInput )
//                return false;

			pt = input.pos;

            // If caption is enabled, offset the Y coordinate by the negative of its height.
//            if( m_bCaption )
//                mousePoint.y -= m_nCaptionHeight;

			// Not yet handled, see if the mouse is over any controls
			CGM_Control* pControl = GetControlAtPoint( pt );
			if( pControl != NULL && pControl->IsEnabled() )
			{
	//			bHandled = pControl->HandleMouseInput( input );
				bHandled = SendInputTo( input, pControl );
				if( bHandled )
					return true;
			}
			else
			{
				// Mouse cursor not over any controls in this dialog, if there was a control
				// which had focus it just lost it
				if( input.code == CGM_Input::MOUSE_BUTTON_L && 
					input.type == CGM_InputData::TYPE_PRESSED &&
					m_pDialogManager->ControlFocus() && 
					m_pDialogManager->ControlFocus()->m_pDialog == this )
				{
					m_pDialogManager->ControlFocus()->OnFocusOut();
					m_pDialogManager->ControlFocus() = NULL;
				}
			}

			// Still not handled, hand this off to the dialog. Return false to indicate the
			// message should still be handled by the application (usually to move the camera).
			switch( input.code )
			{
			case CGM_Input::MOUSE_AXIS_X:
			case CGM_Input::MOUSE_AXIS_Y:
				OnMouseMove( pt );
				return false;
			}
		}
		break;

	case CGM_Input::SHIFT_FOCUS_UP:
	case CGM_Input::NEXT_CONTROL_UP:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OnFocusShiftRequested( CGM_Control::NEXT_UP );
		break;

	case CGM_Input::SHIFT_FOCUS_LEFT:
	case CGM_Input::NEXT_CONTROL_LEFT:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OnFocusShiftRequested( CGM_Control::NEXT_LEFT );
		break;

	case CGM_Input::SHIFT_FOCUS_DOWN:
	case CGM_Input::NEXT_CONTROL_DOWN:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OnFocusShiftRequested( CGM_Control::NEXT_DOWN );
		break;

	case CGM_Input::SHIFT_FOCUS_RIGHT:
	case CGM_Input::NEXT_CONTROL_RIGHT:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OnFocusShiftRequested( CGM_Control::NEXT_RIGHT );
		break;

	case CGM_Input::OK:
		// If a control is in focus, it belongs to this dialog, and it's enabled, then give
		// it the first chance at handling the message.
		if( m_pDialogManager->ControlFocus() && 
			m_pDialogManager->ControlFocus()->m_pDialog == this && 
			m_pDialogManager->ControlFocus()->IsEnabled() )
		{
			if( m_pDialogManager->ControlFocus()->HandleKeyboardInput( input ) )
				return true;
		}
		break;

	case CGM_Input::CANCEL:
		if( input.type == CGM_InputData::TYPE_PRESSED )
		{
			Close( CGM_SubEvent::DC_CANCELED );

			return true;
		}
		break;

	case CGM_Input::NEXT_DIALOG_RIGHT:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OpenNextDialog( NEXT_RIGHT );
		break;

	case CGM_Input::NEXT_DIALOG_LEFT:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OpenNextDialog( NEXT_LEFT );
		break;

	case CGM_Input::NEXT_DIALOG_DOWN:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OpenNextDialog( NEXT_DOWN );
		break;

	case CGM_Input::NEXT_DIALOG_UP:
		if( input.type == CGM_InputData::TYPE_PRESSED )
			return OpenNextDialog( NEXT_UP );
		break;

	default:
		break;
    }

	return false;
}


CGM_Control *CGM_Dialog::GetControlForFirstFocus()
{
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
		if( m_vecpControl[i]->CanHaveFocus() )
		{
			return m_vecpControl[i];
		}
//		else
//			MsgBoxFmt( "control[%d] (type: %d) cannot have focus", i, m_vecpControl[i]->GetType() );
	}
	return NULL;	// no control to focus
}


CGM_Control *CGM_Dialog::GetControlForNextFocus( CGM_Control *pCurrentlyFocused, unsigned int dir )
{
	if( !pCurrentlyFocused )
		return NULL;

	size_t num_checked = 0, num_controls = m_vecpControl.size();
	for( CGM_Control* pNext = pCurrentlyFocused->GetNextControl( dir );
		pNext && num_checked < num_controls;
		pNext = pNext->GetNextControl(dir), num_checked++ )
	{
		if( pNext->CanHaveFocus() )
		{
			return pNext;
		}
	}

	return NULL;
}


bool CGM_Dialog::OnFocusShiftRequested( unsigned int direction )
{
	if( m_vecpControl.size() == 0 )
		return false;

	if( m_pDialogManager->ControlFocus() )
	{
		if( m_pDialogManager->ControlFocus()->m_pDialog != this )
			return false;

		// if a sub-dialog button is currently focused and its sub-dialog is open,
		// focus shifts should not happen in this dialog
		if( m_pDialogManager->ControlFocus()->GetType() == CGM_Control::SUBDIALOGBUTTON
			&& ((CGM_SubDialogButton *)m_pDialogManager->ControlFocus())->IsSubDialogOpen() )
			return false;

		// no focus shifting while a button is pressed
		CGM_Button *pButton = dynamic_cast<CGM_Button *>(m_pDialogManager->ControlFocus());
		if( pButton && pButton->IsPressed() )
			return false;

		/* 070225 - skip controls that cannot have focus. e.g.) static controls */
		CGM_Control *pNext = GetControlForNextFocus( m_pDialogManager->ControlFocus(), direction );

		if( pNext )
		{
			// move the focus to the control 'pNext'
			bool focus_shifted = m_pDialogManager->RequestFocus( pNext );
			if( focus_shifted )
				SendEvent( CGM_Event::FOCUS_SHIFTED, true, pNext );
			return true;
		}
		else
		{
			// no control was found for the next focus
			return false;
		}
	}
	else
	{
		CGM_Control* pFirstFocus = GetControlForFirstFocus();
		if( pFirstFocus )
		{
			bool focus_shifted = m_pDialogManager->RequestFocus( pFirstFocus );

			// send a FOCUS_SHIFTED event or not?
			// - There has been no focus until now, so 'shifted' is not exactly the right word
			//   - A new focus is set. What kinda event should be sent?

			return focus_shifted;
//			m_pDialogManager->ControlFocus() = pFirstFocus;
//			m_pDialogManager->ControlFocus()->OnFocusIn();
//			return true;
		}

		// the dialog has no control to focus on
		return false;
	}
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


CGM_ControlRendererManager *CGM_Dialog::GetRendererMgr()
{
	return m_pDialogManager->GetControlRendererManager();
}


CGM_Static *CGM_Dialog::AddControl( CGM_StaticDesc *pStaticDesc )
{
	CGM_Static *pNewControl = new CGM_Static( this, pStaticDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pStaticDesc->pRenderer ? pStaticDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateStaticRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_Button *CGM_Dialog::AddControl( CGM_ButtonDesc *pButtonDesc )
{
	CGM_Button *pNewControl = new CGM_Button( this, pButtonDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pButtonDesc->pRenderer ? pButtonDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateButtonRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_SubDialogButton *CGM_Dialog::AddControl( CGM_SubDialogButtonDesc *pSubDlgDesc )
{
	CGM_SubDialogButton *pNewControl = new CGM_SubDialogButton( this, pSubDlgDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pSubDlgDesc->pRenderer ? pSubDlgDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateSubDialogButtonRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_CheckBox *CGM_Dialog::AddControl( CGM_CheckBoxDesc *pCheckBoxDesc )
{
	CGM_CheckBox *pNewControl = new CGM_CheckBox( this, pCheckBoxDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pCheckBoxDesc->pRenderer ? pCheckBoxDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateCheckBoxRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_RadioButton *CGM_Dialog::AddControl( CGM_RadioButtonDesc *pRButtonDesc )
{
	CGM_RadioButton *pNewControl = new CGM_RadioButton( this, pRButtonDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pRButtonDesc->pRenderer ? pRButtonDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateRadioButtonRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_DialogCloseButton *CGM_Dialog::AddControl( CGM_DialogCloseButtonDesc *pCButtonDesc )
{
	CGM_DialogCloseButton *pNewControl = new CGM_DialogCloseButton( this, pCButtonDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pCButtonDesc->pRenderer ? pCButtonDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateDialogCloseButtonRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_Slider *CGM_Dialog::AddControl( CGM_SliderDesc *pSliderDesc )
{
	CGM_Slider *pNewControl = new CGM_Slider( this, pSliderDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pSliderDesc->pRenderer ? pSliderDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateSliderRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_ListBox *CGM_Dialog::AddControl( CGM_ListBoxDesc *pListBoxDesc )
{
	CGM_ListBox *pNewControl = new CGM_ListBox( this, pListBoxDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pListBoxDesc->pRenderer ? pListBoxDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateListBoxRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


CGM_ScrollBar *CGM_Dialog::AddControl( CGM_ScrollBarDesc *pScrollBarDesc )
{
	CGM_ScrollBar *pNewControl = new CGM_ScrollBar( this, pScrollBarDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pScrollBarDesc->pRenderer ? pScrollBarDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreateScrollBarRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( *pNewControl );
	return pNewControl;
}


#ifdef UI_EXTENSION_EDIT
// NOT IMPLEMENTED YET
CGM_PaintBar *CGM_Dialog::AddControl( CGM_PaintBarDesc *pPaintBarDesc )
{
	CGM_PaintBar *pNewControl = new CGM_PaintBar( this, pPaintBarDesc );
	CGM_ControlRendererSharedPtr pRenderer
		= pPaintBarDesc->pRenderer ? pPaintBarDesc->pRenderer : CGM_ControlRendererSharedPtr( GetRendererMgr()->CreatePaintBarRenderer( pNewControl ) );

	RegisterControl( pNewControl, pRenderer );
	pRenderer->Init( pNewControl );
	return pNewControl;
}
#endif /* UI_EXTENSION_EDIT */


CGM_Control *CGM_Dialog::AddControl( CGM_ControlDesc *pControlDesc )
{
	if( !pControlDesc )
		return NULL;

	switch(pControlDesc->GetType())
	{
	case CGM_Control::STATIC:            return AddControl( dynamic_cast<CGM_StaticDesc *>(pControlDesc) );
	case CGM_Control::BUTTON:			 return AddControl( dynamic_cast<CGM_ButtonDesc *>(pControlDesc) );
	case CGM_Control::SUBDIALOGBUTTON:	 return AddControl( dynamic_cast<CGM_SubDialogButtonDesc *>(pControlDesc) );
	case CGM_Control::CHECKBOX:			 return AddControl( dynamic_cast<CGM_CheckBoxDesc *>(pControlDesc) );
	case CGM_Control::RADIOBUTTON:		 return AddControl( dynamic_cast<CGM_RadioButtonDesc *>(pControlDesc) );
	case CGM_Control::DIALOGCLOSEBUTTON: return AddControl( dynamic_cast<CGM_DialogCloseButtonDesc *>(pControlDesc) );
	case CGM_Control::SLIDER:			 return AddControl( dynamic_cast<CGM_SliderDesc *>(pControlDesc) );
	case CGM_Control::LISTBOX:			 return AddControl( dynamic_cast<CGM_ListBoxDesc *>(pControlDesc) );
	case CGM_Control::SCROLLBAR:		 return AddControl( dynamic_cast<CGM_ScrollBarDesc *>(pControlDesc) );
	default:
		return NULL;
	}
}


void CGM_Dialog::RegisterControl( CGM_Control *pNewControl, CGM_ControlRendererSharedPtr pRenderer )
{
	if( !pNewControl )
		return;

	CGM_ControlRendererManager* pRendererMgr = m_pDialogManager->GetControlRendererManager();
//	CGM_ControlRendererSharedPtr pRenderer = pControlDesc->pRenderer;

	// set default sound player
	if( !pNewControl->HasSoundPlayer() )
		pNewControl->SetSoundPlayer( m_pDialogManager->GetDefaultSoundPlayer() );

	// set links for focus shifts
	if( 0 < m_vecpControl.size() )
	{
		// vertical direction
        m_vecpControl.back()->SetNextControl( CGM_Control::NEXT_DOWN, pNewControl );
		pNewControl->SetNextControl( CGM_Control::NEXT_UP, m_vecpControl.back() );

		// wrapping
		m_vecpControl[0]->SetNextControl( CGM_Control::NEXT_UP, pNewControl );
		pNewControl->SetNextControl( CGM_Control::NEXT_DOWN, m_vecpControl[0] );
 
		// horizontal direction
		m_vecpControl.back()->SetNextControl( CGM_Control::NEXT_RIGHT, pNewControl );
		pNewControl->SetNextControl( CGM_Control::NEXT_LEFT, m_vecpControl.back() );

		// wrapping
		m_vecpControl[0]->SetNextControl( CGM_Control::NEXT_LEFT, pNewControl );
		pNewControl->SetNextControl( CGM_Control::NEXT_RIGHT, m_vecpControl[0] );
	}

	// add to the list of controls
	m_vecpControl.push_back( pNewControl );

	CGM_Control *pFirstFocus = GetControlForFirstFocus();
	if( pFirstFocus )
	{
		m_pLastFocusedControl = pFirstFocus;

//		if( pFirstFocus->GetType() == CGM_Control::LISTBOX )
//			MsgBoxFmt( "first focus - %d", pFirstFocus->GetType() );
	}

	// the first control in the root dialog will be given the initial focus
	// 21:32 2007-12-22 - changed: root dialog is opened by CGM_DialogManager::OpenRootDialog()
	//                    and the first focus is set there
//	if( m_bRootDialog && pFirstFocus )
//		RequestFocus( pFirstFocus/*pNewControl*/ );

	// init control renderer
	if( pRenderer.get() )
	{
		pNewControl->SetRendererSharedPtr( pRenderer );
		pRenderer->SetControlRendererManager( m_pDialogManager->GetControlRendererManager() );
		pRenderer->SetControl( pNewControl );
//		pRenderer->Init();
	}
}


void SetBasicDesc( int id, const SRect& bound_rect, CGM_ControlDesc* desc, CGM_ControlRendererSharedPtr pRenderer )
{
	desc->ID = id;
	desc->Rect = bound_rect;
	desc->pRenderer = pRenderer;
}


void SetStaticDesc( int id, const SRect& bound_rect, const string& title, CGM_StaticDesc* desc, CGM_ControlRendererSharedPtr pRenderer )
{
	SetBasicDesc( id, bound_rect, desc, pRenderer );
	desc->strText = title;
}


CGM_Static *CGM_Dialog::AddStatic( int id, const SRect& bound_rect, const string& title, CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_StaticDesc desc;
	SetStaticDesc( id, bound_rect, title, &desc, pRenderer );
	return AddControl( &desc );
}

CGM_Button *CGM_Dialog::AddButton( int id, const SRect& bound_rect, const string& title, CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_ButtonDesc desc;
	SetStaticDesc( id, bound_rect, title, &desc, pRenderer );
	return AddControl( &desc );
}

CGM_CheckBox *CGM_Dialog::AddCheckBox( int id, const SRect& bound_rect, const string& title, bool checked, CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_CheckBoxDesc desc;
	desc.bChecked = checked;
	SetStaticDesc( id, bound_rect, title, &desc, pRenderer );
	return AddControl( &desc );
}

CGM_RadioButton *CGM_Dialog::AddRadioButton( int id, const SRect& bound_rect, int group, const std::string& title, CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_RadioButtonDesc desc;
	SetStaticDesc( id, bound_rect, title, &desc, pRenderer );
	desc.iButtonGroup = group;
	return AddControl( &desc );
}

CGM_SubDialogButton *CGM_Dialog::AddSubDialogButton( int id, const SRect& bound_rect, const string& title, CGM_Dialog* pSubDialog, CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_SubDialogButtonDesc desc;
	SetStaticDesc( id, bound_rect, title, &desc, pRenderer );
	desc.pSubDialog = pSubDialog;
	return AddControl( &desc );
}

CGM_DialogCloseButton *CGM_Dialog::AddDialogCloseButton( int id, const SRect& bound_rect, const std::string& title, CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_DialogCloseButtonDesc desc;
	SetStaticDesc( id, bound_rect, title, &desc, pRenderer );
	return AddControl( &desc );
}

CGM_ListBox *CGM_Dialog::AddListBox( int id, const SRect& bound_rect, const std::string& title, int style_flag, int item_text_height,
									 CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_ListBoxDesc desc;
	desc.ID          = id;
	desc.Rect        = bound_rect;
	desc.Style       = style_flag;
	desc.nTextHeight = item_text_height;
	desc.pRenderer   = pRenderer;
	return AddControl( &desc );
}


CGM_Slider *CGM_Dialog::AddSlider( int id, const SRect& bound_rect, int min_val, int max_val, int init_val,
								   CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_SliderDesc desc;
	desc.ID            = id;
	desc.Rect          = bound_rect;
	desc.iMin          = min_val;
	desc.iMax          = max_val;
	desc.iInitialValue = init_val;
	desc.pRenderer     = pRenderer;
	return AddControl( &desc );
}

/*
CGM_ScrollBar *CGM_Dialog::AddScrollBar( int id, const SRect& bound_rect )
{
	CGM_ScrollBarDesc desc;
	SetBasicDesc( id, bound_rect, &desc );
	return AddControl( &desc );
}*/


CGM_Control* CGM_Dialog::GetControl( int id )
{
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
		if( m_vecpControl[i]->GetID() == id )
			return m_vecpControl[i];
	}

	return NULL;
}


CGM_Control* CGM_Dialog::GetControl( const std::string& string_id )
{
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
		if( m_vecpControl[i]->GetStringID() == string_id )
			return m_vecpControl[i];
	}

	return NULL;
}


void CGM_Dialog::SendEvent( CGM_Event::EventType event_type, bool bTriggeredByUser, CGM_ControlBase* pControl, CGM_SubEvent::Type sub_event )
{
	CGM_Event event( event_type, pControl );
	event.SubType = sub_event;

    if( m_pEventHandler || m_pEventHandlerFn )
	{
		// Discard events triggered programatically if these types of events haven't been
		// enabled
		if( !bTriggeredByUser && !m_bNonUserEvents )
			return;

		if( m_pEventHandler )
			m_pEventHandler->HandleEvent( event );
		else if( m_pEventHandlerFn )
			(*m_pEventHandlerFn)( event );
	}

	// play sound
	if( m_pSoundPlayer.get() )
	{
		m_pSoundPlayer->HandleEvent( event );
	}
}


void CGM_Dialog::RequestFocus( CGM_Control* pControl )
{
	m_pDialogManager->RequestFocus( pControl );
}


bool CGM_Dialog::IsOpen() const
{
	return m_bIsOpen;
}


void CGM_Dialog::Open()
{
	LOG_PRINT( " (" + m_strTitle + ")" );

	m_bIsOpen = true;

	if( m_pRenderer )
		m_pRenderer->OnDialogOpened( *this );

	SendEvent( CGM_Event::DIALOG_OPENED, true, this );

	// the first control in this dialog will be given the focus
	// changed - after opening the dialog, focus will be given
	//           from the owner sub-dialog button
/*	if( 0 < m_vecpControl.size() )
	{
		RequestFocus( m_vecpControl[0] );
	}*/
}


void CGM_Dialog::Close( CGM_SubEvent::Type sub_event )
{
	LOG_PRINT( " (" + m_strTitle + ")" );

	if( !IsOpen() )
		return; // already closed

	if( m_StyleFlag & STYLE_ALWAYS_OPEN )
	{
		if( m_pRenderer )
			m_pRenderer->OnOpenDialogAttemptedToClose( *this );

		SendEvent( CGM_Event::OPENDIALOG_ATTEMPTED_TO_CLOSE, true, this );
		return; // the dialog cannot be closed
	}

	// close all the sub-dialogs linked to this dialog by sub-dialog buttons
	CloseSubDialogs();

	m_bIsOpen = false;

	if( m_pRenderer )
	{
		m_pRenderer->OnDialogClosed( *this );
	}

//	if( m_pDialogManager->ControlFocus() && m_pDialogManager->ControlFocus()->m_pDialog == this )
//		m_pLastFocusedControl = m_pDialogManager->ControlFocus();
	UpdateLastFocusedControl();

	if( m_pDialogManager->ControlFocus()
	 && m_pDialogManager->ControlFocus()->m_pDialog == this )
	{
		// There is a focused control and it belongs to this dialog
		m_pDialogManager->ControlFocus()->OnFocusOut();
		m_pDialogManager->ControlFocus() = NULL;
	}

	if( m_pOwnerButton )
        m_pOwnerButton->OnSubDialogClosed();

	SendEvent( CGM_Event::DIALOG_CLOSED, true, this, sub_event );

	m_pDialogManager->OnDialogClosed( this );
}


// --- NOT PROPERLY IMPLEMENTED ---
bool CGM_Dialog::OpenNextDialog( unsigned int direction )
{
	LOG_PRINT( " (" + m_strTitle + ") direction: " + to_string(direction) );

	CGM_Dialog *pNextDialog = m_apNextDialog[direction];
/*
	if( !pNextDialog || !pNextDialog->m_pOwnerButton )
		return false;

	// first, close this dialog
	Close();

	pNextDialog->m_pOwnerButton->OpenDialog();
*/

	if( !pNextDialog )
		return false;

	// first, close this dialog
	Close();

	if( pNextDialog->m_pOwnerButton )
	{
		// set focus on the last focused control in m_pOwnerButton->m_pDialog
		// set focus on m_pOwnerButton
		// update last focused control of m_pOwnerButton->m_pDialog, which is m_pOwnerButton

		// move to non-root dialog
		pNextDialog->m_pOwnerButton->OpenDialog();
	}
	else
	{
		// move to root dialog
//		pNextDialog->OnDialogFocused();
		pNextDialog->Open();
	}

	pNextDialog->SetFocusOnLastFocusedControl();

	return true;
}


void CGM_Dialog::CloseSubDialogs()
{
	CGM_SubDialogButton *pSubDialogButton;

	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
		if( m_vecpControl[i]->GetType() == CGM_Control::SUBDIALOGBUTTON )
		{
			pSubDialogButton = (CGM_SubDialogButton *)m_vecpControl[i];

			// all the child subdialogs will also be recursively closed in this call
			pSubDialogButton->CloseSubDialogs();
		}
	}

}


CGM_Control *CGM_Dialog::GetControlAtPoint( SPoint& pt )
{
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i < num_controls; i++ )
	{
		if( m_vecpControl[i]->IsPointInside( pt ) )
			return m_vecpControl[i];
	}

	return NULL;
}


void CGM_Dialog::ClearRadioButtonGroup( int iButtonGroup )
{
    // Find all radio buttons with the given group number
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i < num_controls; i++ )
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
    if( m_pDialogManager->ControlFocus() )
    {
        m_pDialogManager->ControlFocus()->OnFocusOut();
        m_pDialogManager->ControlFocus() = NULL;
    }
}


void CGM_Dialog::ClearPressedControl()
{
	if( m_pDialogManager->ControlPressed() )
	{
//		m_pDialogManager->ControlPressed()->OnFocusOut();
		m_pDialogManager->ControlPressed() = NULL;
	}
}


void CGM_Dialog::ChangeScale( float factor )
{
	CGM_ControlBase::ChangeScale( factor );

	m_iCaptionHeight = (int)(m_iCaptionHeight * factor);

	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
        m_vecpControl[i]->ChangeScale( factor );
	}
}


void CGM_Dialog::SetFocusOnLastFocusedControl()
{
	if( m_pLastFocusedControl )
		RequestFocus( m_pLastFocusedControl );
	else if( 0 < m_vecpControl.size() )
	{
		// no last focus - the dialog is opened for the first time
		// just set the focus on the first control
		RequestFocus( m_vecpControl[0] );
	}
}


void CGM_Dialog::UpdateLastFocusedControl()
{
	if( m_pDialogManager->ControlFocus() && m_pDialogManager->ControlFocus()->m_pDialog == this )
		m_pLastFocusedControl = m_pDialogManager->ControlFocus();
}


void CGM_Dialog::SetNextDialog( int direction, CGM_Dialog* pDialog )
{
	m_apNextDialog[direction] = pDialog;
}


void CGM_Dialog::SetNextDialogsUD( CGM_Dialog* pUp, CGM_Dialog* pDown )
{
	m_apNextDialog[NEXT_UP]   = pUp;
	m_apNextDialog[NEXT_DOWN] = pDown;
}


void CGM_Dialog::SetNextDialogsLR( CGM_Dialog* pLeft, CGM_Dialog* pRight )
{
	m_apNextDialog[NEXT_LEFT]  = pLeft;
	m_apNextDialog[NEXT_RIGHT] = pRight;
}


CGM_Control* CGM_Dialog::GetControlByID( int id )
{
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
        if( m_vecpControl[i]->GetID() == id )
			return m_vecpControl[i];	// id matched
	}

	return NULL;	// not found
}


CGM_Control* CGM_Dialog::GetControlByStringID( const std::string& string_id )
{
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
        if( m_vecpControl[i]->GetStringID() == string_id )
			return m_vecpControl[i];	// id matched
	}

	return NULL;	// not found
}
	

void CGM_Dialog::SetDepth( int depth )
{
	if( MAX_UI_LAYER_DEPTH <= depth )
	{
		g_Log.Print( WL_WARNING, "%s depth = %d", __FUNCTION__, depth );
		return; // no more recursive calls
	}

	// set depth of this dialog
	CGM_ControlBase::SetDepth( depth );

	// set depth of the controls on this dialog
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
		// set depth recursively
		// - subdialog button control call SetDepth() of its subdialog
		m_vecpControl[i]->SetDepth( depth + 1 );
	}
}


int CGM_Dialog::GetMaxDepth() const
{
	CGM_SubDialogButton *pSubDialogButton;

	int depth = 0;
	int max_depth = GetDepth() + 1;
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
		if( m_vecpControl[i]->GetType() == CGM_Control::SUBDIALOGBUTTON )
		{
			pSubDialogButton = dynamic_cast<CGM_SubDialogButton *>(m_vecpControl[i]);

			depth = pSubDialogButton->GetMaxDepth();

			if( max_depth < depth )
                max_depth = depth;
		}
	}

	// controls on the current dialog are the leaf nodes
	return max_depth;
}


void CGM_Dialog::UpdateGraphicsProperties()
{
	// update the controls first
	size_t i, num_controls = m_vecpControl.size();
	for( i=0; i<num_controls; i++ )
	{
		m_vecpControl[i]->UpdateGraphicsProperties();
	}

	// then update the dialog
	CGM_ControlBase::UpdateGraphicsProperties();
}

