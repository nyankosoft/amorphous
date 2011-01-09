#include "TextureFontTest.hpp"
#include "gds/Graphics.hpp"
#include "gds/Graphics/Font/TrueTypeTextureFont.hpp"
#include "gds/Input.hpp"
#include "gds/Support/ParamLoader.hpp"

using std::string;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CTextureFontTest();
}


extern const std::string GetAppTitle()
{
	return string("TextureFontTest");
}


CTextureFontTest::CTextureFontTest()
:
m_FontWidth(24),
m_FontHeight(48),
m_FontFlags(0)
{
}


CTextureFontTest::~CTextureFontTest()
{
}


int CTextureFontTest::Init()
{
	shared_ptr<CTrueTypeTextureFont> pFont( new CTrueTypeTextureFont() );
//	m_pFont = shared_ptr<CTrueTypeTextureFont>( new CTrueTypeTextureFont() );
	pFont->InitFont( "fonts/rationalinteger.ttf", 64, 8, 16 );

	m_pFont = pFont;
	m_pFont->SetFontSize( m_FontWidth, m_FontHeight );
	m_pFont->SetFontColor( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f ) );

	m_pFont->SetFlags( CFontBase::SHADOW );
	m_pFont->SetShadowColor( SFloatRGBAColor( 0.0f, 0.0f, 0.0f, 0.5f ) );
//	m_pFont->SetShadowShift( Vector2( 10, 10 ) );

	string bg_image = "images/bg.jpg";
	LoadParamFromFile( "params.txt", "background", bg_image );
	bool image_loaded = m_BGTexture.Load( bg_image );

	return 0;
}


void CTextureFontTest::Update( float dt )
{
}


void CTextureFontTest::Render()
{
	if( !m_pFont )
		return;

	C2DRect bg_rect( RectLTWH( 0, 0, CGraphicsComponent::GetScreenWidth(), CGraphicsComponent::GetScreenHeight() ) );
	bg_rect.SetColor( SFloatRGBAColor::White() );
	bg_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	bg_rect.Draw( m_BGTexture );

	SetRenderStatesForTextureFont( AlphaBlend::InvSrcAlpha );

//	string text = "abcdefg ABCDEFG 0123456789";
	string text = "Jackdaws love my big sphinx of quartz. 12334567890";

	m_pFont->DrawText( text.c_str(), Vector2( 10, 100 ), 0xFFFFFFFF );
}


void CTextureFontTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case '0':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			shared_ptr<CTextureFont> pFont( new CTextureFont() );
			pFont->InitFont( g_BitstreamVeraSansMono_Bold_256 );
			pFont->SetFontSize( 24, 48 );
			m_pFont = pFont;
		}
		break;

	case 'S':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_FontFlags ^= CFontBase::SHADOW;
			if( m_pFont )
				m_pFont->SetFlags( m_FontFlags );
		}
		break;

	case GIC_PAGE_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_FontWidth  *= 2;
			m_FontHeight *= 2;
			if( m_pFont )
				m_pFont->SetFontSize( m_FontWidth, m_FontHeight );
		}
		break;

	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_pFont && 4 < m_FontWidth )
			{
				m_FontWidth  /= 2;
				m_FontHeight /= 2;
			}

			if( m_pFont )
				m_pFont->SetFontSize( m_FontWidth, m_FontHeight );
		}
		break;

	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		break;
	}
}
