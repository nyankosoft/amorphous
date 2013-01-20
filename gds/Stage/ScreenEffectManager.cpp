#include "ScreenEffectManager.hpp"
#include <algorithm>
#include "3DMath/MathMisc.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/PostProcessEffectManager.hpp"
#include "Graphics/SimpleMotionBlur.hpp"
#include "Graphics/LensFlare.hpp"
#include "Graphics/TextureGenerators/NoiseTextureGenerators.hpp"

#include "Graphics/RenderTask.hpp"
#include "Graphics/RenderTaskProcessor.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"


namespace amorphous
{

//using namespace std;
using namespace boost;


class CSimpleMotionBlurRenderTask : public CRenderTask
{
	/// borrowed reference
	CSimpleMotionBlur *m_pMotionBlurMgr;

public:

	CSimpleMotionBlurRenderTask( CSimpleMotionBlur *pMotionBlurMgr )
		:
	m_pMotionBlurMgr(pMotionBlurMgr)
	{
	}

	void Render()
	{
		// restore the prev render target
		m_pMotionBlurMgr->End();

		// draw two rects, one with prev scene texture and the other with current scene texture
		// - use alpha blend prev to blur the scene
		m_pMotionBlurMgr->Render();
	}
};



class CPostProcessPerformRenderTask : public CRenderTask
{
	/// borrowed reference
	CPostProcessEffectManager *m_pPPMgr;

public:

	CPostProcessPerformRenderTask( CPostProcessEffectManager *pPPMgr )
		:
	m_pPPMgr(pPPMgr)
	{
		m_TypeFlags = DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE;
	}

	~CPostProcessPerformRenderTask() {}

	void Render()
	{
		if( !m_pPPMgr )
			return;

//		HRESULT hr;

		// perform post-processes on the scene
		// includes BeginScene() & EndScene() calls pair
//		hr = m_pPPMgr->PerformPostProcess();

//		hr = m_pPPMgr->ResetTextureRenderTarget();
	}
};


class CPostProcessRenderTask : public CRenderTask
{
	/// borrowed reference
	CPostProcessEffectManager *m_pPPMgr;

public:

	CPostProcessRenderTask( CPostProcessEffectManager *pPPMgr )
		:
	m_pPPMgr(pPPMgr)
	{
	}

	~CPostProcessRenderTask() {}

