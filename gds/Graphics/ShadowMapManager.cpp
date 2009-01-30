#include "ShadowMapManager.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Graphics/2DPrimitive/2DTexRect.hpp"
#include "Graphics/LightStructs.hpp"
#include "Support/Log/DefaultLog.hpp"

using namespace std;
using namespace boost;


//std::string CShadowMapManager::ms_strDefaultShaderFilename = "Shader/ShadowMap.fx";


static const bool gs_Debug = true;


class CShadowMapLightVisitor : public CLightVisitor
{
	CShadowMap *m_pShadowMap;

public:

	CShadowMapLightVisitor( CShadowMap *pShadowMap )
		:
	m_pShadowMap(pShadowMap)
	{}

	void VisitPointLight( CPointLight& point_light ) { m_pShadowMap->UpdateLight( point_light ); }
	void VisitDirectionalLight( CDirectionalLight& directional_light ) { m_pShadowMap->UpdateLight( directional_light ); }
};


class CShadowMapFactory
{
public:

	shared_ptr<CShadowMap> CreateShadowMap( const CLight& light )
	{
		switch( light.GetLightType() )
		{
		case CLight::DIRECTIONAL: return shared_ptr<CShadowMap>( new CDirectionalLightShadowMap() );
		case CLight::POINT:       return shared_ptr<CShadowMap>( new CPointLightShadowMap() );
//		case CLight::SPOT:        return shared_ptr<CShadowMap>( new CSpotLightShadowMap() );
		default:
			LOG_PRINT_ERROR( " An unsupported type of light was specified. light type id: " + to_string(light.GetLightType()) );
			return shared_ptr<CShadowMap>();
		}
	}
};


CShadowMapManager::CShadowMapManager()
:
m_ShadowMapShaderFilename("Shader/ShadowMap.fx"),
m_DisplayShadowMapTexturesForDebugging(gs_Debug),
m_IDCounter(0)
{
	SetDefault();

	m_iTextureWidth  = GetScreenWidth();
	m_iTextureHeight = GetScreenHeight();

//	m_iTextureWidth  = 1;
//	m_iTextureHeight = 1;
}


CShadowMapManager::CShadowMapManager( int texture_width, int texture_height )
:
m_ShadowMapShaderFilename("Shader/ShadowMap.fx"),
m_DisplayShadowMapTexturesForDebugging(gs_Debug),
m_IDCounter(0)
{
	SetDefault();

	m_iTextureWidth  = texture_width;
	m_iTextureHeight = texture_height;

}


CShadowMapManager::~CShadowMapManager()
{
	ReleaseGraphicsResources();
}


bool CShadowMapManager::Init()
{
//	m_ShaderManager.Release();
	m_Shader.Release();

	ReleaseTextures();

	bool shader_loaded = m_Shader.Load( m_ShadowMapShaderFilename );

	if( !shader_loaded )
	{
		LOG_PRINT_ERROR( "Cannot load shader: " + m_ShadowMapShaderFilename );
		return false;
	}

//	HRESULT hr = CreateShadowMapTextures();
//	if( FAILED(hr) )
//		LOG_PRINT_ERROR( "Failed to create shadow map texture" );

	bool c = CreateSceneShadowMapTextures();

	m_SceneRenderTarget.InitScreenSizeRenderTarget();

	if( shader_loaded && c )//&& SUCCEEDED(hr) )
		return true;
	else
		return false;
}


void CShadowMapManager::SetDefault()
{
	m_ShadowMapSize = 512;

	// set default light direction & position
/*	Vector3 vLightDir =  Vector3(-0.56568f, -0.70711f, -0.42426f);
	Vector3 vLightPos =  Vector3( 5.0f, 10.0f,  6.0f );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir( vLightDir ) );
	m_LightCamera.SetPosition( vLightPos );
//	m_LightCamera.SetNearClip( 0.1f );
//	m_LightCamera.SetFarClip( 100.0f );
*/
	// set the default pose of the scene camera
	m_SceneCamera.SetPosition( Vector3( 0.0f, 1.0f, -5.0f ) );
	m_SceneCamera.SetOrientation( Matrix33Identity() );
}


void CShadowMapManager::ReleaseTextures()
{
	for( int i=0; i<2; i++ )
		m_aShadowTexture[i].ReleaseTextures();

//	SAFE_RELEASE( m_pShadowedView );
//	SAFE_RELEASE( m_pDSShadowedView );
}

void CShadowMapManager::SetSceneRenderer( CShadowMapSceneRenderer *pSceneRenderer )
{
	m_pSceneRenderer = pSceneRenderer;

	for( IDtoShadowMap::iterator itr = m_mapIDtoShadowMap.begin();
		itr != m_mapIDtoShadowMap.end();
		itr++ )
	{
		itr->second->SetSceneRenderer( m_pSceneRenderer );
	}
}

