#include "TextureFontTest.hpp"
#include "Graphics/all.hpp"
#include "Graphics/Font/TrueTypeTextureFont.hpp"
#include "Input.hpp"
#include "Support/Profile.hpp"
#include "Support/BitmapImage.hpp"

using namespace std;
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
{
}


CTextureFontTest::~CTextureFontTest()
{
}


int CTextureFontTest::Init()
{
	m_pFont = shared_ptr<CTrueTypeTextureFont>( new CTrueTypeTextureFont() );
	m_pFont->InitFont( "./fonts/rationalinteger.ttf", 64, 8, 16 );
	m_pFont->SetFontSize( 24, 48 );
	m_pFont->SetFontColor( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f ) );

	return 0;
}


void CTextureFontTest::Update( float dt )
{
}


void CTextureFontTest::Render()
{
	if( !m_pFont )
		return;

	SetRenderStatesForTextureFont( AlphaBlend::InvSrcAlpha );

//	string text = "abcdefg ABCDEFG 0123456789";
	string text = "Jackdaws love my big sphinx of quartz. 12334567890";

	m_pFont->DrawText( text.c_str(), Vector2( 10, 100 ), 0xFFFFFFFF );
}


void CTextureFontTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		break;
	}
}

/*
void CTextureFontTest::UpdateViewTransform( const D3DXMATRIX& matView )
{
//	m_pCubeMapManager->UpdateViewTransform( matView );
}


void CTextureFontTest::UpdateProjectionTransform( const D3DXMATRIX& matProj )
{
//	m_pCubeMapManager->UpdateProjectionTransform( matProj );
}

*/
