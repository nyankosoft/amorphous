#include "CubeMapManager.hpp"
#include "CubeTextureRenderTarget.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"


namespace amorphous
{


CubeMapManager::CubeMapManager()
:
m_pCubeMapSceneRenderer(NULL)
{
	// set projection matrix proerties of the camera
	m_Camera.SetFOV( (float)PI * 0.5f );
	m_Camera.SetAspectRatio( 1.0f );
	m_Camera.SetNearClip( 0.1f );
	m_Camera.SetFarClip( 500.0f );
}


CubeMapManager::~CubeMapManager()
{
//	ReleaseGraphicsResources();
}


void CubeMapManager::Init( int tex_edge_length, TextureFormat::Format tex_format )
{
	m_pCubeTextureRenderTarget = CubeTextureRenderTarget::Create();

	CreateTextures( tex_edge_length, tex_format );
}


bool CubeMapManager::IsReady()
{
	if( m_pCubeTextureRenderTarget
	 && m_pCubeTextureRenderTarget->IsReady() )
	{
		return true;
	}
	else
	{
		return false;
	}
}


/// \param tex_format [in] TextureFormat::A8R8G8B8 if cube map is used to render the scene
void CubeMapManager::CreateTextures( int tex_edge_length, TextureFormat::Format tex_format )
{
}


void CubeMapManager::UpdateCameraOrientation( int face )
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


// Calls RenderSceneToCubeMap() of the cube map scene renderer 6 times
void CubeMapManager::RenderToCubeMap()
{
	if( !m_pCubeMapSceneRenderer )
		return;

	// set view matrix

	// The projection matrix has a FOV of 90 degrees and asp ratio of 1
//	D3DXMATRIXA16 matProj;
//	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 0.01f, 100.0f );

//	D3DXMATRIXA16 mViewDir( *g_Camera.GetViewMatrix() );
//	mViewDir._41 = mViewDir._42 = mViewDir._43 = 0.0f;

	m_Camera.SetPosition( m_pCubeMapSceneRenderer->GetCameraPosition() );

	m_pCubeTextureRenderTarget->Begin();

	// set render target
//	for( int nCube = 0; nCube < m_NumCubes; ++nCube )
//	{
		for( int nFace = 0; nFace < 6; ++nFace )
		{
			m_pCubeTextureRenderTarget->SetRenderTarget( nFace );

//			D3DXMATRIXA16 mView = DXUTGetCubeMapViewMatrix( nFace );
//			D3DXMatrixMultiply( &mView, &mViewDir, &mView );

			UpdateCameraOrientation( nFace ); 

			// Begin the scene
			if( true /*SUCCEEDED( pd3dDevice->BeginScene() )*/ )
			{
				GetShaderManagerHub().PushViewAndProjectionMatrices( m_Camera );

				m_pCubeMapSceneRenderer->RenderSceneToCubeMap( m_Camera );
//				RenderScene( pd3dDevice, &mView, &mProj, &g_pTech[nCube], false, fTime );

				if( nFace == 5 )
                    GetShaderManagerHub().PopViewAndProjectionMatrices();
				else
                    GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();

				// End the scene.
				// pd3dDevice->EndScene();
			}
		}
//	}

	m_pCubeTextureRenderTarget->End();
}


//void CubeMapManager::EndRenderToCubeMap(){}


void CubeMapManager::SaveCubeTextureToFile( const std::string& output_filename )
{
//	if( m_pCurrentCubeMap )
//	{
//		size_t ext_pos = output_filename.rfind( "." );
//		if( ext_pos != std::string::npos )
//		{
//			D3DXIMAGE_FILEFORMAT img_fmt = GetD3DXImageFormatFromFileExt( output_filename.substr(ext_pos) );
//			D3DXSaveTextureToFile( output_filename.c_str(), img_fmt, m_pCurrentCubeMap, NULL );
//		}
//	}
}


} // namespace amorphous
