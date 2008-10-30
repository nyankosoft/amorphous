
#include "TextureRenderTarget.h"

#include "3DCommon/Direct3D9.h"

#include "Support/BMPImageExporter.h"


CTextureRenderTarget::CTextureRenderTarget()
{
	m_pRenderTargetTexture      = NULL;
	m_pRenderTargetSurface      = NULL;
	m_pRenderTargetDepthSurface = NULL;

	m_pRenderTargetCopyTexture = NULL;
	m_pRenderTargetCopySurface = NULL;

	m_iTextureWidth  = 1;
	m_iTextureHeight = 1;

	m_dwBackgroundColor = 0x00000000;
}


CTextureRenderTarget::CTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
{
	m_pRenderTargetTexture      = NULL;
	m_pRenderTargetSurface      = NULL;
	m_pRenderTargetDepthSurface = NULL;

	m_pRenderTargetCopyTexture = NULL;
	m_pRenderTargetCopySurface = NULL;

	m_iTextureWidth  = texture_width;
	m_iTextureHeight = texture_height;

	m_dwBackgroundColor = 0x00000000;

	LoadTextures();
}


CTextureRenderTarget::~CTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


void CTextureRenderTarget::Init( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
{
	m_iTextureWidth  = texture_width;
	m_iTextureHeight = texture_height;

	m_dwBackgroundColor = 0x00000000;

	LoadTextures();
}


void CTextureRenderTarget::InitScreenSizeRenderTarget()
{
	m_bScreenSizeRenderTarget = true;

	Init( GetScreenWidth(), GetScreenHeight(), TextureFormat::A8R8G8B8, 0 );
}


void CTextureRenderTarget::ReleaseTextures()
{
	SAFE_RELEASE( m_pRenderTargetTexture );
	SAFE_RELEASE( m_pRenderTargetSurface );
	SAFE_RELEASE( m_pRenderTargetDepthSurface );

	SAFE_RELEASE( m_pRenderTargetCopyTexture );
	SAFE_RELEASE( m_pRenderTargetCopySurface );
}



bool CTextureRenderTarget::LoadTextures()
{
	ReleaseTextures();

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// create a texture on which the scene is rendered
	hr = pd3dDev->CreateTexture( m_iTextureWidth, m_iTextureHeight, 
                                 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8 /*D3DFMT_X8R8G8B8*/, 
                                 D3DPOOL_DEFAULT, &m_pRenderTargetTexture, NULL );

	if( FAILED(hr) )
		return false;

	hr = m_pRenderTargetTexture->GetSurfaceLevel(0, &m_pRenderTargetSurface);


	// create surface object to hold a copy of render target surface
	hr = pd3dDev->CreateTexture( m_iTextureWidth, m_iTextureHeight, 
                                 1, 0, D3DFMT_A8R8G8B8 /*D3DFMT_X8R8G8B8*/, 
                                 D3DPOOL_SYSTEMMEM, &m_pRenderTargetCopyTexture, NULL );

	hr = m_pRenderTargetCopyTexture->GetSurfaceLevel(0, &m_pRenderTargetCopySurface);

	// decrement the reference count
//	m_pRenderTargetCopySurface->Release();

	if( FAILED(hr) )
		return false;


	// get the current depth buffer format
	hr = pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );
	D3DSURFACE_DESC surface_desc;
	m_pOriginalDepthSurface->GetDesc( &surface_desc );

	// decrement the reference count
//	m_pOriginalDepthSurface->Release();

	hr = pd3dDev->CreateDepthStencilSurface( m_iTextureWidth,
	                                         m_iTextureHeight,
		                                     surface_desc.Format, /*D3DFMT_D16,*/
											 D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pRenderTargetDepthSurface, NULL );

/*	hr = pd3dDev->CreateDepthStencilSurface( m_iTextureWidth, m_iTextureHeight,
		                                     D3DFMT_D16, D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pRenderTargetDepthSurface, NULL );
											 */
	if( FAILED(hr) ) return false;

	return true;
}


