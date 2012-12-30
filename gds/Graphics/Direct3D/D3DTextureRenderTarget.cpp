#include "D3DTextureRenderTarget.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/Direct3D/D3DSurfaceFormat.hpp"
#include "Support/Profile.hpp"


namespace amorphous
{
//#include "Support/BMPImageExporter.hpp"


CD3DTextureRenderTarget::CD3DTextureRenderTarget()
{
//	m_pRenderTargetTexture      = NULL;
	m_pRenderTargetSurface      = NULL;
	m_pRenderTargetDepthSurface = NULL;

//	m_pRenderTargetCopyTexture = NULL;
	m_pRenderTargetCopySurface = NULL;

	m_pOriginalSurface = NULL;
	m_pOriginalDepthSurface = NULL;
}


CD3DTextureRenderTarget::CD3DTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
:
CTextureRenderTarget( texture_width, texture_height, texture_format, option_flags )
{
//	m_pRenderTargetTexture      = NULL;
	m_pRenderTargetSurface      = NULL;
	m_pRenderTargetDepthSurface = NULL;

//	m_pRenderTargetCopyTexture = NULL;
	m_pRenderTargetCopySurface = NULL;

	m_pOriginalSurface = NULL;
	m_pOriginalDepthSurface = NULL;

	LoadTextures();
}


CD3DTextureRenderTarget::CD3DTextureRenderTarget( const CTextureResourceDesc& texture_desc )
:
CTextureRenderTarget(texture_desc)
{
//	m_pRenderTargetTexture      = NULL;
	m_pRenderTargetSurface      = NULL;
	m_pRenderTargetDepthSurface = NULL;

//	m_pRenderTargetCopyTexture = NULL;
	m_pRenderTargetCopySurface = NULL;

	m_pOriginalSurface = NULL;
	m_pOriginalDepthSurface = NULL;

	LoadTextures();
}


CD3DTextureRenderTarget::~CD3DTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


void CD3DTextureRenderTarget::ReleaseTextures()
{
//	SAFE_RELEASE( m_pRenderTargetTexture );
	SAFE_RELEASE( m_pRenderTargetSurface );
	SAFE_RELEASE( m_pRenderTargetDepthSurface );

//	SAFE_RELEASE( m_pRenderTargetCopyTexture );
	SAFE_RELEASE( m_pRenderTargetCopySurface );
}



bool CD3DTextureRenderTarget::LoadTextures()
{
	ReleaseTextures();

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// create a texture on which the scene is rendered
//	hr = pd3dDev->CreateTexture( m_TextureDesc.Width,
//		                         m_TextureDesc.Height, 
//								 1,
//								 D3DUSAGE_RENDERTARGET,
//								 ConvertTextureFormatToD3DFORMAT(m_TextureDesc.Format), // D3DFMT_A8R8G8B8 /*D3DFMT_X8R8G8B8*/,
//								 D3DPOOL_DEFAULT, &m_pRenderTargetTexture, NULL );
//
//	if( FAILED(hr) )
//	{
//		LOG_PRINT_ERROR( fmt_string("IDirect3DDevice9::CreateTexture() failed ( texture (w,h) = (%d,%d), fotmat: %s ).", m_TextureDesc.Width, m_TextureDesc.Height, GetSurfaceFormatName(m_TextureDesc.Format) ) );
//		return false;
//	}

//	hr = m_pRenderTargetTexture->GetSurfaceLevel(0, &m_pRenderTargetSurface);

//	m_TextureDesc.ResourcePath = fmt_string("<TextureRenderTarget width=%d height=%d>", m_TextureDesc.Width, m_TextureDesc.Height );
	m_RenderTargetTexture.Load( m_TextureDesc );

	if( m_RenderTargetTexture.GetTexture() )
		hr = m_RenderTargetTexture.GetTexture()->GetSurfaceLevel(0, &m_pRenderTargetSurface);

	// create surface object to hold a copy of render target surface
//	hr = pd3dDev->CreateTexture( m_TextureDesc.Width,
//		                         m_TextureDesc.Height, 
//                                 1, 0,
//								 ConvertTextureFormatToD3DFORMAT(m_TextureDesc.Format), // D3DFMT_A8R8G8B8 /*D3DFMT_X8R8G8B8*/, 
//                                 D3DPOOL_SYSTEMMEM, &m_pRenderTargetCopyTexture, NULL );
//
//	if( FAILED(hr) )
//	{
//		LOG_PRINT_ERROR( fmt_string("IDirect3DDevice9::CreateTexture() for the copy of the render target texture failed ( texture (w,h) = (%d,%d), fotmat: %s ).", m_TextureDesc.Width, m_TextureDesc.Height, GetSurfaceFormatName(m_TextureDesc.Format) ) );
//		return false;
//	}

//	hr = m_pRenderTargetCopyTexture->GetSurfaceLevel(0, &m_pRenderTargetCopySurface);

	// decrement the reference count
//	m_pRenderTargetCopySurface->Release();

//	if( FAILED(hr) )
//		return false;

	CTextureResourceDesc copy_desc = m_TextureDesc;
	copy_desc.UsageFlags = 0;
	m_RenderTargetCopyTexture.Load( copy_desc );

	if( m_RenderTargetCopyTexture.GetTexture() )
		hr = m_RenderTargetCopyTexture.GetTexture()->GetSurfaceLevel(0, &m_pRenderTargetCopySurface);


	// get the current depth buffer format
	hr = pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );
	D3DSURFACE_DESC surface_desc;
	m_pOriginalDepthSurface->GetDesc( &surface_desc );

