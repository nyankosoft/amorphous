#include "SimpleMotionBlur.hpp"
#include "amorphous/Graphics/TextureRenderTarget.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/StringAux.hpp"


namespace amorphous
{

using namespace std;


SimpleMotionBlur::SimpleMotionBlur()
{
	m_TargetTexIndex = 0;

	m_bFirst = true;

	m_fBlurWeight = 0.35f;

//	m_pSceneRenderTarget = NULL;

//	m_apTexRenderTarget[0] = NULL;
//	m_apTexRenderTarget[1] = NULL;
}


SimpleMotionBlur::SimpleMotionBlur( int texture_width, int texture_height )
{
	m_TargetTexIndex = 0;

	m_bFirst = true;

	m_fBlurWeight = 0.35f;

//	m_pSceneRenderTarget = NULL;

//	m_apTexRenderTarget[0] = NULL;
//	m_apTexRenderTarget[1] = NULL;

	m_TextureWidth  = texture_width;
	m_TextureHeight = texture_height;

	Init( texture_width, texture_height );
}


SimpleMotionBlur::~SimpleMotionBlur()
{
	ReleaseTextures();
}


void SimpleMotionBlur::Init( int texture_width, int texture_height )
{
	ReleaseTextures();

	m_pSceneRenderTarget = TextureRenderTarget::Create();
	m_pSceneRenderTarget->Init( texture_width, texture_height );

	for( int i=0; i<2; i++ )
	{
		m_apTexRenderTarget[i] = TextureRenderTarget::Create();
		m_apTexRenderTarget[i]->Init( texture_width, texture_height );
	}

	InitTextureRenderTargetBGColors();

}


void SimpleMotionBlur::InitForScreenSize()
{
	ReleaseTextures();

	m_pSceneRenderTarget = TextureRenderTarget::Create();
	m_pSceneRenderTarget->InitScreenSizeRenderTarget();

	for( int i=0; i<2; i++ )
	{
		m_apTexRenderTarget[i] = TextureRenderTarget::Create();
		m_apTexRenderTarget[i]->InitScreenSizeRenderTarget();
	}

	InitTextureRenderTargetBGColors();

	// SimpleMotionBlur::Render() uses these variables to
	// determine rect size
	m_TextureWidth  = GraphicsComponent::GetScreenWidth();
	m_TextureHeight = GraphicsComponent::GetScreenHeight();
}


void SimpleMotionBlur::ReleaseTextures()
{
	m_pSceneRenderTarget.reset();
	m_apTexRenderTarget[0].reset();
	m_apTexRenderTarget[1].reset();
}


void SimpleMotionBlur::InitTextureRenderTargetBGColors()
{
	m_pSceneRenderTarget->SetBackgroundColor( SFloatRGBAColor(1,0,1,1) );//( 0xFFFF00FF );
	m_apTexRenderTarget[0]->SetBackgroundColor( SFloatRGBAColor(0,1,1,1) );//( 0xFF00FFFF );
	m_apTexRenderTarget[1]->SetBackgroundColor( SFloatRGBAColor(0,1,1,1) );//( 0xFF00FFFF );
}


void SimpleMotionBlur::Begin()
{
	if( !m_pSceneRenderTarget )
		return;

	m_pSceneRenderTarget->SetRenderTarget();
}


void SimpleMotionBlur::End()
{
	if( !m_pSceneRenderTarget )
		return;

	m_pSceneRenderTarget->ResetRenderTarget();
}


static void SetRectRenderStates( TextureHandle& texture )
{
	// enable alpha blending
	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::DEPTH_TEST );
//	pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );

	FixedFunctionPipelineManager().SetTexture( 0, texture );
/*
//	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
*/
	TextureStage ts0, ts1;

	ts0.ColorOp   = TexStageOp::SELECT_ARG1;
	ts0.ColorArg0 = TexStageArg::DIFFUSE;
	ts0.ColorArg1 = TexStageArg::TEXTURE;
	ts0.AlphaOp   = TexStageOp::MODULATE;
	ts0.AlphaArg0 = TexStageArg::DIFFUSE;
	ts0.AlphaArg1 = TexStageArg::TEXTURE;
	GraphicsDevice().SetTextureStageParams( 0, ts0 );

