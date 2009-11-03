#include "PostProcessEffectManager.hpp"
#include "PostProcessEffect.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Direct3D/2DPrimitive/2DPrimitiveRenderer_D3D.hpp"
#include <boost/filesystem.hpp>


using namespace std;
using namespace boost;


#define V(x) { hr = x; if( FAILED(hr) ) { LOG_PRINT_ERROR( string(#x) + " failed." ); } }
#define V_RETURN(x) { hr = x; if(FAILED(hr)) return hr; }


class CMultiSampleType
{
public:
	enum Name
	{
		NONE = 0,
		NON_MASKABLE = 1,
/*		2_SAMPLES = 2,
		3_SAMPLES = 3,
		4_SAMPLES = 4,
		5_SAMPLES = 5,
		6_SAMPLES = 6,
		7_SAMPLES = 7,
		8_SAMPLES = 8,
		9_SAMPLES = 9,
		10_SAMPLES = 10,
		11_SAMPLES = 11,
		12_SAMPLES = 12,
		13_SAMPLES = 13,
		14_SAMPLES = 14,
		15_SAMPLES = 15,
		16_SAMPLES = 16,*/
	};
};


class CRenderTargetDesc
{
public:
	CMultiSampleType::Name MultiSampleType;
	int MultiSampleQuality;

	CRenderTargetDesc()
		:
	MultiSampleType(CMultiSampleType::NONE),
	MultiSampleQuality(0)
	{}
};



//========================================================================
// CPostProcessFilterShader
//========================================================================

CPostProcessFilterShader::CPostProcessFilterShader()
:
m_hTPostProcess(NULL)//,
//m_nRenderTarget(0)
{
	ZeroMemory( m_hTexSource,    sizeof(m_hTexSource) );
	ZeroMemory( m_hTexScene,     sizeof(m_hTexScene) );
	ZeroMemory( m_bWrite,        sizeof(m_bWrite) );
	ZeroMemory( m_ahParam,       sizeof(m_ahParam) );
	ZeroMemory( m_awszParamName, sizeof(m_awszParamName) );
	ZeroMemory( m_awszParamDesc, sizeof(m_awszParamDesc) );
	ZeroMemory( m_anParamSize,   sizeof(m_anParamSize) );
	ZeroMemory( m_avParamDef,    sizeof(m_avParamDef) );
}


