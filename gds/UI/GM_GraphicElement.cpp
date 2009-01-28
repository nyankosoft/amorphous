
#include "GM_GraphicElement.h"
#include "GM_Dialog.h"

#include <math.h>


void CGM_BlendColor::Blend( int iState, float fElapsedTime, float fRate )
{
//	m_CurrentColor = SFloatRGBAColor( 1.0, 1.0, 1.0, 1.0 );
//	m_CurrentColor = m_aColor[iState];
//	return;

//	float f = 1.0f - powf( fRate, 30 * fElapsedTime );
	float f = 1.0f - (float)pow( fRate, 30.0f * fElapsedTime );
	m_CurrentColor = m_CurrentColor + ( m_aColor[iState] - m_CurrentColor ) * f;
}


/*
void DXUTBlendColor::Blend( UINT iState, float fElapsedTime, float fRate )
{
    D3DXCOLOR destColor = States[ iState ];
    D3DXColorLerp( &Current, &Current, &destColor, 1.0f - powf( fRate, 30 * fElapsedTime ) );
}
*/

/*
CGM_GraphicElement::CGM_GraphicElement()
{
	m_iFont = m_iTexture = 0;
}


void CGM_GraphicElement::SetFont( int iFont, D3DCOLOR defaultFontColor, DWORD dwTextFormat )
{
    this->iFont = iFont;
    this->dwTextFormat = dwTextFormat;

	m_FontColor.Init( defaultFontColor );
}
*/