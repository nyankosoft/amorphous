#include "ShadowMaps.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/LightStructs.h"
#include "3DCommon/CubeMapManager.h"
#include "3DCommon/Shader/ShaderManagerHub.h"
#include "3DCommon/3DGameMath.h"
#include "Support/Log/DefaultLog.h"

using namespace std;
using namespace boost;


CShadowMap::~CShadowMap()
{
	ReleaseTextures();
}


D3DFORMAT GetShadowMapTextureFormat() { return D3DFMT_R32F; }

bool CFlatShadowMap::CreateShadowMapTextures()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

	// Restore the effect variables
//	V_RETURN( m_pEffect->SetVector( "g_vLightDiffuse", (D3DXVECTOR4 *)&m_Light.Diffuse ) );
//	V_RETURN( m_pEffect->SetFloat( "g_fCosTheta", cosf( g_Light.Theta ) ) );

	D3DFORMAT format = GetShadowMapTextureFormat();

	// Create the shadow map texture
	hr = pd3dDevice->CreateTexture( m_ShadowMapSize, m_ShadowMapSize,
									1,
									D3DUSAGE_RENDERTARGET,
									format, // Color argument of Clear() does not work if D3DFMT_R32F is used?
									// D3DFMT_A8R8G8B8, // use this to render the shadowmap texture for debugging
									D3DPOOL_DEFAULT,
									&m_pShadowMap,
									NULL );

	if( FAILED(hr) )
		return false;

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
	LPDIRECT3DSURFACE9 pOriginalDepthSurface;
	hr = pd3dDevice->GetDepthStencilSurface( &pOriginalDepthSurface );
	D3DSURFACE_DESC surface_desc;
	pOriginalDepthSurface->GetDesc( &surface_desc );
	hr = pd3dDevice->CreateDepthStencilSurface( m_ShadowMapSize,
												m_ShadowMapSize,
//												d3dSettings.pp.AutoDepthStencilFormat,
												surface_desc.Format,
												D3DMULTISAMPLE_NONE,
												0,
												TRUE,
												&m_pShadowMapDepthBuffer,
												NULL );

	if( FAILED(hr) )
		return false;

	// Initialize the shadow projection matrix
//	D3DXMatrixPerspectiveFovLH( &m_mShadowProj, g_fLightFov, 1, 0.01f, 100.0f);

	return true;
}


void CFlatShadowMap::UpdateLightPositionAndDirection()
{
	HRESULT hr;
	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();

	D3DXVECTOR3 vWorldLightPos = m_LightCamera.GetPosition();
	D3DXVECTOR3 vWorldLightDir = m_LightCamera.GetFrontDirection();

	hr = pEffect->SetFloatArray( "g_vLightPos", (float *)&vWorldLightPos, 3 );
	hr = pEffect->SetFloatArray( "g_vLightDir", (float *)&vWorldLightDir, 3 );
/*
	// set light pos and dir in scene camera space
	D3DXMATRIX matSceneCamView;
	m_SceneCamera.GetCameraMatrix( matSceneCamView );

	D3DXVECTOR3 vWorldLightPos = m_LightCamera.GetPosition();
	D3DXVECTOR3 vWorldLightDir = m_LightCamera.GetFrontDirection();

	D3DXVECTOR3 vViewLightPos, vViewLightDir;
	D3DXVec3TransformCoord( &vViewLightPos, &vWorldLightPos, &matSceneCamView );

	// Apply only the rotation to direction vector
	// - set translation to zero
	matSceneCamView._41 = matSceneCamView._42 = matSceneCamView._43 = 0;
	D3DXVec3TransformCoord( &vViewLightDir, &vWorldLightDir, &matSceneCamView );

	hr = pEffect->SetFloatArray( "g_vLightPos", (float *)&vViewLightPos, 3 );
	hr = pEffect->SetFloatArray( "g_vLightDir", (float *)&vViewLightDir, 3 );
*/
}


