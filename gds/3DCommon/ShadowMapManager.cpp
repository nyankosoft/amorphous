
#include "ShadowMapManager.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/2DTexRect.h"
#include "Support/Log/DefaultLog.h"

#include <string>
using namespace std;


std::string CShadowMapManager::ms_strDefaultShaderFilename = "Shader/ShadowMap.fx";


CShadowMapManager::CShadowMapManager()
{
	SetDefault();

	m_iTextureWidth  = GetScreenWidth();
	m_iTextureHeight = GetScreenHeight();

//	m_iTextureWidth  = 1;
//	m_iTextureHeight = 1;
}


CShadowMapManager::CShadowMapManager( int texture_width, int texture_height )
{
	SetDefault();

	m_iTextureWidth  = texture_width;
	m_iTextureHeight = texture_height;

}


bool CShadowMapManager::Init()
{
	m_ShaderManager.Release();
	ReleaseTextures();

	bool shader_loaded = m_ShaderManager.LoadShaderFromFile( ms_strDefaultShaderFilename );

	if( !shader_loaded )
	{
		LOG_PRINT_ERROR( "Cannot load shader: " + ms_strDefaultShaderFilename );
		return false;
	}

	HRESULT hr = CreateShadowMapTextures();
	bool c = CreateSceneShadowMapTextures();

	m_SceneRenderTarget.InitScreenSizeRenderTarget();

	if( shader_loaded && SUCCEEDED(hr) && shader_loaded )
		return true;
	else
		return false;
}


void CShadowMapManager::SetDefault()
{
	m_pShadowMap = NULL;
	m_pDSShadow = NULL;

	m_pShadowedView = NULL;
	m_pDSShadowedView = NULL;

	m_ShadowMapSize = 512;

	// set default light direction & position
	Vector3 vLightDir =  Vector3(-0.56568f, -0.70711f, -0.42426f);
	Vector3 vLightPos =  Vector3( 5.0f, 10.0f,  6.0f );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir( vLightDir ) );
	m_LightCamera.SetPosition( vLightPos );

	// set the default pose of the scene camera
	m_SceneCamera.SetPosition( Vector3( 0.0f, 1.0f, -5.0f ) );
	m_SceneCamera.SetOrientation( Matrix33Identity() );
}


CShadowMapManager::~CShadowMapManager()
{
	ReleaseGraphicsResources();
}


void CShadowMapManager::ReleaseTextures()
{
	SAFE_RELEASE( m_pShadowMap );
	SAFE_RELEASE( m_pDSShadow );

	SAFE_RELEASE( m_pShadowedView );
	SAFE_RELEASE( m_pDSShadowedView );
}


#define V_RETURN( x ) hr = x; if( FAILED(hr) ) return hr;


HRESULT CShadowMapManager::CreateShadowMapTextures()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

	// Setup the camera's projection parameters
//	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
//	g_VCamera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 100.0f );
//	g_LCamera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 100.0f );

	// Create the default texture (used when a triangle does not use a texture)
//	V_RETURN( pd3dDevice->CreateTexture( 1, 1, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT, &m_pTexDef, NULL ) );
//	D3DLOCKED_RECT lr;
//	V_RETURN( m_pTexDef->LockRect( 0, &lr, NULL, 0 ) );
//	*(LPDWORD)lr.pBits = D3DCOLOR_RGBA( 255, 255, 255, 255 );
//	V_RETURN( m_pTexDef->UnlockRect( 0 ) );

	// Restore the effect variables