Result::Name CPostProcessFilterShader::Init( const std::string& filename )
{
	m_ShaderFilename = filename;
	bool loaded = m_Shader.Load( m_ShaderFilename );

	return loaded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name CFilterShaderContainer::AddShader( const std::string& filepath )
{
	m_vecpShader.resize( 1 );
	m_vecpShader[0] = shared_ptr<CPostProcessFilterShader>( new CPostProcessFilterShader );

	return m_vecpShader[0]->Init( filepath );
}


boost::shared_ptr<CPostProcessFilterShader> CFilterShaderContainer::GetFilterShader( const std::string& name )
{
	if( 0 < m_vecpShader.size() )
		return m_vecpShader[0];
	else
		return shared_ptr<CPostProcessFilterShader>();
}


boost::shared_ptr<CPostProcessFilterShader> CFilterShaderContainer::GetShader( const std::string& technique_name )
{
	if( 0 < m_vecpShader.size() )
		return m_vecpShader[0];
	else
		return shared_ptr<CPostProcessFilterShader>();

/*	for( i=0; i<; i++ )
	{
		if(  )
		{
			return m_vecpShader[i];
		}
	}

	return shared_ptr<CPostProcessFilterShader>();*/
}

/*
boost::shared_ptr<CPostProcessFilterShader> GetShaderFromFilename( const std::string& filename )
{
	for( i=0; i<; i++ )
	{
		const string& filepath = m_vecpShader[i]->GetFilepath();
		if( filepath.find( filename )  )
		{
			return ==  ;
		}
	}

	return shared_ptr<CPostProcessFilterShader>();
}
*/


//================================================================================
// CPostProcessEffectManager
//================================================================================

CPostProcessEffectManager::CPostProcessEffectManager()
:
m_EnabledEffectFlags(0),
m_IsRedering(false),
m_bUseMultiSampleFloat16(false),
m_pSurfLDR(NULL),
m_pSurfDS(NULL),
m_pFloatMSRT(NULL),
m_pFloatMSDS(NULL)
{
	m_pTextureCache = shared_ptr<CRenderTargetTextureCache>( new CRenderTargetTextureCache );
	m_pTextureCache->m_pSelf = m_pTextureCache;
}


CPostProcessEffectManager::~CPostProcessEffectManager()
{
}


Result::Name CPostProcessEffectManager::Init( const std::string& base_shader_directory_path )
{
	using namespace boost::filesystem;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    const D3DSURFACE_DESC* pBackBufferDesc = GetD3D9BackBufferSurfaceDesc();

	path shader_filepath = path(base_shader_directory_path) / path("HDRPostProcessor.fx");

	// load shader
	m_FilterShaderContainer.AddShader( shader_filepath.string() );
/*
	m_FilterShaderContainer.m_vecpShader.resize( 1 );
	m_FilterShaderContainer.m_vecpShader[0]
	= shared_ptr<CPostProcessFilterShader>( new CPostProcessFilterShader );

	m_FilterShaderContainer.m_vecpShader[0]->Init( "HDRPostProcessor.fx" );
*/

/*
    // Create the Multi-Sample floating point render target
    D3DFORMAT dfmt = D3DFMT_UNKNOWN;
    if( m_bSupportsD16 )
        dfmt = D3DFMT_D16;
    else if( g_bSupportsD32 )
        dfmt = D3DFMT_D32;
    else if( g_bSupportsD24X8 )
        dfmt = D3DFMT_D24X8;
    else
        m_bUseMultiSampleFloat16 = false;

    if( m_bUseMultiSampleFloat16 )
    {
        hr = pd3dDevice->CreateRenderTarget( pBackBufferDesc->Width, pBackBufferDesc->Height,
                                               D3DFMT_A16B16G16R16F,
                                               g_MaxMultiSampleType, g_dwMultiSampleQuality,
                                               FALSE, &g_pFloatMSRT, NULL );
        if( FAILED( hr ) )
            m_bUseMultiSampleFloat16 = false;
        else
        {
            hr = pd3dDevice->CreateDepthStencilSurface( pBackBufferDesc->Width, pBackBufferDesc->Height,
                                                          dfmt,
                                                          g_MaxMultiSampleType, g_dwMultiSampleQuality,
                                                          TRUE, &g_pFloatMSDS, NULL );
            if( FAILED( hr ) )
            {
                m_bUseMultiSampleFloat16 = false;
                SAFE_RELEASE( g_pFloatMSRT );
            }
        }
    }
*/
	TextureFormat::Format orig_scene_buffer_format = TextureFormat::A16R16G16B16F;

	SPlane2 bb = GetBackBufferWidthAndHeight();
	m_pOrigSceneHolder = shared_ptr<CRenderTargetTextureHolder>( new CRenderTargetTextureHolder() );
	m_pOrigSceneHolder->m_Desc.Width  = bb.width;
	m_pOrigSceneHolder->m_Desc.Height = bb.height;
	m_pOrigSceneHolder->m_Desc.Format = orig_scene_buffer_format;
	m_pOrigSceneHolder->m_Desc.UsageFlags = UsageFlag::RENDER_TARGET;
	bool loaded = m_pOrigSceneHolder->m_Texture.Load( m_pOrigSceneHolder->m_Desc );
	if( !loaded )
		LOG_PRINT_ERROR( " Failed to create a render target texture for original scene." );

	// Create the HDR scene texture
//	hr = pd3dDevice->CreateTexture( pBackBufferDesc->Width, pBackBufferDesc->Height,
//									  1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
//									  D3DPOOL_DEFAULT, &m_SceneRenderTarget, NULL );

	m_pOriginalSceneFilter = shared_ptr<COriginalSceneFilter>( new COriginalSceneFilter( m_pOrigSceneHolder ) );

	return loaded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


// Save the current render target.
// Set a render target on which client renders the scene.
Result::Name CPostProcessEffectManager::BeginRender()
{
	m_IsRedering = true;

	if( m_EnabledEffectFlags == 0 )
		return Result::UNKNOWN_ERROR;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	PDIRECT3DSURFACE9 pSurfHDR = NULL; // High dynamic range surface to store 
	// intermediate floating point color values

	HRESULT hr = S_OK;

	// Store the old render target
	V( pd3dDevice->GetRenderTarget( 0, &m_pSurfLDR ) );
	V( pd3dDevice->GetDepthStencilSurface( &m_pSurfDS ) );

	// Save the copy of the original render target
	// The final filter renders to this surface.
	m_pTextureCache->m_pOrigRenderTarget = m_pSurfLDR;

	// Setup HDR render target
	LPDIRECT3DTEXTURE9 pOrigSceneTexture = m_pOrigSceneHolder->m_Texture.GetTexture();
	V( pOrigSceneTexture->GetSurfaceLevel( 0, &m_pOrigSceneHolder->m_pTexSurf ) );
//	V( m_SceneRenderTarget.GetTexture()->GetSurfaceLevel( 0, &pSurfHDR ) );
	if( m_bUseMultiSampleFloat16 )
	{
		V( pd3dDevice->SetRenderTarget( 0, m_pFloatMSRT ) );
		V( pd3dDevice->SetDepthStencilSurface( m_pFloatMSDS ) );
	}
	else
	{
		V( pd3dDevice->SetRenderTarget( 0, m_pOrigSceneHolder->m_pTexSurf ) );
//		V( pd3dDevice->SetRenderTarget( 0, pSurfHDR ) );
	}

	// Clear the viewport
	V( pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA( 0, 0, 0, 0 ), 1.0f, 0L ) );

	// Render the scene
//	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
//	{
		// Render the HDR Scene
/*		{
			CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Scene" );
			RenderScene();
		}
*/

	return Result::SUCCESS;
}


Result::Name CPostProcessEffectManager::EndRender()
{
/*	if( m_EnabledEffectFlags == 0 )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	V( pd3dDevice->SetRenderTarget( 0, m_pSurfLDR ) );*/

	return Result::SUCCESS;
}

// Run post process effect filters.
// Renders the final post process effects to the original render target
// saved in CPostProcessEffectManager::BeginRender().
Result::Name CPostProcessEffectManager::RenderPostProcessEffects()
{
	if( m_EnabledEffectFlags == 0 )
	{
		m_IsRedering = false;
		return Result::SUCCESS;
	}

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	shared_ptr<CFilter> pLastFilter;
	if( m_pHDRLightingFilter && (m_EnabledEffectFlags & CPostProcessEffect::TF_HDR_LIGHTING) )
	{
		m_pHDRLightingFilter->ClearNextFilters();

		m_pFilter = m_pHDRLightingFilter;

		pLastFilter = m_pHDRLightingFilter;
	}

	if( m_pMonochromeColorFilter && (m_EnabledEffectFlags & CPostProcessEffect::TF_MONOCHROME_COLOR) )
	{
		m_pMonochromeColorFilter->ClearNextFilters();
		if( pLastFilter )
			pLastFilter->AddNextFilter( m_pMonochromeColorFilter );
		else
			m_pFilter = m_pMonochromeColorFilter;

		pLastFilter = m_pMonochromeColorFilter;
	}

	if( m_pFullScreenBlurFilter && (m_EnabledEffectFlags & CPostProcessEffect::TF_BLUR) )
	{
		m_pFullScreenBlurFilter->ClearNextFilters();
		if( pLastFilter )
			pLastFilter->AddNextFilter( m_pFullScreenBlurFilter );
		else
			m_pFilter = m_pFullScreenBlurFilter;

		pLastFilter = m_pFullScreenBlurFilter;
	}

	if( !m_pFilter )
	{
		m_IsRedering = false;
		return Result::SUCCESS;
	}

	// increment the lock count of the original scene because it is decremented by the first filter
	m_pOrigSceneHolder->IncrementLockCount();

	// send the base scene to the first filter and start the post process effects
	// The last filter renders the result to m_pTextureCache->m_pOrigRenderTarget(=m_pSurfLDR), the original render target
	m_pFilter->RenderBase( *(m_pOriginalSceneFilter.get()) );

	m_pFilter = shared_ptr<CFilter>();

	// reset the debug setting
	CFilter::ms_SaveFilterResultsAtThisFrame = 0;

	// If using floating point multi sampling, stretchrect to the rendertarget
/*	if( m_bUseMultiSampleFloat16 )
	{
		V( pd3dDevice->StretchRect( m_pFloatMSRT, NULL, pSurfHDR, NULL, D3DTEXF_NONE ) );
		V( pd3dDevice->SetRenderTarget( 0, pSurfHDR ) );
		V( pd3dDevice->SetDepthStencilSurface( pSurfDS ) );
	}*/

	SAFE_RELEASE( m_pSurfLDR );
	SAFE_RELEASE( m_pSurfDS );

	// done by SAFE_RELEASE( m_pSurfLDR )
//	m_pOrigSceneHolder->ReleaseSurface();

	SAFE_RELEASE( m_pTextureCache->m_pOrigRenderTarget );

	m_IsRedering = false;

	bool debug_hdr_luminance_calc = true;
	if( debug_hdr_luminance_calc && m_pHDRLightingFilter )
	{
		DisplayAdaptedLuminance();
	}

	return Result::SUCCESS;

/*
		// Create a scaled copy of the scene
		Scene_To_SceneScaled();

		// Setup tone mapping technique
		if( g_bToneMap )
			MeasureLuminance();

		// If FrameMove has been called, the user's adaptation level has also changed
		// and should be updated
		if( g_bAdaptationInvalid )
		{
			// Clear the update flag
			g_bAdaptationInvalid = false;

			// Calculate the current luminance adaptation level
			CalculateAdaptation();
		}

		// Now that luminance information has been gathered, the scene can be bright-pass filtered
		// to remove everything except bright lights and reflections.
		SceneScaled_To_BrightPass();

		// Blur the bright-pass filtered image to create the source texture for the star effect
		BrightPass_To_StarSource();

		// Scale-down the source texture for the star effect to create the source texture
		// for the bloom effect
		StarSource_To_BloomSource();

		// Render post-process lighting effects
		{
			CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Bloom" );
			RenderBloom();
		}
		{
			CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Star" );
			RenderStar();
		}
*/
}


void CPostProcessEffectManager::SetFirstFilterParams()
{
/*	if( !m_pFirstFilter )
		return;

	DWORD dwCropWidth  = m_dwCropWidth;
	DWORD dwCropHeight = m_dwCropHeight;

	// Place the rectangle in the center of the back buffer surface
	RECT rectSrc;
	rectSrc.left = ( pBackBufferDesc->Width  - dwCropWidth )  / 2;
	rectSrc.top  = ( pBackBufferDesc->Height - dwCropHeight ) / 2;
	rectSrc.right = rectSrc.left + dwCropWidth;
	rectSrc.bottom = rectSrc.top + dwCropHeight;

	m_pFirstFilter->SetSourceRect( rectSrc );

	m_pFirstFilter->SetPrevSceneTexture(  );*/
}


Result::Name CPostProcessEffectManager::InitHDRLightingFilter()
{
	m_pHDRLightingFilter = shared_ptr<CHDRLightingFilter>( new CHDRLightingFilter );
	Result::Name res = m_pHDRLightingFilter->Init( *(m_pTextureCache.get()), m_FilterShaderContainer );
	if( res != Result::SUCCESS )
	{
		LOG_PRINT_ERROR(( "Failed to initialize HDR filter." ));
	}
	
	return res;
}


Result::Name CPostProcessEffectManager::InitBlurFilter()
{
	m_pFullScreenBlurFilter = shared_ptr<CFullScreenBlurFilter>( new CFullScreenBlurFilter );
	return m_pFullScreenBlurFilter->Init( *(m_pTextureCache.get()), m_FilterShaderContainer );
}


Result::Name CPostProcessEffectManager::InitMonochromeColorFilter()
{
	m_pMonochromeColorFilter = shared_ptr<CMonochromeColorFilter>( new CMonochromeColorFilter );
	Result::Name res;
	res = m_pMonochromeColorFilter->Init( *(m_pTextureCache.get()), m_FilterShaderContainer );

	const SPlane2 cbb = GetCropWidthAndHeight();
	m_pMonochromeColorFilter->SetRenderTargetSize( cbb.width, cbb.height );

	CTextureResourceDesc tex_desc;
	tex_desc.Width  = cbb.width;
	tex_desc.Height = cbb.height;
	tex_desc.Format = TextureFormat::A8R8G8B8;
	tex_desc.UsageFlags = UsageFlag::RENDER_TARGET;
	int num = m_pTextureCache->GetNumTextures( tex_desc );
	for( int i=num; i<2; i++ )
		m_pTextureCache->AddTexture( tex_desc );

	return res;
}


Result::Name CPostProcessEffectManager::EnableHDRLighting( bool enable )
{
	if( m_IsRedering )
		return Result::UNKNOWN_ERROR;

	if( enable )
	{
		m_EnabledEffectFlags |= CPostProcessEffect::TF_HDR_LIGHTING;
		if( !m_pHDRLightingFilter )
			return InitHDRLightingFilter();
	}
	else
		m_EnabledEffectFlags &= ~(CPostProcessEffect::TF_HDR_LIGHTING);

	return Result::SUCCESS;
}


Result::Name CPostProcessEffectManager::EnableBlur( bool enable )
{
	if( m_IsRedering )
		return Result::UNKNOWN_ERROR;

	if( enable )
	{
		m_EnabledEffectFlags |= CPostProcessEffect::TF_BLUR;
		if( !m_pFullScreenBlurFilter )
			return InitBlurFilter();
	}
	else
		m_EnabledEffectFlags &= ~(CPostProcessEffect::TF_BLUR);

	return Result::SUCCESS;
}


Result::Name CPostProcessEffectManager::EnableEffect( U32 effect_flags )
{
	if( m_IsRedering )
		return Result::UNKNOWN_ERROR;

	if( true )
	{
		m_EnabledEffectFlags |= effect_flags;

		Result::Name res = Result::UNKNOWN_ERROR;

		if( effect_flags & CPostProcessEffect::TF_HDR_LIGHTING )
		{
			if( !m_pHDRLightingFilter )
				res = InitHDRLightingFilter();
		}

		if( effect_flags & CPostProcessEffect::TF_MONOCHROME_COLOR )
		{
			if( !m_pMonochromeColorFilter )
				res = InitMonochromeColorFilter();
		}

		if( effect_flags & CPostProcessEffect::TF_BLUR )
		{
			if( !m_pFullScreenBlurFilter )
				res = InitBlurFilter();
		}
	}

	return Result::SUCCESS;
}


Result::Name CPostProcessEffectManager::DisableEffect( U32 effect_flags )
{
	m_EnabledEffectFlags &= ~(effect_flags);

	return Result::SUCCESS;
}


void CPostProcessEffectManager::SetHDRLightingParams( U32 hdr_lighting_param_flags, const CHDRLightingParams& params )
{
	if( !m_pHDRLightingFilter )
		return;

	if( hdr_lighting_param_flags & CHDRLightingParams::KEY_VALUE )
		m_pHDRLightingFilter->SetToneMappingKeyValue( params.key_value );

	if( hdr_lighting_param_flags & CHDRLightingParams::TONE_MAPPING )
		m_pHDRLightingFilter->EnableToneMapping( params.tone_mapping );

	if( hdr_lighting_param_flags & CHDRLightingParams::LUMINANCE_ADAPTATION_RATE )
		m_pHDRLightingFilter->SetLuminanceAdaptationRate( params.luminance_adaptation_rate );
}


void CPostProcessEffectManager::SetBlurStrength( float fBlurStrength )
{
	if( !m_pFullScreenBlurFilter )
		return;

	m_pFullScreenBlurFilter->SetBlurStrength( fBlurStrength );
}


void CPostProcessEffectManager::SetMonochromeColorOffset( const SFloatRGBColor& color )
{
}


void CPostProcessEffectManager::ReleaseGraphicsResources()
{
}


void CPostProcessEffectManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


void CPostProcessEffectManager::DisplayAdaptedLuminance()
{
	// TODO: implement the feature to render 2D rects with programmable shader
	shared_ptr<CRenderTargetTextureHolder> pRTTexHolder
		= m_pHDRLightingFilter->GetAdaptationCalcFilter()->GetAdaptedLuminanceTexture();

	shared_ptr<CPostProcessFilterShader> pFilterShader
		= m_FilterShaderContainer.GetShader( "AdaptedLuminanceDisplay" );

	if( pRTTexHolder && pFilterShader )
	{
		const int tex_index = 3;

		CShaderManager *pShader
			= pFilterShader->GetShader().GetShaderManager();
//		pShader->SetTexture( tex_index, pRTTexHolder->m_Texture );
		DIRECT3D9.GetDevice()->SetTexture( tex_index, pRTTexHolder->m_Texture.GetTexture() );
/*
		static CTextureHandle s_tex;
		s_tex.Load( "debug/test_images/Meerkat_256.jpg" );
		pShader->SetTexture( tex_index, s_tex );
*/
		CShaderTechniqueHandle t;
		t.SetTechniqueName( "AdaptedLuminanceDisplay" );
		pShader->SetTechnique( t );
		pShader->GetEffect()->CommitChanges();

		C2DRect rect( RectLTWH( 20, 20, 40, 40 ) );
		rect.SetColor( SFloatRGBAColor::White() );
		rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

		PrimitiveRenderer().RenderRect( *pShader, rect );
//		rect.Draw( s_tex );
	}
}


/*
	boost::shared_ptr<CPostProcessEffectManager> m_pPostProcessEffectManager;
	m_pPostProcessEffectManager = shared_ptr<CPostProcessEffectManager>( new CPostProcessEffectManager );

	m_pPostProcessEffectManager->Init();
*/


/*
// Name of the postprocess .fx files
LPCWSTR g_aszFxFile[] =
{
    L"PP_ColorMonochrome.fx",
    L"PP_ColorInverse.fx",
    L"PP_ColorGBlurH.fx",
    L"PP_ColorGBlurV.fx",
    L"PP_ColorBloomH.fx",
    L"PP_ColorBloomV.fx",
    L"PP_ColorBrightPass.fx",
    L"PP_ColorToneMap.fx",
    L"PP_ColorEdgeDetect.fx",
    L"PP_ColorDownFilter4.fx",
    L"PP_ColorUpFilter4.fx",
    L"PP_ColorCombine.fx",
    L"PP_ColorCombine4.fx",
    L"PP_NormalEdgeDetect.fx",
    L"PP_DofCombine.fx",
    L"PP_NormalMap.fx",
    L"PP_PositionMap.fx",
};


// Description of each postprocess supported
LPCWSTR g_aszPpDesc[] =
{
    L"[Color] Monochrome",
    L"[Color] Inversion",
    L"[Color] Gaussian Blur Horizontal",
    L"[Color] Gaussian Blur Vertical",
    L"[Color] Bloom Horizontal",
    L"[Color] Bloom Vertical",
    L"[Color] Bright Pass",
    L"[Color] Tone Mapping",
    L"[Color] Edge Detection",
    L"[Color] Down Filter 4x",
    L"[Color] Up Filter 4x",
    L"[Color] Combine",
    L"[Color] Combine 4x",
    L"[Normal] Edge Detection",
    L"DOF Combine",
    L"Normal Map",
    L"Position Map",
};*/