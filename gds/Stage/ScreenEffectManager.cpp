
#include "ScreenEffectManager.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/Camera.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/PostProcessManager.h"
#include "3DCommon/SimpleMotionBlur.h"
#include "3DCommon/D3DMisc.h"

#include "3DCommon/RenderTask.h"
#include "3DCommon/RenderTaskProcessor.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"

#include "3DMath/MathMisc.h"

#include <algorithm>

using namespace std;


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
	CPostProcessManager *m_pPPMgr;

public:

	CPostProcessPerformRenderTask( CPostProcessManager *pPPMgr )
		:
	m_pPPMgr(pPPMgr)
	{
		m_TypeFlags = DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE;
	}

	~CPostProcessPerformRenderTask() {}

	void Render()
	{
		HRESULT hr;

		// perform post-processes on the scene
		// includes BeginScene() & EndScene() calls pair
		hr = m_pPPMgr->PerformPostProcess();

		hr = m_pPPMgr->ResetTextureRenderTarget();
	}
};


class CPostProcessRenderTask : public CRenderTask
{
	/// borrowed reference
	CPostProcessManager *m_pPPMgr;

public:

	CPostProcessRenderTask( CPostProcessManager *pPPMgr )
		:
	m_pPPMgr(pPPMgr)
	{
	}

	~CPostProcessRenderTask() {}

	void Render()
	{
		// render the result of the post-processed image
		// to the restored render target
		// render target is restored in this call
		HRESULT hr = m_pPPMgr->DrawSceneWithPostProcessEffects();
	}
};


//string CScreenEffectManager::ms_DefaultShaderFilename = "Shader\\Default.fx";


bool CScreenEffectManager::CExtraTextureEffect::LoadTexture()
{
	return m_Texture.Load( m_TextureFilepath );
}


CScreenEffectManager::CScreenEffectManager()
:
m_EffectFlag(0),
//m_pShaderManager(NULL),
m_pPPManager(NULL),
m_pTargetSceneRenderer(NULL)
{
	ClearBlurEffect();

	m_iTexRenderTargetEnabled = 0;

	m_EffectFlag |= ScreenEffect::PostProcessEffects;

//	m_EffectFlag |= ScreenEffect::Glare;
//	SetGlareLuminanceThreshold( 0.09f );

	m_pSimpleMotionBlur = NULL;

	m_DefaultShaderTechnique.SetTechniqueName( "Default" );
}


CScreenEffectManager::~CScreenEffectManager()
{
	ReleaseGraphicsResources();

	SafeDelete( m_pSimpleMotionBlur );
}


