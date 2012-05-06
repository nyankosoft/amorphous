#include "ShadowMapManager.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Graphics/2DPrimitive/2DTexRect.hpp"
#include "Graphics/LightStructs.hpp"
#include "Graphics/HemisphericLight.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/ParamLoader.hpp"

using std::map;
using namespace boost;


const char *sg_pShadowMapDebugParamsFile = ".debug/shadowmap.txt";


//std::string CShadowMapManager::ms_strDefaultShaderFilename = "Shader/ShadowMap.fx";


class CShadowMapLightVisitor : public CConstLightVisitor
{
	CShadowMap *m_pShadowMap;

public:

	CShadowMapLightVisitor( CShadowMap *pShadowMap )
		:
	m_pShadowMap(pShadowMap)
	{}

	void VisitPointLight( const CPointLight& point_light )                   { m_pShadowMap->UpdatePointLight( point_light ); }
	void VisitDirectionalLight( const CDirectionalLight& directional_light ) { m_pShadowMap->UpdateDirectionalLight( directional_light ); }
	void VisitSpotlight( const CSpotlight& spotlight )                       { m_pShadowMap->UpdateSpotlight( spotlight ); }
	void VisitHemisphericPointLight( const CHemisphericPointLight& hs_point_light )                   { m_pShadowMap->UpdatePointLight( hs_point_light ); }
	void VisitHemisphericDirectionalLight( const CHemisphericDirectionalLight& hs_directional_light ) { m_pShadowMap->UpdateDirectionalLight( hs_directional_light ); }
	void VisitHemisphericSpotlight( const CHemisphericSpotlight& hs_spotlight )                       { m_pShadowMap->UpdateSpotlight( hs_spotlight ); }
};


class CShadowMapFactory
{
public:

	shared_ptr<CShadowMap> CreateShadowMap( const CLight& light )
	{
		shared_ptr<CShadowMap> pShadowMap;

		switch( light.GetLightType() )
		{
		case CLight::DIRECTIONAL:
		case CLight::HEMISPHERIC_DIRECTIONAL:
//			pShadowMap.reset( new CDirectionalLightShadowMap() );
//			pShadowMap.reset( new CSpotlightShadowMap() );
			pShadowMap.reset( new COrthoShadowMap() );
			break;

//		case CLight::POINT:
//		case CLight::HEMISPHERIC_POINT:
//			pShadowMap.reset( new CPointLightShadowMap() );
			break;

		case CLight::SPOTLIGHT:
		case CLight::HEMISPHERIC_SPOTLIGHT:
			pShadowMap.reset( new CSpotlightShadowMap() );
			break;

		default:
//			LOG_PRINT_ERROR( " An unsupported type of light was specified. light type id: " + to_string(light.GetLightType()) );
			break;
		}

		return pShadowMap;
	}
};



//===================================================================
// CShadowMapManager
//===================================================================

CShadowMapManager::CShadowMapManager()
:
//m_ShadowMapShaderFilename("Shader/VarianceShadowMap.fx"),
m_ShadowMapShaderFilename("Shader/SimpleShadowMap.fx"),
m_pSceneRenderTarget( CTextureRenderTarget::Create() ),
m_DisplayShadowMapTexturesForDebugging(false),
m_IDCounter(0),
m_ShadowMapSize( 1024 )
{
	SetDefault();

	SetSceneShadowTextureSize( GetScreenWidth(), GetScreenHeight() );

//	m_iTextureWidth  = 1;
//	m_iTextureHeight = 1;
}


CShadowMapManager::~CShadowMapManager()
{
	ReleaseGraphicsResources();
}


void CShadowMapManager::SetSceneShadowTextureSize( int texture_width, int texture_height )
{
	m_iTextureWidth  = texture_width;
	m_iTextureHeight = texture_height;
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

	m_pSceneRenderTarget->InitScreenSizeRenderTarget();

	if( shader_loaded && c )//&& SUCCEEDED(hr) )
		return true;
	else
		return false;
}