	void Render()
	{
		if( !m_pPPMgr )
			return;

		// render the result of the post-processed image
		// to the restored render target
		// render target is restored in this call
//		HRESULT hr = m_pPPMgr->DrawSceneWithPostProcessEffects();
		HRESULT hr = m_pPPMgr->RenderPostProcessEffects();
	}
};


//string CScreenEffectManager::ms_DefaultShaderFilename = "Shader\\Default.fx";


bool CScreenEffectManager::CExtraTextureEffect::LoadTexture()
{
	return m_Texture.Load( m_Desc );
}


CScreenEffectManager::CScreenEffectManager()
:
m_EffectFlag(0),
m_pTargetSceneRenderer(NULL)
{
	ClearBlurEffect();

	m_iTexRenderTargetEnabled = 0;

	m_EffectFlag |= ScreenEffect::PostProcessEffects;

//	m_EffectFlag |= ScreenEffect::Glare;
//	SetGlareLuminanceThreshold( 0.09f );

	m_DefaultShaderTechnique.SetTechniqueName( "Default" );

	m_pLensFlare.reset( new CLensFlare );
}


CScreenEffectManager::~CScreenEffectManager()
{
	ReleaseGraphicsResources();
}


bool CScreenEffectManager::Init()
{

	UpdateScreenSize();
/*
	m_TexRenderTarget.Init( 512, 512 );
//	m_TexRenderTarget.Init( 256, 256 );
//	m_TexRenderTarget.Init( 16, 16 );
	m_TexRenderTarget.SetBackgroundColor( 0xFF000000 );
//	m_TexRenderTarget.SetBackgroundColor( 0x00000000 );
*/

	// set up textured-rectangles
	// rectangle sizes have been alreay set in UpdateScreenSize()

	m_vecExtraTexEffect.resize( 1 );
	m_vecExtraTexEffect[0].Rect.SetTextureUV( TEXCOORD2(0.0f, 0.0f), TEXCOORD2(32.0f, 32.0f) );
//	m_vecExtraTexEffect[0].SetTextureFilename( "Texture\\stripe.dds" );
	shared_ptr<CStripeTextureGenerator> pTexLoader( new CStripeTextureGenerator );
	pTexLoader->m_Color0.SetRGBA( 0.0f, 0.0f, 0.0f, 0.0f );
	pTexLoader->m_Color1.SetRGBA( 0.0f, 0.0f, 0.0f, 0.5f );
	pTexLoader->m_StripeWidth = 4;
	m_vecExtraTexEffect[0].m_Desc.pLoader = pTexLoader;
	m_vecExtraTexEffect[0].LoadTexture();

//	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
//	HRESULT hr;

	// retrieve the back buffer size
//	D3DSURFACE_DESC back_buffer_desc;
//	IDirect3DSurface9 *pBackBuffer;
//	pd3dDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
//	pBackBuffer->GetDesc( &back_buffer_desc );

	return true;
}


Result::Name CScreenEffectManager::InitPostProcessEffect()
{
	m_pPPEffectManager.reset( new CPostProcessEffectManager );
	Result::Name res = m_pPPEffectManager->Init( "Shader/PostProcessEffect" );

	return res;
}


void CScreenEffectManager::ReleasePostProcessEffect()
{
	m_pPPEffectManager.reset();
}


Result::Name CScreenEffectManager::InitMotionBlurEffect()
{
	m_pSimpleMotionBlur.reset( new CSimpleMotionBlur );

	m_pSimpleMotionBlur->InitForScreenSize();
	m_pSimpleMotionBlur->SetBlurWeight( 0.85f );
//	m_pSimpleMotionBlur->SetBlurWeight( 1.00f );

	return Result::SUCCESS;
}


void CScreenEffectManager::ReleaseMotionBlurEffect()
{
	m_pSimpleMotionBlur.reset();
}


Result::Name CScreenEffectManager::InitNoiseEffect()
{
//	m_NoiseEffect.LoadNoiseTextures();
	m_NoiseEffect.Init( 0.5f, 2 );
	m_NoiseEffect.UpdateScreenSize();

	return Result::SUCCESS;
}


void CScreenEffectManager::ReleaseNoiseEffect()
{
//	m_NoiseEffect.Release();
}


/*
void CScreenEffectManager::BeginRender( const Camera &rCam )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();

	if( pEffect )
	{
		if( m_EffectFlag & ScreenEffect::PseudoNightVision )
			pEffect->SetBool( "g_bNightVision", true );
		else
			pEffect->SetBool( "g_bNightVision", false );
	}

	if( m_pSimpleMotionBlur && GetEffectFlag() & ScreenEffect::PseudoMotionBlur )
	{
		m_pSimpleMotionBlur->Begin();
		// TODO: save flag for effect flag changes during BeginRender() & RenderPostProcessEffects()
//		m_bMotionBlurOn = true;
	}

	if( m_EffectFlag & ScreenEffect::PostProcessEffects )
	{
		// clear all the previous post process effects
		m_pPPEffectManager->GetPostProcessInstance().resize( 0 );

		if( m_EffectFlag & ScreenEffect::PseudoBlur && 0 < m_fBlurWidth && 0 < m_fBlurHeight )
		{
			// set a texture as a render target
			// the texture will be used later to create a blurred image of the scene
//			m_TexRenderTarget.SetRenderTarget();
//			m_iTexRenderTargetEnabled = 1;

			UpdateBlurEffect();
		}

		// set post-process instances for glare effect
		if( m_EffectFlag & ScreenEffect::Glare )
			UpdateGlareEffect();

		// set post-process instances for monochrome color effect
		if( m_EffectFlag & ScreenEffect::MonochromeColor )
			UpdateMonochromeColorEffect();

		// save the original render target
		// NOTE: this method will increase the internal reference count
		//       on the IDirect3DSurface9 interface
//		pd3dDev->GetRenderTarget( 0, &m_pOrigRenderTarget );

		// set the texture render target for post process effect
		m_pPPEffectManager->SetTextureRenderTarget( 0 );
	}

//	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );

}
*/

void CScreenEffectManager::UpdateBlurEffect()
{
	if( !m_pPPEffectManager )
		return;

	if( m_mapBlurStrength.size() == 0 )
		return;
/*
	m_aFilterIndex[SF_DOWNFILTER_FOR_BLUR] = (int)m_pPPEffectManager->GetPostProcessInstance().size();
	m_pPPEffectManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_DOWNFILTER4 ) );
	m_pPPEffectManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_GBLUR_H ) );
	m_pPPEffectManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_GBLUR_V ) );
	m_aFilterIndex[SF_UPFILTER_FOR_BLUR] = (int)m_pPPEffectManager->GetPostProcessInstance().size();
	m_pPPEffectManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_UPFILTER4 ) );
*/
	Vector2 blur_strength = m_mapBlurStrength.begin()->second;
	m_fBlurWidth = blur_strength.x;
	m_fBlurWidth = blur_strength.y;
	Limit( m_fBlurWidth,  0.0f, 2.0f );
	Limit( m_fBlurHeight, 0.0f, 2.0f );

	m_pPPEffectManager->SetBlurStrength( m_fBlurWidth );
/*
	float weight[7];

	float x2;
	float s2 = m_fBlurWidth * m_fBlurWidth;
	for( int i=0; i<7; i++ )
	{
		x2 = (float)(i*i);
		weight[i] = pow( 2.72f, - 0.5f * x2 / s2 ); 
	}

	float sum = weight[0];
	for( int i=1; i<7; i++ )
		sum += weight[i] * 2.0f;

	for( int i=0; i<7; i++ )
		weight[i] /= sum;

	LPD3DXEFFECT pEffect[2];
	pEffect[0] = m_pPPEffectManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_GBLUR_H]).GetEffect();
	pEffect[1] = m_pPPEffectManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_GBLUR_V]).GetEffect();

	HRESULT hr;
	char str[32];
	for( int j=0; j<2; j++ )
	{
		if( !pEffect[j] )
			continue;

		pEffect[j]->SetFloat( "BlurWeights[6]", weight[0] );

		for( int i=1; i<6; i++ )
		{
			sprintf( str, "BlurWeights[%d]", 6-i );
			hr = pEffect[j]->SetFloat( str, weight[i] );

			sprintf( str, "BlurWeights[%d]", 6+i );
			hr = pEffect[j]->SetFloat( str, weight[i] );
		}
	}

	float up_scale = ( m_fBlurWidth + 1.0f ) * 2.0f;
	float down_scale = 1.0f / up_scale;

	m_pPPEffectManager->GetPostProcessInstance()[m_aFilterIndex[SF_DOWNFILTER_FOR_BLUR]].SetScale( down_scale, down_scale );
	m_pPPEffectManager->GetPostProcessInstance()[m_aFilterIndex[SF_UPFILTER_FOR_BLUR]].SetScale( up_scale, up_scale );
*/
}

/*
template<class T>
T& CalcPostProcessEffectParams( map<int,T>& mapParam )
{
	std::sort( mapParam.begin(), mapParam.end() );

	return (mapParam.begin())->second;
}
*/

void CScreenEffectManager::UpdateGlareEffect()
{
	if( !m_pPPEffectManager )
		return;
/*
	vector<CPProcInstance>& rPostProcessInstance = m_pPPEffectManager->GetPostProcessInstance();

	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_DOWNFILTER4 ) );
	rPostProcessInstance.back().SetScale( 0.25f, 0.25f );
	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_DOWNFILTER4 ) );
	rPostProcessInstance.back().SetScale( 0.25f, 0.25f );

	m_aFilterIndex[SF_BRIGHTPASS] = (int)rPostProcessInstance.size();
	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_BRIGHTPASS ) );

	// set luminance. (smaller luminance value causes more glare)
//	rPostProcessInstance.back().m_avParam[0].x = m_fGlareLuminanceThreshold;


	// take the param of the highest priority id
	rPostProcessInstance.back().m_avParam[0].x = (m_mapGlareLuminanceThreshold.begin())->second;


	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_BLOOM_H ) );
	rPostProcessInstance.back().m_avParam[0].x = 1.5f;
	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_BLOOM_V ) );
	rPostProcessInstance.back().m_avParam[0].x = 1.5f;
	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_BLOOM_H ) );
	rPostProcessInstance.back().m_avParam[0].x = 1.5f;
	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_BLOOM_V ) );
	rPostProcessInstance.back().m_avParam[0].x = 1.5f;

	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_UPFILTER4 ) );
	rPostProcessInstance.back().SetScale( 4.0f, 4.0f );
	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_COMBINE4 ) );
	rPostProcessInstance.back().SetScale( 4.0f, 4.0f );
*/
}


void CScreenEffectManager::UpdateMonochromeColorEffect()
{
	if( !m_pPPEffectManager )
		return;

	if( m_mapMonochromeColor.size() == 0 )
		return;
/*
	vector<CPProcInstance>& rPostProcessInstance = m_pPPEffectManager->GetPostProcessInstance();

	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_MONOCHROME ) );

	LPD3DXEFFECT pEffect = m_pPPEffectManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_MONOCHROME]).GetEffect();
*/
	const SFloatRGBColor color = m_mapMonochromeColor.begin()->second;
//	const float rgba[4] = { color.red, color.green, color.blue, 1.0f }; // always set alpha to 1.0
//	pEffect->SetFloatArray( "ColorOffset", rgba, 4 );

	m_pPPEffectManager->EnableEffect( CPostProcessEffect::TF_MONOCHROME_COLOR );
	m_pPPEffectManager->SetMonochromeColorOffset( color );
}


void CScreenEffectManager::RenderOverlayEffects()
{
//	HRESULT hr;
//	hr = DIRECT3D9.GetDevice()->SetVertexShader( NULL );
//	hr = DIRECT3D9.GetDevice()->SetPixelShader( NULL );

	int i;
	for( i=0; i<NUM_MAX_SIMULTANEOUS_FADES; i++ )
	{
		if( !m_aFadeEffect[i].IsInUse() )
			continue;

		m_aFadeEffect[i].Draw();
	}

	if( m_EffectFlag & ScreenEffect::PseudoNightVision )
	{
		if( !m_NoiseEffect.IsInitialized() )
			InitNoiseEffect();

		m_NoiseEffect.SetNoiseTexture();
		m_NoiseEffect.RenderNoiseEffect();
		m_vecExtraTexEffect[0].Rect.Draw( m_vecExtraTexEffect[0].m_Texture );
	}
}


void CScreenEffectManager::Update( float frametime )
{
	int i;
	for( i=0; i<NUM_MAX_SIMULTANEOUS_FADES; i++ )
	{
		if( !m_aFadeEffect[i].IsInUse() )
			continue;

		m_aFadeEffect[i].Update( frametime );
	}
}


void CScreenEffectManager::SetFadeEffect( int iType, const SFloatRGBAColor& color, float fTime, AlphaBlend::Mode alpha_blend_mode )
{
	int i;
	for( i=0; i<NUM_MAX_SIMULTANEOUS_FADES; i++ )
	{
		if( m_aFadeEffect[i].IsInUse() )
			continue;

		m_aFadeEffect[i].SetFadeEffect( iType, color, fTime, alpha_blend_mode );
		break;
	}
	
}


void CScreenEffectManager::FadeOutTo( const SFloatRGBAColor& color, float fTime, AlphaBlend::Mode alpha_blend_mode )
{
	SetFadeEffect( SCREENEFFECT_FADE_OUT, color, fTime, alpha_blend_mode );
}


void CScreenEffectManager::FadeInFrom( const SFloatRGBAColor& color, float fTime, AlphaBlend::Mode alpha_blend_mode )
{
	SetFadeEffect( SCREENEFFECT_FADE_IN, color, fTime, alpha_blend_mode );
}


void CScreenEffectManager::UpdateMotionBlurParams()
{
	if( m_pSimpleMotionBlur )
	{
		// adopt the motion blur strength specified by the highest priority id
		m_pSimpleMotionBlur->SetBlurWeight( (m_mapMotionBlurStrength.begin())->second );
	}
//		m_pSimpleMotionBlur->SetBlurWeight( motion_blur_weight );
}


void CScreenEffectManager::UpdateScreenSize()
{
	int width  = GetScreenWidth();
	int height = GetScreenHeight();

	int i;
	for( i=0; i<NUM_MAX_SIMULTANEOUS_FADES; i++ )
	{
		m_aFadeEffect[i].SetScreenSize( width, height );
	}

	int num_tex_effects = m_vecExtraTexEffect.size();
	for( i=0; i<num_tex_effects; i++ )
	{
		m_vecExtraTexEffect[i].Rect.SetPosition( Vector2(0.0f, 0.0f), Vector2((float)width, (float)height) );
	}

}


Result::Name CScreenEffectManager::SetHDRLightingParams( U32 param_flags, const CHDRLightingParams& params )
{
	if( !m_pPPEffectManager )
		return Result::UNKNOWN_ERROR;

	m_pPPEffectManager->SetHDRLightingParams( param_flags, params );

	return Result::SUCCESS;
}


Result::Name CScreenEffectManager::EnableHDRLighting( bool enable )
{
	if( !m_pPPEffectManager )
		return Result::UNKNOWN_ERROR;

	if( enable )
		return m_pPPEffectManager->EnableEffect( CPostProcessEffect::TF_HDR_LIGHTING );
	else
		return m_pPPEffectManager->DisableEffect( CPostProcessEffect::TF_HDR_LIGHTING );
}


void CScreenEffectManager::ReleaseGraphicsResources()
{
//	m_pSimpleMotionBlur - graphics component
}


void CScreenEffectManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	m_NoiseEffect.LoadGraphicsResources( const CGraphicsParameters& rParam );

