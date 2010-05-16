#include "PseudoNoiseEffect.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
//#include "../Graphics/Shader/ShaderManager.hpp"
#include "Graphics/NoiseTextureGenerators.hpp"
#include "../Support/MTRand.hpp"

using namespace boost;


//============================================================================================
// CPseudoNoiseEffect
//============================================================================================

CPseudoNoiseEffect::CPseudoNoiseEffect()
:
m_NoiseTextureSize(512),
m_NoisePixelSize(1)
{
	int i;
	for(i=0; i<4; i++)
	{
		m_avTextureRect[i].color = 0xFFFFFFFF;
		m_avTextureRect[i].rhw   = 1.0f;
	}
	m_avTextureRect[0].tex[0] = TEXCOORD2( 0.0f, 0.0f );
	m_avTextureRect[1].tex[0] = TEXCOORD2( 1.0f, 0.0f );
	m_avTextureRect[2].tex[0] = TEXCOORD2( 1.0f, 1.0f );
	m_avTextureRect[3].tex[0] = TEXCOORD2( 0.0f, 1.0f );
}


CPseudoNoiseEffect::~CPseudoNoiseEffect()
{
}


void CPseudoNoiseEffect::ReleaseGraphicsResources()
{
}


void CPseudoNoiseEffect::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	UpdateScreenSize();
}


void CPseudoNoiseEffect::Init( float strength, uint noise_pixel_size )
{
	UpdateScreenSize();

	shared_ptr<CUniformSingleColorNoiseTextureGenerator> pTexGenerator( new CUniformSingleColorNoiseTextureGenerator );
	pTexGenerator->m_fDensity = strength;
	pTexGenerator->m_fMin     = 0.0f;
	pTexGenerator->m_fMax     = strength;

	m_NoisePixelSize = noise_pixel_size;

	CTextureResourceDesc tex_desc;
	tex_desc.Width   = m_NoiseTextureSize;
	tex_desc.Height  = m_NoiseTextureSize;
	tex_desc.Format  = TextureFormat::A8R8G8B8;
	tex_desc.pLoader = pTexGenerator;
	bool loaded = m_NoiseTexture.Load( tex_desc );
}


bool CPseudoNoiseEffect::LoadNoiseTextures()
{
	ReleaseGraphicsResources();

	// load textures used for noise
	char acFilename[256];
	int i;
	for(i=0; i<NUM_NVNOISE_TEXTURES; i++)
	{
		sprintf( acFilename, "./Texture/pntex%d.dds", i );

		bool res = m_aNoiseTexture[i].Load( acFilename );
		if( !res )
			return false;
	}

	return true;
}

/*
void CPseudoNoiseEffect::LoadExtraTexture( const char *pcTextureFilename, float fScale )
{
}*/


void CPseudoNoiseEffect::UpdateScreenSize()
{
//	m_iScreenWidth  = GameWindowManager().GetScreenWidth();
//	m_iScreenHeight = GameWindowManager().GetScreenHeight();

	uint w=0,h=0;
	GraphicsDevice().GetViewportSize( w, h );
	m_iScreenWidth  = (int)w;
	m_iScreenHeight = (int)h;

	float width  = (float)m_iScreenWidth;
	float height = (float)m_iScreenHeight;

	m_avTextureRect[0].vPosition = D3DXVECTOR3( 0,     0,      0);
	m_avTextureRect[1].vPosition = D3DXVECTOR3( width, 0,      0);
	m_avTextureRect[2].vPosition = D3DXVECTOR3( width, height, 0);
	m_avTextureRect[3].vPosition = D3DXVECTOR3( 0,     height, 0);

	m_FullscreenRect.SetPositionLTRB( 0, 0, m_iScreenWidth - 1, m_iScreenHeight - 1 );
}


void CPseudoNoiseEffect::RenderNoiseEffect()
{
	// single texture & rect class version
	m_FullscreenRect.SetColor( 0x30FFFFFF );
	m_FullscreenRect.Draw( m_NoiseTexture );
	return;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

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

}


void CPseudoNoiseEffect::SetNoiseTexture()
{
	TLVERTEX2 *pavRect = m_avTextureRect;
	int i;
	for(i=0; i<4; i++)
		pavRect[i].tex[0].u = pavRect[i].tex[0].v = 0.0f;

//	int iRand = rand() * 4 / RAND_MAX;
	int iRand = RangedRand( 0, 3 );
	float t = (float)m_iScreenWidth / 512.0f;	// scalse the noise texture according to the screen size
	switch(iRand)
	{
	case 0:
		pavRect[1].tex[0].u = t; pavRect[2].tex[0].u = t; pavRect[2].tex[0].v = t; pavRect[3].tex[0].v = t;
		break;
	case 1:
		pavRect[0].tex[0].u = t; pavRect[3].tex[0].u = t; pavRect[3].tex[0].v = t; pavRect[2].tex[0].v = t;
		break;
	case 2:
		pavRect[2].tex[0].u = t; pavRect[1].tex[0].u = t; pavRect[1].tex[0].v = t; pavRect[0].tex[0].v = t;
		break;
	case 3:
	default:
		pavRect[3].tex[0].u = t; pavRect[0].tex[0].u = t; pavRect[0].tex[0].v = t; pavRect[1].tex[0].v = t;
		break;
	}

	// shift the texture coord to make it  appear more random
	float rand_tu = t * RangedRand( 0.0f, 1.0f );
	float rand_tv = t * RangedRand( 0.0f, 1.0f );
	for( i=0; i<4; i++ )
	{
		pavRect[i].tex[0].u += rand_tu;
		pavRect[i].tex[0].v += rand_tv;
	}

//	iRand = rand() * 4 / RAND_MAX;
	iRand = RangedRand( 0, 3 );
	if( 4 <= iRand )
		iRand = 3;

	DIRECT3D9.GetDevice()->SetTexture( 0, m_aNoiseTexture[iRand].GetTexture() );

	//>>> single texture & rect class version
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
	TEXCOORD2 shift( TEXCOORD2(rand_tu,rand_tv) );
	m_FullscreenRect.SetTextureCoord( i0, 0, TEXCOORD2( 0, 0 ) + shift );
	m_FullscreenRect.SetTextureCoord( i1, 0, TEXCOORD2( u, 0 ) + shift );
	m_FullscreenRect.SetTextureCoord( i2, 0, TEXCOORD2( u, v ) + shift );
	m_FullscreenRect.SetTextureCoord( i3, 0, TEXCOORD2( 0, v ) + shift );
	//<<< single texture & rect class version
}