void CShadowMapManager::SetDefault()
{
	m_ShadowMapSize = 512;

	// set the default pose of the scene camera
	m_SceneCamera.SetPosition( Vector3( 0.0f, 1.0f, -5.0f ) );
	m_SceneCamera.SetOrientation( Matrix33Identity() );

	UPDATE_PARAM( sg_pShadowMapDebugParamsFile, "debug_shadowmap", CShadowMap::ms_DebugShadowMap );

	if( CShadowMap::ms_DebugShadowMap )
		UPDATE_PARAM( sg_pShadowMapDebugParamsFile, "shadowmap_size", m_ShadowMapSize );
}


void CShadowMapManager::ReleaseTextures()
{
	for( int i=0; i<NUM_MAX_SCENE_SHADOW_TEXTURES; i++ )
	{
		if( m_apShadowTexture[i] )
		{
			m_apShadowTexture[i]->ReleaseTextures();
		}
	}
}


void CShadowMapManager::SetSceneRenderer( shared_ptr<CShadowMapSceneRenderer> pSceneRenderer )
{
	m_pSceneRenderer = pSceneRenderer;

	// set the scene renderer to all the shadowmap objects
	for( IDtoShadowMap::iterator itr = m_mapIDtoShadowMap.begin();
		itr != m_mapIDtoShadowMap.end();
		itr++ )
	{
		itr->second->SetSceneRenderer( m_pSceneRenderer.get() );
	}
}

/// returns a valid shadowmap id on success (0 <= id)
/// returns -1 on failure
/// \param [in] light must be either directional or point light
/// TODO: support spotlight
std::map< int, boost::shared_ptr<CShadowMap> >::iterator CShadowMapManager::CreateShadowMap( U32 id, const CLight& light )
{
	if( light.GetLightType() != CLight::DIRECTIONAL
	 && light.GetLightType() != CLight::HEMISPHERIC_DIRECTIONAL
	 && light.GetLightType() != CLight::POINT
	 && light.GetLightType() != CLight::HEMISPHERIC_POINT )
	{
		return m_mapIDtoShadowMap.end();
	}

	CShadowMapFactory factory;

	// Create a shadow map object
	// - shadow map texture is not created in this call.
	shared_ptr<CShadowMap> pShadowMap = factory.CreateShadowMap( light );
	if( !pShadowMap )
		return m_mapIDtoShadowMap.end();

	m_mapIDtoShadowMap[id] = pShadowMap;

	pShadowMap->SetShadowMapSize( m_ShadowMapSize );

	pShadowMap->SetShadowMapTextureFormat( GetShadowMapTextureFormat() );

//	pShadowMap->UseLightPosInWorldSpace( m_UseLightPosInWorldSpace );

	pShadowMap->SetShader( m_Shader );

	bool init = pShadowMap->CreateShadowMapTextures();
	if( !init )
		return m_mapIDtoShadowMap.end();

	pShadowMap->SetSceneRenderer( m_pSceneRenderer.get() );

	pShadowMap->SetSceneCamera( &m_SceneCamera );

	return m_mapIDtoShadowMap.find( id );
}


Result::Name CShadowMapManager::UpdateLightForShadow( U32 id, const CLight& light )
{
	map< int, shared_ptr<CShadowMap> >::iterator itrShadowMap
		= m_mapIDtoShadowMap.find((int)id);

	static uint s_NumMaxShadowMaps = 1;

	uint num_max_shadowmaps = 1;
	if( itrShadowMap == m_mapIDtoShadowMap.end() )
	{
		if( s_NumMaxShadowMaps <= (uint)m_mapIDtoShadowMap.size() )
			return Result::UNKNOWN_ERROR; // Cannot create any more shadowmaps

		// The shadow map for this light does not exist
		// - create a new shadow map.
		itrShadowMap = CreateShadowMap( id, light );

		if( itrShadowMap == m_mapIDtoShadowMap.end() )
			return Result::UNKNOWN_ERROR;
	}

	// update light properties
	CShadowMapLightVisitor v( itrShadowMap->second.get() );
	light.Accept( v );

	return Result::SUCCESS;
}


