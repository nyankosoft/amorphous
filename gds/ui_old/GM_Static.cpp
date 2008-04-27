
#include "GM_Static.h"

#include "GM_ControlDescBase.h"
#include "GM_Dialog.h"


//========================================================================================
// CGM_Static
//========================================================================================

CGM_Static::CGM_Static( CGM_Dialog *pDialog, CGM_StaticDesc* pDesc )
: CGM_Control( pDialog, pDesc )
{
	m_strText = pDesc->strText;

	if( pDesc->pDefaultFontElement )
		m_DefaultFont = *pDesc->pDefaultFontElement;

	if( pDesc->pDefaultRectElement )
	{
		m_DefaultRect = *pDesc->pDefaultRectElement;
	}

	m_DefaultRect.m_RectColor.m_CurrentColor = m_DefaultRect.m_RectColor.m_aColor[GM_STATE_NORMAL];
//	m_DefaultRect.UpdateRect();
}



//========================= debug ==============================//

#include "3DCommon/Direct3D9.h"
#include "3DCommon/font.h"

void CGM_Static::Render( float fElapsedTime )
{
	// debug
	D3DXVECTOR2 vTopLeft     = D3DXVECTOR2( m_BoundingBox.left,  m_BoundingBox.top );

	int	iState = GM_STATE_NORMAL;

	DWORD dwColor = D3DCOLOR_ARGB( (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fAlpha * 255.0),
		                           (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fRed   * 255.0),
								   (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fGreen * 255.0f),
								   (int)(m_DefaultFont.m_FontColor.m_aColor[iState].fBlue  * 255.0f) );

	CFontBase *pFont = m_pDialog->GetFont( m_DefaultFont.m_iFontIndex );
	pFont->DrawText( m_strText.c_str(), vTopLeft, dwColor );

	m_DefaultRect.m_RectColor.Blend( GM_STATE_NORMAL, fElapsedTime, 1.0f );
	m_DefaultRect.Draw( m_pDialog );
}