#include "ShadowMaps.hpp"
#include "2DPrimitive/2DRect.hpp"
#include "LightStructs.hpp"
#include "CubeMapManager.hpp"
#include "TextureRenderTarget.hpp"
#include "Shader/ShaderManagerHub.hpp"
#include "Shader/ShaderManager.hpp"
#include "Shader/MiscShaderGenerator.hpp"
#include "Meshgenerators/Meshgenerators.hpp"
#include "Mesh/BasicMesh.hpp"
#include "amorphous/3DMath/3DGameMath.hpp"
#include "amorphous/3DMath/MatrixConversions.hpp"
#include "amorphous/Support/SafeDelete.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Vec3_StringAux.hpp"
#include "amorphous/Support/ParamLoader.hpp"


namespace amorphous
{

using std::string;


float g_fShadowMapNearClip = 0.1f;
float g_fShadowMapFarClip = 100.0f;


int ShadowMap::ms_DebugShadowMap = 0;

ShadowMap::~ShadowMap()
{
	ReleaseTextures();
}


void ShadowMap::SaveShadowMapTextureToFile( const std::string& file_or_directory_path )
{
	string filepath;
	if( file_or_directory_path.rfind("/") == file_or_directory_path.length() - 1 )
	{
		string directory_path = file_or_directory_path;

		filepath
			= directory_path
			+ CreateTextureFilename();
	}
	else
	{
		filepath = file_or_directory_path;
	}

	SaveShadowMapTextureToFileInternal( filepath );

//	HRESULT hr = D3DXSaveTextureToFile( filepath.c_str(), D3DXIFF_DDS, m_pShadowMap, NULL );
}


void ShadowMap::RenderSceneToShadowMap( Camera& camera )
{
	if( !m_pSceneRenderer )
		return;

	// set shadow map texture, etc.
	BeginSceneShadowMap();

	ShaderHandle shaders[] = { m_Shader, m_Shader };
	ShaderTechniqueHandle shader_techniques[] = { ShaderTechniqueHandle(), ShaderTechniqueHandle() };

	m_pSceneRenderer->RenderSceneToShadowMap( m_LightCamera, shaders, shader_techniques );

	EndSceneShadowMap();
}


void ShadowMap::RenderShadowReceivers( Camera& camera )
{
	if( !m_pSceneRenderer )
		return;

	BeginSceneShadowReceivers();

	ShaderHandle shaders[] = { m_Shader, m_Shader };
	ShaderTechniqueHandle shader_techniques[] = { ShaderTechniqueHandle(), ShaderTechniqueHandle() };

	m_pSceneRenderer->RenderShadowReceivers( camera, shaders, shader_techniques );
}


void ShadowMap::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	CreateShadowMapTextures();
}



//============================================================================
// FlatShadowMap
//============================================================================

FlatShadowMap::FlatShadowMap()
{
	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
//	pBoxMeshGenerator->SetTexturePath( texture_filepath );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;
	bool loaded = m_ShadowCasterBoundingBox.Load( mesh_desc );
}


ShaderTechniqueHandle& FlatShadowMap::ShadowMapTechnique( VertexBlendType::Name vertex_blend_type )
{
	switch( vertex_blend_type )
	{
	case VertexBlendType::NONE:
	default:
		return m_ShadowMapTechnique;
	case VertexBlendType::QUATERNION_AND_VECTOR3:
		return m_VertexBlendShadowMapTechnique;
//	case VertexBlendType::MATRIX:
//		return ???;
	}
}


ShaderTechniqueHandle& FlatShadowMap::DepthTestTechnique( VertexBlendType::Name vertex_blend_type )
{
	switch( vertex_blend_type )
	{
	case VertexBlendType::NONE:
	default:
		return m_DepthTestTechnique;
	case VertexBlendType::QUATERNION_AND_VECTOR3:
		return m_VertexBlendDepthTestTechnique;
//	case VertexBlendType::MATRIX:
//		return ???;
	}
}


