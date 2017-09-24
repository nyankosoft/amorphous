#include "TextMessageRenderer.hpp"

#include "amorphous/Graphics/GraphicsEffectManager.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/StringAux.hpp"


namespace amorphous
{

using namespace boost;




//========================================================================================
// CTextMessageRenderer
//========================================================================================

CTextMessageRenderer::CTextMessageRenderer( boost::shared_ptr<GraphicsElementAnimationManager> pEffectMgr )
:
m_pEffectManager(pEffectMgr)
{
//	m_pWindow = new CTextMessageWindow( this );
}


CTextMessageRenderer::~CTextMessageRenderer()
{
//	SafeDelete( m_pWindow );
}

void CTextMessageRenderer::Update( float dt )
{
	if( /*m_ManageEffectAndElementMgr*/ false )
		m_pEffectManager->UpdateEffects( dt );
}


//========================================================================================
// CDefaultTextMessageRenderer
//========================================================================================

CDefaultTextMessageRenderer::CDefaultTextMessageRenderer( boost::shared_ptr<GraphicsElementAnimationManager> pEffectMgr,
														  int top_layer,
														  int bottom_layer )
:
CTextMessageRenderer(pEffectMgr)
//m_pEffectManager( pEffectMgr )
{
	if( !m_pEffectManager )
	{
		// create the instance
	}

	// set graphics components in 800x600 resolution
//	m_BaseFontSize = SPoint( 12, 24 );

	shared_ptr<GraphicsElementManager> pGraphicsMgr = m_pEffectManager->GetGraphicsElementManager();

	int offset = 20;
	m_aFontID[FONT_TEXT]    = offset;
	m_aFontID[FONT_SPEAKER] = offset + 1;

	pGraphicsMgr->LoadFont( m_aFontID[FONT_TEXT],    "‚l‚r ‚oƒSƒVƒbƒN", FontBase::FONTTYPE_UTF, 20, 40 );
	pGraphicsMgr->LoadFont( m_aFontID[FONT_SPEAKER], "Arial",           FontBase::FONTTYPE_UTF, 12, 24 );

	m_fBGRectAlpha = 0.3f;
	SRect bg_rect = RectAtCenterTop( 1000, 110, 45 );
	SFloatRGBAColor color = SFloatRGBAColor( 0.0f, 0.0f, 0.0f, m_fBGRectAlpha );
	m_pWindowBGRect = pGraphicsMgr->CreateFillRect( bg_rect, color, top_layer + 3 );

	LoadGraphicsResources( GetCurrentGraphicsParams() );

	memset( m_acText,    0, sizeof(m_acText) );
	memset( m_acSpeaker, 0, sizeof(m_acSpeaker) );

	float x,y;
	x = y = 0.0f;
	SFloatRGBAColor font_color = SFloatRGBAColor( 0.9f, 0.9f, 0.9f, 1.0f );
	int w=0,h=0;
	int layer = 0;
	m_pText    = pGraphicsMgr->CreateText( m_aFontID[FONT_TEXT],   "", (float)bg_rect.left + 4, (float)bg_rect.top + 4,  font_color, w, h, layer );
	m_pSpeaker = pGraphicsMgr->CreateText( m_aFontID[FONT_SPEAKER],"", (float)bg_rect.left,     (float)bg_rect.top - 32, font_color, w, h, layer  );
}


CDefaultTextMessageRenderer::~CDefaultTextMessageRenderer()
{
	ReleaseGraphicsResources();
}


void CDefaultTextMessageRenderer::Update( float dt )
{
	CTextMessageRenderer::Update( dt );
}


void CDefaultTextMessageRenderer::Render()
{
	if( NoMessage() )
		return;

	if( /* call Render() of m_ManageElementManager */ false )
		m_pEffectManager->GetGraphicsElementManager()->Render();

//	m_aFontID[FONT_TEXT]->SetText( m_acText, m_vTextPos.x, m_vTextPos.y, 0xFFC0C0C0 );
//	m_aFontID[FONT_SPEAKER]->SetText( m_acSpeaker, m_vSpeakerPos.x, m_vSpeakerPos.y, 0xFF60C0FF );
}


void CDefaultTextMessageRenderer::UpdateSpeaker( const char *pSpeaker )
{
	m_pSpeaker->SetText( pSpeaker );
}


void CDefaultTextMessageRenderer::UpdateText( const char *pText )
{
	m_pText->SetText( pText );

	if( 0 < strlen(pText) )
	{
		// cancel the fadeout effect of the background rect
		m_pEffectManager->CancelEffect( m_BGRectFade );
		m_pWindowBGRect->SetAlpha( 0, m_fBGRectAlpha );
	}
}


void CDefaultTextMessageRenderer::OnTextMessageCleared()
{
	// no message to display - hide rect for text message
	m_pEffectManager->SetTimeOffset();
	m_BGRectFade = m_pEffectManager->ChangeAlphaTo( m_pWindowBGRect,
		0.5, 1.0, 0, 0.0f, GraphicsElementAnimation::TRANS_LINEAR );
}


void CDefaultTextMessageRenderer::ReleaseGraphicsResources()
{
//	m_WindowTexture.Release();
}


void CDefaultTextMessageRenderer::LoadGraphicsResources( const GraphicsParameters& rParam )
{
/*	float screen_width = (float)GetScreenWidth();
	float factor = (float)(screen_width / 800.0f);

//	m_WindowTexture.Load();

	SPoint font_size_spkr = m_BaseFontSize * ( factor * 0.5f );
	SPoint font_size_text = m_BaseFontSize * factor;
	
	PrintLog( string("TextMessafeManager - font size") +  to_string(font_size_text.x) + to_string(font_size_text.y) );

    SRect rect = m_BaseWindowRect * factor;

	m_WindowRect.SetPositionLTRB( rect.left, rect.top, rect.right, rect.bottom );

	m_WindowRect.SetColor( 0x50000000 );	// a translucent, black rectangle

	m_vTextPos		= SPoint( 130, 48 ) * factor;
	m_vSpeakerPos	= SPoint( 130, 34 ) * factor;*/
}



} // namespace amorphous