/// returns a valid shadowmap id on success (0 <= id)
/// returns -1 on failure
/// \param [in] light must be either directional or point light
/// TODO: support spotlight
int CShadowMapManager::AddShadowForLight( CLight& light )
{
	if( light.GetLightType() != CLight::DIRECTIONAL
	 && light.GetLightType() != CLight::POINT )
	{
		return -1;
	}

	CShadowMapFactory factory;

	shared_ptr<CShadowMap> pShadowMap = factory.CreateShadowMap( light );

	int shadowmap_id = m_IDCounter++;

	m_mapIDtoShadowMap[shadowmap_id] = pShadowMap;

	pShadowMap->SetShader( m_Shader );

	bool init = pShadowMap->CreateShadowMapTextures();

	pShadowMap->SetSceneRenderer( m_pSceneRenderer );

	pShadowMap->SetSceneCamera( &m_SceneCamera );

	// update light properties
	CShadowMapLightVisitor v( pShadowMap.get() );
	light.Accept( v );

	return shadowmap_id;
}


void CShadowMapManager::RemoveShadowForLight( int shadowmap_id )
{
	IDtoShadowMap::iterator itr = m_mapIDtoShadowMap.find( shadowmap_id );

	if( itr == m_mapIDtoShadowMap.end() )
		return; // not found

	m_mapIDtoShadowMap.erase( itr );
}


void CShadowMapManager::UpdateLight( int shadowmap_id, CLight& light )
{
	IDtoShadowMap::iterator itr = m_mapIDtoShadowMap.find( shadowmap_id );

	if( itr == m_mapIDtoShadowMap.end() )
		return; // not found

	CShadowMapLightVisitor v( itr->second.get() );
	light.Accept( v );
}


void CShadowMapManager::RenderShadowCasters( CCamera& camera )
{
	if( m_mapIDtoShadowMap.empty() )
		return;

	BeginSceneShadowMap();

	IDtoShadowMap::iterator itr;
	for( itr = m_mapIDtoShadowMap.begin();
		 itr != m_mapIDtoShadowMap.end();
		 itr++ )
	{
		itr->second->RenderSceneToShadowMap( camera );
	}

	EndSceneShadowMap();
}


void CShadowMapManager::RenderShadowReceivers( CCamera& camera )
{
	if( m_mapIDtoShadowMap.empty() )
		return;

	BeginSceneDepthMap();

	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();

	// render the first shadow texture

	IDtoShadowMap::iterator itr = m_mapIDtoShadowMap.begin();

	m_aShadowTexture[0].SetRenderTarget();

	itr->second->RenderShadowReceivers( camera );
	itr++;

	m_aShadowTexture[0].ResetRenderTarget();

	// render the shadow textures for the rest of shadow maps
/*
	int shadow_tex_index = 1, prev_index = 0;
	for( ;
		 itr != m_mapIDtoShadowMap.end();
		 itr++, prev_index = shadow_tex_index, shadow_tex_index = (shadow_tex_index + 1) % 2 )
	{
		m_aShadowTexture[shadow_tex_index].SetRenderTarget();

		pEffect->SetTexture( "PrevShadowTexture", m_aShadowTexture[prev_index].GetRenderTargetTexture() );

//		itr->second->CreateShadowTexture( camera );

		m_aShadowTexture[shadow_tex_index].ResetRenderTarget();

		// combine the shadow textures
	}
*/
	// Render to scene depth texture
	// BeginScene() and EndScene() pair is called inside

//	m_pSceneRenderer->RenderShadowReceivers();

	EndSceneDepthMap();
}


void CShadowMapManager::SetShaderTechniqueForShadowCaster()
{
	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();
	HRESULT hr = pEffect->SetTechnique( "ShadowMap" );
}


void CShadowMapManager::SetShaderTechniqueForShadowReceiver()
{
	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();
	HRESULT hr = pEffect->SetTechnique( "SceneShadowMap" );
}


bool CShadowMapManager::CreateSceneShadowMapTextures()
{
//	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
//	HRESULT hr;

	for( int i=0; i<2; i++ )
	{
		m_aShadowTexture[i].Init( m_iTextureWidth, m_iTextureHeight );
	}

/*	// create a texture on which the scene is rendered
	hr = pd3dDev->CreateTexture( m_iTextureWidth, m_iTextureHeight, 
								 1,
								 D3DUSAGE_RENDERTARGET,
								 D3DFMT_A8R8G8B8, //D3DFMT_X8R8G8B8, 
								 D3DPOOL_DEFAULT,
								 &m_pShadowedView,
								 NULL );

	if( FAILED(hr) )
		return false;

//	hr = m_pRenderTargetTexture->GetSurfaceLevel(0, &m_pRenderTargetSurface);

	// get the current depth buffer format
	hr = pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );
	D3DSURFACE_DESC surface_desc;
	m_pOriginalDepthSurface->GetDesc( &surface_desc );

	hr = pd3dDev->CreateDepthStencilSurface( m_iTextureWidth,
											 m_iTextureHeight,
											 surface_desc.Format, //D3DFMT_D16,
											 D3DMULTISAMPLE_NONE,
											 0, TRUE, &m_pDSShadowedView, NULL );
	if( FAILED(hr) ) return false;
*/
	return true;
}