bool FlatShadowMap::CreateShadowMapTextures()
{
/*	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

	// Restore the effect variables
//	V_RETURN( m_pEffect->SetVector( "g_vLightDiffuse", (D3DXVECTOR4 *)&m_Light.Diffuse ) );
//	V_RETURN( m_pEffect->SetFloat( "g_fCosTheta", cosf( g_Light.Theta ) ) );

	D3DFORMAT format = ConvertTextureFormatToD3DFORMAT( GetShadowMapTextureFormat() );

	// Create the shadow map texture
	hr = pd3dDevice->CreateTexture( m_ShadowMapSize, m_ShadowMapSize,
									1,
									D3DUSAGE_RENDERTARGET,
									format, // Color argument of Clear() does not work if D3DFMT_R32F is used?
									//D3DFMT_A8R8G8B8, // use this to render the shadowmap texture for debugging
									D3DPOOL_DEFAULT,
									&m_pShadowMap,
									NULL );

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( " Failed to create shadowmap texture" );
		return false;
	}

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
	{
		LOG_PRINT_ERROR( " Failed to create depth buffer for shadowmap texture" );
		return false;
	}
*/

	// Shadow map rendering test
//	m_ShadowMapTextureFormat = TextureFormat::A8R8G8B8;

	uint option_flags = 0;
	m_pShadowmapRenderTarget = TextureRenderTarget::Create();
	m_pShadowmapRenderTarget->Init(
		m_ShadowMapSize,
		m_ShadowMapSize,
		GetShadowMapTextureFormat(),
		0
		);

	return true;
}


void FlatShadowMap::UpdateLightPositionAndDirection()
{
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	Vector3 vWorldLightPos = m_LightCamera.GetPosition();
	Vector3 vWorldLightDir = m_LightCamera.GetFrontDirection();

//	if( m_UseLightPosInWorldSpace )
	if( true )
	{
		shader_mgr.SetParam( "g_vLightPos", vWorldLightPos );
		shader_mgr.SetParam( "g_vLightDir", vWorldLightDir );
	}
	else
	{
		// set light pos and dir in scene camera space
		Matrix44 scene_cam_view;
		m_pSceneCamera->GetCameraMatrix( scene_cam_view );
//		D3DXMATRIX matSceneCamView;
//		scene_cam_view.GetRowMajorMatrix44( (float *)&matSceneCamView );

		Vector3 vViewLightPos, vViewLightDir;
//		D3DXVec3TransformCoord( &vViewLightPos, &vWorldLightPos, &matSceneCamView );
		vViewLightPos = scene_cam_view * vWorldLightPos;

		// Apply only the rotation to direction vector
		// - set translation to zero
//		matSceneCamView._41 = matSceneCamView._42 = matSceneCamView._43 = 0;
//		D3DXVec3TransformCoord( &vViewLightDir, &vWorldLightDir, &matSceneCamView );
		scene_cam_view(0,3) = 0;
		scene_cam_view(1,3) = 0;
		scene_cam_view(2,3) = 0;
		vViewLightDir = scene_cam_view * vWorldLightDir;

		shader_mgr.SetParam( "g_vLightPos", vViewLightPos );
		shader_mgr.SetParam( "g_vLightDir", vViewLightDir );
	}
}


static float sg_fOrigNearClip = 0;
static float sg_fOrigFarClip = 0;

void FlatShadowMap::BeginSceneShadowMap()
{
	m_pShadowmapRenderTarget->SetBackgroundColor( SFloatRGBAColor( 0.5f, 1.0f, 0.5f, 1.0f ) );

	// Sets the depth stencil surface as well?
	m_pShadowmapRenderTarget->SetRenderTarget();

	UpdateShadowMapSettings();

/*	if( render_shadow_caster_bounding_box )
	{
		const float far_clip = 100.0f;//m_LightCamera.GetFarClip();
		Matrix44 scaling( Matrix44Scaling( 1000.0f, 1000.0f, far_clip ) );
//		scaling(2,3) = far_clip * 0.5f - 5.0f;

		Matrix44 world = ToMatrix44( m_LightCamera.GetPose() ) * scaling;

		RenderBox( world, MeshPolygonDirection::INWARD );
	}*/
}