//	V_RETURN( m_pEffect->SetVector( "g_vLightDiffuse", (D3DXVECTOR4 *)&m_Light.Diffuse ) );
//	V_RETURN( m_pEffect->SetFloat( "g_fCosTheta", cosf( g_Light.Theta ) ) );

	// Create the shadow map texture
	V_RETURN( pd3dDevice->CreateTexture( m_ShadowMapSize, m_ShadowMapSize,
										 1, D3DUSAGE_RENDERTARGET,
										 D3DFMT_R32F,
										 D3DPOOL_DEFAULT,
										 &m_pShadowMap,
										 NULL ) );

	// Create the depth-stencil buffer to be used with the shadow map
	// We do this to ensure that the depth-stencil buffer is large
	// enough and has correct multisample type/quality when rendering
	// the shadow map.  The default depth-stencil buffer created during
	// device creation will not be large enough if the user resizes the
	// window to a very small size.  Furthermore, if the device is created
	// with multisampling, the default depth-stencil buffer will not
	// work with the shadow map texture because texture render targets
	// do not support multisample.
//	DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();
	// get the current depth buffer format
	hr = pd3dDevice->GetDepthStencilSurface( &m_pOriginalDepthSurface );
	D3DSURFACE_DESC surface_desc;
	m_pOriginalDepthSurface->GetDesc( &surface_desc );
	V_RETURN( pd3dDevice->CreateDepthStencilSurface( m_ShadowMapSize,
													 m_ShadowMapSize,
//													 d3dSettings.pp.AutoDepthStencilFormat,
													 surface_desc.Format,
													 D3DMULTISAMPLE_NONE,
													 0,
													 TRUE,
													 &m_pDSShadow,
													 NULL ) );

	// Initialize the shadow projection matrix
//	D3DXMatrixPerspectiveFovLH( &m_mShadowProj, g_fLightFov, 1, 0.01f, 100.0f);

	return S_OK;
}


bool CShadowMapManager::CreateSceneShadowMapTextures()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// create a texture on which the scene is rendered
	hr = pd3dDev->CreateTexture( m_iTextureWidth, m_iTextureHeight, 
								 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8 /*D3DFMT_X8R8G8B8*/, 
								 D3DPOOL_DEFAULT, &m_pShadowedView, NULL );

	if( FAILED(hr) )
		return false;

//	hr = m_pRenderTargetTexture->GetSurfaceLevel(0, &m_pRenderTargetSurface);

	// get the current depth buffer format
	hr = pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );
	D3DSURFACE_DESC surface_desc;
	m_pOriginalDepthSurface->GetDesc( &surface_desc );

	hr = pd3dDev->CreateDepthStencilSurface( m_iTextureWidth,
											 m_iTextureHeight,
											 surface_desc.Format, /*D3DFMT_D16,*/
											 D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pDSShadowedView, NULL );

/*	hr = pd3dDev->CreateDepthStencilSurface( m_iTextureWidth, m_iTextureHeight,
											 D3DFMT_D16, D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pRenderTargetDepthSurface, NULL );
											 */
	if( FAILED(hr) ) return false;

	return true;
}


void CShadowMapManager::ReleaseGraphicsResources()
{
	m_ShaderManager.Release();
	ReleaseTextures();
}


void CShadowMapManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	Init();
}


void CShadowMapManager::RenderSceneWithShadow( int sx, int sy, int ex, int ey )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	if( /* debug == */ true )
	{
		int w = (ex - sx + 1) / 2;
		int h = (ey - sy + 1) / 2;

		RenderShadowMapTexture(      sx,     sy, sx + w - 1,   sy + h - 1 );

		RenderSceneShadowMapTexture( sx + w, sy, sx + w*2 - 1, sy + h - 1 );

		C2DRect screen_rect( sx, sy + h, sx + w - 1, sy + h*2 - 1, 0xFFFFFFFF );
		screen_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		screen_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture() );

		C2DTexRect tex_rect( sx + w, sy + h, sx + w*2 - 1, sy + h*2 - 1, 0xFFFFFFFF );
		tex_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		tex_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture(), m_pShadowedView );

		return;
	}

