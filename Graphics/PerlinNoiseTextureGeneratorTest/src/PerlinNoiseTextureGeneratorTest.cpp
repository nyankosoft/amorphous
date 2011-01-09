#include "PerlinNoiseTextureGeneratorTest.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/PerlinNoiseTextureGenerator.hpp" //<<< This header contains the classes to test
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"
#include "gds/Support/MTRand.hpp"
#include "gds/Utilities/PerlinAux.hpp"
#include "gds/GUI.hpp"

using std::string;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CPerlinNoiseTextureGeneratorTest();
}


extern const std::string GetAppTitle()
{
	return string("PerlinNoiseTextureGeneratorTest");
}


CPerlinNoiseTextureGeneratorTest::CPerlinNoiseTextureGeneratorTest()
:
m_ImageWidth( 64 ),
m_fTextureRepeats( 1.0f )
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f ) );

	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );

	m_pTimer.reset( new CTimer );

	InitRand( (unsigned long)timeGetTime() );
}


CPerlinNoiseTextureGeneratorTest::~CPerlinNoiseTextureGeneratorTest()
{
}


void CPerlinNoiseTextureGeneratorTest::CreateSampleUI()
{
}


bool CPerlinNoiseTextureGeneratorTest::InitShader()
{
	// initialize shader
/*	bool shader_loaded = m_Shader.Load( "./shaders/PerlinNoiseTextureGeneratorTest.fx" );
	
	if( !shader_loaded )
		return false;

	CShaderLightManager *pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager().get();

	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

//	pShaderLightMgr->SetLight( 0, light );
//	pShaderLightMgr->SetDirectionalLightOffset( 0 );
//	pShaderLightMgr->SetNumDirectionalLights( 1 );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	Matrix44 proj = Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );
	m_Shader.GetShaderManager()->SetProjectionTransform( proj );
*/
	return true;
}


void CPerlinNoiseTextureGeneratorTest::CreatePerlinNoiseTexture()
{
	CPerlinNoiseParams pn_params;
	pn_params.seed = (int)RandInt32();
//	int octaves = 1; float freq = 1; float amp = 1; int seed = 0;
	LoadParamFromFile( "params.txt", "image_size",      m_ImageWidth );
	LoadParamFromFile( "params.txt", "texture_repeats", m_fTextureRepeats );

	int tilable = 0;
	LoadParamFromFile( "params.txt", "octaves", pn_params.octaves );
	LoadParamFromFile( "params.txt", "freq",    pn_params.freq );
	LoadParamFromFile( "params.txt", "amp",     pn_params.amp );
	LoadParamFromFile( "params.txt", "range",   pn_params.min_value, pn_params.max_value );
	LoadParamFromFile( "params.txt", "seed",    pn_params.seed );
	LoadParamFromFile( "params.txt", "tilable", tilable );
	pn_params.tilable = (tilable!=0) ? true : false;

	bool loaded = false;

	CTextureResourceDesc tex_desc;
	tex_desc.pLoader.reset( new CPerlinNoiseTextureGenerator(pn_params) );
	tex_desc.Width  = m_ImageWidth;
	tex_desc.Height = m_ImageWidth;
	tex_desc.Format = TextureFormat::A8R8G8B8;

	loaded = m_PerlinNoiseTexture.Load( tex_desc );

	CTextureResourceDesc nmap_desc;
	nmap_desc.pLoader.reset( new CPerlinNoiseNormalMapGenerator(pn_params) );
	nmap_desc.Width  = m_ImageWidth;
	nmap_desc.Height = m_ImageWidth;
	nmap_desc.Format = TextureFormat::A8R8G8B8;

	loaded = m_PerlinNoiseNormalMap.Load( nmap_desc );
}


int CPerlinNoiseTextureGeneratorTest::Init()
{
	shared_ptr<CFontBase> pFont( CreateDefaultBuiltinFont() );
	pFont->SetFontSize( 6, 12 );

	CreatePerlinNoiseTexture();

	return 0;
}




void CPerlinNoiseTextureGeneratorTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );
}


void CPerlinNoiseTextureGeneratorTest::Render()
{
	PROFILE_FUNCTION();

	if( m_pSampleUI )
		m_pSampleUI->Render();

/*	shared_ptr<CGraphicsResourceEntry> pTexEntry( m_PerlinNoiseTexture.GetEntry() );
	if( pTexEntry )
	{
		shared_ptr<CTextureResource> pTex = GetTextureResource();
		if( pTex )
	}*/

	int w = m_ImageWidth;
	float t = m_fTextureRepeats;
	C2DRect rect( 0, 0, m_ImageWidth - 1, m_ImageWidth - 1 );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(t,t) );
	rect.SetColor( SFloatRGBAColor::White() );
	rect.Draw( m_PerlinNoiseTexture );

	rect.SetPositionLTWH( w, 0, w, w );
	rect.Draw( m_PerlinNoiseNormalMap );

/*	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = g_Camera.GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
*/
}


void CPerlinNoiseTextureGeneratorTest::SaveTexturesAsImageFiles()
{
	CTextureHandle textures[] = { m_PerlinNoiseTexture, m_PerlinNoiseNormalMap };
	const char *names[] = { "pn", "pn_nmap" };
	static int s_counter = 0;

	for( int i=0; i<2; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = textures[i].GetEntry();
		if( !pEntry )
			continue;

		shared_ptr<CTextureResource> pTexture = pEntry->GetTextureResource();
		if( !pTexture )
			continue;

		pTexture->SaveTextureToImageFile( fmt_string( "./%s%02d.png", names[i], s_counter++ ) );
	}
}


void CPerlinNoiseTextureGeneratorTest::HandleInput( const SInputData& input )
{
	if( m_pUIInputHandler )
	{
//		CInputHandler::ProcessInput() does not take const SInputData&
		SInputData input_copy = input;
		m_pUIInputHandler->ProcessInput( input_copy );

		if( m_pUIInputHandler->PrevInputProcessed() )
			return;
	}

	switch( input.iGICode )
	{
	case GIC_F5:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			CreatePerlinNoiseTexture();
		}
		break;
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			SaveTexturesAsImageFiles();
		}
		break;
	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			m_pSampleUI->GetDialog(UIID_DLG_RESOLUTION)->Open();
		}
		break;
	default:
		break;
	}
}


void CPerlinNoiseTextureGeneratorTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CPerlinNoiseTextureGeneratorTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