void CShadowMapManager::RemoveShadowForLight( int shadowmap_id )
{
	IDtoShadowMap::iterator itr = m_mapIDtoShadowMap.find( shadowmap_id );

	if( itr == m_mapIDtoShadowMap.end() )
		return; // not found

	m_mapIDtoShadowMap.erase( itr );
}


void CShadowMapManager::UpdateLight( int shadowmap_id, const CLight& light )
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
		m_pCurrentShadowMap = itr->second;

		// Render a shadow map to a shadow map texture.
		// In the case of an ortho shadowmap, i.e. simplest type,
		// distances from a light to each sampling point are saved to a shadow map texture
		itr->second->RenderSceneToShadowMap( camera );

		// variance shadow maps are blurred in this call
		PostProcessShadowMap( *(itr->second.get()) );

		m_pCurrentShadowMap = shared_ptr<CShadowMap>();
	}

	EndSceneShadowMap();
}


void CShadowMapManager::RenderShadowReceivers( CCamera& camera )
{
	if( m_mapIDtoShadowMap.empty() )
		return;

	if( !m_apShadowTexture[0] )
		return;

	BeginSceneDepthMap();

	// render the first shadow texture

	IDtoShadowMap::iterator itr = m_mapIDtoShadowMap.begin();

	m_pCurrentShadowMap = itr->second;

//	m_apShadowTexture[0]->SetBackgroundColor( SFloatRGBAColor::White().GetARGB32() );
	m_apShadowTexture[0]->SetBackgroundColor( SFloatRGBAColor::Magenta() );
	m_apShadowTexture[0]->SetRenderTarget();

	itr->second->RenderShadowReceivers( camera );
	itr++;

	m_pCurrentShadowMap = shared_ptr<CShadowMap>();

	m_apShadowTexture[0]->ResetRenderTarget();

	// render the shadow textures for the rest of shadow maps
/*
	int shadow_tex_index = 1, prev_index = 0;
	for( ;
		 itr != m_mapIDtoShadowMap.end();
		 itr++, prev_index = shadow_tex_index, shadow_tex_index = (shadow_tex_index + 1) % 2 )
	{
		m_apShadowTexture[shadow_tex_index].SetRenderTarget();

		pEffect->SetTexture( "PrevShadowTexture", m_apShadowTexture[prev_index].GetRenderTargetTexture() );

//		itr->second->RenderShadowReceivers( camera );

		m_apShadowTexture[shadow_tex_index].ResetRenderTarget();

		// combine the shadow textures
	}
*/
	// Render to scene depth texture
	// BeginScene() and EndScene() pair is called inside

//	m_pSceneRenderer->RenderShadowReceivers();

	EndSceneDepthMap();
}


CShaderTechniqueHandle CShadowMapManager::ShaderTechniqueForShadowCaster( CVertexBlendType::Name vertex_blend_type )
{
/*	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();
//	HRESULT hr = pEffect->SetTechnique( "ShadowMap" );
	HRESULT hr = pEffect->SetTechnique( "OrthoShadowMap" );
*/
//	if( m_pCurrentShadowMap )
//		m_Shader.GetShaderManager()->SetTechnique( m_pCurrentShadowMap->ShadowMapTechnique() );

	static CShaderTechniqueHandle s_Null;
	if( m_pCurrentShadowMap )
		return m_pCurrentShadowMap->ShadowMapTechnique( vertex_blend_type );
	else
		return s_Null;
}


CShaderTechniqueHandle CShadowMapManager::ShaderTechniqueForShadowReceiver( CVertexBlendType::Name vertex_blend_type )
{
/*	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();
//	HRESULT hr = pEffect->SetTechnique( "SceneShadowMap" );
	HRESULT hr = pEffect->SetTechnique( "OrthoSceneShadowMap" );
*/
//	if( m_pCurrentShadowMap )
//		m_Shader.GetShaderManager()->SetTechnique( m_pCurrentShadowMap->DepthTestTechnique() );

	static CShaderTechniqueHandle s_Null;
	if( m_pCurrentShadowMap )
		return m_pCurrentShadowMap->DepthTestTechnique( vertex_blend_type );
	else
		return s_Null;
}