	UpdateScreenSize();

//	m_pSimpleMotionBlur - graphics component
}


void CScreenEffectManager::BeginRender( const Camera &rCam )
{
//	DoPseudoNightVisionEffectSettings();

	m_bPseudoMotionBlurEnabled
		= GetEffectFlag() & ScreenEffect::PseudoMotionBlur 
		&& m_pSimpleMotionBlur;

	unsigned int pp_effect_flag_ors
		= ScreenEffect::Glare
		| ScreenEffect::MonochromeColor
		| ScreenEffect::PseudoBlur;

	m_bPostProcessEffectEnabled
		= m_EffectFlag & ScreenEffect::PostProcessEffects
		&& m_EffectFlag & pp_effect_flag_ors
		&& m_pPPEffectManager;

	// the two bool variables above are also used later in Render()

	if( m_bPseudoMotionBlurEnabled )
	{
		Result::Name res = Result::SUCCESS;
		if( !m_pSimpleMotionBlur )
			res = InitMotionBlurEffect();

		if( res == Result::SUCCESS && m_pSimpleMotionBlur )
		{
			// set motion blur strength
			UpdateMotionBlurParams();

			// set texture render target for psedo motion blur
			m_pSimpleMotionBlur->Begin();
			// TODO: save flag for effect flag changes during BeginRender() & RenderPostProcessEffects()
//			m_bMotionBlurOn = true;
		}
	}

	if( m_bPostProcessEffectEnabled )
	{
		if( !m_pPPEffectManager )
			InitPostProcessEffect();

		// clear all the previous post process effects
//		m_pPPEffectManager->GetPostProcessInstance().resize( 0 );

		if( m_EffectFlag & ScreenEffect::PseudoBlur )//&& 0 < m_fBlurWidth && 0 < m_fBlurHeight )
			UpdateBlurEffect();

		// set post-process instances for glare effect
		if( m_EffectFlag & ScreenEffect::Glare )
			UpdateGlareEffect();

		// set post-process instances for monochrome color effect
		if( m_EffectFlag & ScreenEffect::MonochromeColor )
			UpdateMonochromeColorEffect();

		// save the current render target and
		// set the texture render target for post process effect
//		m_pPPEffectManager->SetTextureRenderTarget( 0 );

		Result::Name res = m_pPPEffectManager->BeginRender();
	}
}


