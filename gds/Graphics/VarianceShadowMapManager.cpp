#include "VarianceShadowMapManager.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/2DPrimitive/2DRect.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Shader/ShaderManagerHub.h"
#include "Graphics/2DPrimitive/2DTexRect.h"
#include "Graphics/TextureRenderTarget.h"
#include "Support/Log/DefaultLog.h"

using namespace std;
using namespace boost;


inline static float ComputeGaussian( float n )
{
    float theta = 2.0f + 0.0001f;//float.Epsilon;

    return theta = (float)((1.0f / sqrtf(2 * (float)PI * theta)) * 
		expf(-(n * n) / (2 * theta * theta)));
}


// Lifted from Mahdi's Post Processor (TM)
static void SetBlurParameters( float dx, 
							   float dy, 
							   Vector2* vSampleOffsets, 
							   float* fSampleWeights)
{
	// The first sample always has a zero offset.
	fSampleWeights[0] = ComputeGaussian(0);
	vSampleOffsets[0] = Vector2(0,0);

	// Maintain a sum of all the weighting values.
	float totalWeights = fSampleWeights[0];

	// Add pairs of additional sample taps, positioned
	// along a line in both directions from the center.
	for (int i = 0; i < 15 / 2; i++)
	{
		// Store weights for the positive and negative taps.
		float weight = ComputeGaussian((float)(i + 1));

		fSampleWeights[i * 2 + 1] = weight;
		fSampleWeights[i * 2 + 2] = weight;

		totalWeights += weight * 2;

		// To get the maximum amount of blurring from a limited number of
		// pixel shader samples, we take advantage of the bilinear filtering
		// hardware inside the texture fetch unit. If we position our texture
		// coordinates exactly halfway between two texels, the filtering unit
		// will average them for us, giving two samples for the price of one.
		// This allows us to step in units of two texels per sample, rather
		// than just one at a time. The 1.5 offset kicks things off by
		// positioning us nicely in between two texels.
		float sampleOffset = i * 2 + 1.5f;

		Vector2 delta = Vector2(dx, dy) * sampleOffset;

		// Store texture coordinate offsets for the positive and negative taps.
		vSampleOffsets[i * 2 + 1] = delta;
		vSampleOffsets[i * 2 + 2] = -delta;
	}

	// Normalize the list of sample weightings, so they will always sum to one.
	for (int i = 0; i < 15/*fSampleWeights.Length*/; i++)
	{
		fSampleWeights[i] /= totalWeights;
	}
}


//std::string CVarianceShadowMapManager::ms_strDefaultVSMShaderFilename = "Shader/VarianceShadowMap.fx";

/*
CVarianceShadowMapManager::SetShaderTechniqueForShadowCaster()
{
}
*/


const char *gs_pDefaultShaderName = "Shader/VarianceShadowMap.fx";


Vector2 sampleOffsetsH[15];
float sampleWeightsH[15];



CVarianceShadowMapManager::CVarianceShadowMapManager()
//:
//m_pHBlurredShadowMap(NULL),
//m_pBlurredShadowMap(NULL)
{
	m_ShadowMapShaderFilename = gs_pDefaultShaderName;
}


CVarianceShadowMapManager::CVarianceShadowMapManager( int texture_width, int texture_height )
:
CShadowMapManager(texture_width,texture_height)
//m_pHBlurredShadowMap(NULL),
//m_pBlurredShadowMap(NULL)
{
	m_ShadowMapShaderFilename = gs_pDefaultShaderName;
}


CVarianceShadowMapManager::~CVarianceShadowMapManager()
{
	ReleaseGraphicsResources();
}


/*
void CVarianceShadowMapManager::ReleaseTextures()
{}
*/


