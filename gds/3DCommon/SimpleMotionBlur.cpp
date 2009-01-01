
#include "SimpleMotionBlur.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DMisc.h"
#include "3DCommon/2DPrimitive/2DRect.h"
#include "Support/SafeDelete.h"
#include "Support/Macro.h"
#include "Support/StringAux.h"

#include <string>
using namespace std;


CSimpleMotionBlur::CSimpleMotionBlur()
{
	m_TargetTexIndex = 0;

	m_bFirst = true;

	m_fBlurWeight = 0.35f;

	m_pSceneRenderTarget = NULL;

	m_apTexRenderTarget[0] = NULL;
	m_apTexRenderTarget[1] = NULL;
}


CSimpleMotionBlur::CSimpleMotionBlur( int texture_width, int texture_height )
{
	m_TargetTexIndex = 0;

	m_bFirst = true;

	m_fBlurWeight = 0.35f;

	m_pSceneRenderTarget = NULL;

	m_apTexRenderTarget[0] = NULL;
	m_apTexRenderTarget[1] = NULL;

	m_TextureWidth  = texture_width;
	m_TextureHeight = texture_height;

	Init( texture_width, texture_height );
}


CSimpleMotionBlur::~CSimpleMotionBlur()
{
	ReleaseTextures();
}


void CSimpleMotionBlur::Init( int texture_width, int texture_height )
{
	ReleaseTextures();

	m_pSceneRenderTarget = new CTextureRenderTarget( texture_width, texture_height );

	m_apTexRenderTarget[0] = new CTextureRenderTarget( texture_width, texture_height );
	m_apTexRenderTarget[1] = new CTextureRenderTarget( texture_width, texture_height );

	InitTextureRenderTargetBGColors();

}


void CSimpleMotionBlur::InitForScreenSize()
{
	ReleaseTextures();

	m_pSceneRenderTarget = new CTextureRenderTarget();
	m_pSceneRenderTarget->InitScreenSizeRenderTarget();

	for( int i=0; i<2; i++ )
	{
		m_apTexRenderTarget[i] = new CTextureRenderTarget();
		m_apTexRenderTarget[i]->InitScreenSizeRenderTarget();
	}

	InitTextureRenderTargetBGColors();

	// CSimpleMotionBlur::Render() uses these variables to
	// determine rect size
	m_TextureWidth  = GetScreenWidth();
	m_TextureHeight = GetScreenHeight();

}


void CSimpleMotionBlur::ReleaseTextures()
{
	SafeDelete( m_pSceneRenderTarget );
	SafeDelete( m_apTexRenderTarget[0] );
	SafeDelete( m_apTexRenderTarget[1] );
}


void CSimpleMotionBlur::InitTextureRenderTargetBGColors()
{
	m_pSceneRenderTarget->SetBackgroundColor( 0xFFFF00FF );
	m_apTexRenderTarget[0]->SetBackgroundColor( 0xFF00FFFF );
	m_apTexRenderTarget[1]->SetBackgroundColor( 0xFF00FFFF );
}


void CSimpleMotionBlur::Begin()
{
	if( !m_pSceneRenderTarget )
		return;

	m_pSceneRenderTarget->SetRenderTarget();
}


void CSimpleMotionBlur::End()
{
	if( !m_pSceneRenderTarget )
		return;

	m_pSceneRenderTarget->ResetRenderTarget();
}


static void SetRectRenderStates( LPDIRECT3DTEXTURE9 pTexture )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
//	pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );

	pd3dDev->SetTexture( 0, pTexture );

//	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
}


void CSimpleMotionBlur::Render()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// create a rect that covers the entire screen
//	C2DRect rect( 0, 0, m_TextureWidth, m_TextureHeight );
	C2DRect rect( -0.5f, -0.5f, (float)m_TextureWidth - 0.5f, (float)m_TextureHeight - 0.5f );
	rect.SetTextureUV( TEXCOORD2(0.0f,0.0f), TEXCOORD2(1.0f,1.0f) );

	CTextureRenderTarget* pDestRenderTarget = m_apTexRenderTarget[m_TargetTexIndex];
	CTextureRenderTarget* pPrevRenderTarget = m_apTexRenderTarget[(m_TargetTexIndex+1)%2];

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

	// render the prev scene
	rect.SetColor( 0xFFFFFFFF );
///	rect.Draw();
	SetRectRenderStates( pPrevRenderTarget->GetRenderTargetTexture() );
	rect.draw();
//	rect.Draw( pPrevRenderTarget->GetRenderTargetTexture() );

	// render the current scene with alpha
	DWORD dwColor = 0x00FFFFFF | ( ((int)(m_fBlurWeight * 255.0f)) << 24 );
	rect.SetColor( dwColor );
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


void CSimpleMotionBlur::ReleaseGraphicsResources()
{
	// m_pSceneRenderTarget - derived from CGraphicsComponent
	// m_apTexRenderTarget  - derived from CGraphicsComponent
}


void CSimpleMotionBlur::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	// m_pSceneRenderTarget - derived from CGraphicsComponent
	// m_apTexRenderTarget  - derived from CGraphicsComponent

	InitTextureRenderTargetBGColors();
}
