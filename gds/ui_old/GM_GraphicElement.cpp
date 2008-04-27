
#include "GM_GraphicElement.h"
#include "GM_Dialog.h"



void CGM_BlendColor::Blend( int iState, float fElapsedTime, float fRate )
{
	float f = 1.0f - powf( fRate, 30 * fElapsedTime );
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

void CGM_TextureRectElement::Draw( CGM_Dialog *pDialog )
{
	m_2DRect.SetColor( m_RectColor.m_CurrentColor.Get32BitARGBColor() );

	if( 0 <= m_iTextureIndex )
	{
		m_2DRect.Draw( pDialog->GetTexture( m_iTextureIndex ) );
	}
	else
	{
		m_2DRect.Draw();
	}

}


void CGM_TextureRectElement::SetPosition( float fPosX, float fPosY, float fWidth, float fHeight )
{
	m_Rect.SetValues( fPosX, fPosY, fPosX + fWidth, fPosY + fHeight );

	m_2DRect.SetPosition( D3DXVECTOR2( fPosX, fPosY ),
		                  D3DXVECTOR2( fPosX + fWidth, fPosY + fHeight ) );
}


void CGM_TextureRectElement::UpdateRect()
{
	m_2DRect.SetPosition( D3DXVECTOR2(m_Rect.left,  m_Rect.top),
		                  D3DXVECTOR2(m_Rect.right, m_Rect.bottom) );
}