CShaderTechniqueHandle CShadowMapManager::ShaderTechniqueForNonShadowedCasters( CVertexBlendType::Name vertex_blend_type )
{
	CShaderTechniqueHandle tech;
	tech.SetTechniqueName( "SceneShadowMap_NonShadowed" );

	return tech;
}


bool CShadowMapManager::CreateSceneShadowMapTextures()
{
	for( int i=0; i<2; i++ )
	{
		m_apShadowTexture[i] = CTextureRenderTarget::Create();
		m_apShadowTexture[i]->Init( m_iTextureWidth, m_iTextureHeight );
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


void CShadowMapManager::RenderSceneWithShadow( int sx, int sy, int ex, int ey )
{
	if( !m_apShadowTexture[0] )
		return;

	GraphicsDevice().Disable( RenderStateType::DEPTH_TEST );

	if( CShadowMap::ms_DebugShadowMap )
	{
		static int display_shadowmap_tex = 0;
		UPDATE_PARAM( sg_pShadowMapDebugParamsFile, "display_shadowmap_textures", display_shadowmap_tex );
		m_DisplayShadowMapTexturesForDebugging = (display_shadowmap_tex==1) ? true : false;
	}
	else
		m_DisplayShadowMapTexturesForDebugging = false;

	if( m_DisplayShadowMapTexturesForDebugging )
	{
//		DisplayTextureRenderTargetsForDebugging();
		int w = (ex - sx + 1) / 2;
		int h = (ey - sy + 1) / 2;

		if( 0 < m_mapIDtoShadowMap.size() )
			m_mapIDtoShadowMap.begin()->second->RenderShadowMapTexture( sx,     sy, sx + w - 1,   sy + h - 1 );

//		RenderShadowMapTexture(      sx,     sy, sx + w - 1,   sy + h - 1 );

		RenderSceneShadowMapTexture( sx + w, sy, sx + w*2 - 1, sy + h - 1 );

		if( m_pSceneRenderTarget )
		{
			C2DRect screen_rect( sx, sy + h, sx + w - 1, sy + h*2 - 1, 0xFFFFFFFF );
			screen_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
			screen_rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture() );

//			CShaderManager *pShaderMgr = 
//			PrimitiveRenderer().RenderRect( *pShaderMgr, scree_rect );
			C2DTexRect tex_rect( sx + w, sy + h, sx + w*2 - 1, sy + h*2 - 1, 0xFFFFFFFF );
			tex_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
			tex_rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture().GetTexture(), m_apShadowTexture[0]->GetRenderTargetTexture().GetTexture() );
//			tex_rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture(), m_pShadowedView );
		}
	}
	else
	{
		// Render the fullscreen rect with 2 textures
		// - The original scene texture
		// - The shadow overlay texture
		C2DTexRect tex_rect( sx, sy, ex, ey, 0xFFFFFFFF );
		tex_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		tex_rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture().GetTexture(), m_apShadowTexture[0]->GetRenderTargetTexture().GetTexture() );
//		tex_rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture(), m_pShadowedView );
	}

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );


/*
	// render using HLSL

	LPD3DXEFFECT pEffect = m_ShaderManager.GetEffect();
	HRESULT hr = pEffect->SetTechnique( "SceneWithShadow" );

	m_ShaderManager.SetTexture( 0, m_pSceneRenderTarget->GetRenderTargetTexture() );
	m_ShaderManager.SetTexture( 1, m_pShadowedView );

	pEffect->CommitChanges();

	screen_rect.draw();
/*

/*
	pd3dDev->SetTexture( 0, m_pSceneRenderTarget->GetRenderTargetTexture() );
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
	rect.Draw( m_apShadowTexture[0]->GetRenderTargetTexture() );
}


void CShadowMapManager::RenderSceneWithoutShadow( int sx, int sy, int ex, int ey )
{
	C2DRect rect( sx, sy, ex, ey, 0xFFFFFFFF );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	rect.Draw( m_pSceneRenderTarget->GetRenderTargetTexture() );
//	rect.Draw();
}


CTextureHandle CShadowMapManager::GetSceneShadowTexture()
{
	return m_apShadowTexture[0] ? m_apShadowTexture[0]->GetRenderTargetTexture() : CTextureHandle();
}


//void CShadowMapManager::BeginSceneForShadowCaster()
void CShadowMapManager::BeginSceneShadowMap()
{
//	ShaderManagerHub.PushViewAndProjectionMatrices( m_LightCamera );

	// save the current settings
//	hr = pd3dDev->GetRenderTarget( 0, &m_pOriginalSurface );
//	hr = pd3dDev->GetDepthStencilSurface( &m_pOriginalDepthSurface );
}


//void CShadowMapManager::EndSceneForShadowCaster()
void CShadowMapManager::EndSceneShadowMap()
{
/*
//	ShaderManagerHub.PopViewAndProjectionMatrices();

	// restore the original render tareget

	if( m_pOriginalDepthSurface )
	{
		pd3dDev->SetDepthStencilSurface( m_pOriginalDepthSurface );
		SAFE_RELEASE( m_pOriginalDepthSurface );
	}

	pd3dDev->SetRenderTarget( 0, m_pOriginalSurface );
	SAFE_RELEASE( m_pOriginalSurface );*/
}


//void CShadowMapManager::BeginSceneForShadowReceiver()
void CShadowMapManager::BeginSceneDepthMap()
{
	Matrix44 view, proj;
	m_SceneCamera.GetCameraMatrix( view );
	m_SceneCamera.GetProjectionMatrix( proj );

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	pShaderMgr->SetWorldViewProjectionTransform( Matrix44Identity(), view, proj );

//	ShaderManagerHub.PushViewAndProjectionMatrices( m_SceneCamera );

	// set the shadow map texture to determine shadowed pixels
//	m_ShaderManager.SetTexture( 3, m_pShadowMap );
//	pEffect->SetTexture( "g_txShadow", m_pShadowMap );


	// done in BeginSceneShadowReceivers() of shadowmap classes
/*	HRESULT hr = S_OK;
	if( 0 < m_mapIDtoShadowMap.size() )
	{
		hr = pEffect->SetTexture( "g_txShadow", (m_mapIDtoShadowMap.begin())->second->GetShadowMapTexture() );
	}*/


	pShaderMgr->SetParam( "g_ShadowMapSize", m_ShadowMapSize );

	if( CShadowMap::ms_DebugShadowMap )
	{
		static float dist_tolerance = 0.05f;
		UPDATE_PARAM( sg_pShadowMapDebugParamsFile, "shadowmap_dist_tolerance", dist_tolerance );
		pShaderMgr->SetParam( "g_fShadowMapDistTolerance", dist_tolerance );
	}

//	pd3dDev->BeginScene();
}


//void CShadowMapManager::EndSceneForShadowReceiver()
void CShadowMapManager::EndSceneDepthMap()
{
//	pd3dDev->EndScene();

//	ShaderManagerHub.PopViewAndProjectionMatrices();

	// restore the original render tareget
/*
	if( m_pOriginalDepthSurface )
	{
		pd3dDev->SetDepthStencilSurface( m_pOriginalDepthSurface );
		m_pOriginalDepthSurface->Release();
		m_pOriginalDepthSurface = NULL;
	}

	pd3dDev->SetRenderTarget( 0, m_pOriginalSurface );
	SAFE_RELEASE( m_pOriginalSurface );*/
}


void CShadowMapManager::BeginScene()
{
	m_pSceneRenderTarget->SetRenderTarget();

//	DIRECT3D9.GetDevice()->BeginScene();
}


void CShadowMapManager::EndScene()
{
//	DIRECT3D9.GetDevice()->EndScene();

	m_pSceneRenderTarget->ResetRenderTarget();
}


void CShadowMapManager::SaveSceneTextureToFile( const std::string& filename )
{
	m_pSceneRenderTarget->OutputImageFile( filename.c_str() );
}