/*
	// render using HLSL

	LPD3DXEFFECT pEffect = m_ShaderManager.GetEffect();
	HRESULT hr = pEffect->SetTechnique( "SceneWithShadow" );

	m_ShaderManager.SetTexture( 0, m_SceneRenderTarget.GetRenderTargetTexture() );
	m_ShaderManager.SetTexture( 1, m_pShadowedView );

	pEffect->CommitChanges();

	screen_rect.draw();
/*

/*
	pd3dDev->SetTexture( 0, m_SceneRenderTarget.GetRenderTargetTexture() );
	pd3dDev->SetTexture( 1, m_pShadowedView );

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDev->SetRenderState( D3DRS_ZENABLE, FALSE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
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

	screen_rect.draw();
*/


	C2DTexRect tex_rect( sx, sy, ex, ey, 0xFFFFFFFF );
	tex_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	tex_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture(), m_pShadowedView );

}


void CShadowMapManager::RenderSceneWithShadow()
{
	RenderSceneWithShadow( 0, 0, GetScreenWidth() - 1, GetScreenHeight() - 1 );
}


void CShadowMapManager::RenderShadowMapTexture( int sx, int sy, int ex, int ey )
{
	C2DRect rect( sx, sy, ex, ey, 0xFFFFFFFF );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	rect.Draw( m_pShadowMap );
}


void CShadowMapManager::RenderSceneShadowMapTexture( int sx, int sy, int ex, int ey )
{
	C2DRect rect( sx, sy, ex, ey, 0xFFFFFFFF );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	rect.Draw( m_pShadowedView );
}


void CShadowMapManager::RenderSceneWithoutShadow( int sx, int sy, int ex, int ey )
{
	C2DRect rect( sx, sy, ex, ey, 0xFFFFFFFF );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture() );
//	rect.Draw();
}



//void CShadowMapManager::BeginSceneForShadowCaster()
void CShadowMapManager::BeginSceneShadowMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// set shadow map texture as a render target

	// save the current settings
//	LPDIRECT3DSURFACE9 pOldRT = NULL;
//	V( pd3dDev->GetRenderTarget( 0, &pOldRT ) );
	hr = pd3dDev->GetRenderTarget( 0, &m_pOriginalSurface );

	LPDIRECT3DSURFACE9 pShadowSurf;
	if( SUCCEEDED( m_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf ) ) )
	{
		pd3dDev->SetRenderTarget( 0, pShadowSurf );
		SAFE_RELEASE( pShadowSurf );
	}

//	LPDIRECT3DSURFACE9 pOldDS = NULL;
	if( SUCCEEDED( pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface ) ) )
		pd3dDev->SetDepthStencilSurface( m_pDSShadow );
//	{
//		CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Shadow Map" );
//		RenderScene( pd3dDevice, true, fElapsedTime, &mLightView, &m_mShadowProj );
//	}

	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFFFFFF00, 1.0f, 0 );

	// set the shadow map shader
	// meshes that renderes themselves as shadow casters use the techniques
	// in this shadow map shader
	CShader::Get()->SetShaderManager( &m_ShaderManager );

	D3DXMATRIX matWorld, matView, matProj;

	D3DXMatrixIdentity( &matWorld );
	m_LightCamera.GetCameraMatrix( matView );
	m_LightCamera.GetProjectionMatrix( matProj );

	m_ShaderManager.SetWorldViewProjectionTransform( matWorld, matView, matProj );

	pd3dDev->BeginScene();
}


//void CShadowMapManager::EndSceneForShadowCaster()
void CShadowMapManager::EndSceneShadowMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->EndScene();

	// restore the original render tareget

	if( m_pOriginalDepthSurface )
	{
		pd3dDev->SetDepthStencilSurface( m_pOriginalDepthSurface );
		SAFE_RELEASE( m_pOriginalDepthSurface );
	}

	pd3dDev->SetRenderTarget( 0, m_pOriginalSurface );
	SAFE_RELEASE( m_pOriginalSurface );
}


