#include "PerlinNoiseTextureGeneratorDemo.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Graphics/TextureGenerators/PerlinNoiseTextureGenerator.hpp" //<<< This header contains the classes to test
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Utilities/PerlinAux.hpp"
#include "amorphous/GUI.hpp"

using std::string;


PerlinNoiseTextureGeneratorDemo::PerlinNoiseTextureGeneratorDemo()
:
m_ImageWidth( 64 ),
m_fTextureRepeats( 1.0f )
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f ) );

	m_pTimer.reset( new Timer );

	InitRand( (unsigned long)timeGetTime() );
}


PerlinNoiseTextureGeneratorDemo::~PerlinNoiseTextureGeneratorDemo()
{
}


bool PerlinNoiseTextureGeneratorDemo::InitShader()
{
	return true;
}


void PerlinNoiseTextureGeneratorDemo::CreatePerlinNoiseTexture()
{
	PerlinNoiseParams pn_params;
	pn_params.seed = (int)RandInt32();
//	int octaves = 1; float freq = 1; float amp = 1; int seed = 0;
	const char *params_file = "PerlinNoiseTextureGeneratorDemo/params.txt";
	LoadParamFromFile( params_file, "image_size",      m_ImageWidth );
	LoadParamFromFile( params_file, "texture_repeats", m_fTextureRepeats );

	int tilable = 0;
	LoadParamFromFile( params_file, "octaves", pn_params.octaves );
	LoadParamFromFile( params_file, "freq",    pn_params.freq );
	LoadParamFromFile( params_file, "amp",     pn_params.amp );
	LoadParamFromFile( params_file, "range",   pn_params.min_value, pn_params.max_value );
	LoadParamFromFile( params_file, "seed",    pn_params.seed );
	LoadParamFromFile( params_file, "tilable", tilable );
	pn_params.tilable = (tilable!=0) ? true : false;

	bool loaded = false;

	TextureResourceDesc tex_desc;
	tex_desc.pLoader.reset( new PerlinNoiseTextureGenerator(pn_params) );
	tex_desc.Width  = m_ImageWidth;
	tex_desc.Height = m_ImageWidth;
	tex_desc.Format = TextureFormat::A8R8G8B8;

	loaded = m_PerlinNoiseTexture.Load( tex_desc );

	TextureResourceDesc nmap_desc;
	nmap_desc.pLoader.reset( new PerlinNoiseNormalMapGenerator(pn_params) );
	nmap_desc.Width  = m_ImageWidth;
	nmap_desc.Height = m_ImageWidth;
	nmap_desc.Format = TextureFormat::A8R8G8B8;

	loaded = m_PerlinNoiseNormalMap.Load( nmap_desc );
}


int PerlinNoiseTextureGeneratorDemo::Init()
{
	CreatePerlinNoiseTexture();

	return 0;
}




void PerlinNoiseTextureGeneratorDemo::Update( float dt )
{
}


void PerlinNoiseTextureGeneratorDemo::Render()
{
	PROFILE_FUNCTION();

/*	shared_ptr<CGraphicsResourceEntry> pTexEntry( m_PerlinNoiseTexture.GetEntry() );
	if( pTexEntry )
	{
		shared_ptr<TextureResource> pTex = GetTextureResource();
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
}


void PerlinNoiseTextureGeneratorDemo::SaveTexturesAsImageFiles()
{
	TextureHandle textures[] = { m_PerlinNoiseTexture, m_PerlinNoiseNormalMap };
	const char *names[] = { "pn", "pn_nmap" };
	static int s_counter = 0;

	for( int i=0; i<2; i++ )
	{
		shared_ptr<GraphicsResourceEntry> pEntry = textures[i].GetEntry();
		if( !pEntry )
			continue;

		shared_ptr<TextureResource> pTexture = pEntry->GetTextureResource();
		if( !pTexture )
			continue;

		pTexture->SaveTextureToImageFile( fmt_string( "./%s%02d.png", names[i], s_counter++ ) );
	}
}


void PerlinNoiseTextureGeneratorDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F5:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			CreatePerlinNoiseTexture();
		}
		break;
	case GIC_F11:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			SaveTexturesAsImageFiles();
		}
		break;
	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}
