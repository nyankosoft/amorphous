
#include "CubeMapManager.h"

#include "Support/memory_helpers.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/Shader/ShaderManagerHub.h"


CCubeMapManager::CCubeMapManager()
:
m_pCubeMapSceneRenderer(NULL)
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

	// set projection matrix proerties of the camera
	m_Camera.SetFOV( D3DX_PI * 0.5f );
	m_Camera.SetAspectRatio( 1.0f );
	m_Camera.SetNearClip( 0.1f );
	m_Camera.SetFarClip( 500.0f );

}


CCubeMapManager::~CCubeMapManager()
{
	ReleaseGraphicsResources();
}


void CCubeMapManager::Init( int tex_edge_length, TextureFormat::Format tex_format )
{
	CreateTextures( tex_edge_length, tex_format );
}


/// \param tex_format [in] TextureFormat::A8R8G8B8 if cube map is used to render the scene
void CCubeMapManager::CreateTextures( int tex_edge_length, TextureFormat::Format tex_format )
{
	m_CubeTextureSize = tex_edge_length;
	m_TextureFormat = tex_format;

	D3DFORMAT d3d_fmt;
	if( tex_format == TextureFormat::Invalid )
	/* || any ohter texture formats invalid for cube mapping ) */
	{
		LOG_PRINT_ERROR( fmt_string("An unsupported texture format: %d", tex_format) );
		return;
	}

	d3d_fmt = ConvertTextureFormatToD3DFORMAT( tex_format );

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
		return;
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

	if( !m_pCurrentCubeMap )
	{
		LOG_PRINT_WARNING( "CreateDepthStencilSurface() failed. Cannot create depth stencil surface." );
		return;
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

}


bool CCubeMapManager::IsReady()
{
	if( m_pCurrentCubeMap && m_pDepthCube )
		return true;
	else
		return false;
}


void CCubeMapManager::UpdateCameraOrientation( int face )
{
	Vector3 vDir, vUp, vRight;

	switch(face)
	{
	case 0: vRight = Vector3( 0, 0,-1); vUp = Vector3( 0, 1, 0); vDir = Vector3( 1, 0, 0); break; // pos-x
	case 1: vRight = Vector3( 0, 0, 1); vUp = Vector3( 0, 1, 0); vDir = Vector3(-1, 0, 0); break; // neg-x
	case 2: vRight = Vector3( 1, 0, 0); vUp = Vector3( 0, 0,-1); vDir = Vector3( 0, 1, 0); break; // pos-y
	case 3: vRight = Vector3( 1, 0, 0); vUp = Vector3( 0, 0, 1); vDir = Vector3( 0,-1, 0); break; // neg-y
	case 4: vRight = Vector3( 1, 0, 0); vUp = Vector3( 0, 1, 0); vDir = Vector3( 0, 0, 1); break; // pos-z
	case 5: vRight = Vector3(-1, 0, 0); vUp = Vector3( 0, 1, 0); vDir = Vector3( 0, 0,-1); break; // neg-z
	default:
		break;
	}

	m_Camera.SetOrientation( Matrix33( vRight, vUp, vDir ) );
}


void CCubeMapManager::RenderToCubeMap()
{
	if( !m_pCubeMapSceneRenderer )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	// set view matrix
	HRESULT hr;

	// The projection matrix has a FOV of 90 degrees and asp ratio of 1
//	D3DXMATRIXA16 matProj;
//	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 0.01f, 100.0f );

//	D3DXMATRIXA16 mViewDir( *g_Camera.GetViewMatrix() );
//	mViewDir._41 = mViewDir._42 = mViewDir._43 = 0.0f;

	m_Camera.SetPosition( m_pCubeMapSceneRenderer->GetCameraPosition() );

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

	// set render target
	for( int nCube = 0; nCube < m_NumCubes; ++nCube )
	{
		for( int nFace = 0; nFace < 6; ++nFace )
		{
			LPDIRECT3DSURFACE9 pSurf;

//			hr = m_apCubeMap[nCube]->GetCubeMapSurface( (D3DCUBEMAP_FACES)nFace, 0, &pSurf );
			hr = m_pCurrentCubeMap->GetCubeMapSurface( (D3DCUBEMAP_FACES)nFace, 0, &pSurf );
			pd3dDevice->SetRenderTarget( 0, pSurf );
			SAFE_RELEASE( pSurf );

//			D3DXMATRIXA16 mView = DXUTGetCubeMapViewMatrix( nFace );
//			D3DXMatrixMultiply( &mView, &mViewDir, &mView );

			UpdateCameraOrientation( nFace ); 

			hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x000000ff, 1.0f, 0L );

			// Begin the scene
			if( SUCCEEDED( pd3dDevice->BeginScene() ) )
			{
				ShaderManagerHub.PushViewAndProjectionMatrices( m_Camera );

				m_pCubeMapSceneRenderer->RenderSceneToCubeMap( m_Camera );
//				RenderScene( pd3dDevice, &mView, &mProj, &g_pTech[nCube], false, fTime );

				if( nFace == 5 )
                    ShaderManagerHub.PopViewAndProjectionMatrices();
				else
                    ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();

				// End the scene.
				pd3dDevice->EndScene();
			}
		}
	}


	// restore original render target

    // Restore depth-stencil buffer and render target
    if( m_pOrigDepthStencilSurface )
    {
        hr = pd3dDevice->SetDepthStencilSurface( m_pOrigDepthStencilSurface );
        SAFE_RELEASE( m_pOrigDepthStencilSurface );	// decrement the ref count
    }

    pd3dDevice->SetRenderTarget( 0, m_pOrigRenderTarget );
    SAFE_RELEASE( m_pOrigRenderTarget );	// decrement the ref count

	// set cube texture to shader managers
	ShaderManagerHub.SetCubeTexture( 0, m_pCubeMap32 );
}


