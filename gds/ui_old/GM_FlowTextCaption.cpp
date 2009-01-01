
#include "GM_FlowTextCaption.h"
#include "GM_DialogManager.h"

#include "Graphics/Font.h"

#include "assert.h"


void CGM_FlowTextCaption::CaptionControlChanged( CGM_Control *pNewControl )
{
	// reset timer
	m_fElapsedTime = 0;
}


void CGM_FlowTextCaption::Render( float fElapsedTime,
								  const string& strCaption,
                                  const CGM_CaptionParameters& rCaptionParam )
{
	assert( m_pDialogManager && "CGM_FlowTextCaption::Render() - dialog manager is not set." );

	m_fElapsedTime += fElapsedTime;

	int num_chars = (int)(m_fElapsedTime * 75.0f);

	static string s_strText;
	s_strText = strCaption.substr( 0, num_chars );

	D3DXVECTOR2 pos = D3DXVECTOR2( rCaptionParam.fPosX, rCaptionParam.fPosY );

	CFontBase *pFont = m_pDialogManager->GetFont( rCaptionParam.iFontIndex );

	pFont->DrawText( s_strText.c_str(), pos, rCaptionParam.FontColor.Get32BitARGBColor() );
}