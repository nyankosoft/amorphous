#include "VarianceShadowMapManager.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManagerHub.h"
#include "3DCommon/2DTexRect.h"
#include "3DCommon/TextureRenderTarget.h"
#include "Support/Log/DefaultLog.h"

using namespace std;
using namespace boost;


//std::string CVarianceShadowMapManager::ms_strDefaultVSMShaderFilename = "Shader/VarianceShadowMap.fx";

/*
CVarianceShadowMapManager::SetShaderTechniqueForShadowCaster()
{
}
*/

CVarianceShadowMapManager::CVarianceShadowMapManager()
//:
//m_pHBlurredShadowMap(NULL),
//m_pBlurredShadowMap(NULL)
{
	m_ShadowMapShaderFilename = "Shader/VarianceShadowMap.fx";
}


CVarianceShadowMapManager::CVarianceShadowMapManager( int texture_width, int texture_height )
:
CShadowMapManager(texture_width,texture_height)
//m_pHBlurredShadowMap(NULL),
//m_pBlurredShadowMap(NULL)
{
	m_ShadowMapShaderFilename = "Shader/VarianceShadowMap.fx";
}


bool CVarianceShadowMapManager::Init()
{
	bool base_init = CShadowMapManager::Init();

	if( !base_init )
		return false;

	// Init shader for blur?

	bool loaded = m_BlurShader.Load( "./Shader/BlurVSM.fx" );
	if( !loaded )
		return false;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	CTextureResourceDesc desc;
	desc.Width  = m_iTextureWidth;
	desc.Height = m_iTextureHeight;
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

	LPDIRECT3DSURFACE9 pHBlurred;
	hr = m_pHBlurredShadowMap->GetSurfaceLevel( 0, &pHBlurred );

	if( FAILED(hr) )
		return;

	hr = pd3dDev->SetRenderTarget( 0, &pHBlurred );

	C2DRect screen_rect( 0, 0, m_iTextureWidth-1, m_iTextureHeight-1, 0xFFFFFFFF );

	screen_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	screen_rect.Draw( m_pShadowMap );

	pd3dDev->SetRenderTarget( 0, prev_target );

	if( FAILED(hr) )
		return;

*/
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

CVarianceShadowMapManager::~CVarianceShadowMapManager()
{
	ReleaseGraphicsResources();
}

/*
void CVarianceShadowMapManager::ReleaseTextures()
{}
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
