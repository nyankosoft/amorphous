#include "TextureFontTest.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/Font/TrueTypeTextureFont.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Input/InputHandler.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/filesystem_aux.hpp"

using std::vector;
using std::string;
using namespace boost;


CTextureFontTest::CTextureFontTest()
:
m_FontWidth(24),
m_FontHeight(48),
m_FontFlags(0),
m_EnableRotation(false),
m_fRotationAngle(0.0f)
{
}


CTextureFontTest::~CTextureFontTest()
{
}


int CTextureFontTest::Init()
{
	string font_directory = "fonts";
	LoadParamFromFile( "params.txt", "font_directory", font_directory );
	font_directory = "TextureFontDemo/" + font_directory;
	vector<string> font_file_pathnames;
	find_files_in_directory( font_directory, font_file_pathnames );
	m_FontFilePathnames = font_file_pathnames;

	LoadCurrentFont();
/*
	shared_ptr<TrueTypeTextureFont> pFont( new TrueTypeTextureFont() );
//	m_pFont = shared_ptr<TrueTypeTextureFont>( new CTrueTypeTextureFont() );
	pFont->InitFont( "fonts/rationalinteger.ttf", 64, 8, 16 );
*/
	if( m_pFont )
	{
//		m_pFont = pFont;
		m_pFont->SetFontSize( m_FontWidth, m_FontHeight );
		m_pFont->SetFontColor( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f ) );

		U32 initial_flags = FontBase::SHADOW;
		m_pFont->SetFlags( initial_flags );
		m_FontFlags = initial_flags;
		m_pFont->SetShadowColor( SFloatRGBAColor( 0.0f, 0.0f, 0.0f, 0.5f ) );
//		m_pFont->SetShadowShift( Vector2( 10, 10 ) );
	}

	string bg_image = "images/bg.jpg";
	LoadParamFromFile( "TextureFontDemo/params.txt", "background", bg_image );
	bg_image = "TextureFontDemo/" + bg_image;
	bool image_loaded = m_BGTexture.Load( bg_image );

	int rotation = 0;
	LoadParamFromFile( "params.txt", "rotation", rotation );
	m_EnableRotation = (rotation != 0) ? true : false;

	float rotation_angle = 0;
	LoadParamFromFile( "params.txt", "rotation_angle", rotation_angle );
	m_fRotationAngle = rotation_angle;

	return 0;
}


void CTextureFontTest::Update( float dt )
{
}


void CTextureFontTest::RenderText()
{
	if( !m_pFont )
		return;

//	string text = "abcdefg ABCDEFG 0123456789";

	string font_file_pathname;
	m_FontFilePathnames.get_current( font_file_pathname );

	string text =  font_file_pathname + "\n";
	text += "Jackdaws love my big sphinx of quartz.\n";
	text += "12334567890\n";
	text += "_+-*/=<>[]{}@#$%^&!?|;:,.\n";

	Vector2 position = Vector2( 10, 120 );
	if( m_EnableRotation )
		m_pFont->DrawText( text.c_str(), position, position, m_fRotationAngle, SFloatRGBAColor::White() );
	else
		m_pFont->DrawText( text.c_str(), position, 0xFFFFFFFF );
}


void CTextureFontTest::Render()
{

	C2DRect bg_rect( RectLTWH( 0, 0, GraphicsComponent::GetScreenWidth(), GraphicsComponent::GetScreenHeight() ) );
	bg_rect.SetColor( SFloatRGBAColor::White() );
	bg_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	bg_rect.Draw( m_BGTexture );

	SetRenderStatesForTextureFont( AlphaBlend::InvSrcAlpha );

	RenderText();
}


void CTextureFontTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case '0':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_pFont = CreateDefaultBuiltinFont();
			m_pFont->SetFontSize( 24, 48 );
		}
		break;

	case 'S':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_FontFlags ^= FontBase::SHADOW;
			if( m_pFont )
				m_pFont->SetFlags( m_FontFlags );
		}
		break;

	case 'R':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_EnableRotation = !m_EnableRotation;
		break;

	case 'T':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_fRotationAngle += 0.5f;
		break;

	case 'Y':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_fRotationAngle -= 0.5f;
		break;

	case 'K':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_FontWidth  *= 2;
			m_FontHeight *= 2;
			if( m_pFont )
				m_pFont->SetFontSize( m_FontWidth, m_FontHeight );
		}
		break;

	case 'J':
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

	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_FontFilePathnames--;
			LoadCurrentFont();
		}

		break;

	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_FontFilePathnames++;
			LoadCurrentFont();
		}
		break;

	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}


void CTextureFontTest::LoadCurrentFont()
{
	string font_file_pathname;
	m_FontFilePathnames.get_current( font_file_pathname );

	shared_ptr<TrueTypeTextureFont> pFont( new TrueTypeTextureFont() );
//	m_pFont = shared_ptr<TrueTypeTextureFont>( new TrueTypeTextureFont() );
	bool initialized = pFont->InitFont( font_file_pathname, 64, 8, 16 );
	if( initialized )
	{
		m_pFont = pFont;
		m_pFont->SetFontSize( m_FontWidth, m_FontHeight );
		m_pFont->SetFlags( m_FontFlags );
	}
	else
		m_pFont.reset();
}
