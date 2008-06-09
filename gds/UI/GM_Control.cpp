
#include "GM_Control.h"
#include "GM_ControlDescBase.h"
#include "GM_Dialog.h"
#include "GM_DialogManager.h"
#include "GM_ControlRenderer.h"
#include "GM_ControlRendererManager.h"


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
	if( m_pRenderer.get() )
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

	if( m_pRenderer.get() )
		m_pRenderer->ChangeScale( factor );
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
        m_apNext[i] = NULL;
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
		m_pRenderer->OnFocused();
	}
}


void CGM_Control::OnFocusOut()
{
	m_bHasFocus = false;

	if( m_pRenderer )
	{
		m_pRenderer->OnFocusCleared();
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

/*
void CGM_Control::RegisterRenderer( CGM_ControlRenderer* pRenderer )
{
//	m_pDialog->GetDalogManager()->GetControlRendererManager()->RegisterControlRenderer( pRenderer, this );
}
*/