//void CShadowMapManager::BeginSceneForShadowReceiver()
void CShadowMapManager::BeginSceneDepthMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// set shadow map texture as a render target

	hr = pd3dDev->GetRenderTarget( 0, &m_pOriginalSurface );

	LPDIRECT3DSURFACE9 pShadowSurf;
	if( SUCCEEDED( m_pShadowedView->GetSurfaceLevel( 0, &pShadowSurf ) ) )
	{
		pd3dDev->SetRenderTarget( 0, pShadowSurf );
		SAFE_RELEASE( pShadowSurf );
	}

	if( SUCCEEDED( pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface ) ) )
		pd3dDev->SetDepthStencilSurface( m_pDSShadowedView );

	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFFFF00FF, 1.0f, 0 );
//	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );

	// set the shadow map shader
	// meshes that renderes themselves as shadow casters use the techniques
	// in this shadow map shader
	CShader::Get()->SetShaderManager( &m_ShaderManager );

	LPD3DXEFFECT pEffect = m_ShaderManager.GetEffect();
	D3DXMATRIX matWorld, matView, matProj;

	D3DXMatrixIdentity( &matWorld );
	m_SceneCamera.GetCameraMatrix( matView );
	m_SceneCamera.GetProjectionMatrix( matProj );

	m_ShaderManager.SetWorldViewProjectionTransform( matWorld, matView, matProj );

	D3DXMATRIX matWorldToLightProj;
	m_LightCamera.GetCameraMatrix( matView );
	m_LightCamera.GetProjectionMatrix( matProj );
	D3DXMatrixMultiply( &matWorldToLightProj, &matView, &matProj );
	hr = pEffect->SetMatrix( "g_mWorldToLightProj", &matWorldToLightProj );

//	pEffect->SetFloatArray( "g_vLightPos", (float *)&m_LightCamera.GetPosition(), 3 );
//	pEffect->SetFloatArray( "g_vLightDir", (float *)&m_LightCamera.GetFrontDirection(), 3 );

	// set light pos and dir in scene camera space
	D3DXMATRIX matSceneCamView;
	m_SceneCamera.GetCameraMatrix( matSceneCamView );

	D3DXVECTOR3 vWorldLightPos = m_LightCamera.GetPosition();
	D3DXVECTOR3 vWorldLightDir = m_LightCamera.GetFrontDirection();

	D3DXVECTOR3 vViewLightPos, vViewLightDir;
	D3DXVec3TransformCoord( &vViewLightPos, &vWorldLightPos, &matSceneCamView );
	D3DXVec3TransformCoord( &vViewLightDir, &vWorldLightDir, &matSceneCamView );

	hr = pEffect->SetFloatArray( "g_vLightPos", (float *)&vViewLightPos, 3 );
	hr = pEffect->SetFloatArray( "g_vLightDir", (float *)&vViewLightDir, 3 );

	// set the shadow map texture to determine shadowed pixels
//	m_ShaderManager.SetTexture( 3, m_pShadowMap );
	pEffect->SetTexture( "g_txShadow", m_pShadowMap );

	pd3dDev->BeginScene();
}


//void CShadowMapManager::EndSceneForShadowReceiver()
void CShadowMapManager::EndSceneDepthMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->EndScene();

	// restore the original render tareget

	if( m_pOriginalDepthSurface )
	{
		pd3dDev->SetDepthStencilSurface( m_pOriginalDepthSurface );
		m_pOriginalDepthSurface->Release();
		m_pOriginalDepthSurface = NULL;
	}

	pd3dDev->SetRenderTarget( 0, m_pOriginalSurface );
	SAFE_RELEASE( m_pOriginalSurface );
}


void CShadowMapManager::BeginScene()
{
	m_SceneRenderTarget.SetRenderTarget();

	DIRECT3D9.GetDevice()->BeginScene();
}


void CShadowMapManager::EndScene()
{
	DIRECT3D9.GetDevice()->EndScene();

	m_SceneRenderTarget.ResetRenderTarget();
}



