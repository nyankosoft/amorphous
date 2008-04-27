
#include "MultipassTexFont.h"

#include "3DCommon/Direct3D9.h"


// static member variables of CTextureRenderTarget
CTextureRenderTarget CMultipassTexFont::m_TexRenderTarget;
int CMultipassTexFont::ms_RefCount = 0;

static int s_TexSize = 1024;

CMultipassTexFont::CMultipassTexFont()
{
	if( ms_RefCount == 0 )
	{
		// create a large texture as a render target of text
		m_TexRenderTarget.Init( s_TexSize, s_TexSize );
	}

	ms_RefCount++;

	// init rectangle
	ZeroMemory(m_TexRectVertex, sizeof(TLVERTEX) * 4);
	for(int i=0; i<4; i++)
	{
		m_TexRectVertex[i].rhw = 1.0f;
		m_TexRectVertex[i].color = 0xFF000000;		//opaque by default
	}

	m_TexRectVertex[0].tu = 0; m_TexRectVertex[0].tv = 0;
	m_TexRectVertex[1].tu = 1; m_TexRectVertex[1].tv = 0;
	m_TexRectVertex[2].tu = 1; m_TexRectVertex[2].tv = 1;
	m_TexRectVertex[3].tu = 0; m_TexRectVertex[3].tv = 1;

	m_BaseFontWidth = 12;
	m_BaseFontHeight = 24;
}


CMultipassTexFont::~CMultipassTexFont()
{
	ms_RefCount--;

	if( ms_RefCount <= 0 )
		m_TexRenderTarget.ReleaseTextures();
}


int CMultipassTexFont::InitFont( const char* pcFontName,
								 int base_font_width,
								 int base_font_height,
								 int font_width,
								 int font_height )
{
	bool result = CreateFont( pcFontName, m_BaseFontWidth, m_BaseFontHeight );

	if( result )
	{
		m_strFontName = pcFontName;
		m_BaseFontWidth  = base_font_width;
		m_BaseFontHeight = base_font_height;
		m_FontWidth  = font_width;
		m_FontHeight = font_height;
		return 1;
	}
	else 
	{
		return 0;
	}
}


void CMultipassTexFont::SetFontSize( int width, int height )
{
	m_FontWidth  = width;
	m_FontHeight = height;
}


void CMultipassTexFont::DrawText( char *str, const D3DXVECTOR2& vPos, DWORD color )
{
	/// set the texture as a render target
	m_TexRenderTarget.SetRenderTarget();

	CFont::DrawText( str, D3DXVECTOR2(0,0), color );

	m_TexRenderTarget.ResetRenderTarget();

	float tu,tv;
	float x,y;
	tu = strlen(str) * (float)m_BaseFontWidth / (float)s_TexSize;
	tv = (float)m_BaseFontHeight / (float)s_TexSize * 0.5f;
	x = strlen(str) * m_FontWidth;
	y = m_FontHeight;
	D3DXVECTOR3 pos = D3DXVECTOR3( vPos.x, vPos.y, 0 );
	m_TexRectVertex[0].vPosition = pos + D3DXVECTOR3(0,0,0);
	m_TexRectVertex[1].vPosition = pos + D3DXVECTOR3(x,0,0);
	m_TexRectVertex[2].vPosition = pos + D3DXVECTOR3(x,y,0);
	m_TexRectVertex[3].vPosition = pos + D3DXVECTOR3(0,y,0);
	m_TexRectVertex[1].tu = tu;
	m_TexRectVertex[2].tu = tu; m_TexRectVertex[2].tv = tv;
	m_TexRectVertex[3].tv = tv;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_iDestAlphaBlend );

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	//pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );

	pd3dDev->SetTexture( 0, m_TexRenderTarget.GetRenderTargetTexture() );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	// draw a rectangle
	HRESULT hr ;

	pd3dDev->SetFVF( TLVERTEX::FVF );
	hr = pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_TexRectVertex, sizeof(TLVERTEX) );

	if( FAILED(hr) )
		MessageBox(NULL, "DrawPrimUP failed.", "Error", MB_OK);

}


void CMultipassTexFont::Release()
{
	CFont::Release();
}


void CMultipassTexFont::Reload()
{
	bool result = CreateFont( m_strFontName.c_str(),
		                      m_BaseFontWidth,
							  m_BaseFontHeight );
}


void CMultipassTexFont::SetBaseFontSize( int width, int height )
{
	bool result = CreateFont( m_strFontName.c_str(), width, height );

	if( result )
	{
		m_BaseFontWidth  = width;
		m_BaseFontHeight = height;
	}
}


/*
void CMultipassTexFont::DrawText( const char *str, const D3DXVECTOR2& vPos, DWORD color = D3DCOLOR_XRGB(255,255,255) )
{
}
*/