bool CScreenEffectManager::Init()
{
	m_vecExtraTexEffect.resize( 1 );

//	LoadShader();
	UpdateScreenSize();

	m_NoiseEffect.LoadNoiseTextures();
	m_NoiseEffect.UpdateScreenSize();
/*
	m_TexRenderTarget.Init( 512, 512 );
//	m_TexRenderTarget.Init( 256, 256 );
//	m_TexRenderTarget.Init( 16, 16 );
	m_TexRenderTarget.SetBackgroundColor( 0xFF000000 );
//	m_TexRenderTarget.SetBackgroundColor( 0x00000000 );
*/

	// set up textured-rectangles
	// rectangle sizes have been alreay set in UpdateScreenSize()

	m_vecExtraTexEffect[0].Rect.SetTextureUV( TEXCOORD2(0.0f, 0.0f), TEXCOORD2(32.0f, 32.0f) );
	m_vecExtraTexEffect[0].SetTextureFilename( "Texture\\stripe.dds" );
	m_vecExtraTexEffect[0].LoadTexture();


	// initialize post-process effect manager
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// retrieve the back buffer size
//	D3DSURFACE_DESC back_buffer_desc;
//	IDirect3DSurface9 *pBackBuffer;
//	pd3dDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
//	pBackBuffer->GetDesc( &back_buffer_desc );

	m_pPPManager = new CPostProcessManager;
//	hr = m_pPPManager->OnCreateDevice( pd3dDev, &back_buffer_desc, "Shader\\PostProcess\\PostProcess.fx", NULL );
	hr = m_pPPManager->OnCreateDevice( "Shader\\PostProcess\\PostProcess.fx" );
	if( !FAILED(hr) )
	{
//		hr = m_pPPManager->OnResetDevice( pd3dDev, &back_buffer_desc, NULL );
		hr = m_pPPManager->OnResetDevice();

		m_aPPEffectIndex[PP_COLOR_GBLUR_H]     = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorGBlurH.fx" );
		m_aPPEffectIndex[PP_COLOR_GBLUR_V]     = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorGBlurV.fx" );
		m_aPPEffectIndex[PP_COLOR_INVERSE]     = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorInverse.fx" );
		m_aPPEffectIndex[PP_COLOR_MONOCHROME]  = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorMonochrome.fx" );
		m_aPPEffectIndex[PP_COLOR_DOWNFILTER4] = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorDownFilter4.fx" );
		m_aPPEffectIndex[PP_COLOR_UPFILTER4]   = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorUpFilter4.fx" );
		m_aPPEffectIndex[PP_COLOR_BLOOM_H]     = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorBloomH.fx" );
		m_aPPEffectIndex[PP_COLOR_BLOOM_V]     = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorBloomV.fx" );
		m_aPPEffectIndex[PP_COLOR_BRIGHTPASS]  = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorBrightPass.fx" );
		m_aPPEffectIndex[PP_COLOR_COMBINE4]    = m_pPPManager->AddPostProcessShader( "Shader\\PostProcess\\PP_ColorCombine4.fx" );

		m_pPPManager->GetPostProcessInstance().reserve( 16 );
	}

	m_pSimpleMotionBlur = new CSimpleMotionBlur();
	m_pSimpleMotionBlur->InitForScreenSize();
	m_pSimpleMotionBlur->SetBlurWeight( 0.85f );
//	m_pSimpleMotionBlur->SetBlurWeight( 1.00f );

	return true;
}

/*
void CScreenEffectManager::BeginRender( const CCamera &rCam )
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
		m_pPPManager->GetPostProcessInstance().resize( 0 );

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
		m_pPPManager->SetTextureRenderTarget( 0 );
	}

//	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );

}
*/

void CScreenEffectManager::UpdateBlurEffect()
{
	if( m_mapBlurStrength.size() == 0 )
		return;

	m_aFilterIndex[SF_DOWNFILTER_FOR_BLUR] = (int)m_pPPManager->GetPostProcessInstance().size();
	m_pPPManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_DOWNFILTER4 ) );
	m_pPPManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_GBLUR_H ) );
	m_pPPManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_GBLUR_V ) );
	m_aFilterIndex[SF_UPFILTER_FOR_BLUR] = (int)m_pPPManager->GetPostProcessInstance().size();
	m_pPPManager->GetPostProcessInstance().push_back( CPProcInstance( PP_COLOR_UPFILTER4 ) );

	Vector2 blur_strength = m_mapBlurStrength.begin()->second;
	m_fBlurWidth = blur_strength.x;
	m_fBlurWidth = blur_strength.y;
	Limit( m_fBlurWidth,  0.0f, 2.0f );
	Limit( m_fBlurHeight, 0.0f, 2.0f );

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
	pEffect[0] = m_pPPManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_GBLUR_H]).GetEffect();
	pEffect[1] = m_pPPManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_GBLUR_V]).GetEffect();

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

	m_pPPManager->GetPostProcessInstance()[m_aFilterIndex[SF_DOWNFILTER_FOR_BLUR]].SetScale( down_scale, down_scale );
	m_pPPManager->GetPostProcessInstance()[m_aFilterIndex[SF_UPFILTER_FOR_BLUR]].SetScale( up_scale, up_scale );
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
	vector<CPProcInstance>& rPostProcessInstance = m_pPPManager->GetPostProcessInstance();

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
}


void CScreenEffectManager::UpdateMonochromeColorEffect()
{
	vector<CPProcInstance>& rPostProcessInstance = m_pPPManager->GetPostProcessInstance();

	rPostProcessInstance.push_back( CPProcInstance( PP_COLOR_MONOCHROME ) );

	LPD3DXEFFECT pEffect = m_pPPManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_MONOCHROME]).GetEffect();

	pEffect->SetFloatArray( "ColorOffset", m_MonochromeColorOffset, 4 );
}