void CShadowMapManager::ReleaseGraphicsResources()
{
//	m_ShaderManager.Release();
	ReleaseTextures();
}


void CShadowMapManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	Init();
}


void CShadowMapManager::RenderSceneWithShadow( int sx, int sy, int ex, int ey )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	m_DisplayShadowMapTexturesForDebugging = true;

	if( m_DisplayShadowMapTexturesForDebugging )
	{
		int w = (ex - sx + 1) / 2;
		int h = (ey - sy + 1) / 2;

		if( 0 < m_mapIDtoShadowMap.size() )
			m_mapIDtoShadowMap.begin()->second->RenderShadowMapTexture( sx,     sy, sx + w - 1,   sy + h - 1 );

//		RenderShadowMapTexture(      sx,     sy, sx + w - 1,   sy + h - 1 );

		RenderSceneShadowMapTexture( sx + w, sy, sx + w*2 - 1, sy + h - 1 );

		C2DRect screen_rect( sx, sy + h, sx + w - 1, sy + h*2 - 1, 0xFFFFFFFF );
		screen_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		screen_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture() );

		C2DTexRect tex_rect( sx + w, sy + h, sx + w*2 - 1, sy + h*2 - 1, 0xFFFFFFFF );
		tex_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		tex_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture(), m_aShadowTexture[0].GetRenderTargetTexture() );
//		tex_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture(), m_pShadowedView );
	}
	else
	{
		C2DTexRect tex_rect( sx, sy, ex, ey, 0xFFFFFFFF );
		tex_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		tex_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture(), m_aShadowTexture[0].GetRenderTargetTexture() );
//		tex_rect.Draw( m_SceneRenderTarget.GetRenderTargetTexture(), m_pShadowedView );
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

}


void CShadowMapManager::RenderSceneWithShadow()
{
	RenderSceneWithShadow( 0, 0, GetScreenWidth() - 1, GetScreenHeight() - 1 );
}

/*
void CShadowMapManager::RenderShadowMapTexture( int sx, int sy, int ex, int ey )
{
	C2DRect rect( sx, sy, ex, ey, 0xFFFFFFFF );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	rect.Draw( m_pShadowMap );
}
*/

void CShadowMapManager::RenderSceneShadowMapTexture( int sx, int sy, int ex, int ey )
{
	C2DRect rect( sx, sy, ex, ey, 0xFFFFFFFF );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	rect.Draw( m_aShadowTexture[0].GetRenderTargetTexture() );
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

	// set the shadow map shader
	// meshes that renderes themselves as shadow casters use the techniques
	// in this shadow map shader
	CShader::Get()->SetShaderManager( m_Shader.GetShaderManager() );

//	ShaderManagerHub.PushViewAndProjectionMatrices( m_LightCamera );

	// save the current settings
	hr = pd3dDev->GetRenderTarget( 0, &m_pOriginalSurface );
	hr = pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );
}


//void CShadowMapManager::EndSceneForShadowCaster()
void CShadowMapManager::EndSceneShadowMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	ShaderManagerHub.PopViewAndProjectionMatrices();

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
//	HRESULT hr;

	// set shadow map texture as a render target
/*
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
*/
	// set the shadow map shader
	// meshes that renderes themselves as shadow casters use the techniques
	// in this shadow map shader
	CShader::Get()->SetShaderManager( m_Shader.GetShaderManager() );

	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();
	D3DXMATRIX matWorld, matView, matProj;

	D3DXMatrixIdentity( &matWorld );
	m_SceneCamera.GetCameraMatrix( matView );
	m_SceneCamera.GetProjectionMatrix( matProj );

	m_Shader.GetShaderManager()->SetWorldViewProjectionTransform( matWorld, matView, matProj );

//	ShaderManagerHub.PushViewAndProjectionMatrices( m_SceneCamera );

	// set the shadow map texture to determine shadowed pixels
//	m_ShaderManager.SetTexture( 3, m_pShadowMap );
//	pEffect->SetTexture( "g_txShadow", m_pShadowMap );

	pEffect->SetInt( "g_ShadowMapSize", m_ShadowMapSize );

//	pd3dDev->BeginScene();
}


//void CShadowMapManager::EndSceneForShadowReceiver()
void CShadowMapManager::EndSceneDepthMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	pd3dDev->EndScene();

//	ShaderManagerHub.PopViewAndProjectionMatrices();

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

//	DIRECT3D9.GetDevice()->BeginScene();
}


void CShadowMapManager::EndScene()
{
//	DIRECT3D9.GetDevice()->EndScene();

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
