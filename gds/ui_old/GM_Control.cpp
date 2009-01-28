
#include "GM_Control.h"
#include "GM_ControlDescBase.h"

#include "GameInput/InputHandler.h"


//========================================================================================
// CGM_Control
//========================================================================================

CGM_Control::CGM_Control( CGM_Dialog *pDialog, CGM_ControlDesc *pDesc )
{
	m_pDialog = pDialog;

	m_Type = pDesc->GetType();

	m_ID = pDesc->ID;
	m_fPosX   = pDesc->fPosX;
	m_fPosY   = pDesc->fPosY;
	m_fWidth  = pDesc->fWidth;
	m_fHeight = pDesc->fHeight;

	UpdateRects();

	m_bEnabled   = pDesc->bEnabled;
	m_bVisible   = pDesc->bVisible;
	m_bIsDefault = pDesc->bIsDefault;
	m_bMouseOver = false;
	m_bHasFocus  = pDesc->bHasFocus;

	m_strCaption = pDesc->strCaption;

}


CGM_Control::~CGM_Control()
{
}


void CGM_Control::UpdateRects()
{
	m_BoundingBox.SetValues( m_fPosX, m_fPosY, m_fPosX + m_fWidth, m_fPosY + m_fHeight );
}


// draw the boundary with wireframe for debug

#include "Graphics/Direct3D9.h"
#include "Graphics/2DRect.h"

void CGM_Control::Render( float fElapsedTime )
{
	// debug
//	float width = (float)GM_SCREEN_WIDTH;
	D3DXVECTOR2 vTopLeft     = D3DXVECTOR2( m_BoundingBox.left,  m_BoundingBox.top );
	D3DXVECTOR2 vBottomRight = D3DXVECTOR2( m_BoundingBox.right, m_BoundingBox.bottom );
	C2DRect rect;
	rect.SetPosition( vTopLeft, vBottomRight );

	if( m_bMouseOver == true )
		rect.SetColor( 0xFF5050FF );
	else
		rect.SetColor( 0xFFFFFFFF );

	rect.DrawWireframe();
}