void FlatShadowMap::EndSceneShadowMap()
{
	GetShaderManagerHub().PopViewAndProjectionMatrices();

	if( m_pShadowmapRenderTarget )
		m_pShadowmapRenderTarget->ResetRenderTarget();

//	ResetShadowMapSettings();

	// necessary only for spotlight shadow map
	m_LightCamera.SetNearClip( sg_fOrigNearClip );
	m_LightCamera.SetFarClip( sg_fOrigFarClip );
}


void FlatShadowMap::BeginSceneShadowReceivers()
{
	// set the shadow map texture to determine shadowed pixels
//	m_ShaderManager.SetTexture( 3, m_pShadowMap );
//	m_Shader.GetShaderManager()->GetEffect()->SetTexture( "g_txShadow", m_pShadowMap );
	ShaderParameter<TextureParam> tex;
	tex.SetParameterName( "g_txShadow" );
	tex.Parameter().m_Handle = m_pShadowmapRenderTarget->GetRenderTargetTexture();
	m_Shader.GetShaderManager()->SetParam( tex );

	UpdateLightPositionAndDirection();

	SetWorldToLightSpaceTransformMatrix();
}


void FlatShadowMap::ReleaseTextures()
{
//	SAFE_RELEASE( m_pShadowMap );
//	SAFE_RELEASE( m_pShadowMapDepthBuffer );
}


void FlatShadowMap::RenderShadowMapTexture( int sx, int sy, int ex, int ey )
{
	C2DRect rect( sx, sy, ex, ey, 0xFFFFFFFF );
	rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
//	rect.Draw( m_pShadowMap );
	rect.Draw( m_pShadowmapRenderTarget->GetRenderTargetTexture() );
}


std::string FlatShadowMap::CreateTextureFilename()
{
	return fmt_string( "shadowmap_of_directional_light_or_spotlight_pos%s_dir%s.dds",
						to_string(m_LightCamera.GetPosition()).c_str(),
						to_string(m_LightCamera.GetFrontDirection()).c_str() );

}


void FlatShadowMap::SaveShadowMapTextureToFileInternal( const std::string& filepath )
{
//	HRESULT hr = D3DXSaveTextureToFile( filepath.c_str(), D3DXIFF_DDS, m_pShadowMap, NULL );
	if( m_pShadowmapRenderTarget )
		m_pShadowmapRenderTarget->GetRenderTargetTexture().SaveTextureToImageFile( filepath );
}



//============================================================================
// OrthoShadowMap
//============================================================================

float OrthoShadowMap::ms_fCameraShiftDistance = 50.0f;

OrthoShadowMap::OrthoShadowMap()
{
	m_ShadowMapTechnique.SetTechniqueName( "OrthoShadowMap" );
	m_DepthTestTechnique.SetTechniqueName( "OrthoSceneShadowMap" );
	m_VertexBlendShadowMapTechnique.SetTechniqueName( "OrthoShadowMap_VertexBlend" );
	m_VertexBlendDepthTestTechnique.SetTechniqueName( "OrthoSceneShadowMap_VertexBlend" );


	m_Shader = CreateMiscShader( MiscShader::ORTHO_SHADOW_MAP );
}


void OrthoShadowMap::UpdateDirectionalLight( const DirectionalLight& light )
{
	if( ShadowMap::ms_DebugShadowMap )
		UPDATE_PARAM( "debug/graphics_params.txt", "light_cam_shift_distance", ms_fCameraShiftDistance );

	const float light_cam_shift = ms_fCameraShiftDistance;

	Vector3 vLightCameraPos = m_pSceneCamera->GetPosition() - light.vDirection * light_cam_shift;

	m_LightCamera.SetPosition( vLightCameraPos );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir(light.vDirection) );
}