void CScreenEffectManager::EndRender()
{
}


void CScreenEffectManager::Render( Camera &rCam )
{
	PROFILE_FUNCTION();

	BeginRender( rCam );

	if( m_EffectFlag & ScreenEffect::MonochromeColor )
	{
		int monochrome_pp_effect_is_enabled = 1;
	}

		if( m_pTargetSceneRenderer )
			m_pTargetSceneRenderer->RenderSceneForScreenEffect( rCam );

	EndRender();

	RenderPostProcessEffects();

	if( m_pLensFlare )
	{
		m_pLensFlare->UpdateViewTransform( rCam.GetCameraMatrix() );
		m_pLensFlare->UpdateProjectionTransform( rCam.GetProjectionMatrix() );
		m_pLensFlare->UpdateLensFlares();

		m_pLensFlare->Render();
	}
}


void CScreenEffectManager::RenderPostProcessEffects()
{
	PROFILE_FUNCTION();

	if( m_bPostProcessEffectEnabled && m_pPPEffectManager )
	{
/*		// perform post-processes on the scene
		m_pPPEffectManager->PerformPostProcess();

		m_pPPEffectManager->ResetTextureRenderTarget();

		// restore the original render target
		// and draw the post processed scene as a textured rect
		// to the restored render target
		m_pPPEffectManager->DrawSceneWithPostProcessEffects();
*/
		m_pPPEffectManager->RenderPostProcessEffects();
	}

	if( m_bPseudoMotionBlurEnabled && m_pSimpleMotionBlur )
	{
		m_pSimpleMotionBlur->End();
		m_pSimpleMotionBlur->Render();
	}

	// Now the original back buffer is the render target
	// - render some simple fullscreen quad effects

	RenderOverlayEffects();
}