/*
void CScreenEffectManager::RenderPostProcessEffects()
{
	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	if( m_EffectFlag & ScreenEffect::PostProcessEffects )
	{
		// perform post-processes on the scene
		m_pPPManager->PerformPostProcess( pd3dDev );

		// removed 18:42 2008-02-11
		// original render target is restored in m_pPPManager->RenderPostProcess() call below

		// render the result of the post-processed image
		// to the restored render target
		m_pPPManager->RenderPostProcess( pd3dDev );
	}

	if( pEffect )
	{
		UINT cPasses;
//		m_pShaderManager->SetTechnique( SHADER_TECH_DEFAULT );
		m_pShaderManager->SetTechnique( m_DefaultShaderTechnique );
		pEffect->Begin( &cPasses, 0);
		pEffect->BeginPass(0);	// set to default pass
	}

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


void CScreenEffectManager::RenderOverlayEffects()
{
	int i;
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
}


void CScreenEffectManager::RenderPostProcessEffects()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	if( m_EffectFlag & ScreenEffect::PostProcessEffects )
	{
		hr = m_pPPManager->DrawSceneWithPostProcessEffects();
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


void CScreenEffectManager::EndRender()
{
}


void CScreenEffectManager::Render( CCamera &rCam )
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
		m_pPPManager->DrawSceneWithPostProcessEffects();
	}
	else
	{
		// no effects that use texture render target
		if( m_pTargetSceneRenderer )
			m_pTargetSceneRenderer->RenderSceneForScreenEffect( rCam );
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


void CScreenEffectManager::SetFadeEffect( int iType, U32 dwColor, float fTime, AlphaBlend::Mode alpha_blend_mode )
{
	int i;
	for( i=0; i<NUM_MAX_SIMULTANEOUS_FADES; i++ )
	{
		if( m_aFadeEffect[i].IsInUse() )
			continue;

		m_aFadeEffect[i].SetFadeEffect( iType, dwColor, fTime, alpha_blend_mode );
		break;
	}
	
}


void CScreenEffectManager::FadeOutTo( U32 dwColor, float fTime, AlphaBlend::Mode alpha_blend_mode )
{
	SetFadeEffect( SCREENEFFECT_FADE_OUT, dwColor, fTime, alpha_blend_mode );
}


void CScreenEffectManager::FadeInFrom( U32 dwColor, float fTime, AlphaBlend::Mode alpha_blend_mode )
{
	SetFadeEffect( SCREENEFFECT_FADE_IN, dwColor, fTime, alpha_blend_mode );
}

/*
bool CScreenEffectManager::LoadShader()
{
	m_pShaderManager = new CShaderManager;

	if( !m_pShaderManager->LoadShaderFromFile( ms_DefaultShaderFilename ) )
///	if( !m_pShaderManager->LoadShaderFromFile( (LPCTSTR)"Shader\\Default.fx" ) )
//	if( !m_pShaderManager->LoadShaderFromFile( "Shader\\Default.fx" ) )
//	if( !m_pShaderManager->LoadShaderFromFile( "Shader\\SGViewerFG.fx" ) )
	{
		SafeDelete( m_pShaderManager );
		return false;
	}

	CShader::Get()->SetShaderManager( m_pShaderManager );

	// check if the shader file has been properly loaded
	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();
	
	if( !pEffect )
	{
		SafeDelete( m_pShaderManager );
		return false;
	}

	return true;
}*/


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
		m_vecExtraTexEffect[i].Rect.SetPosition( D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)width, (float)height) );
	}

}


void CScreenEffectManager::ReleaseGraphicsResources()
{
//	SafeDelete( m_pShaderManager );

	SafeDelete( m_pPPManager );

//	int i, num_tex_effects = m_vecExtraTexEffect.size();
//	for( i=0; i<num_tex_effects; i++ )
//		m_vecExtraTexEffect[i].ReleaseTexture();

//	m_pSimpleMotionBlur - graphics component
}


void CScreenEffectManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	m_NoiseEffect.LoadGraphicsResources( const CGraphicsParameters& rParam );

//	LoadShader();

	UpdateScreenSize();

//	int i, num_tex_effects = m_vecExtraTexEffect.size();
//	for( i=0; i<num_tex_effects; i++ )
//		m_vecExtraTexEffect[i].LoadTexture();

//	m_pSimpleMotionBlur - graphics component
}

/*
void CScreenEffectManager::SetShaderManager()
{
	CShader::Get()->SetShaderManager( m_pShaderManager );
}*/


/// add render tasks that are done after the scene is rendered
/// make sure render tasks for scene are already registered
void CScreenEffectManager::CreateRenderTasks()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = NULL;//m_pShaderManager->GetEffect();

	bool bExtraRenderTarget = false;

	if( pEffect )
	{
		if( m_EffectFlag & ScreenEffect::PseudoNightVision )
			pEffect->SetBool( "g_bNightVision", true );
		else
			pEffect->SetBool( "g_bNightVision", false );
	}

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
		&& m_pPPManager;

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
		m_pPPManager->GetPostProcessInstance().resize( 0 );

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
		m_pPPManager->SetTextureRenderTarget( 0 );

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
		RenderTaskProcessor.AddRenderTask( new CPostProcessPerformRenderTask( m_pPPManager ) );
	}

	if( m_bPseudoMotionBlurEnabled )
	{
		if( m_bPostProcessEffectEnabled )
		{
			// render the post processed scene to the texture render target
			// for the pseudo motion blur module
			RenderTaskProcessor.AddRenderTask( new CPostProcessRenderTask( m_pPPManager ) );
		}

		RenderTaskProcessor.AddRenderTask( new CSimpleMotionBlurRenderTask( m_pSimpleMotionBlur ) );
	}
}


void CScreenEffectManager::BeginRender( const CCamera &rCam )
{


//	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );

}

/// old pseudo blur effect
/*
	if( m_EffectFlag & ScreenEffect::PseudoBlur || m_iTexRenderTargetEnabled != 0 )
	{
		// render rectangles with the scene texture
		m_TexRenderTarget.ResetRenderTarget();
		m_iTexRenderTargetEnabled = 0;

		C2DRect screen_rect;
		const int screen_width  = GAMEWINDOWMANAGER.GetScreenWidth();
		const int screen_height = GAMEWINDOWMANAGER.GetScreenHeight();

		// render a background rect with opaque black
		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		screen_rect.SetPosition( D3DXVECTOR2(0,0), D3DXVECTOR2((float)screen_width,(float)screen_height) );
		screen_rect.SetColor( 0xFF000000 );
		screen_rect.Draw();

		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		screen_rect.SetDestAlphaBlendMode( D3DBLEND_ONE );
		screen_rect.SetTextureUV( D3DXVECTOR2(0,0), D3DXVECTOR2(1,1) );

		float blur_width  = (float)m_fBlurWidth;
		float blur_height = (float)m_fBlurHeight;
		int num_blur_draws = (int)sqrtf( blur_width * blur_width + blur_height * blur_height );

		if( 10.0f < blur_width )			blur_width = 10.0f;
		if( 10.0f < blur_height )			blur_height = 10.0f;
		if( num_blur_draws <= 0 )			num_blur_draws = 1;
		else if( 8 < num_blur_draws )		num_blur_draws = 8;

//		screen_rect.SetColor( 0xFFFFFFFF );
		screen_rect.SetColor( D3DCOLOR_ARGB( 255/(num_blur_draws/2+1), 255, 255, 255 ) );

		for( i=0; i<=num_blur_draws; i+=2 )
		{
			float f = (float)i / (float)num_blur_draws - 0.5f;	// -0.5 <= f <= 0.5
			D3DXVECTOR2 vMin = D3DXVECTOR2( f * blur_width, f * blur_height );
			D3DXVECTOR2 vMax = vMin + D3DXVECTOR2(screen_width,screen_height);
			screen_rect.SetPosition( vMin, vMax );
			screen_rect.Draw( m_TexRenderTarget.GetRenderTargetTexture() );
		}
	}*/

