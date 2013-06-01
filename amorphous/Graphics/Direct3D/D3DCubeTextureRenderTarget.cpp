#include "D3DCubeTextureRenderTarget.hpp"
#include "Direct3D9.hpp"
#include "D3DSurfaceFormat.hpp"


namespace amorphous
{


D3DCubeTextureRenderTarget::D3DCubeTextureRenderTarget()
{
	m_apCubeMapFp[0] = NULL;
	m_apCubeMapFp[1] = NULL;
	m_pCubeMap32 = NULL;
	m_pDepthCube = NULL;

	m_pCurrentCubeMap = NULL;

	m_CubeTextureSize = 256;

	m_pOrigRenderTarget = NULL;
	m_pOrigDepthStencilSurface = NULL;

	m_NumCubes = 1;
}


D3DCubeTextureRenderTarget::D3DCubeTextureRenderTarget( uint texture_size, TextureFormat::Format texture_format, uint option_flags )
//:
//TextureRenderTarget( texture_width, texture_height, texture_format, option_flags )
{
}


D3DCubeTextureRenderTarget::D3DCubeTextureRenderTarget( const TextureResourceDesc& texture_desc )
//:
//TextureRenderTarget(texture_desc)
{
}


D3DCubeTextureRenderTarget::~D3DCubeTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


void D3DCubeTextureRenderTarget::ReleaseTextures()
{
	SAFE_RELEASE( m_apCubeMapFp[0] );
	SAFE_RELEASE( m_apCubeMapFp[1] );
	SAFE_RELEASE( m_pCubeMap32 );
	SAFE_RELEASE( m_pDepthCube );

	m_pCurrentCubeMap = NULL;
}



bool D3DCubeTextureRenderTarget::CreateTextures( uint texture_size, TextureFormat::Format texture_format )
{
	m_CubeTextureSize = texture_size;
	m_TextureFormat = texture_format;

	D3DFORMAT d3d_fmt;
	if( texture_format == TextureFormat::Invalid )
	/* || any ohter texture formats invalid for cube mapping ) */
	{
		LOG_PRINT_ERROR( fmt_string("An unsupported texture format: %d", texture_format) );
		return false;
	}

	d3d_fmt = ConvertTextureFormatToD3DFORMAT( texture_format );

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	HRESULT hr;

	m_NumCubes = 1; // always use the single cube texture

/*	if( d3d_fmt == D3DFMT_A16B16G16R16F )
	{
		// Create the cube textures
		ZeroMemory( m_apCubeMapFp, sizeof( m_apCubeMapFp ) );
		hr = pd3dDevice->CreateCubeTexture( m_CubeTextureSize,
											1,
											D3DUSAGE_RENDERTARGET,
											D3DFMT_A16B16G16R16F,
											D3DPOOL_DEFAULT,
											&m_apCubeMapFp[0],
											NULL );

		m_pCurrentCubeMap = m_apCubeMapFp[0];
	}
*/

	hr = pd3dDevice->CreateCubeTexture( m_CubeTextureSize,
										1,
										D3DUSAGE_RENDERTARGET,
										d3d_fmt,
										D3DPOOL_DEFAULT,
										&m_pCubeMap32,
										NULL );

	if( FAILED(hr) || !m_pCubeMap32 )
	{
		LOG_PRINT_WARNING( "CreateCubeTexture() failed. Cannot create cube texture." );
		return false;
	}

	m_pCurrentCubeMap =  m_pCubeMap32;

//	DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();

	LPDIRECT3DSWAPCHAIN9 pSwapChain;
	pd3dDevice->GetSwapChain( 0, &pSwapChain );

	D3DPRESENT_PARAMETERS pp;
	pSwapChain->GetPresentParameters( &pp );

    hr = pd3dDevice->CreateDepthStencilSurface( m_CubeTextureSize,
                                                m_CubeTextureSize,
//                                              d3dSettings.d3d9.pp.AutoDepthStencilFormat,
                                                pp.AutoDepthStencilFormat,
                                                D3DMULTISAMPLE_NONE,
                                                0,
                                                TRUE,
                                                &m_pDepthCube,
                                                NULL );

	if( FAILED(hr) || !m_pCurrentCubeMap )
	{
		LOG_PRINT_WARNING( "CreateDepthStencilSurface() failed. Cannot create depth stencil surface." );
		return false;
	}

/*
	// Initialize the number of cube maps required when using floating point format
//	IDirect3D9* pD3D = DXUTGetD3D9Object(); 
//	D3DCAPS9 caps;
//	hr = pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &caps );

//	if( FAILED( pD3D->CheckDeviceFormat( caps.AdapterOrdinal, caps.DeviceType,
//										 pDeviceSettings->d3d9.AdapterFormat, D3DUSAGE_RENDERTARGET, 
//										 D3DRTYPE_CUBETEXTURE, D3DFMT_A16B16G16R16F ) ) )

	if( FAILED( pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, DIRECT3D9.GetDeviceType(),
										 DIRECT3D9.GetAdapterFormat(), D3DUSAGE_RENDERTARGET, 
										 D3DRTYPE_CUBETEXTURE, D3DFMT_A16B16G16R16F ) ) )
	{
		m_NumCubes = m_NumFpCubeMap = 2;
	}
	else
	{
		m_NumCubes = m_NumFpCubeMap = 1;
	}

	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
	// then switch to SWVP.
//	if( (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
//			caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
//	{
//		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
//	}*/

	return true;
}


bool D3DCubeTextureRenderTarget::IsReady()
{
	if( m_pCurrentCubeMap && m_pDepthCube )
		return true;
	else
		return false;
}


void D3DCubeTextureRenderTarget::Begin()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;

