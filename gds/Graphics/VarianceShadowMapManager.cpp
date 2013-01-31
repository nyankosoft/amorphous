#include "VarianceShadowMapManager.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"
#include "Graphics/TextureRenderTarget.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


class VSShadowMapPostProcessor : public ShadowMapVisitor
{
	VarianceShadowMapManager *m_pVSShadowMapManager;
public:

	VSShadowMapPostProcessor( VarianceShadowMapManager *pMgr ) { m_pVSShadowMapManager = pMgr; }

//	void Visit( DirectionalLightShadowMap& shadow_map ) { m_pVSShadowMapManager->PostProcessDirectionalLightShadowMap( shadow_map ); }
	void Visit( OrthoShadowMap& shadow_map ) { m_pVSShadowMapManager->PostProcessDirectionalLightShadowMap( shadow_map ); }
//	void Visit( PointLightShadowMap& shadow_map ) { m_pVSShadowMapManager->PostProcessPointLightShadowMap( shadow_map ); }
//	void Visit( SpotLightShadowMap& shadow_map ) { m_pVSShadowMapManager->PostProcessPSpotLightShadowMap( shadow_map ); }
};


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


//std::string VarianceShadowMapManager::ms_strDefaultVSMShaderFilename = "Shader/VarianceShadowMap.fx";

/*
VarianceShadowMapManager::SetShaderTechniqueForShadowCaster()
{
}
*/


const char *gs_pDefaultShaderName = "Shader/VarianceShadowMap.fx";
//const char *gs_pDefaultShaderName = "Shader/PCFShadowMap.fx";


Vector2 sampleOffsetsH[15];
float sampleWeightsH[15];



VarianceShadowMapManager::VarianceShadowMapManager()
//:
//m_pHBlurredShadowMap(NULL),
//m_pBlurredShadowMap(NULL)
{
	m_ShadowMapShaderFilename = gs_pDefaultShaderName;
}


VarianceShadowMapManager::~VarianceShadowMapManager()
{
	ReleaseGraphicsResources();
}


/*
void VarianceShadowMapManager::ReleaseTextures()
{}
*/