//======================================================================
// Implementation for CRenderTask version
//======================================================================

/*
void CScreenEffectManager::Render( Camera &rCam )
{
	if( m_bPseudoMotionBlurEnabled )
	{
		m_pSimpleMotionBlur->End();
		m_pSimpleMotionBlur->Render();
	}
	else if( m_bPostProcessEffectEnabled )
	{
		// restore the original render target
		// and draw the post processed scene as a textured rect
		m_pPPEffectManager->DrawSceneWithPostProcessEffects();
	}
	else
	{
		// no effects that use texture render target
		if( m_pTargetSceneRenderer )
			m_pTargetSceneRenderer->RenderSceneForScreenEffect( rCam );
	}
}


void CScreenEffectManager::RenderPostProcessEffects()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	if( m_EffectFlag & ScreenEffect::PostProcessEffects )
	{
		hr = m_pPPEffectManager->DrawSceneWithPostProcessEffects();
	}

	if( m_pSimpleMotionBlur && GetEffectFlag() & ScreenEffect::PseudoMotionBlur )
	{
		// render the fullscreen-sized rectangle of the blurred scene
		m_pSimpleMotionBlur->Render();
	}

	LPD3DXEFFECT pEffect = NULL;//m_pShaderManager->GetEffect();
	if( pEffect )
	{
		UINT cPasses;
//		m_pShaderManager->SetTechnique( m_DefaultShaderTechnique );
		pEffect->Begin( &cPasses, 0 );

		// assumes that the shader technique for overlay effects
		// uses fixed function vertex and pixel shader
		pEffect->BeginPass(0);	// set to default pass
		RenderOverlayEffects();
		pEffect->EndPass();

		pEffect->End();
	}
	else
	{
		RenderOverlayEffects();
	}
}
*/