	// decrement the reference count
//	m_pOriginalDepthSurface->Release();

	hr = pd3dDev->CreateDepthStencilSurface( m_TextureDesc.Width,
	                                         m_TextureDesc.Height,
		                                     surface_desc.Format, /*D3DFMT_D16,*/
											 D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pRenderTargetDepthSurface, NULL );

/*	hr = pd3dDev->CreateDepthStencilSurface( m_iTextureWidth, m_iTextureHeight,
		                                     D3DFMT_D16, D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pRenderTargetDepthSurface, NULL );
											 */
	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( fmt_string("IDirect3DDevice9::CreateDepthStencilSurface() failed ( texture (w,h) = (%d,%d), fotmat: %s ).", m_TextureDesc.Width, m_TextureDesc.Height, GetD3DSurfaceFormatName(surface_desc.Format) ) );
		return false;
	}

	return true;
}


void CD3DTextureRenderTarget::CopyRenderTarget()
{
	DIRECT3D9.GetDevice()->GetRenderTargetData( m_pRenderTargetSurface, m_pRenderTargetCopySurface );
}


void CD3DTextureRenderTarget::SetRenderTarget()
{
	PROFILE_FUNCTION();

	if( //!m_pRenderTargetTexture ||
		!m_pRenderTargetSurface ||
		!m_pRenderTargetDepthSurface ||
//		!m_pRenderTargetCopyTexture ||
		!m_pRenderTargetCopySurface
		)
		return;

	int iTextureWidth  = m_TextureDesc.Width;
	int iTextureHeight = m_TextureDesc.Height;

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
	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, m_BackgroundColor.GetARGB32(), 1.0f, 0 );

	// set viewport
	D3DVIEWPORT9 viewport = { 0, 0, iTextureWidth, iTextureHeight, 0.0f, 1.0f };
	pd3dDev->SetViewport( &viewport );

}


void CD3DTextureRenderTarget::ResetRenderTarget()
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


void CD3DTextureRenderTarget::OutputImageFile( const std::string& image_file_path )
{
//	CBMPImageExporter bmp_exporter;
//	DWORD *pdwTexelData;

	if( image_file_path.length() <= 4 )
		return;

	CopyRenderTarget();

	LPDIRECT3DTEXTURE9 pTex = GetD3DRenderTargetCopyTexture();

	std::string ext = image_file_path.substr( image_file_path.length()-3 );
	D3DXIMAGE_FILEFORMAT img_fmt = GetD3DXImageFormatFromFileExt( ext );

	D3DXSaveTextureToFile( image_file_path.c_str(), img_fmt, pTex, NULL );

/*	D3DLOCKED_RECT locked_rect;
	pTex->LockRect( 0, &locked_rect, NULL, 0 );
	pdwTexelData = (DWORD *)locked_rect.pBits;

	bmp_exporter.OutputImage_24Bit( filename, m_TextureDesc.Width, m_TextureDesc.Height, pdwTexelData );

	pTex->UnlockRect( 0 );*/
}


} // namespace amorphous