void OrthoShadowMap::UpdateLightPositionAndDirection()
{
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	Vector3 vWorldLightPos = m_LightCamera.GetPosition();
	Vector3 vWorldLightDir = m_LightCamera.GetFrontDirection();

	shader_mgr.SetParam( "g_vLightPos", vWorldLightPos );
	shader_mgr.SetParam( "g_vLightDir", vWorldLightDir );

	// Set the plane that faces along the direction of the light ray
	// - used for orthographic projection
	float dist = Vec3Dot( m_LightCamera.GetPosition(), m_LightCamera.GetFrontDirection() );
	float light_plane[4] = { vWorldLightDir.x, vWorldLightDir.y, vWorldLightDir.z, dist };
//	shader_mgr.GetEffect()->SetFloatArray( "g_vLightPlane", light_plane, 4 );
	shader_mgr.SetParam( "g_vLightPlane", light_plane, 4 );
}


void OrthoShadowMap::UpdateShadowMapSettings()
{
	// orthographic projection
	Matrix44 ortho_proj = Matrix44OrthoLH( 50.0f, 50.0f, 1.0f, 150.0f );
	GetShaderManagerHub().PushViewAndProjectionMatrices( m_LightCamera.GetCameraMatrix(), ortho_proj );

	UpdateLightPositionAndDirection();
}


void OrthoShadowMap::SetWorldToLightSpaceTransformMatrix()
{
	float fOrigCamNearClip = m_LightCamera.GetNearClip();
	float fOrigCamFarClip  = m_LightCamera.GetFarClip();

	if( ShadowMap::ms_DebugShadowMap )
	{
		UPDATE_PARAM( "debug/graphics_params.txt", "dir_light_cam_nearclip", g_fShadowMapNearClip );
		UPDATE_PARAM( "debug/graphics_params.txt", "dir_light_cam_farclip", g_fShadowMapFarClip );
	}

	m_LightCamera.SetNearClip( g_fShadowMapNearClip );
	m_LightCamera.SetFarClip( g_fShadowMapFarClip );

	const Matrix44 proj_view
		= Matrix44OrthoLH( 50.0f, 50.0f, 1.0f, 150.0f )
		* m_LightCamera.GetCameraMatrix();
	m_Shader.GetShaderManager()->SetParam( "g_mWorldToLightProj", proj_view );

	// debug - wanted to check the relations of viewport, FOV, projection matrix, etc.
	Viewport vp;
	GraphicsDevice().GetViewport( vp );


	m_LightCamera.SetNearClip( fOrigCamNearClip );
	m_LightCamera.SetFarClip( fOrigCamFarClip );
}




//============================================================================
// CSpotLightShadowMap
//============================================================================

float SpotlightShadowMap::ms_fCameraShiftDistance = 50.0f;

SpotlightShadowMap::SpotlightShadowMap()
{
	m_ShadowMapTechnique.SetTechniqueName( "ShadowMap" );
	m_DepthTestTechnique.SetTechniqueName( "SceneShadowMap" );
	m_VertexBlendShadowMapTechnique.SetTechniqueName( "ShadowMap_VertexBlend" );
	m_VertexBlendDepthTestTechnique.SetTechniqueName( "SceneShadowMap_VertexBlend" );

	static float near_clip = 0.5f;
	static float far_clip = 100.0f;
	m_LightCamera.SetNearClip( near_clip );
	m_LightCamera.SetFarClip( far_clip );
	m_LightCamera.SetFOV( (float)PI / 4.0f );
	m_LightCamera.SetAspectRatio( 1.0f );

	m_Shader = CreateMiscShader( MiscShader::SPOTLIGHT_SHADOW_MAP );
}


void SpotlightShadowMap::UpdateDirectionalLight( const DirectionalLight& light )
{
	if( ShadowMap::ms_DebugShadowMap )
		UPDATE_PARAM( "debug/graphics_params.txt", "light_cam_shift_distance", ms_fCameraShiftDistance );

	const float light_cam_shift = ms_fCameraShiftDistance;

	Vector3 vLightCameraPos = m_pSceneCamera->GetPosition() - light.vDirection * light_cam_shift;

	m_LightCamera.SetPosition( vLightCameraPos );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir(light.vDirection) );
}


