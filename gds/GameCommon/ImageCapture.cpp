//=================================================================================
//  ImageCapture.cpp
//
//  capture the screen image by rendering the scene to a texture
//
//=================================================================================

#include "ImageCapture.h"
#include "3DCommon/Direct3D9.h"


CImageCapture::CImageCapture()
{
	m_iImageWidth  = 0;
	m_iImageHeight = 0;

	m_pRenderTargetTexture = NULL;	// used as render targets
	m_pRenderTargetSurface = NULL;
	m_pRenderTargetDepthSurface = NULL;

	m_pOriginalSurface = NULL;
	m_pOriginalDepthSurface = NULL;
}


CImageCapture::~CImageCapture()
{
	Release();
}


void CImageCapture::Release()
{
	SAFE_RELEASE( m_pRenderTargetTexture );	// used as render targets
	SAFE_RELEASE( m_pRenderTargetSurface );
	SAFE_RELEASE( m_pRenderTargetDepthSurface );
}


void CImageCapture::SetImageSize( int iWidth, int iHeight )
{
	Release();

	m_iImageWidth  = iWidth;
	m_iImageHeight = iHeight;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// create a texture on which scene is rendered
    hr = pd3dDev->CreateTexture( m_iImageWidth, m_iImageHeight, 
                                 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                                 D3DPOOL_DEFAULT, &m_pRenderTargetTexture, NULL );
	if( FAILED(hr) ) return;

	m_pRenderTargetTexture->GetSurfaceLevel(0, &m_pRenderTargetSurface);


	// depth buffer is not directly used for screenshot but it is required to render a scene to a texture usnig Direct3D
	hr = pd3dDev->CreateDepthStencilSurface( m_iImageWidth, m_iImageHeight,
		                                     D3DFMT_D24S8/*D3DFMT_D16*/, D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pRenderTargetDepthSurface, NULL );
	if( FAILED(hr) ) return;

}


void CImageCapture::EnterCaptureMode()
{
	int iTextureWidth  = m_iImageWidth;
	int iTextureHeight = m_iImageHeight;

//	MessageBox(NULL, "point 0","image capture", MB_OK);
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// turn off the alpha test
	pd3dDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDev->SetRenderState( D3DRS_ZENABLE, TRUE );

	// save access to original settings (viewport, render target and depth buffer)
	pd3dDev->GetViewport( &m_OriginalViewport );

	pd3dDev->GetRenderTarget(0, &m_pOriginalSurface );		// save the pointer to the current render target surface
	pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );	// save the pointer to the current depth buffer

//	D3DXMATRIX matOriginalProj;
//	pd3dDev->GetTransform( D3DTS_PROJECTION, &matOriginalProj ); 


	// set surface and viewport for image capture
//	MessageBox(NULL, "point 1","image capture", MB_OK);
	hr = pd3dDev->SetRenderTarget(0, m_pRenderTargetSurface );
	if( FAILED(hr) )
		return;

	hr = pd3dDev->ColorFill( m_pRenderTargetSurface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0) );

	hr = pd3dDev->SetDepthStencilSurface( m_pRenderTargetDepthSurface );
	if( FAILED(hr) )
		return;

//	MessageBox(NULL, "point 2","image capture", MB_OK);
	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x80000000, 1.0f, 0 );

	D3DVIEWPORT9 image_viewport = { 0, 0, iTextureWidth, iTextureHeight, 0.0f, 1.0f };
	pd3dDev->SetViewport( &image_viewport );

/*	float fAspectRatio = (float)image_viewport.Width / (float)image_viewport.Height;
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	D3DXMatrixPerspectiveFovLH( &matProj, 3.141592f / 3.0f, fAspectRatio, 0.05f, 1600.0f );
	pd3dDev->SetTransform( D3DTS_PROJECTION, &matProj );*/

}


void CImageCapture::ExitCaptureMode()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// restore the original settings
//	MessageBox(NULL, "point 3","image capture", MB_OK);
	pd3dDev->SetViewport( &m_OriginalViewport );

	pd3dDev->SetRenderTarget(0, m_pOriginalSurface );
	pd3dDev->SetDepthStencilSurface( m_pOriginalDepthSurface );

//	pd3dDev->SetTransform( D3DTS_PROJECTION, &matOriginalProj );

	// release graphic resources created for image capture
//	Release();

}


bool CImageCapture::GetCapturedImage( DWORD *pdwImageData )
{
	if( m_iImageWidth == 0 || m_iImageHeight == 0 || !m_pRenderTargetTexture || !m_pRenderTargetSurface || !m_pRenderTargetDepthSurface )
		return false;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// create another texture to copy the rendered image
	LPDIRECT3DTEXTURE9 pImageTexture;
    hr = pd3dDev->CreateTexture( m_iImageWidth, m_iImageHeight, 1, 0, D3DFMT_A8R8G8B8, 
                                 D3DPOOL_SYSTEMMEM, &pImageTexture, NULL );

	LPDIRECT3DSURFACE9 pImageSurface;
	hr = pImageTexture->GetSurfaceLevel(0, &pImageSurface);

	if( FAILED(hr) )
		return false;

	// A texture resource created with D3DUSAGE_RENDERTARGET must be assigned to D3DPOOL_DEFAULT memory
	// and is therefore not lockable. In this case, use instead IDirect3DDevice9::GetRenderTargetData
	// to copy texture data from device memory to system memory.
	hr = pd3dDev->GetRenderTargetData( m_pRenderTargetSurface, pImageSurface );

	if( FAILED(hr) )
		return false;

	D3DLOCKED_RECT locked_rect;

/*	hr = m_pRenderTargetTexture->LockRect( 0,				// UINT Level,
		                            &locked_rect,	// D3DLOCKED_RECT *pLockedRect,
			         				NULL,			// CONST RECT *pRect,
							        0				// DWORD Flags
							        );*/

	hr = pImageSurface->LockRect( &locked_rect,	// D3DLOCKED_RECT *pLockedRect,
			         		      NULL,			// CONST RECT *pRect,
							      0				// DWORD Flags
							      );

	if( FAILED(hr) )
		return false;

//	D3DCOLOR* pARGBColor = (DWORD *)locked_rect.pBits;
//	BYTE red, green, blue;

//	DWORD *pdwImageData = new DWORD [ m_iImageWidth * m_iImageHeight ];

	memcpy( pdwImageData, locked_rect.pBits, sizeof(DWORD) * m_iImageWidth * m_iImageHeight );

//	m_pRenderTargetTexture->UnlockRect( 0 /* UINT Level */ );
	pImageSurface->UnlockRect();

//	*ppImageData = pdwImageData;

	return true;
}
