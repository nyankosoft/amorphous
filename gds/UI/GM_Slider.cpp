
#include "GM_Slider.h"
#include "GM_ControlDescBase.h"
#include "GM_Dialog.h"
#include "GM_Input.h"
#include "GM_ControlRenderer.h"


//========================================================================================
// CGM_Slider
//========================================================================================

CGM_Slider::CGM_Slider( CGM_Dialog *pDialog, CGM_SliderDesc *pDesc )
: CGM_Control( pDialog, pDesc )
{
	m_iMax   = pDesc->iMax;
	m_iMin   = pDesc->iMin;
	m_iValue = pDesc->iInitialValue;

/*	if( pDesc->pRectElement )
		m_BoundingRectElement = *pDesc->pRectElement;
	else
	{	// rect is not specified - use the default bounding box
		m_BoundingRectElement.SetPosition( m_fPosX, m_fPosY, m_fWidth, m_fHeight );
		m_BoundingRectElement.m_RectColor.m_aColor[CGM_Control::STATE_NORMAL].SetRGBA(0.7f, 0.7f, 0.7f, 0.8f);
		m_BoundingRectElement.m_RectColor.m_aColor[CGM_Control::STATE_PRESSED].SetRGBA(0.7f, 0.7f, 0.7f, 0.8f);
		m_BoundingRectElement.m_RectColor.m_aColor[CGM_Control::STATE_MOUSEOVER].SetRGBA(0.7f, 0.7f, 0.7f, 0.8f);
	}

	if( pDesc->pButtonElement )
		m_ButtonElement = *pDesc->pButtonElement;
	else
	{	// rect is not specified - set the default button
		m_ButtonElement.m_RectColor.m_aColor[CGM_Control::STATE_NORMAL].SetRGBA(0.7f, 0.7f, 0.7f, 1.0f);
		m_ButtonElement.m_RectColor.m_aColor[CGM_Control::STATE_PRESSED].SetRGBA(1.0f, 1.0f, 1.0f, 1.0f);
		m_ButtonElement.m_RectColor.m_aColor[CGM_Control::STATE_MOUSEOVER].SetRGBA(0.8f, 0.8f, 0.8f, 1.0f);
	}

	UpdateRects();*/
}


bool CGM_Slider::IsPointInside( SPoint& pt )
{
	if( m_BoundingBox.ContainsPoint(pt) ||
		m_ButtonRect.ContainsPoint(pt) )
		return true;
	else
		return false;
}


bool CGM_Slider::HandleMouseInput( CGM_InputData& input )
{
    if( !IsEnabled() || !IsVisible() )
        return false;

	SPoint pt = input.pos;
    switch( input.code )
    {
	case CGM_Input::MOUSE_BUTTON_L:
        if( input.type == CGM_InputData::TYPE_PRESSED )
		{
            if( m_ButtonRect.ContainsPoint( pt ) )
            {
                // Pressed while inside the control
                m_bPressed = true;
//				SetCapture( DXUTGetHWND() );

				m_iDragX = pt.x;
//				m_iDragY = pt.y;
				m_iDragOffset = m_iButtonX - m_iDragX;

				// m_nDragValue = m_nValue;

                if( !HasFocus() )
                    m_pDialog->RequestFocus( this );

                return true;
            }

			if( m_BoundingBox.ContainsPoint( pt ) )
			{	// pressed inside the slider control
				if( m_iButtonX + m_BoundingBox.left < pt.x )
				{
					SetValueInternal( m_iValue + 1, true );
					return true;
				}

				if( pt.x < m_BoundingBox.left + m_iButtonX )
				{
					SetValueInternal( m_iValue - 1, true );
					return true;
				}
			}

			break;
		}

		else if( input.type == CGM_InputData::TYPE_RELEASED )
		{
			if( IsPressed() )
			{
				m_bPressed = false;
//				ReleaseCapture();

//				m_pDialog->ClearFocus();
				m_pDialog->SendEvent( CGM_Event::SLIDER_VALUE_CHANGED, true, this );

				return true;
			}
		}
		break;

	case CGM_Input::MOUSE_AXIS_X:
	case CGM_Input::MOUSE_AXIS_Y:
		if( IsPressed() )
		{
			SetValueInternal( ValueFromPos( m_BoundingBox.left + pt.x + m_iDragOffset ), true );
			return true;
		}
		break;

	}

	return false;
}