void CTextureRenderTarget::ReleaseGraphicsResources()
{
	ReleaseTextures();
}


void CTextureRenderTarget::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	if( m_bScreenSizeRenderTarget )
	{
		// resize the render target texture size for a new screen resolution
		m_iTextureWidth  = GetScreenWidth();
		m_iTextureHeight = GetScreenHeight();
	}

	LoadTextures();
}


void CTextureRenderTarget::CopyRenderTarget()
{
	DIRECT3D9.GetDevice()->GetRenderTargetData( m_pRenderTargetSurface, m_pRenderTargetCopySurface );
}


void CTextureRenderTarget::SetRenderTarget()
{
	if( !m_pRenderTargetTexture ||
		!m_pRenderTargetSurface ||
		!m_pRenderTargetDepthSurface ||
		!m_pRenderTargetCopyTexture ||
		!m_pRenderTargetCopySurface
		)
		return;

	int iTextureWidth  = m_iTextureWidth;
	int iTextureHeight = m_iTextureHeight;

//	MessageBox(NULL, "point 0","texture render target", MB_OK);
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// turn off the alpha test
	pd3dDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	// save access to original settings (viewport, render target and depth buffer)
	pd3dDev->GetViewport( &m_OriginalViewport );

	// save the pointer to the current render target surface
	hr = pd3dDev->GetRenderTarget( 0, &m_pOriginalSurface );

	if( FAILED(hr) )
		return;

	// save the pointer to the current depth buffer	
	hr = pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );

	if( FAILED(hr) )
		return;

	// surface and viewport for glare image
//	MessageBox(NULL, "point 1","texture render target", MB_OK);
	hr = pd3dDev->SetRenderTarget( 0, m_pRenderTargetSurface );
//	m_pRenderTargetSurface->Release();

	if( FAILED(hr) )
		return;

	hr = pd3dDev->ColorFill( m_pRenderTargetSurface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0) );

	hr = pd3dDev->SetDepthStencilSurface( m_pRenderTargetDepthSurface );
//	m_pRenderTargetDepthSurface->Release();

	if( FAILED(hr) )
		return;

//	MessageBox(NULL, "point 2","texture render target", MB_OK);
//	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x205050FF, 1.0f, 0 );
//	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x20505050, 1.0f, 0 );
	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, m_dwBackgroundColor, 1.0f, 0 );

	// set viewport
	D3DVIEWPORT9 viewport = { 0, 0, iTextureWidth, iTextureHeight, 0.0f, 1.0f };
	pd3dDev->SetViewport( &viewport );

}


void CTextureRenderTarget::ResetRenderTarget()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

//	MessageBox(NULL, "point 3","texture render target", MB_OK);

	// restore original render target

	pd3dDev->SetViewport( &m_OriginalViewport );

	hr = pd3dDev->SetRenderTarget( 0, m_pOriginalSurface );

	if( FAILED(hr) )
		return;

	// decrement the reference count
///	SAFE_RELEASE( m_pOriginalSurface );

	// restore the original depth stencil surface
	hr = pd3dDev->SetDepthStencilSurface( m_pOriginalDepthSurface );

	if( FAILED(hr) )
		return;

///	SAFE_RELEASE( m_pOriginalDepthSurface );

	m_pOriginalSurface = NULL;
	m_pOriginalDepthSurface = NULL;
	memset( &m_OriginalViewport, 0, sizeof(D3DVIEWPORT9) );
}


void CTextureRenderTarget::OutputImageFile( const char* filename )
{
	CBMPImageExporter bmp_exporter;
	DWORD *pdwTexelData;

	CopyRenderTarget();

	LPDIRECT3DTEXTURE9 pTex = GetRenderTargetCopyTexture();

	D3DLOCKED_RECT locked_rect;
	pTex->LockRect( 0, &locked_rect, NULL, 0 );
	pdwTexelData = (DWORD *)locked_rect.pBits;

	bmp_exporter.OutputImage_24Bit( filename, m_iTextureWidth, m_iTextureHeight, pdwTexelData );

	pTex->UnlockRect( 0 );
}
