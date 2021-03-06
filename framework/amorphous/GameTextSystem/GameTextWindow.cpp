#include "GameTextWindow.hpp"
#include "GameTextSystem.hpp"
//#include "amorphous/Graphics/Font/Font.hpp"
#include "amorphous/Graphics/Font/TextureFont.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/memory_helpers.hpp"

#include "amorphous/App/GameWindowManager_Win32.hpp"


namespace amorphous
{


//===========================================================================
// CGameTextWindow
//===========================================================================

CGameTextWindow::CGameTextWindow()
{
//	m_bWaitingUserResponse = false;
	m_iCurrentState = STATE_CLOSED;
	m_iCurrentComponent = 0;

	m_iNumMaxLines = 4;
	m_iNumMaxCharsPerLine = 42;
	m_fNumCharsForCurrentLine = 0;
	m_iCurrentLine = 0;

	m_pCurrentTextSet = NULL;

	m_pFont = NULL;
	m_fFontWidth  = 0;
	m_fFontHeight = 0;

	m_WindowRect.SetColor( 0x9A000000 );	// a translucent, black rectangle
	m_TexturedIcon.SetColor( 0xFFFFFFFF );	// use texture color only

//	m_iNumIconTextures = 0;

//	int i;
//	for(i=0; i<NUM_MAX_ICONTEXTURES; i++)
//		m_apTexture[i] = NULL;

	UpdateScreenSize();

}


void CGameTextWindow::InitFont( const char *pcFontName, float fFontWidth, float fFontHeight )
{
	m_strFontName = pcFontName;
	m_fFontWidth  = fFontWidth;
	m_fFontHeight = fFontHeight;

	float fScreenWidth = (float)GetGameWindowManager().GetScreenWidth();

	SafeDelete( m_pFont );
//	CFont *pFont = new CFont;
	TextureFont *pFont = new TextureFont;
	pFont->InitFont( pcFontName, (int)(fFontWidth * fScreenWidth), (int)(fFontHeight * fScreenWidth) );
	m_pFont = pFont;
}


CGameTextWindow::~CGameTextWindow()
{
	ReleaseGraphicsResources();

	SafeDelete( m_pFont );
}


void CGameTextWindow::OpenTextWindow(CGameTextSet* pGameTextSet)
{
	if( m_iCurrentState == STATE_OPEN || m_iCurrentState == STATE_WAITING_USER_RESPONSE )
		return;	// currently used by some other text event

	m_iCurrentState = STATE_OPEN;
	m_iCurrentLine = 0;
	m_iCurrentComponent = 0;
	m_fNumCharsForCurrentLine = 0.0f;
	m_pCurrentTextSet = pGameTextSet;
}


void CGameTextWindow::Render(int iInput)
{
	if( m_iCurrentState == STATE_CLOSED )
		return;
	else if( m_iCurrentState == STATE_WAITING_USER_RESPONSE )
	{
		if( iInput == GTW_INPUT_OK )
			m_iCurrentState = STATE_OPEN;
	}

	// draw the window with translucent black
	m_WindowRect.Draw();

	float fScreenWidth  = (float)GetGameWindowManager().GetScreenWidth();
	float fScreenHeight = (float)GetGameWindowManager().GetScreenHeight();

	// draw complete lines
	int iLine;
	Vector2 vTopLeft = Vector2(0.194f, 0.560f) * fScreenWidth;
	float fFontHeight = m_fFontHeight;
	for(iLine=0; iLine<m_iCurrentLine; iLine++)
	{
		m_apLine[iLine]->DrawLine( vTopLeft, m_iNumMaxCharsPerLine, m_pFont );
		vTopLeft.y += (fFontHeight + 0.004f) * fScreenWidth;
	}

	if( m_iCurrentState == STATE_WAITING_USER_RESPONSE )
		return;

	m_fNumCharsForCurrentLine += GlobalTimer().GetFrameTime() * 35.0f;

	// draw proceeding lines and respond to events
	bool bBreakLoop = false;
	while(!bBreakLoop)
	{
		CGameTextComponent* pGameTextComponent = m_pCurrentTextSet->GetComponent( m_iCurrentComponent );

		int i;
		switch( pGameTextComponent->GetType() )
		{
		case GTC_TEXTLINE:
			if( m_iNumMaxLines-1 < m_iCurrentLine )
			{	// need vertical scroll
				for(i=1; i<m_iNumMaxLines; i++)
					m_apLine[i-1] = m_apLine[i];
				m_iCurrentLine = m_iNumMaxLines-1;
				bBreakLoop = true;	// give up this time, because maximum number of lines are already drawn.
				break;
			}
			if( pGameTextComponent->GetLineLength() <= (int)m_fNumCharsForCurrentLine )
			{	// draw all the letters in this line and prepare for the next line
				m_fNumCharsForCurrentLine -= (float)pGameTextComponent->GetLineLength();
				pGameTextComponent->DrawLine( vTopLeft, m_iNumMaxCharsPerLine, m_pFont );
				vTopLeft.y += (fFontHeight + 0.004f) * fScreenWidth;
				m_apLine[m_iCurrentLine] = pGameTextComponent;
				m_iCurrentLine++;
				m_iCurrentComponent++;
				continue;
			}
			else
			{
				pGameTextComponent->DrawLine( vTopLeft, (int)m_fNumCharsForCurrentLine, m_pFont );
				bBreakLoop = true;
			}
			break;

		case GTC_IMAGE:
			m_iCurrentComponent++;
			break;

		case GTC_CLEARTEXT:
			m_iCurrentLine = 0;
			m_iCurrentComponent++;
			break;

		case GTC_WAITUSERRESPONSE:
			m_iCurrentState = STATE_WAITING_USER_RESPONSE;
			m_iCurrentComponent++;
//			m_iCurrentLine++;
			bBreakLoop = true;
			break;

		case GTC_TERMINATE:
			m_iCurrentState = STATE_CLOSED;
			m_pCurrentTextSet = NULL;
			bBreakLoop = true;
			break;

		default:
			m_iCurrentComponent++;
			break;
		}
	}
}


void CGameTextWindow::UpdateScreenSize()
{
	int iScreenWidth = GetGameWindowManager().GetScreenWidth();

	Vector2 vMin, vMax;

	vMin = Vector2( 0.175f, 0.550f ) * (float)iScreenWidth;
	vMax = Vector2( 0.962f, 0.725f ) * (float)iScreenWidth;
	m_WindowRect.SetPosition(vMin, vMax);

	vMin = Vector2( 0.025f         , 0.550f          ) * (float)iScreenWidth;
	vMax = Vector2( 0.025f + 0.138f, 0.550f + 0.138f ) * (float)iScreenWidth;
	m_TexturedIcon.SetPosition(vMin, vMax);
}


void CGameTextWindow::ReleaseGraphicsResources()
{
	m_pFont->Release();

/*	int i;
	for(i=0; i<m_iNumIconTextures; i++)
	{
		SAFE_RELEASE( m_apTexture[i] );
	}*/
}


void CGameTextWindow::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	InitFont( m_strFontName.c_str(), m_fFontWidth, m_fFontHeight );

	UpdateScreenSize();

	// Create the texture
/*	HRESULT hr;
	int i;
	for(i=0; i<m_iNumIconTextures; i++)
	{
		hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), &(m_acTextureFilename[i][0]), &m_apTexture[i] );
	    if( FAILED( hr ) )
	        m_apTexture[i] = NULL;
	}*/
}

} // namespace amorphous
