#include "ParticleTextureGeneratorDemo.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Graphics/TextureGenerators/ParticleTextureGenerator.hpp" //<<< This header contains the classes to test
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Utilities/PerlinAux.hpp"
#include "amorphous/GUI.hpp"

using std::string;
using namespace boost;


ParticleTextureGeneratorDemo::ParticleTextureGeneratorDemo()
:
m_ImageWidth( 64 ),
m_fTextureRepeats( 1.0f ),
m_NumColorVariations(0)
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f ) );

//	m_pTimer.reset( new Timer );

	InitRand( (unsigned long)timeGetTime() );
}


ParticleTextureGeneratorDemo::~ParticleTextureGeneratorDemo()
{
}


bool ParticleTextureGeneratorDemo::InitShader()
{
	return true;
}


void ParticleTextureGeneratorDemo::CreateParticleTextures()
{
	TextureResourceDesc tex_desc;
	tex_desc.Width  = 64;
	tex_desc.Height = 64;
	tex_desc.Format = TextureFormat::A8R8G8B8;

	SFloatRGBAColor colors[] = {
		SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ),
		SFloatRGBAColor( 1.0f, 0.9f, 0.5f, 0.0f ),
		SFloatRGBAColor( 0.5f, 1.0f, 0.9f, 0.0f ),
		SFloatRGBAColor( 0.6f, 0.8f, 1.0f, 0.0f ),
		SFloatRGBAColor( 1.0f, 0.3f, 0.3f, 0.0f ),
		SFloatRGBAColor( 0.7f, 0.3f, 1.0f, 0.0f )
	};

	m_NumColorVariations = numof(colors);

	float rhos[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 1.0f, 2.0f, 3.0f};
	m_ParticleTextures.reserve(numof(colors) * numof(rhos));

	for( int i=0; i<numof(colors); i++ )
	{
		boost::shared_ptr<ParticleTextureGenerator> pGenerator;
		for( int j=0; j<numof(rhos); j++ )
		{
			pGenerator.reset( new ParticleTextureGenerator );
			pGenerator->m_fStandardDeviation = rhos[j];

			pGenerator->m_Color = colors[i];

			m_ParticleTextures.push_back( TextureHandle() );

			tex_desc.pLoader = pGenerator;
			bool loaded = m_ParticleTextures.back().Load( tex_desc );

			m_ParticleTextures.back().SaveTextureToImageFile(fmt_string(".debug/particle%02d_%02d.png",i,j));
		}
	}
}


int ParticleTextureGeneratorDemo::Init()
{
	CreateParticleTextures();

	string bg_textures[] = 
	{
		"./Common/images/worn_brown_brickwork.jpg",
		"./2DPrimitivesDemo/textures/SidePanel44_fb.jpg",
		"./2DPrimitivesDemo/textures/SidePanel44b_fb.jpg",
		"./2DPrimitivesDemo/textures/silvery_blue.jpg"
	};

	m_BackgroundTexture.Load( bg_textures[3] );

	return 0;
}




void ParticleTextureGeneratorDemo::Update( float dt )
{
}


void ParticleTextureGeneratorDemo::Render()
{
	PROFILE_FUNCTION();

//	int w = m_ImageWidth;
//	float t = m_fTextureRepeats;
	int w = 512;
	float t = 1.0f;
	C2DRect rect( 0, 0, w - 1, w - 1 );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(t,t) );
	rect.SetColor( SFloatRGBAColor::White() );
	rect.Draw( m_BackgroundTexture );

	float pw = 50.0f;
	float x = 0.0f;
	float y = 50.0f;
	C2DRect particle_rect;
	particle_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	particle_rect.SetColor( SFloatRGBAColor::White() );
	int num_variations_per_color = (int)m_ParticleTextures.size() / m_NumColorVariations;
	for( int i=0; i<m_NumColorVariations; i++ )
	{
		y = 50.0f + pw * (float)i;
		for( int j=0; j<num_variations_per_color; j++ )
		{
			x = pw * (float)j;
			particle_rect.SetPositionLTWH( x, y, pw, pw );
//			particle_rect.Draw( m_ParticleTextures[i] );

//			GraphicsDevice().SetTexture( 0, m_ParticleTextures[i] );

//			TextureStage tex_stage_0, tex_stage_1;

//			tex_stage_0.ColorOp   = TexStageOp::MODULATE;
//			tex_stage_0.ColorArg0 = TexStageArg::DIFFUSE;
//			tex_stage_0.ColorArg1 = TexStageArg::TEXTURE;
//			tex_stage_0.AlphaOp   = TexStageOp::MODULATE;
//			tex_stage_0.AlphaArg0 = TexStageArg::DIFFUSE;
//			tex_stage_0.AlphaArg1 = TexStageArg::TEXTURE;
//			GraphicsDevice().SetTextureStageParams( 0, tex_stage_0 );

//			tex_stage_1.ColorOp = TexStageOp::DISABLE;
//			tex_stage_1.AlphaOp = TexStageOp::DISABLE;
//			GraphicsDevice().SetTextureStageParams( 1, tex_stage_1 );

			GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, true );
			GraphicsDevice().SetSourceBlendMode( AlphaBlend::One );
			GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

			GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST,   false );

//			particle_rect.draw();
			particle_rect.Draw( m_ParticleTextures[i * num_variations_per_color + j] );
		}
	}


//	rect.SetPositionLTWH( w, 0, w, w );
//	rect.Draw( m_PerlinNoiseNormalMap );
}


void ParticleTextureGeneratorDemo::SaveTexturesAsImageFiles()
{
}


void ParticleTextureGeneratorDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F5:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
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