//void CCubeMapManager::EndRenderToCubeMap(){}


inline static D3DXIMAGE_FILEFORMAT GetD3DXImageFormatFromFileExt( const std::string& file_ext )
{
	if( file_ext == "bmp" ) return D3DXIFF_BMP;
	else if( file_ext == "jpg" ) return D3DXIFF_JPG;
	else if( file_ext == "tga" ) return D3DXIFF_TGA;
	else if( file_ext == "png" ) return D3DXIFF_PNG;
	else if( file_ext == "dds" ) return D3DXIFF_DDS;
	else if( file_ext == "ppm" ) return D3DXIFF_PPM;
	else if( file_ext == "dib" ) return D3DXIFF_DIB;
	else if( file_ext == "hdr" ) return D3DXIFF_HDR;
	else if( file_ext == "pfm" ) return D3DXIFF_PFM;
	else
		return D3DXIFF_BMP; // save as bmp image by default
}


void CCubeMapManager::SaveCubeTextureToFile( const std::string& output_filename )
{
	if( m_pCurrentCubeMap )
	{
		size_t ext_pos = output_filename.rfind( "." );
		if( ext_pos != std::string::npos )
		{
			D3DXIMAGE_FILEFORMAT img_fmt = GetD3DXImageFormatFromFileExt( output_filename.substr(ext_pos) );
			D3DXSaveTextureToFile( output_filename.c_str(), img_fmt, m_pCurrentCubeMap, NULL );
		}
	}
}


void CCubeMapManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	CreateTextures( m_CubeTextureSize, m_TextureFormat );
}


void CCubeMapManager::ReleaseGraphicsResources()
{
	SAFE_RELEASE( m_apCubeMapFp[0] );
	SAFE_RELEASE( m_apCubeMapFp[1] );
	SAFE_RELEASE( m_pCubeMap32 );
	SAFE_RELEASE( m_pDepthCube );

	m_pCurrentCubeMap = NULL;
}