	ts0.ColorOp = TexStageOp::DISABLE;
	ts0.AlphaOp = TexStageOp::DISABLE;
	GraphicsDevice().SetTextureStageParams( 1, ts1 );
}


void SimpleMotionBlur::Render()
{
	// create a rect that covers the entire screen
//	C2DRect rect( 0, 0, m_TextureWidth, m_TextureHeight );
	C2DRect rect( -0.5f, -0.5f, (float)m_TextureWidth - 0.5f, (float)m_TextureHeight - 0.5f );
	rect.SetTextureUV( TEXCOORD2(0.0f,0.0f), TEXCOORD2(1.0f,1.0f) );

	std::shared_ptr<TextureRenderTarget>& pDestRenderTarget = m_apTexRenderTarget[m_TargetTexIndex];
	std::shared_ptr<TextureRenderTarget>& pPrevRenderTarget = m_apTexRenderTarget[(m_TargetTexIndex+1)%2];

	if( !pDestRenderTarget
	 || !pPrevRenderTarget )
	{
		return;
	}

	if( m_bFirst )
	{
		pPrevRenderTarget->SetRenderTarget();
		rect.SetColor( 0xFFFFFFFF );
		rect.Draw();
		rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture() );
		pPrevRenderTarget->ResetRenderTarget();
		m_bFirst = false;
	}

//	static int s_ImageCount = 0;
//	string filename = fmt_string( "debug/tex_image02.%02d.bmp", s_ImageCount++ );
//	PERIODICAL( 1200, pPrevRenderTarget->OutputImageFile( filename.c_str() ) )

	// set dest buffer
	pDestRenderTarget->SetRenderTarget();


//	DWORD tex_filter[3];
//	pd3dDev->GetSamplerState( 0, D3DSAMP_MAGFILTER, &tex_filter[0] );
//	pd3dDev->GetSamplerState( 0, D3DSAMP_MINFILTER, &tex_filter[1] );
//	pd3dDev->GetSamplerState( 0, D3DSAMP_MIPFILTER, &tex_filter[2] );
//	pd3dDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
//	pd3dDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
//	pd3dDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	// Render the prev scene without alpha
	// - This is the motion blurred scene created from the last few scenes
	rect.SetColor( 0xFFFFFFFF );
///	rect.Draw();
	SetRectRenderStates( pPrevRenderTarget->GetRenderTargetTexture() );
	rect.draw();
//	rect.Draw( pPrevRenderTarget->GetRenderTargetTexture() );

	// Render the current scene with alpha, on top of the motion blurred scene.
	U32 argb32_color = 0x00FFFFFF | ( ((int)(m_fBlurWeight * 255.0f)) << 24 );
	rect.SetColor( argb32_color );
	rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture() );
//	SetRectRenderStates( m_pSceneRenderTarget->GetRenderTargetTexture() );
//	rect.draw();

	// restore the original render target
	pDestRenderTarget->ResetRenderTarget();

//	pd3dDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, tex_filter[0] );
//	pd3dDev->SetSamplerState( 0, D3DSAMP_MINFILTER, tex_filter[1] );
//	pd3dDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, tex_filter[2] );

	// the original buffer has been restored
	// put the blended image on it

//	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	rect.SetColor( 0xFFFFFFFF );
///	rect.Draw( pDestRenderTarget->GetRenderTargetTexture() );
	SetRectRenderStates( pDestRenderTarget->GetRenderTargetTexture() );
	rect.draw();

	// test
	rect.SetPositionLTWH( 0,0,160,120 );
	rect.SetColor(0xFF00FF00);
	rect.Draw();
	rect.SetColor(0xFFFFFFFF);
	rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture() );
	rect.DrawWireframe();

	rect.SetPositionLTWH( 160,0,160,120 );
	rect.Draw( pPrevRenderTarget->GetRenderTargetTexture() );
	rect.DrawWireframe();

	m_TargetTexIndex = ( m_TargetTexIndex + 1 ) % 2;
}


} // namespace amorphous