bool CVarianceShadowMapManager::Init()
{
	bool base_init = CShadowMapManager::Init();

	if( !base_init )
		return false;

	// Init shader for blur?

	bool loaded = m_BlurShader.Load( "./Shader/BlurVSM.fx" );
	if( !loaded )
		return false;

	// init samples
	float texel_size = 1.0f / (float)m_ShadowMapSize;
	SetBlurParameters( texel_size, 0, m_avSampleOffsetH, m_afSampleWeights );
	SetBlurParameters( 0, texel_size, m_avSampleOffsetV, m_afSampleWeights );

	m_SampleWeights  = CShaderParamFloatArray( "sampleWeights[0]" );
	m_SampleOffsetsH = CShaderParamFloatArray( "sampleOffsets[0]" );
	m_SampleOffsetsV = CShaderParamFloatArray( "sampleOffsets[0]" );

	m_SampleWeights.Parameter().resize( NUM_GAUSSIAN_SAMPLES );
	m_SampleOffsetsH.Parameter().resize( NUM_GAUSSIAN_SAMPLES * 2 );
	m_SampleOffsetsV.Parameter().resize( NUM_GAUSSIAN_SAMPLES * 2 );

	for( int i=0; i<NUM_GAUSSIAN_SAMPLES; i++ )
	{
		m_SampleWeights.Parameter()[i] = m_afSampleWeights[i];
		m_SampleOffsetsH.Parameter()[i*2]   = m_avSampleOffsetH[i].x;
		m_SampleOffsetsH.Parameter()[i*2+1] = m_avSampleOffsetH[i].y;
		m_SampleOffsetsV.Parameter()[i*2]   = m_avSampleOffsetV[i].x;
		m_SampleOffsetsV.Parameter()[i*2+1] = m_avSampleOffsetV[i].y;
	}

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
//	HRESULT hr;

	CTextureResourceDesc desc;
	desc.Width  = m_ShadowMapSize;// m_iTextureWidth;
	desc.Height = m_ShadowMapSize;// m_iTextureHeight;
	desc.Format = TextureFormat::G16R16F;
	desc.UsageFlags = UsageFlag::RENDER_TARGET;

	bool initialized = false;

	m_pHBlurredShadowMap = shared_ptr<CTextureRenderTarget>( new CTextureRenderTarget() );

	initialized = m_pHBlurredShadowMap->Init( desc );

	if( !initialized )
		return false;

	m_pBlurredShadowMap  = shared_ptr<CTextureRenderTarget>( new CTextureRenderTarget() );

	initialized = m_pBlurredShadowMap->Init( desc );

	if( !initialized )
		return false;


/*
	// create a texture on which the blurred shadow map is rendered
	hr = pd3dDev->CreateTexture( m_iTextureWidth, m_iTextureHeight, 
								 1,
								 D3DUSAGE_RENDERTARGET,
								 GetShadowMapTextureFormat(), 
								 D3DPOOL_DEFAULT,
								 &m_pHBlurredShadowMap,
								 NULL );

	hr = pd3dDev->CreateTexture( m_iTextureWidth, m_iTextureHeight, 
								 1,
								 D3DUSAGE_RENDERTARGET,
								 GetShadowMapTextureFormat(), 
								 D3DPOOL_DEFAULT,
								 &m_pBlurredShadowMap,
								 NULL );
*/
	return true;
}