bool VarianceShadowMapManager::Init()
{
	bool base_init = ShadowMapManager::Init();

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

	TextureResourceDesc desc;
	desc.Width  = m_ShadowMapSize;
	desc.Height = m_ShadowMapSize;
	desc.Format = TextureFormat::G16R16F;
	desc.UsageFlags = UsageFlag::RENDER_TARGET;

	bool initialized = false;

	m_pHBlurredShadowMap = TextureRenderTarget::Create();

	initialized = m_pHBlurredShadowMap->Init( desc );

	if( !initialized )
		return false;

	m_pBlurredShadowMap  = TextureRenderTarget::Create();

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


void VarianceShadowMapManager::PostProcessShadowMap( ShadowMap& shadow_map )
{
	VSShadowMapPostProcessor post_processor( this );
	shadow_map.Accept( post_processor );
}


void VarianceShadowMapManager::EndSceneShadowMap()
{
	ShadowMapManager::EndSceneShadowMap();
}


void VarianceShadowMapManager::UpdateLightPositionAndDirection()
{
/*	HRESULT hr;
	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();

	D3DXVECTOR3 vWorldLightPos = m_LightCamera.GetPosition();
	D3DXVECTOR3 vWorldLightDir = m_LightCamera.GetFrontDirection();

	hr = pEffect->SetFloatArray( "g_vLightPos", (float *)&vWorldLightPos, 3 );
	hr = pEffect->SetFloatArray( "g_vLightDir", (float *)&vWorldLightDir, 3 );
*/
}


/// input: shadow map
/// final output: shadow map (horizontally and vertically blurred)
//void VarianceShadowMapManager::PostProcessDirectionalLightShadowMap( DirectionalLightShadowMap& shadow_map )
void VarianceShadowMapManager::PostProcessDirectionalLightShadowMap( OrthoShadowMap& shadow_map )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	if( !m_pHBlurredShadowMap || !m_pBlurredShadowMap )
		return;

	LPDIRECT3DTEXTURE9 pShadowMap = shadow_map.GetShadowMapTexture();

	m_pHBlurredShadowMap->SetRenderTarget();

//	pd3dDev->SetVertexShader( NULL );
//	pd3dDev->SetPixelShader( NULL );

	ShaderManager *pShaderMgr = m_BlurShader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& rShaderMgr = *pShaderMgr;

	// horizontally blur the shadowmap

	ShaderTechniqueHandle blur_h;
	blur_h.SetTechniqueName( "GaussianBlurH" );
	rShaderMgr.SetTechnique( blur_h );

//	rShaderMgr.SetParam( m_SampleWeights );
//	rShaderMgr.SetParam( m_SampleOffsetsH );

	C2DRect screen_rect( 0, 0, m_ShadowMapSize-1, m_ShadowMapSize-1, 0xFFFFFFFF );

	screen_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	rShaderMgr.SetTexture( 0, pShadowMap );

	LPD3DXEFFECT pEffect = rShaderMgr.GetEffect();
	if( !pEffect )
		return;

	pEffect->CommitChanges();

//	screen_rect.Draw( m_pShadowMap ); // test with fixed-function shader. Remeber floating point buffer does not work with this

	PrimitiveRenderer().RenderRect( rShaderMgr, screen_rect );

/*	UINT passes;
	pEffect->Begin(&passes,0);
	for( UINT i=0; i<passes; i++ )
	{
		pEffect->BeginPass(i);
		screen_rect.draw();
		pEffect->EndPass();
	}
	pEffect->End();
*/
	m_pHBlurredShadowMap->ResetRenderTarget();

	// vertically blur the shadowmap

	// save the current RT
	LPDIRECT3DSURFACE9 pOrigSurf;
	hr = pd3dDev->GetRenderTarget( 0, &pOrigSurf );

	LPDIRECT3DSURFACE9 pSurf;
	hr = pShadowMap->GetSurfaceLevel( 0, &pSurf );

	hr = pd3dDev->SetRenderTarget( 0, pSurf );

	ShaderTechniqueHandle blur_v;
	blur_v.SetTechniqueName( "GaussianBlurV" );
	rShaderMgr.SetTechnique( blur_v );

//	rShaderMgr.SetParam( m_SampleOffsetsV );

	hr = rShaderMgr.SetTexture( 0, m_pHBlurredShadowMap->GetRenderTargetTexture() );

	hr = rShaderMgr.GetEffect()->CommitChanges();

//	screen_rect.Draw( m_pHBlurredShadowMap->GetRenderTargetTexture() ); // test with fixed-function shader

	PrimitiveRenderer().RenderRect( rShaderMgr, screen_rect );
/*
	pEffect->Begin(&passes,0);
	for( UINT i=0; i<passes; i++ )
	{
		pEffect->BeginPass(i);
		screen_rect.draw();
		pEffect->EndPass();
	}
	pEffect->End();
*/
	// restore the original RT
	hr = pd3dDev->SetRenderTarget( 0, pOrigSurf );


//	m_pBlurredShadowMap->SetRenderTarget();

//	m_pBlurredShadowMap->ResetRenderTarget();

//	pd3dDev->SetRenderTarget( 0, prev_target );

//	if( FAILED(hr) )
//		return;
}


void VarianceShadowMapManager::BeginSceneDepthMap()
{
	ShadowMapManager::BeginSceneDepthMap();
/*
	CShaderParamFloatArray farclip = CShaderParamFloatArray( "g_fFarClip" );
	farclip.Parameter().resize( 1 );
	farclip.Parameter()[0] = m_SceneCamera.GetFarClip();

	m_Shader.GetShaderManager()->SetParam( farclip );*/
}


/*
void VarianceShadowMapManager::SetDefault()
{
	ShadowMapManager::SetDefault();

	// set default light direction & position
	Vector3 vLightDir =  Vector3(-0.56568f, -0.70711f, -0.42426f);
	Vector3 vLightPos =  Vector3( 5.0f, 10.0f,  6.0f );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir( vLightDir ) );
	m_LightCamera.SetPosition( vLightPos );
//	m_LightCamera.SetNearClip( 0.1f );
//	m_LightCamera.SetFarClip( 100.0f );
}
*/


void VarianceShadowMapManager::ReleaseGraphicsResources()
{
	ShadowMapManager::ReleaseGraphicsResources();

	// release blur shader
}


void VarianceShadowMapManager::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	ShadowMapManager::LoadGraphicsResources( rParam );

	Init();
}


} // namespace amorphous