void CFlatShadowMap::BeginSceneShadowMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// set shadow map texture as a render target

	LPDIRECT3DSURFACE9 pShadowSurf;
	if( SUCCEEDED( m_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf ) ) )
	{
		hr = pd3dDev->SetRenderTarget( 0, pShadowSurf );
		SAFE_RELEASE( pShadowSurf );
	}

	hr = pd3dDev->SetDepthStencilSurface( m_pShadowMapDepthBuffer );

	ShaderManagerHub.PushViewAndProjectionMatrices( m_LightCamera );

	LPD3DXEFFECT pEffect = m_Shader.GetShaderManager()->GetEffect();
	D3DXMATRIX matWorldToLightProj, matView, matProj;
	m_LightCamera.GetCameraMatrix( matView );
	m_LightCamera.GetProjectionMatrix( matProj );
	D3DXMatrixMultiply( &matWorldToLightProj, &matView, &matProj );
	hr = pEffect->SetMatrix( "g_mWorldToLightProj", &matWorldToLightProj );

	// update light position and direction, etc.
	UpdateLightPositionAndDirection();

	hr = pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF80FF80, 1.0f, 0 );

	pd3dDev->BeginScene();
}


void CFlatShadowMap::EndSceneShadowMap()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->EndScene();
}


void CShadowMap::RenderSceneToShadowMap( CCamera& camera )
{
	if( !m_pSceneRenderer )
		return;

	// set shadow map texture, etc.
	BeginSceneShadowMap();

	m_pSceneRenderer->RenderSceneToShadowMap( camera );

	EndSceneShadowMap();
}

void CShadowMap::RenderShadowReceivers( CCamera& camera )
{
	if( !m_pSceneRenderer )
		return;

	m_pSceneRenderer->RenderShadowReceivers( camera );
}

void CShadowMap::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	CreateShadowMapTextures();
}



//============================================================================
// CFlatShadowMap
//============================================================================

void CFlatShadowMap::ReleaseTextures()
{
	SAFE_RELEASE( m_pShadowMap );
	SAFE_RELEASE( m_pShadowMapDepthBuffer );
}



//============================================================================
// CDirectionalLightShadowMap
//============================================================================

void CDirectionalLightShadowMap::UpdateLight( CDirectionalLight& light )
{
	m_LightCamera.SetPosition( light.vPseudoPosition );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir(light.vDirection) );
}



//============================================================================
// CSpotLightShadowMap
//============================================================================



//============================================================================
// CPointLightShadowMap
//============================================================================

CPointLightShadowMap::CPointLightShadowMap()
{
	m_pCubeShadowMapManager = new CCubeMapManager();
	m_pCubeShadowMapManager->SetCubeMapSceneRenderer( &m_CubeShadowMapSceneRenderer );
//	m_CubeShadowMapSceneRenderer.SetRenderer( m_pSceneRenderer );
//	m_pCubeShadowMapManager->SetSceneRenderer( m_pCubeShadowMapSceneRenderer );
}


CPointLightShadowMap::~CPointLightShadowMap()
{
	SafeDelete( m_pCubeShadowMapManager );
}


bool CPointLightShadowMap::CreateShadowMapTextures()
{
	TextureFormat::Format fmt = TextureFormat::R16F;

	m_pCubeShadowMapManager->Init( m_ShadowMapSize, fmt );

	return true;
}


void CPointLightShadowMap::RenderSceneToShadowMap()
{
	m_pCubeShadowMapManager->RenderToCubeMap();

//	CCamera cam;
//	m_CubeShadowMapSceneRenderer.RenderSceneToCubeMap( cam );
}


void CPointLightShadowMap::BeginSceneShadowMap()
{
}


void CPointLightShadowMap::EndSceneShadowMap()
{
}


void CPointLightShadowMap::UpdateLight( CPointLight& light )
{
	m_LightCamera.SetPosition( light.vPosition );
}