void CVarianceShadowMapManager::EndSceneShadowMap()
{
	CShadowMapManager::EndSceneShadowMap();
/*
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	if( !m_pHBlurredShadowMap || !m_pBlurredShadowMap )
		return;

	m_pHBlurredShadowMap->SetRenderTarget();

//	pd3dDev->SetVertexShader( NULL );
//	pd3dDev->SetPixelShader( NULL );

	CShaderManager& rShaderMgr = *(m_BlurShader.GetShaderManager());

	// horizontally blur the shadowmap

	CShaderTechniqueHandle blur_h;
	blur_h.SetTechniqueName( "GaussianBlurH" );
	rShaderMgr.SetTechnique( blur_h );

//	rShaderMgr.SetParam( m_SampleWeights );
//	rShaderMgr.SetParam( m_SampleOffsetsH );

	C2DRect screen_rect( 0, 0, m_ShadowMapSize-1, m_ShadowMapSize-1, 0xFFFFFFFF );

	screen_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	rShaderMgr.SetTexture( 0, m_pShadowMap );

	LPD3DXEFFECT pEffect = rShaderMgr.GetEffect();

	pEffect->CommitChanges();

//	screen_rect.Draw( m_pShadowMap ); // test with fixed-function shader. Remeber floating point buffer does not work with this

	UINT passes;
	pEffect->Begin(&passes,0);
	for( UINT i=0; i<passes; i++ )
	{
		pEffect->BeginPass(i);
		screen_rect.draw();
		pEffect->EndPass();
	}
	pEffect->End();

	m_pHBlurredShadowMap->ResetRenderTarget();

	// vertically blur the shadowmap

	// save the current RT
	LPDIRECT3DSURFACE9 pOrigSurf;
	hr = pd3dDev->GetRenderTarget( 0, &pOrigSurf );

	LPDIRECT3DSURFACE9 pSurf;
	hr = m_pShadowMap->GetSurfaceLevel( 0, &pSurf );

	hr = pd3dDev->SetRenderTarget( 0, pSurf );

	CShaderTechniqueHandle blur_v;
	blur_v.SetTechniqueName( "GaussianBlurV" );
	rShaderMgr.SetTechnique( blur_v );

//	rShaderMgr.SetParam( m_SampleOffsetsV );

	rShaderMgr.SetTexture( 0, m_pHBlurredShadowMap->GetRenderTargetTexture() );

	rShaderMgr.GetEffect()->CommitChanges();

//	screen_rect.Draw( m_pHBlurredShadowMap->GetRenderTargetTexture() ); // test with fixed-function shader

	pEffect->Begin(&passes,0);
	for( UINT i=0; i<passes; i++ )
	{
		pEffect->BeginPass(i);
		screen_rect.draw();
		pEffect->EndPass();
	}
	pEffect->End();

	// restore the original RT
	hr = pd3dDev->SetRenderTarget( 0, pOrigSurf );


//	m_pBlurredShadowMap->SetRenderTarget();

//	m_pBlurredShadowMap->ResetRenderTarget();

//	pd3dDev->SetRenderTarget( 0, prev_target );

//	if( FAILED(hr) )
//		return;
*/
}


void CVarianceShadowMapManager::UpdateLightPositionAndDirection()
{
/*	HRESULT hr;
	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();

	D3DXVECTOR3 vWorldLightPos = m_LightCamera.GetPosition();
	D3DXVECTOR3 vWorldLightDir = m_LightCamera.GetFrontDirection();

	hr = pEffect->SetFloatArray( "g_vLightPos", (float *)&vWorldLightPos, 3 );
	hr = pEffect->SetFloatArray( "g_vLightDir", (float *)&vWorldLightDir, 3 );
*/
}

void CVarianceShadowMapManager::BeginSceneDepthMap()
{
	CShadowMapManager::BeginSceneDepthMap();

	CShaderParamFloatArray farclip = CShaderParamFloatArray( "g_fFarClip" );
	farclip.Parameter().resize( 1 );
	farclip.Parameter()[0] = m_SceneCamera.GetFarClip();

	m_Shader.GetShaderManager()->SetParam( farclip );
}


/*
void CVarianceShadowMapManager::SetDefault()
{
	CShadowMapManager::SetDefault();

	// set default light direction & position
	Vector3 vLightDir =  Vector3(-0.56568f, -0.70711f, -0.42426f);
	Vector3 vLightPos =  Vector3( 5.0f, 10.0f,  6.0f );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir( vLightDir ) );
	m_LightCamera.SetPosition( vLightPos );
//	m_LightCamera.SetNearClip( 0.1f );
//	m_LightCamera.SetFarClip( 100.0f );
}
*/


void CVarianceShadowMapManager::ReleaseGraphicsResources()
{
	CShadowMapManager::ReleaseGraphicsResources();

	// release blur shader
}


void CVarianceShadowMapManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	CShadowMapManager::LoadGraphicsResources( rParam );

	Init();
}