	// save current render target
	m_pOrigRenderTarget = NULL;
	hr = pd3dDevice->GetRenderTarget( 0, &m_pOrigRenderTarget );

	m_pOrigDepthStencilSurface = NULL;
	if( SUCCEEDED( pd3dDevice->GetDepthStencilSurface( &m_pOrigDepthStencilSurface ) ) )
	{
		// If the device has a depth-stencil buffer, use
		// the depth stencil buffer created for the cube textures.
		pd3dDevice->SetDepthStencilSurface( m_pDepthCube );
	}
}


void D3DCubeTextureRenderTarget::SetRenderTarget( unsigned int face_index )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPDIRECT3DSURFACE9 pSurf;

//	HRESULT hr = m_apCubeMap[nCube]->GetCubeMapSurface( (D3DCUBEMAP_FACES)nFace, 0, &pSurf );
	HRESULT hr = m_pCurrentCubeMap->GetCubeMapSurface( (D3DCUBEMAP_FACES)face_index, 0, &pSurf );
	pd3dDevice->SetRenderTarget( 0, pSurf );
	SAFE_RELEASE( pSurf );

	hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x000000ff, 1.0f, 0L );
}


void D3DCubeTextureRenderTarget::End()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;

	// restore original render target

    // Restore depth-stencil buffer and render target
    if( m_pOrigDepthStencilSurface )
    {
        hr = pd3dDevice->SetDepthStencilSurface( m_pOrigDepthStencilSurface );
        SAFE_RELEASE( m_pOrigDepthStencilSurface );	// decrement the ref count
    }

    pd3dDevice->SetRenderTarget( 0, m_pOrigRenderTarget );
    SAFE_RELEASE( m_pOrigRenderTarget );	// decrement the ref count
}


void D3DCubeTextureRenderTarget::LoadGraphicsResources( const GraphicsParameters& rParam )
{
//	CreateTextures( m_CubeTextureSize, m_TextureFormat );
}


void D3DCubeTextureRenderTarget::ReleaseGraphicsResources()
{
}


} // namespace amorphous