void CGM_Slider::UpdateRects()
{
//	CGM_Control::UpdateRects();

	// set the button to the left corner of the slider
    m_ButtonRect = m_BoundingBox;
    m_ButtonRect.right = m_ButtonRect.left + m_ButtonRect.GetHeight();
    m_ButtonRect.Offset( -m_ButtonRect.GetWidth()/2, 0 );

	// calc the position of the button according to the current value, 'm_iValue'
    m_iButtonX = (int) ( (m_iValue - m_iMin) * (float)m_BoundingBox.GetWidth() / (m_iMax - m_iMin) );
    m_ButtonRect.Offset( m_iButtonX, 0 );
}


void CGM_Slider::SetRange( int iMin, int iMax )
{
	m_iMin = iMin;
	m_iMax = iMax;

    SetValueInternal( m_iValue, false );

}


int CGM_Slider::ValueFromPos( int x )
{ 
	float fValuePerPixel = (float)(m_iMax - m_iMin) / (float)m_BoundingBox.GetWidth();
	return (int) (0.5f + m_iMin + fValuePerPixel * (x - m_BoundingBox.left));
}


void CGM_Slider::SetValueInternal( int iValue, bool bFromInput )
{
    // Clamp to range
    iValue = iValue < m_iMin ? m_iMin : iValue;	// max( m_iMin, iValue );
    iValue = m_iMax < iValue ? m_iMax : iValue;	// min( m_iMax, iValue );
    
    if( iValue == m_iValue )
        return;

    m_iValue = iValue;

    UpdateRects();

    m_pDialog->SendEvent( CGM_Event::SLIDER_VALUE_CHANGED, bFromInput, this );

	if( m_pRenderer )
		m_pRenderer->OnSliderValueChanged();
}


int CGM_Slider::ValueFromPos( int x, int y )
{
	float fValuePerPixel = (float)(m_iMax - m_iMin) / m_BoundingBox.GetWidth();

	return (int)(0.5f + m_iMin + fValuePerPixel * (x - m_BoundingBox.left)) ; 
}


void CGM_Slider::Render()
{
//	m_pRenderManager->RenderSlider( *this );
}


/*
void CGM_Slider::Render()
{
	int iOffsetX = 0;
	int iOffsetY = 0;

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

//	int iState;
//	if( m_bVisible == false )		iState = DXUT_STATE_HIDDEN;
//	else if( m_bEnabled == false )	iState = DXUT_STATE_DISABLED;
//	else if( m_bHasFocus )			iState = DXUT_STATE_FOCUS;
//	else if( m_bPressed )

	if( m_bPressed )
	{
		iState = CGM_Control::STATE_PRESSED;

        iOffsetX = 1;
        iOffsetY = 2;
	}
	else if( m_bMouseOver )
	{
		iState = CGM_Control::STATE_MOUSEOVER;
        
        iOffsetX = -1;
        iOffsetY = -2;
	}
	else
		iState = CGM_Control::STATE_NORMAL;

	float fBlendRate = ( iState == CGM_Control::STATE_PRESSED  ) ? 0.0f : 0.8f;

    // Blend current color
	m_BoundingRectElement.m_RectColor.Blend( iState, fElapsedTime, fBlendRate );
	m_BoundingRectElement.Draw( m_pDialog );


	m_ButtonElement.SetPosition( (m_ButtonRect.left + m_ButtonRect.right) / 2 - 3,	//+ m_iButtonX - 3,
		                         m_ButtonRect.top + 2,
		                         6,
								 m_ButtonRect.bottom - m_ButtonRect.top - 4 );

	m_ButtonElement.m_RectColor.Blend( iState, fElapsedTime, fBlendRate );
//	m_ButtonElement.m_RectColor.m_CurrentColor.SetRGBA( 1,1,1,1 );
	m_ButtonElement.Draw( m_pDialog );

}*/