/*
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	// If the settings dialog is being shown, then
	// render it instead of rendering the app's scene
	if( g_SettingsDlg.IsActive() )
	{
		g_SettingsDlg.OnRender( fElapsedTime );
		return;
	}

	HRESULT hr;

	//
	// Compute the view matrix for the light
	// This changes depending on the light mode
	// (free movement or attached)
	//
	D3DXMATRIXA16 mLightView;
	if( g_bFreeLight )
		mLightView = *g_LCamera.GetViewMatrix();
	else
	{
		// Light attached to car.
		mLightView = g_Obj[2].m_mWorld;
		D3DXVECTOR3 vPos( mLightView._41, mLightView._42, mLightView._43 );  // Offset z by -2 so that it's closer to headlight
		D3DXVECTOR4 vDir = D3DXVECTOR4( 0.0f, 0.0f, -1.0f, 1.0f );  // In object space, car is facing -Z
		mLightView._41 = mLightView._42 = mLightView._43 = 0.0f;  // Remove the translation
		D3DXVec4Transform( &vDir, &vDir, &mLightView );  // Obtain direction in world space
		vDir.w = 0.0f;  // Set w 0 so that the translation part below doesn't come to play
		D3DXVec4Normalize( &vDir, &vDir );
		vPos.x += vDir.x * 4.0f;  // Offset the center by 4 so that it's closer to the headlight
		vPos.y += vDir.y * 4.0f;
		vPos.z += vDir.z * 4.0f;
		vDir.x += vPos.x;  // vDir denotes the look-at point
		vDir.y += vPos.y;
		vDir.z += vPos.z;
		D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
		D3DXMatrixLookAtLH( &mLightView, &vPos, (D3DXVECTOR3*)&vDir, &vUp );
	}

	//
	// Render the shadow map
	//
	LPDIRECT3DSURFACE9 pOldRT = NULL;
	V( pd3dDevice->GetRenderTarget( 0, &pOldRT ) );
	LPDIRECT3DSURFACE9 pShadowSurf;
	if( SUCCEEDED( m_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf ) ) )
	{
		pd3dDevice->SetRenderTarget( 0, pShadowSurf );
		SAFE_RELEASE( pShadowSurf );
	}
	LPDIRECT3DSURFACE9 pOldDS = NULL;
	if( SUCCEEDED( pd3dDevice->GetDepthStencilSurface( &pOldDS ) ) )
		pd3dDevice->SetDepthStencilSurface( m_pDSShadow );

	{
		CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Shadow Map" );
		RenderScene( pd3dDevice, true, fElapsedTime, &mLightView, &m_mShadowProj );
	}

	if( pOldDS )
	{
		pd3dDevice->SetDepthStencilSurface( pOldDS );
		pOldDS->Release();
	}
	pd3dDevice->SetRenderTarget( 0, pOldRT );
	SAFE_RELEASE( pOldRT );

	//
	// Now that we have the shadow map, render the scene.
	//
	const D3DXMATRIX *pmView = g_bCameraPerspective ? g_VCamera.GetViewMatrix() :
													  &mLightView;

	// Initialize required parameter
	V( m_pEffect->SetTexture( "g_txShadow", m_pShadowMap ) );
	// Compute the matrix to transform from view space to
	// light projection space.  This consists of
	// the inverse of view matrix * view matrix of light * light projection matrix
	D3DXMATRIXA16 mViewToLightProj;
	mViewToLightProj = *pmView;
	D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
	D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &mLightView );
	D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &m_mShadowProj );
	V( m_pEffect->SetMatrix( "g_mViewToLightProj", &mViewToLightProj ) );

	{
		CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Scene" );
		RenderScene( pd3dDevice, false, fElapsedTime, pmView, g_VCamera.GetProjMatrix() );
	}
	m_pEffect->SetTexture( "g_txShadow", NULL );
}
*/