/*
void CScreenEffectManager::RenderPostProcessEffects()
{
	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	if( m_EffectFlag & ScreenEffect::PostProcessEffects )
	{
		// perform post-processes on the scene
		m_pPPEffectManager->PerformPostProcess();

		// removed 18:42 2008-02-11
		// original render target is restored in m_pPPEffectManager->RenderPostProcess() call below

		// render the result of the post-processed image
		// to the restored render target
		m_pPPEffectManager->DrawSceneWithPostProcessEffects();
	}
//
//	if( pEffect )
//	{
//		UINT cPasses;
///		m_pShaderManager->SetTechnique( SHADER_TECH_DEFAULT );
//		m_pShaderManager->SetTechnique( m_DefaultShaderTechnique );
//		pEffect->Begin( &cPasses, 0);
//		pEffect->BeginPass(0);	// set to default pass
//	}

	int i;

	if( m_pSimpleMotionBlur && GetEffectFlag() & ScreenEffect::PseudoMotionBlur )
	{
		m_pSimpleMotionBlur->End();
		m_pSimpleMotionBlur->Render();
	}

	for( i=0; i<NUM_MAX_SIMULTANEOUS_FADES; i++ )
	{
		if( !m_aFadeEffect[i].IsInUse() )
			continue;

		m_aFadeEffect[i].Draw();
	}

	if( m_EffectFlag & ScreenEffect::PseudoNightVision )
	{
		m_NoiseEffect.RenderNoiseEffect();
		m_vecExtraTexEffect[0].Rect.Draw( m_vecExtraTexEffect[0].m_Texture );
	}

	if( pEffect )
	{
		pEffect->EndPass();
		pEffect->End();
	}

}*/


