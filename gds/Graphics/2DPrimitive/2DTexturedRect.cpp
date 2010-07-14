#include "2DTexturedRect.hpp"


C2DTexturedRectRenderer_D3D::C2DTexturedRectRenderer_D3D()
{
	m_FVFs[0] = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	m_FVFs[1] = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2;
	m_FVFs[2] = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3;
	m_FVFs[3] = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4;
}


inline void C2DTexturedRectRenderer_D3D::Render( const void *pVertices, DWORD fvf, uint vert_size )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetPixelShader( NULL );
	pd3dDev->SetFVF( fvf );
	HRESULT hr = S_OK;
	hr = pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, pVertices, vert_size );

	if( FAILED(hr) )
		MessageBox(NULL, "DrawPrimUP failed.", "Error", MB_OK);
}


void C2DTexturedRectRenderer_D3D::Render( const C2DTexturedRect<1,U32>& rect, CTextureHandle& tex0 )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetTexture( 0, tex0.GetTexture() );

	// render state settings
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	Render( rect.GetVertices(), m_FVFs[0], sizeof(CTex2DVertex<1,U32>) );
}


void C2DTexturedRectRenderer_D3D::Render( const C2DTexturedRect<2,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1 )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetTexture( 0, tex0.GetTexture() );
	pd3dDev->SetTexture( 1, tex1.GetTexture() );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG2,	D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 2, D3DTSS_COLOROP,	D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1,	D3DTA_CURRENT );
	pd3dDev->SetTextureStageState( 2, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

	Render( rect.GetVertices(), m_FVFs[1], sizeof(CTex2DVertex<2,U32>) );
}


void C2DTexturedRectRenderer_D3D::Render( const C2DTexturedRect<3,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2 )
{
}


void C2DTexturedRectRenderer_D3D::Render( const C2DTexturedRect<4,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2, CTextureHandle& tex3 )
{
}


void C2DTexturedRectRenderer_D3D::Render( const C2DTexturedRect<1,SFloatRGBAColor>& rect, CTextureHandle& tex0 )
{
}


void C2DTexturedRectRenderer_D3D::Render( const C2DTexturedRect<2,SFloatRGBAColor>& rect, CTextureHandle& tex0, CTextureHandle& tex1 )
{
}



C2DTexturedRectRenderer_GL::C2DTexturedRectRenderer_GL()
{}


void C2DTexturedRectRenderer_GL::Render( const C2DTexturedRect<1,U32>& rect, CTextureHandle& tex0 )
{
}


void C2DTexturedRectRenderer_GL::Render( const C2DTexturedRect<2,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1 )
{
}
