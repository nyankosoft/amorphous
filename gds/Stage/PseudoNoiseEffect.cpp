#include "PseudoNoiseEffect.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/D3DMisc.h"
//#include "../Graphics/Shader/ShaderManager.h"
#include "../GameCommon/MTRand.h"


//============================================================================================
// CPseudoNoiseEffect
//============================================================================================

CPseudoNoiseEffect::CPseudoNoiseEffect()
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

//	m_avTextureRect[0].tex[0].u = 0.0f; m_avTextureRect[0].tex[0].v = 0.0f;
//	m_avTextureRect[1].tex[0].u = 1.0f; m_avTextureRect[1].tex[0].v = 0.0f;
//	m_avTextureRect[2].tex[0].u = 1.0f; m_avTextureRect[2].tex[0].v = 1.0f;
//	m_avTextureRect[3].tex[0].u = 0.0f; m_avTextureRect[3].tex[0].v = 1.0f;

	UpdateScreenSize();
}


CPseudoNoiseEffect::~CPseudoNoiseEffect()
{
	ReleaseGraphicsResources();
}


void CPseudoNoiseEffect::ReleaseGraphicsResources()
{
}


void CPseudoNoiseEffect::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	LoadNoiseTextures();
	
	UpdateScreenSize();
}


bool CPseudoNoiseEffect::LoadNoiseTextures()
{
	ReleaseGraphicsResources();

//	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
//	HRESULT hr;

	// load textures used for noise
	char acFilename[256];
	int i;
	for(i=0; i<NUM_NVNOISE_TEXTURES; i++)
	{
		sprintf( acFilename, "Texture\\pntex%d.dds", i );

		bool res = m_aNoiseTexture[i].Load( acFilename );
		if( !res )
			return false;

//		hr = D3DXCreateTextureFromFileA(pd3dDev, acFilename, &m_apNVNoiseTexture[i]);

//		if( FAILED(hr) )
//			return false;
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

	GetViewportSize( m_iScreenWidth, m_iScreenHeight );

	float width  = (float)m_iScreenWidth;
	float height = (float)m_iScreenHeight;

	m_avTextureRect[0].vPosition = D3DXVECTOR3( 0,     0,      0);
	m_avTextureRect[1].vPosition = D3DXVECTOR3( width, 0,      0);
	m_avTextureRect[2].vPosition = D3DXVECTOR3( width, height, 0);
	m_avTextureRect[3].vPosition = D3DXVECTOR3( 0,     height, 0);

}


void CPseudoNoiseEffect::RenderNoiseEffect()
{
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
}