/// add render tasks that are done after the scene is rendered
/// make sure render tasks for scene are already registered
void CScreenEffectManager::CreateRenderTasks()
{
//	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	DoPseudoNightVisionEffectSettings();

	bool bExtraRenderTarget = false;

	m_bPseudoMotionBlurEnabled
		= GetEffectFlag() & ScreenEffect::PseudoMotionBlur 
		&& m_pSimpleMotionBlur;

	unsigned int pp_effect_flag_ors
		= ScreenEffect::Glare
		| ScreenEffect::MonochromeColor
		| ScreenEffect::PseudoBlur;

	m_bPostProcessEffectEnabled
		= m_EffectFlag & ScreenEffect::PostProcessEffects
		&& m_EffectFlag & pp_effect_flag_ors
		&& m_pPPEffectManager;

	// the two bool variables above are also used later in Render()

	if( m_bPseudoMotionBlurEnabled )
	{
		// set motion blur strength
		UpdateMotionBlurParams();

		// set texture render target for psedo motion blur
		m_pSimpleMotionBlur->Begin();
		// TODO: save flag for effect flag changes during BeginRender() & RenderPostProcessEffects()
//		m_bMotionBlurOn = true;

		bExtraRenderTarget = true;
	}

	if( m_bPostProcessEffectEnabled )
	{
		// clear all the previous post process effects
//		m_pPPEffectManager->GetPostProcessInstance().resize( 0 );

		if( m_EffectFlag & ScreenEffect::PseudoBlur )//&& 0 < m_fBlurWidth && 0 < m_fBlurHeight )
			UpdateBlurEffect();

		// set post-process instances for glare effect
		if( m_EffectFlag & ScreenEffect::Glare )
			UpdateGlareEffect();

		// set post-process instances for monochrome color effect
		if( m_EffectFlag & ScreenEffect::MonochromeColor )
			UpdateMonochromeColorEffect();

		// save the current render target and
		// set the texture render target for post process effect
//		m_pPPEffectManager->SetTextureRenderTarget( 0 );

		bExtraRenderTarget = true;
	}

	// create render tasks for child elements
	// - must preceed render tasks of post process effects
	bool create_scene_render_task = bExtraRenderTarget ? true : false;
	if( m_pTargetSceneRenderer )
		m_pTargetSceneRenderer->CreateRenderTasks( create_scene_render_task );

	// register the render task that renders the scene with screen effects

	if( m_bPostProcessEffectEnabled )
	{
		RenderTaskProcessor.AddRenderTask( new CPostProcessPerformRenderTask( m_pPPEffectManager.get() ) );
	}

	if( m_bPseudoMotionBlurEnabled )
	{
		if( m_bPostProcessEffectEnabled )
		{
			// render the post processed scene to the texture render target
			// for the pseudo motion blur module
			RenderTaskProcessor.AddRenderTask( new CPostProcessRenderTask( m_pPPEffectManager.get() ) );
		}

		RenderTaskProcessor.AddRenderTask( new CSimpleMotionBlurRenderTask( m_pSimpleMotionBlur.get() ) );
	}
}


} // namespace amorphous
