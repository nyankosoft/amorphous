
#include "GM_Button.hpp"

#include "GM_ControlDescBase.hpp"
#include "GM_Dialog.hpp"
#include "GameInput/InputHandler.hpp"

#include "Graphics/2DFrameRect.hpp"


//========================================================================================
// CGM_Button
//========================================================================================

CGM_Button::CGM_Button( CGM_Dialog *pDialog, CGM_ButtonDesc *pDesc )
: CGM_Static( pDialog, pDesc )
{
	m_bPressed = pDesc->bPressed;
}


bool CGM_Button::HandleMouseInput( SInputData& input )
{
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
//				SetCapture( DXUTGetHWND() );

                if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );

                return true;
            }
        }
		else if( input.iType == ITYPE_KEY_RELEASED )
		{
            if( m_bPressed )
            {
                m_bPressed = false;
//				ReleaseCapture();

//                if( !m_pDialog->m_bKeyboardInput )
//                    m_pDialog->ClearFocus();

                // Button click
                if( IsPointInside( pt ) )
                    m_pDialog->SendEvent( GM_EVENT_BUTTON_CLICKED, true, this );

                return true;
            }
        }
        break;
    }
    
    return false;
}


//========================= debug ==============================//

#include "Graphics/Direct3D9.hpp"
#include "Graphics/2DRect.hpp"
#include "Graphics/font.h"


void SetTextureStageStatesForFont()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// blend texture color and diffuse color
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
}


void CGM_Button::Render( float fElapsedTime )
{
	// debug

//	float width = (float)GM_SCREEN_WIDTH;
	D3DXVECTOR2 vTopLeft     = D3DXVECTOR2( m_BoundingBox.left,  m_BoundingBox.top );
	D3DXVECTOR2 vBottomRight = D3DXVECTOR2( m_BoundingBox.right, m_BoundingBox.bottom );
	int iState;


  if( m_DefaultRect.m_pRenderRoutine )
  {
    m_DefaultRect.m_pRenderRoutine->Render( fElapsedTime, m_DefaultRect );
  }
  else
  {
///	C2DRect rect;
	C2DFrameRect rect;

	rect.SetBorderWidth( 2 );

	rect.SetPosition( vTopLeft, vBottomRight );

	if( m_bMouseOver == true )
		rect.SetColor( 0xFF5050FF );
	else
		rect.SetColor( 0xFFFFFFFF );

///	rect.DrawWireframe();
	rect.Draw();

	if( m_bPressed )
		iState = GM_STATE_PRESSED;
	else if( m_bMouseOver )
		iState = GM_STATE_MOUSEOVER;
	else
		iState = GM_STATE_NORMAL;

	m_DefaultRect.m_RectColor.Blend( iState, fElapsedTime, 0.2f );
	m_DefaultRect.Draw( m_pDialog );
  }


	DWORD dwColor = D3DCOLOR_ARGB( (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fAlpha * 255.0),
		                           (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fRed   * 255.0),
								   (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fGreen * 255.0f),
								   (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fBlue  * 255.0f) );

	SetTextureStageStatesForFont();

	CFontBase *pFont = m_pDialog->GetFont( m_DefaultFont.m_iFontIndex );
	pFont->DrawText( m_strText.c_str(), vTopLeft, dwColor );
}



//========================================================================================
// CGM_CheckBox
//========================================================================================

CGM_CheckBox::CGM_CheckBox( CGM_Dialog *pDialog, CGM_CheckBoxDesc *pDesc )
: CGM_Button( pDialog, pDesc )
{
	m_bChecked = pDesc->bChecked;
}


CGM_CheckBox::SetCheckedInternal( bool bChecked, bool bFromInput )
{
    m_bChecked = bChecked;

	m_pDialog->SendEvent( GM_EVENT_CHECKBOX_CHANGED, bFromInput, this ); 
}


bool CGM_CheckBox::HandleMouseInput( SInputData& input )
{
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
//				SetCapture( DXUTGetHWND() );

				if( !m_bHasFocus /*&& m_pDialog->m_bKeyboardInput*/ )
					m_pDialog->RequestFocus( this );

                return true;
            }
        }
		else if( input.iType == ITYPE_KEY_RELEASED )
		{
            if( m_bPressed )
            {
                m_bPressed = false;
//				ReleaseCapture();

//                if( !m_pDialog->m_bKeyboardInput )
//                    m_pDialog->ClearFocus();

                // Button click
                if( IsPointInside( pt ) )
                    SetCheckedInternal( !m_bChecked, true );

                return true;
            }
        }
        break;
    }

    return false;
}


//========================= debug ==============================//

void CGM_CheckBox::Render( float fElapsedTime )
{
	// debug
//	float width = (float)GM_SCREEN_WIDTH;
	D3DXVECTOR2 vTopLeft     = D3DXVECTOR2( m_BoundingBox.left,  m_BoundingBox.top );
	D3DXVECTOR2 vBottomRight = D3DXVECTOR2( m_BoundingBox.right, m_BoundingBox.bottom );
	C2DRect rect;
//	C2DFrameRect rect;

	rect.SetPosition( vTopLeft, vBottomRight );

//	rect.SetBorderWidth(2);

	if( m_bMouseOver == true )
		rect.SetColor( 0xFF5050FF );
	else if( m_bChecked )
		rect.SetColor( 0xFFFF1010 );
	else
		rect.SetColor( 0xFFFFFFFF );

	rect.DrawWireframe();
//	rect.Draw();

	int iState;
	if( m_bPressed )
		iState = GM_STATE_PRESSED;
	else if( m_bMouseOver )
		iState = GM_STATE_MOUSEOVER;
	else
		iState = GM_STATE_NORMAL;

	DWORD dwColor = D3DCOLOR_ARGB( (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fAlpha * 255.0),
		                           (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fRed   * 255.0),
								   (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fGreen * 255.0f),
								   (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fBlue  * 255.0f) );

	SetTextureStageStatesForFont();

	CFontBase *pFont = m_pDialog->GetFont( m_DefaultFont.m_iFontIndex );
	pFont->DrawText( m_strText.c_str(), vTopLeft, dwColor );
}



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
    m_pDialog->SendEvent( GM_EVENT_RADIOBUTTON_CHANGED, bFromInput, this );
}


bool CGM_RadioButton::HandleMouseInput( SInputData& input )
{
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
//				SetCapture( DXUTGetHWND() );
				
//				if( !m_bHasFocus && m_pDialog->m_bKeyboardInput )
//                    m_pDialog->RequestFocus( this );

                return true;
            }
        }

		else if( input.iType == ITYPE_KEY_RELEASED )
		{
            if( m_bPressed )
            {
                m_bPressed = false;
//				ReleaseCapture();

                // Button click
                if( IsPointInside( pt ) )
                {
                    m_pDialog->ClearRadioButtonGroup( m_iButtonGroup );
                    m_bChecked = !m_bChecked;

                    m_pDialog->SendEvent( GM_EVENT_RADIOBUTTON_CHANGED, true, this );
                }

                return true;
            }
        }
		break;
    }
    
    return false;
}


void CGM_RadioButton::Render( float fElapsedTime )
{
	CGM_CheckBox::Render( fElapsedTime );
}
