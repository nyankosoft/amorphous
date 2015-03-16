#include "GM_Control.hpp"
#include "GM_ControlDescBase.hpp"
#include "GM_Dialog.hpp"
#include "GM_DialogManager.hpp"
#include "GM_ControlRenderer.hpp"
#include "GM_ControlRendererManager.hpp"
#include "GM_Keybind.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


//========================================================================================
// CGM_ControlBase
//========================================================================================

CGM_ControlBase::CGM_ControlBase( CGM_ControlDescBase* pDesc )
{
	m_ID          = pDesc->ID; // integer identifier (must be unique)

	m_StringID    = pDesc->StringID; // string identifier (must be unique)

	m_BoundingBox = pDesc->Rect; // rectangular area of the control

	m_pRenderer   = pDesc->pRenderer;
}


CGM_ControlBase::~CGM_ControlBase()
{
	if( m_pRenderer.get() )
	{
		// remove this control from this renderer
		// Since renderer is a shared pointer, some other objects may still
		// holds its reference
		m_pRenderer->SetControl( NULL );
	}
}


void CGM_ControlBase::UpdateGraphicsProperties()
{
	if( m_pRenderer )
	{
		// set rendering order of graphics elements
		m_pRenderer->UpdateGraphicsLayerSettings();

		// create group(s) of graphics element
		// - dialog usually create a group element that contains
		//   all the graphics elements of its controls
		m_pRenderer->GroupGraphicsElements();
	}
}


void CGM_ControlBase::ChangeScale( float factor )
{
	m_BoundingBox *= factor;

	if( m_pRenderer )
		m_pRenderer->ChangeScale( factor );
}


int CGM_ControlBase::GetInputCode( CGM_InputData& input )
{
	if( m_pKeybind )
	{
		int code = m_pKeybind->GetInputCode( input.GeneralInputCode );
		if( 0 <= code && code < CGM_Input::NUM_INPUT_CODES )
			return code; // This could be CGM_Input::INVALID, which means the GUI input for the GI code is suppressed.
		else
			return input.code;
	}
	else
		return input.code;
}


void CGM_ControlBase::AssignCustomKey( int gi_code, int gui_input_code )
{
	if( !m_pKeybind )
		m_pKeybind.reset( new CGM_SmallKeybind );

	m_pKeybind->Assign( gi_code, (CGM_Input::InputCode)gui_input_code );
}


//========================================================================================
// CGM_Control
//========================================================================================

CGM_Control::CGM_Control( CGM_Dialog *pDialog, CGM_ControlDesc *pDesc )
:
CGM_ControlBase(pDesc)
{
	m_pDialog = pDialog;

	m_bEnabled   = pDesc->bEnabled;
	m_bVisible   = pDesc->bVisible;
	m_bIsDefault = pDesc->bIsDefault;
	m_bMouseOver = false;
	m_bHasFocus  = pDesc->bHasFocus;

	m_strCaption = pDesc->strCaption;

	const SRect& dlg_rect = m_pDialog->GetBoundingBox();
	switch( pDesc->coord_type )
	{
	case CGM_ControlDesc::COORD_LOCAL:
		m_BoundingBox = pDesc->Rect;
		m_BoundingBox.Offset( dlg_rect.left, dlg_rect.top );
		break;
	case CGM_ControlDesc::COORD_GLOBAL:
		m_BoundingBox = pDesc->Rect;
		break;
	default:
		break;
	}

//	m_pRendererManager = m_pDialog->GetDialogManager()->GetRendererManager();

	// TODO: set adjacent controls
	for( int i=0; i<NUM_MAX_ADJACENT_CONTROLS; i++ )
        m_apNext[i] = nullptr;
}


CGM_Control::~CGM_Control()
{
}


SRect CGM_Control::GetLocalRect() const
{
	SRect rect = GetBoundingBox();
	rect.Offset( - GetOwnerDialog()->GetBoundingBox().GetTopLeftCorner() );
	return rect;
}


void CGM_Control::OnFocusIn()
{
	m_bHasFocus = true;

	if( m_pRenderer )
	{
		m_pRenderer->OnFocused( *this );
	}
}


void CGM_Control::OnFocusOut()
{
	m_bHasFocus = false;

	if( m_pRenderer )
	{
		m_pRenderer->OnFocusCleared( *this );
	}
}


void CGM_Control::OnMouseEnter()
{
	m_bMouseOver = true;

	if( m_pRenderer )
	{
		m_pRenderer->OnMouseCursorEntered( *this );
	}
}


void CGM_Control::OnMouseLeave()
{
	m_bMouseOver = false;

	if( m_pRenderer )
	{
		m_pRenderer->OnMouseCursorLeft( *this );
	}
}


void CGM_Control::SetEnabled( bool bEnabled )
{
	m_bEnabled = bEnabled;

	if( m_pRenderer )
	{
		if( m_bEnabled )
			m_pRenderer->OnEnabled( *this );
		else
			m_pRenderer->OnDisabled( *this );
	}
}


void CGM_Control::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	if( m_pRenderer )
	{
		if( m_bVisible )
			m_pRenderer->OnVisibleSetToTrue( *this );
		else
			m_pRenderer->OnVisibleSetToFalse( *this );
	}
}


unsigned int CGM_Control::GetState() const
{
	if( !IsVisible() )		return STATE_HIDDEN;
	else if( !IsEnabled() )	return STATE_DISABLED;
	else if( HasFocus() )	return STATE_FOCUS;
	else if ( IsMouseOver() ) return STATE_MOUSEOVER;
	else return STATE_NORMAL;
}


} // namespace amorphous
