#include "PseudoNoiseEffect.hpp"
//#include "../Graphics/Shader/ShaderManager.hpp"
#include "TextureGenerators/NoiseTextureGenerators.hpp"
#include "../Support/MTRand.hpp"


namespace amorphous
{



//============================================================================================
// PseudoNoiseEffect
//============================================================================================

PseudoNoiseEffect::PseudoNoiseEffect()
:
m_NoiseTextureSize(512),
m_NoisePixelSize(1)
{
}


PseudoNoiseEffect::~PseudoNoiseEffect()
{
}


void PseudoNoiseEffect::ReleaseGraphicsResources()
{
}


void PseudoNoiseEffect::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	UpdateScreenSize();
}


void PseudoNoiseEffect::Init( float strength, uint noise_pixel_size )
{
	UpdateScreenSize();

	std::shared_ptr<UniformSingleColorNoiseTextureGenerator> pTexGenerator( new UniformSingleColorNoiseTextureGenerator );
	pTexGenerator->m_fDensity = strength;
	pTexGenerator->m_fMin     = 0.0f;
	pTexGenerator->m_fMax     = strength;

	m_NoisePixelSize = noise_pixel_size;

	TextureResourceDesc tex_desc;
	tex_desc.Width   = m_NoiseTextureSize;
	tex_desc.Height  = m_NoiseTextureSize;
	tex_desc.Format  = TextureFormat::A8R8G8B8;
	tex_desc.pLoader = pTexGenerator;
	bool loaded = m_NoiseTexture.Load( tex_desc );
}


/*
void PseudoNoiseEffect::LoadExtraTexture( const char *pcTextureFilename, float fScale )
{
}*/


void PseudoNoiseEffect::UpdateScreenSize()
{
//	m_iScreenWidth  = GetGameWindowManager().GetScreenWidth();
//	m_iScreenHeight = GetGameWindowManager().GetScreenHeight();

	uint w=0,h=0;
	GraphicsDevice().GetViewportSize( w, h );
	m_iScreenWidth  = (int)w;
	m_iScreenHeight = (int)h;

	float width  = (float)m_iScreenWidth;
	float height = (float)m_iScreenHeight;

	m_FullscreenRect.SetPositionLTRB( 0, 0, m_iScreenWidth - 1, m_iScreenHeight - 1 );
}


void PseudoNoiseEffect::RenderNoiseEffect()
{
	// single texture & rect class version
	m_FullscreenRect.SetColor( 0x30FFFFFF );

	// render the rect over the entire screen
	// - Alpha blend settings are done in C2DRect::Draw() -> C2DPrimitive::Draw() -> C2DPrimitive::SetBasicRenderStates()
	// - Should this be rendered with premultiplied alpha?
	m_FullscreenRect.Draw( m_NoiseTexture );

	return;

/*	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetFVF( D3DFVF_TLVERTEX2 );
	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetPixelShader( NULL );
	SetNoiseTexture();

    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
//	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
//	pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
//	pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
//	pd3dDev->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );	
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
//	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
//	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );	
//	pd3dDev->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
	pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

	pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avTextureRect, sizeof(TLVERTEX2) );
*/
}


void PseudoNoiseEffect::SetNoiseTexture()
{
	//>>> single texture & rect class version
	float t = (float)m_iScreenWidth / (float)m_NoiseTextureSize;	// scalse the noise texture according to the screen size
	float rand_tu = t * RangedRand( 0.0f, 1.0f );
	float rand_tv = t * RangedRand( 0.0f, 1.0f );
	TEXCOORD2 shift( TEXCOORD2(rand_tu,rand_tv) );

	int iRand = RangedRand( 0, 3 );
	int i0=0, i1=0, i2=0, i3=0;
	switch(iRand)
	{
	case 0:
		i0 = 0; i1 = 1; i2 = 2; i3 = 3; break;
	case 1:
		i0 = 1; i1 = 0; i2 = 3; i3 = 2; break;
	case 2:
		i0 = 3; i1 = 2; i2 = 1; i3 = 0; break;
	case 3:
	default:
		i0 = 2; i1 = 3; i2 = 0; i3 = 1; break;
	}

	float u = (float)m_iScreenWidth  / (float)m_NoiseTextureSize / (float)m_NoisePixelSize;
	float v = (float)m_iScreenHeight / (float)m_NoiseTextureSize / (float)m_NoisePixelSize;
	m_FullscreenRect.SetTextureCoord( i0, 0, TEXCOORD2( 0, 0 ) + shift );
	m_FullscreenRect.SetTextureCoord( i1, 0, TEXCOORD2( u, 0 ) + shift );
	m_FullscreenRect.SetTextureCoord( i2, 0, TEXCOORD2( u, v ) + shift );
	m_FullscreenRect.SetTextureCoord( i3, 0, TEXCOORD2( 0, v ) + shift );
	//<<< single texture & rect class version
}


} // namespace amorphous
