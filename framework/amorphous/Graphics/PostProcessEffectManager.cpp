#include "PostProcessEffectManager.hpp"
#include "PostProcessEffect.hpp"
#include "Graphics/TextureRenderTarget.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/Embedded/EmbeddedPostProcessEffectShader.hpp"
#include "Support/ParamLoader.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using namespace boost;


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
// PostProcessFilterShader
//========================================================================

PostProcessFilterShader::PostProcessFilterShader()
//:
//m_nRenderTarget(0)
{
	memset( m_bWrite,        0, sizeof(m_bWrite) );
//	memset( m_ahParam,       0, sizeof(m_ahParam) );
//	memset( m_anParamSize,   0, sizeof(m_anParamSize) );
//	memset( m_avParamDef,    0, sizeof(m_avParamDef) );
}


Result::Name PostProcessFilterShader::Init( const ShaderResourceDesc& shader_desc )
{
	m_ShaderDesc = shader_desc;
	bool loaded = m_Shader.Load( m_ShaderDesc );

	return loaded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name PostProcessFilterShader::Init( const std::string& filename )
{
	m_ShaderDesc.ResourcePath = filename;
	bool loaded = m_Shader.Load( m_ShaderDesc );

	return loaded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


shared_ptr<PostProcessFilterShader> FilterShaderContainer::AddShader( const ShaderResourceDesc& shader_desc )
{
	m_vecpShader.push_back( shared_ptr<PostProcessFilterShader>() );
	m_vecpShader.back().reset( new PostProcessFilterShader );

	Result::Name res = m_vecpShader.back()->Init( shader_desc );

	if( res != Result::SUCCESS )
		LOG_PRINT_WARNING( "Failed to initialize a shader." );

	return m_vecpShader.back();
}


Result::Name FilterShaderContainer::AddShader( const std::string& filepath )
{
	ShaderResourceDesc shader_desc;
	shader_desc.ResourcePath = filepath;
	shared_ptr<PostProcessFilterShader> pShader = AddShader( shader_desc );
	return pShader ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


boost::shared_ptr<PostProcessFilterShader> FilterShaderContainer::AddPostProcessEffectShader( const std::string& effect_name )
{
	ShaderResourceDesc shader_desc;
	shader_desc.pShaderGenerator.reset( new PostProcessEffectFilterShaderGenerator( effect_name.c_str() ) );
	return AddShader( shader_desc );
}


boost::shared_ptr<PostProcessFilterShader> FilterShaderContainer::GetFilterShader( const std::string& name )
{
	if( 0 < m_vecpShader.size() )
		return m_vecpShader[0];
	else
		return shared_ptr<PostProcessFilterShader>();
}


boost::shared_ptr<PostProcessFilterShader> FilterShaderContainer::GetShader( const std::string& technique_name )
{
	if( 0 < m_vecpShader.size() )
		return m_vecpShader[0];
	else
		return shared_ptr<PostProcessFilterShader>();

/*	for( i=0; i<; i++ )
	{
		if(  )
		{
			return m_vecpShader[i];
		}
	}

	return shared_ptr<PostProcessFilterShader>();*/
}


//================================================================================
// PostProcessEffectManager
//================================================================================

PostProcessEffectManager::PostProcessEffectManager()
:
m_EnabledEffectFlags(0),
m_IsRedering(false),
m_bUseMultiSampleFloat16(false),
m_DisplayAdaptedLuminance(false)
{
	m_pTextureCache.reset( new RenderTargetTextureCache );
	m_pTextureCache->m_pSelf = m_pTextureCache;
}


PostProcessEffectManager::~PostProcessEffectManager()
{
}


Result::Name PostProcessEffectManager::Init( const std::string& base_shader_directory_path )
{
	using namespace boost::filesystem;

//	ShaderResourceDesc shader_desc;
//	if( base_shader_directory_path.length() == 0 )
//	{
//		// Create the shader which contains all the effects.
//		shader_desc.pShaderGenerator.reset( new PostProcessEffectFilterShaderGenerator );
//	}
//	else
//	{
//		path shader_filepath = path(base_shader_directory_path) / path("HDRPostProcessor.fx");
//		shader_desc.ResourcePath = shader_filepath.string();
//	}
	
	// load shader
//	auto pShader = m_FilterShaderContainer.AddShader( shader_desc );

//	const D3DSURFACE_DESC* pBackBufferDesc = GetD3D9BackBufferSurfaceDesc();

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
//	TextureFormat::Format orig_scene_buffer_format = TextureFormat::A16R16G16B16F;
	TextureFormat::Format orig_scene_buffer_format = TextureFormat::A8R8G8B8;

	SRectangular bb = GetBackBufferWidthAndHeight();

	uint vp_width = 0, vp_height = 0;
	GraphicsDevice().GetViewportSize( vp_width, vp_height );
	SRectangular vp( (int)vp_width, (int)vp_height );

	m_pOrigSceneHolder.reset( new RenderTargetTextureHolder() );
	m_pOrigSceneHolder->m_Desc.Width  = bb.width;
	m_pOrigSceneHolder->m_Desc.Height = bb.height;
	m_pOrigSceneHolder->m_Desc.Format = orig_scene_buffer_format;
	m_pOrigSceneHolder->m_Desc.MipLevels = 1;
	m_pOrigSceneHolder->m_Desc.UsageFlags = UsageFlag::RENDER_TARGET;
//	bool loaded = m_pOrigSceneHolder->m_Texture.Load( m_pOrigSceneHolder->m_Desc );
//	if( !loaded )
//		LOG_PRINT_ERROR( " Failed to create a render target texture for original scene." );

	m_pOrigSceneHolder->m_pTextureRenderTarget = TextureRenderTarget::Create();
	bool loaded = m_pOrigSceneHolder->m_pTextureRenderTarget->Init( m_pOrigSceneHolder->m_Desc );

	// Create the HDR scene texture
//	hr = pd3dDevice->CreateTexture( pBackBufferDesc->Width, pBackBufferDesc->Height,
//									  1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
//									  D3DPOOL_DEFAULT, &m_SceneRenderTarget, NULL );

	m_pOriginalSceneFilter.reset( new OriginalSceneFilter( m_pOrigSceneHolder ) );

	m_pTextureCache->m_pOrigSceneHolder = m_pOrigSceneHolder;

	int display_adapted_luminance = 0;
	LoadParamFromFile( ".debug/PostProcessEffectManager.txt", "display_adapted_luminance", display_adapted_luminance );
	m_DisplayAdaptedLuminance = (display_adapted_luminance == 0) ? false : true;

	return loaded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


// Save the current render target to m_pSurfLDR & m_pTextureCache->m_pOrigRenderTarget.
// Set a render target on which client renders the scene.
Result::Name PostProcessEffectManager::BeginRender()
{
	if( !m_pOrigSceneHolder )
		return Result::UNKNOWN_ERROR;

	m_IsRedering = true;

	if( m_EnabledEffectFlags == 0 )
		return Result::UNKNOWN_ERROR;

	m_pOrigSceneHolder->m_pTextureRenderTarget->SetRenderTarget();

	// Clear the viewport
	GraphicsDevice().SetClearColor( SFloatRGBAColor(0,0,0,0) );
	GraphicsDevice().SetClearDepth( 1.0f );
	GraphicsDevice().Clear( BufferMask::COLOR | BufferMask::DEPTH );

	return Result::SUCCESS;
}


Result::Name PostProcessEffectManager::EndRender()
{
	return Result::SUCCESS;
}

// Run post process effect filters.
// Renders the final post process effects to the original render target
// saved in PostProcessEffectManager::BeginRender().
// The order in which the filters are applied is predetermined as follows.
// 1. HRD lighting
// 2. Monochrome
// 3. Fullscreen blur
Result::Name PostProcessEffectManager::RenderPostProcessEffects()
{
	if( m_EnabledEffectFlags == 0 )
	{
		m_IsRedering = false;
		return Result::SUCCESS;
	}

	shared_ptr<PostProcessEffectFilter> pLastFilter;
	if( m_pHDRLightingFilter && (m_EnabledEffectFlags & PostProcessEffect::TF_HDR_LIGHTING) )
	{
		m_pHDRLightingFilter->ClearNextFilters();

		m_pFilter = m_pHDRLightingFilter;

		pLastFilter = m_pHDRLightingFilter;
	}

	if( m_pMonochromeColorFilter && (m_EnabledEffectFlags & PostProcessEffect::TF_MONOCHROME_COLOR) )
	{
		m_pMonochromeColorFilter->ClearNextFilters();
		if( pLastFilter )
			pLastFilter->AddNextFilter( m_pMonochromeColorFilter );
		else
			m_pFilter = m_pMonochromeColorFilter;

		pLastFilter = m_pMonochromeColorFilter;
	}

	if( m_pFullScreenBlurFilter && (m_EnabledEffectFlags & PostProcessEffect::TF_BLUR) )
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

	if( PostProcessEffectFilter::ms_SaveFilterResultsAtThisFrame == 1 )
	{
		boost::filesystem::create_directories( "debug/post-process_effect" );
//		m_pOrigSceneHolder->m_Texture.SaveTextureToImageFile( "debug/post-process_effect/orig_scene.png" );
	}

	// send the base scene to the first filter and start the post process effects
	// The last filter renders the result to m_pTextureCache->m_pOrigRenderTarget(=m_pSurfLDR), the original render target
	m_pFilter->RenderBase( *(m_pOriginalSceneFilter.get()) );

	m_pFilter = shared_ptr<PostProcessEffectFilter>();

	// reset the debug setting
	PostProcessEffectFilter::ms_SaveFilterResultsAtThisFrame = 0;

	// If using floating point multi sampling, stretchrect to the rendertarget
/*	if( m_bUseMultiSampleFloat16 )
	{
		V( pd3dDevice->StretchRect( m_pFloatMSRT, NULL, pSurfHDR, NULL, D3DTEXF_NONE ) );
		V( pd3dDevice->SetRenderTarget( 0, pSurfHDR ) );
		V( pd3dDevice->SetDepthStencilSurface( pSurfDS ) );
	}*/

//	SAFE_RELEASE( m_pSurfLDR );
//	SAFE_RELEASE( m_pSurfDS );

	// done by SAFE_RELEASE( m_pSurfLDR )
//	m_pOrigSceneHolder->ReleaseSurface();

//	SAFE_RELEASE( m_pTextureCache->m_pOrigRenderTarget );

//	m_pTextureCache->m_pOrigSceneHolder->m_pTextureRenderTarget->ResetRenderTarget();

	m_IsRedering = false;

	if( m_DisplayAdaptedLuminance && m_pHDRLightingFilter )
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


void PostProcessEffectManager::SetFirstFilterParams()
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


Result::Name PostProcessEffectManager::InitHDRLightingFilter()
{
	if( !m_pTextureCache )
		return Result::UNKNOWN_ERROR;

	m_pHDRLightingFilter.reset( new HDRLightingFilter );
	Result::Name res = m_pHDRLightingFilter->Init( *m_pTextureCache, m_FilterShaderContainer );
	if( res != Result::SUCCESS )
	{
		LOG_PRINT_ERROR(( "Failed to initialize HDR filter." ));
	}
	
	return res;
}


Result::Name PostProcessEffectManager::InitBlurFilter()
{
	if( !m_pTextureCache )
		return Result::UNKNOWN_ERROR;

	m_pFullScreenBlurFilter.reset( new FullScreenBlurFilter );
	return m_pFullScreenBlurFilter->Init( *m_pTextureCache, m_FilterShaderContainer );
}


Result::Name PostProcessEffectManager::InitMonochromeColorFilter()
{
	if( !m_pTextureCache )
		return Result::UNKNOWN_ERROR;

	m_pMonochromeColorFilter.reset( new MonochromeColorFilter );

	Result::Name res = m_pMonochromeColorFilter->Init( *m_pTextureCache, m_FilterShaderContainer );

	return res;
}


Result::Name PostProcessEffectManager::EnableHDRLighting( bool enable )
{
	if( m_IsRedering )
		return Result::UNKNOWN_ERROR;

	if( enable )
	{
		m_EnabledEffectFlags |= PostProcessEffect::TF_HDR_LIGHTING;
		if( !m_pHDRLightingFilter )
			return InitHDRLightingFilter();
	}
	else
		m_EnabledEffectFlags &= ~(PostProcessEffect::TF_HDR_LIGHTING);

	return Result::SUCCESS;
}


Result::Name PostProcessEffectManager::EnableBlur( bool enable )
{
	if( m_IsRedering )
		return Result::UNKNOWN_ERROR;

	if( enable )
	{
		m_EnabledEffectFlags |= PostProcessEffect::TF_BLUR;
		if( !m_pFullScreenBlurFilter )
			return InitBlurFilter();
	}
	else
		m_EnabledEffectFlags &= ~(PostProcessEffect::TF_BLUR);

	return Result::SUCCESS;
}


Result::Name PostProcessEffectManager::EnableEffect( U32 effect_flags )
{
	if( m_IsRedering )
		return Result::UNKNOWN_ERROR;

	if( true )
	{
		m_EnabledEffectFlags |= effect_flags;

		Result::Name res = Result::UNKNOWN_ERROR;

		if( effect_flags & PostProcessEffect::TF_HDR_LIGHTING )
		{
			if( !m_pHDRLightingFilter )
				res = InitHDRLightingFilter();
		}

		if( effect_flags & PostProcessEffect::TF_MONOCHROME_COLOR )
		{
			if( !m_pMonochromeColorFilter )
				res = InitMonochromeColorFilter();
		}

		if( effect_flags & PostProcessEffect::TF_BLUR )
		{
			if( !m_pFullScreenBlurFilter )
				res = InitBlurFilter();
		}
	}

	return Result::SUCCESS;
}


Result::Name PostProcessEffectManager::DisableEffect( U32 effect_flags )
{
	m_EnabledEffectFlags &= ~(effect_flags);

	return Result::SUCCESS;
}


void PostProcessEffectManager::SetHDRLightingParams( U32 hdr_lighting_param_flags, const HDRLightingParams& params )
{
	if( !m_pHDRLightingFilter )
		return;

	if( hdr_lighting_param_flags & HDRLightingParams::KEY_VALUE )
		m_pHDRLightingFilter->SetToneMappingKeyValue( params.key_value );

	if( hdr_lighting_param_flags & HDRLightingParams::TONE_MAPPING )
		m_pHDRLightingFilter->EnableToneMapping( params.tone_mapping );

	if( hdr_lighting_param_flags & HDRLightingParams::LUMINANCE_ADAPTATION_RATE )
		m_pHDRLightingFilter->SetLuminanceAdaptationRate( params.luminance_adaptation_rate );
}


void PostProcessEffectManager::SetBlurStrength( float fBlurStrength )
{
	if( !m_pFullScreenBlurFilter )
		return;

	m_pFullScreenBlurFilter->SetBlurStrength( fBlurStrength );
}


void PostProcessEffectManager::SetMonochromeColorOffset( const SFloatRGBColor& color )
{
}


void PostProcessEffectManager::ReleaseGraphicsResources()
{
}


void PostProcessEffectManager::LoadGraphicsResources( const GraphicsParameters& rParam )
{
}


void PostProcessEffectManager::DisplayAdaptedLuminance()
{
	// TODO: implement the feature to render 2D rects with programmable shader
	shared_ptr<RenderTargetTextureHolder> pRTTexHolder
		= m_pHDRLightingFilter->GetAdaptationCalcFilter()->GetAdaptedLuminanceTexture();

	shared_ptr<PostProcessFilterShader> pFilterShader
		= m_FilterShaderContainer.GetShader( "AdaptedLuminanceDisplay" );

	if( pRTTexHolder && pFilterShader )
	{
		const int tex_index = 3;

		ShaderManager *pShader
			= pFilterShader->GetShader().GetShaderManager();

		if( !pShader )
			return;

//		DIRECT3D9.GetDevice()->SetTexture( tex_index, pRTTexHolder->GetTexture().GetTexture() );
		pShader->SetTexture( tex_index, pRTTexHolder->GetTexture() );
/*
		static TextureHandle s_tex;
		s_tex.Load( "debug/test_images/Meerkat_256.jpg" );
		pShader->SetTexture( tex_index, s_tex );
*/
		ShaderTechniqueHandle t;
		t.SetTechniqueName( "AdaptedLuminanceDisplay" );
		pShader->SetTechnique( t );

		pShader->GetEffect()->CommitChanges();

		C2DRect rect( RectLTWH( 20, 20, 40, 40 ) );
		rect.SetColor( SFloatRGBAColor::White() );
		rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

		Get2DPrimitiveRenderer().RenderRect( *pShader, rect );
//		rect.Draw( s_tex );
	}
}


/*
	boost::shared_ptr<PostProcessEffectManager> m_pPostProcessEffectManager;
	m_pPostProcessEffectManager = shared_ptr<PostProcessEffectManager>( new PostProcessEffectManager );

	m_pPostProcessEffectManager->Init();
*/


} // namespace amorphous