void SpotlightShadowMap::UpdateSpotlight( const Spotlight& light )
{
	m_LightCamera.SetPosition( light.vPosition );
	m_LightCamera.SetOrientation( CreateOrientFromFwdDir(light.vDirection) );
}


void SpotlightShadowMap::UpdateShadowMapSettings()
{
	sg_fOrigNearClip = m_LightCamera.GetNearClip();
	sg_fOrigFarClip = m_LightCamera.GetFarClip();
	m_LightCamera.SetNearClip( g_fShadowMapNearClip );
	m_LightCamera.SetFarClip( g_fShadowMapFarClip );

	// perspective projection
	GetShaderManagerHub().PushViewAndProjectionMatrices( m_LightCamera );

//	SetWorldToLightSpaceTransformMatrix();

	// update light position and direction, etc.
	UpdateLightPositionAndDirection();
}


void SpotlightShadowMap::SetWorldToLightSpaceTransformMatrix()
{
	float fOrigCamNearClip = m_LightCamera.GetNearClip();
	float fOrigCamFarClip  = m_LightCamera.GetFarClip();

	if( ShadowMap::ms_DebugShadowMap )
	{
		UPDATE_PARAM( "debug/graphics_params.txt", "dir_light_cam_nearclip", g_fShadowMapNearClip );
		UPDATE_PARAM( "debug/graphics_params.txt", "dir_light_cam_farclip", g_fShadowMapFarClip );
	}

	m_LightCamera.SetNearClip( g_fShadowMapNearClip );
	m_LightCamera.SetFarClip( g_fShadowMapFarClip );

	const Matrix44 proj_view
		= m_LightCamera.GetProjectionMatrix()
		* m_LightCamera.GetCameraMatrix();

	m_Shader.GetShaderManager()->SetParam( "g_mWorldToLightProj", proj_view );

	// debug - wanted to check the relations of viewport, FOV, projection matrix, etc.
	Viewport vp;
	GraphicsDevice().GetViewport( vp );


	m_LightCamera.SetNearClip( fOrigCamNearClip );
	m_LightCamera.SetFarClip( fOrigCamFarClip );
}



//============================================================================
// PointLightShadowMap
//============================================================================

PointLightShadowMap::PointLightShadowMap()
{
	m_pCubeShadowMapManager.reset( new CubeMapManager() );
	m_pCubeShadowMapManager->SetCubeMapSceneRenderer( &m_CubeShadowMapSceneRenderer );
//	m_CubeShadowMapSceneRenderer.SetRenderer( m_pSceneRenderer );
//	m_pCubeShadowMapManager->SetSceneRenderer( m_pCubeShadowMapSceneRenderer );
}


PointLightShadowMap::~PointLightShadowMap()
{
}


bool PointLightShadowMap::CreateShadowMapTextures()
{
	TextureFormat::Format fmt = TextureFormat::R16F;

	m_pCubeShadowMapManager->Init( m_ShadowMapSize, fmt );

	return true;
}


void PointLightShadowMap::RenderSceneToShadowMap()
{
	m_pCubeShadowMapManager->RenderToCubeMap();

//	Camera cam;
//	m_CubeShadowMapSceneRenderer.RenderSceneToCubeMap( cam );
}


void PointLightShadowMap::BeginSceneShadowMap()
{
}


void PointLightShadowMap::EndSceneShadowMap()
{
}


void PointLightShadowMap::UpdatePointLight( const PointLight& light )
{
	m_LightCamera.SetPosition( light.vPosition );
}


std::string PointLightShadowMap::CreateTextureFilename()
{
	return fmt_string( "shadowmap_of_pointlight_pos%s.dds", to_string(m_LightCamera.GetPosition()).c_str() );
}


void PointLightShadowMap::SaveShadowMapTextureToFileInternal( const std::string& filepath )
{
	m_pCubeShadowMapManager->SaveCubeTextureToFile( filepath );
}


} // namespace amorphous
