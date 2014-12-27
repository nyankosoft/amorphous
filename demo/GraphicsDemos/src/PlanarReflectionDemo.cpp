#include "PlanarReflectionDemo.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/TextureRenderTarget.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Timer.hpp"

// TODO: make the source code graphics-library independent.
#include "amorphous/Graphics/Direct3D/Direct3D9.hpp"

using std::string;
using std::vector;
using namespace boost;


Vector3 GetMirroredPosition( const Plane& plane, const Vector3& pos )
{
	float d = plane.GetDistanceFromPoint( pos );
	return pos - plane.normal * d * 2.0f;
}


PlanarReflectionDemo::PlanarReflectionDemo()
:
m_fReflection(0.0f),
m_RenderSkybox(true),
m_Perturbation(false),
m_RenderMirroredScene(true)
{
	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0, 1, -1 ) );
}


PlanarReflectionDemo::~PlanarReflectionDemo()
{
}


PlanarReflectionOption::Name PlanarReflectionDemo::GetReflectionType()
{
	string directory_path = "PlanarReflectionDemo/";
	string reflection_type = "flat";
	LoadParamFromFile( directory_path + "params.txt", "reflection_type", reflection_type );

	PlanarReflectionOption::Name ret = PlanarReflectionOption::FLAT;

	if( reflection_type == "none" )           ret = PlanarReflectionOption::NONE;
	else if( reflection_type == "flat" )      ret = PlanarReflectionOption::FLAT;
	else if( reflection_type == "perturbed" ) ret = PlanarReflectionOption::PERTURBED;

	m_Perturbation = (ret == PlanarReflectionOption::PERTURBED);

	return ret;
}


int PlanarReflectionDemo::Init()
{
	string directory_path = "PlanarReflectionDemo/";

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "Default" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );
/*
	// initialize shader
//	string shader_path = directory_path + "shaders/PlanarReflectionDemo.fx";
	string shader_path = directory_path + "shaders/PerPixelSingleHSDirectionalLight.fx";
	bool shader_loaded = m_Shader.Load( shader_path );
*/
	GenericShaderDesc gs_desc;
	gs_desc.Specular = SpecularSource::NONE;
	gs_desc.NumDirectionalLights = 1;
	ShaderResourceDesc shader_desc;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gs_desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

	bool use_embedded_planar_reflection_shader = false;
	LoadParamFromFile( directory_path + "params.txt", "use_embedded_planar_reflection_shader", use_embedded_planar_reflection_shader );
	if( use_embedded_planar_reflection_shader )
	{
		GenericShaderDesc prs_desc = m_PlanarReflectionShaderDesc;

		prs_desc.Specular = SpecularSource::NONE;
		prs_desc.NumDirectionalLights = 1;
		prs_desc.PlanarReflection = GetReflectionType();
		ShaderResourceDesc sd;
		sd.pShaderGenerator.reset( new GenericShaderGenerator(prs_desc) );
		shader_loaded = m_PlanarReflectionShader.Load( sd );
	}
	else
	{
		string pr_shader_path = "shaders/PerPixelSingleHSDirectionalLight_PR.fx";
		LoadParamFromFile( directory_path + "params.txt", "planar_reflection_shader", pr_shader_path );
		shader_loaded = m_PlanarReflectionShader.Load( directory_path + pr_shader_path );

		if( pr_shader_path.find("WS") != string::npos )
			m_Perturbation = true; // WS indicates the shader is for water surface
	}
	
	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( directory_path + "textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
//	MeshResourceDesc mesh_desc;
//	mesh_desc.ResourcePath = directory_path + "models/terrain06.msh";
//	mesh_desc.MeshType     = MeshType::BASIC;
//	m_TerrainMesh.Load( mesh_desc );

	m_ReflectionSourceMeshes.resize( 1 );
	m_ReflectionSourceMeshes[0].Load( directory_path + "models/wall_and_ceiling.msh" );

	m_ReflectiveSurfaceMeshes.resize( 1 );
	m_ReflectiveSurfaceMeshes[0].Load( directory_path + "models/floor.msh" );

//	m_TestTexture.Load( "./textures/flare02.dds" );

	m_pTextureRenderTarget = TextureRenderTarget::Create();
	m_pTextureRenderTarget->InitScreenSizeRenderTarget();

	m_PerturbationTexture.Load( directory_path + "textures/watersurf_nmap.jpg" );

	return 0;
}


void PlanarReflectionDemo::Update( float dt )
{
}


void PlanarReflectionDemo::UpdateLight( ShaderManager& shader_mgr )
{
	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

	HemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 0.9f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.2f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.5f, -0.9f ) );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	pShaderLightMgr->CommitChanges();
}


void PlanarReflectionDemo::SetLowLight( ShaderManager& shader_mgr )
{
	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

	HemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 0.01f, 0.01f, 0.01f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.00f, 0.00f, 0.00f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.5f, -0.9f ) );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	pShaderLightMgr->CommitChanges();
}


void PlanarReflectionDemo::UpdateReflection()
{
	ShaderManager *pShaderMgr = m_PlanarReflectionShader.GetShaderManager();
	if( !pShaderMgr )
		return;

	pShaderMgr->SetParam( "g_fPlanarReflection", m_fReflection );
}


void PlanarReflectionDemo::RenderReflectionSourceMeshes( const Matrix34& camera_pose, CullingMode::Name culling_mode )
{
//	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

	GraphicsDevice().SetCullingMode( culling_mode );

	if( m_RenderSkybox )
		RenderAsSkybox( m_SkyboxMesh, camera_pose );

	GraphicsDevice().SetCullingMode( culling_mode );

	Matrix44 matWorld = Matrix44Identity();
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	shader_mgr.SetWorldTransform( matWorld );

	UpdateLight( shader_mgr );

	Result::Name res = shader_mgr.SetTechnique( m_MeshTechnique );

	for( size_t i=0; i<m_ReflectionSourceMeshes.size(); i++ )
	{
		shared_ptr<BasicMesh> pMesh = m_ReflectionSourceMeshes[i].GetMesh();
		
		if( !pMesh )
			continue;
		
		pMesh->Render( shader_mgr );
	}
}


void PlanarReflectionDemo::RenderReflectionSurface()
{
	ShaderManager *pShaderMgr = m_PlanarReflectionShader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	if( m_Perturbation )
		shader_mgr.SetTexture( 2, m_PerturbationTexture );

	ShaderTechniqueHandle tech;
	tech.SetTechniqueName( "Default" );
	shader_mgr.SetTechnique( tech );

	shader_mgr.SetTexture( 1, m_pTextureRenderTarget->GetRenderTargetTexture() );

	bool overwrite_reflection_texture = false;
	if( overwrite_reflection_texture )
	{
		static bool s_init = false;
		static TextureHandle s_tex;
		if(!s_init)
		{
			bool loaded = s_tex.Load( ".debug/gridsheet_ex.png" );
			s_init = true;
		}
		shader_mgr.SetTexture( 1, s_tex );
	}

	// shift UV of perturbation texture to make the reflection look like water surface with waves
	vector<float> uv_shift;
	uv_shift.resize( 2 );
	uv_shift[0] = (float)GlobalTimer().GetTime() * 0.05f;
	uv_shift[1] = 0;

	shader_mgr.SetParam( "g_vPerturbationTextureUVShift", uv_shift );

	if( m_Perturbation )
	{
		SetLowLight( shader_mgr ); // Water surface looks better with a reduced light
	}
	else
		UpdateLight( shader_mgr );

	// In Direct3D mode, this makes water surface a lot brighter. Why?
	shader_mgr.SetWorldTransform( Matrix44Identity() );

	shader_mgr.SetTechnique( tech );

	for( size_t i=0; i<m_ReflectiveSurfaceMeshes.size(); i++ )
	{
		shared_ptr<BasicMesh> pMesh = m_ReflectiveSurfaceMeshes[i].GetMesh();
		
		if( !pMesh )
			continue;
		
		pMesh->Render( shader_mgr );
	}

//	shader_mgr.SetTexture( 1, TextureHandle() );
}

/*
void RenderReflectionClipPlane() //const Plane& reflection_plane )
{
	TLVertex<U32> verts[4] =
	{
		TLVertex<U32>( Vector3(-100, 0, 100), 0xFFFF0000 ),//0x00000000 ),
		TLVertex<U32>( Vector3( 100, 0, 100), 0xFFFF0000 ),//0x00000000 ),
		TLVertex<U32>( Vector3( 100, 0,-100), 0xFFFF0000 ),//0x00000000 ),
		TLVertex<U32>( Vector3(-100, 0,-100), 0xFFFF0000 ),//0x00000000 )
	};

	GraphicsDevice().Disable( RenderStateType::ALPHA_TEST );
	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::One );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

	DrawPrimitives( PrimitiveType::TRIANGLE_FAN, verts, 4 );
}
*/


void SetClipPlaneViaD3DXFunctions( const Camera& camera )// const Plane& reflection_plane )
{
	D3DXPLANE clipPlane;
	D3DXVECTOR3 point( 0 , 0, 0 );
	D3DXVECTOR3 normal( 0, -1, 0 ); // negative of the normal vector.
	// create and normalize the plane
	D3DXPlaneFromPointNormal(&clipPlane,&point,&normal);
	D3DXPlaneNormalize(&clipPlane,&clipPlane);

	// To transform a plane from world space to view space there is a methode D3DXPlaneTransform
	// but the peculiar thing about this method is that it takes the inverse transpose of the viewprojection matrix

	Matrix44 proj_view = camera.GetProjectionMatrix() * camera.GetCameraMatrix();
	D3DXMATRIXA16 matrix, view_proj;
	proj_view.GetRowMajorMatrix44( (float *)&view_proj );

	D3DXMatrixInverse(&matrix, NULL, &view_proj); // second parameter is an out parameter for the determinant
	D3DXMatrixTranspose(&matrix, &matrix);

	D3DXPLANE viewSpacePlane;
	D3DXPlaneTransform(&viewSpacePlane, &clipPlane, &matrix);

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;
	hr = pd3dDev->SetClipPlane( 0, (float *)&viewSpacePlane );
	hr = pd3dDev->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
}

/*
void SetClipPlane( const Camera& camera, const Plane& reflection_plane )
{
	Plane src_plane( reflection_plane );
	src_plane.Flip();

	Matrix44 proj_view = camera.GetProjectionMatrix() * camera.GetCameraMatrix();
	Matrix44 inv_proj_view = proj_view.GetInverse();
	Matrix44 inv_transpose_proj_view = Matrix44Transpose( inv_proj_view );

	Matrix44 res = proj_view * inv_proj_view;

	Plane reflection_plane_in_clipping_space;
	Vector4 src_plane_normal4( src_plane.normal.x, src_plane.normal.y, src_plane.normal.z, 1.0f );
	Vector4 reflection_plane_in_clipping_space_normal4 = inv_transpose_proj_view * src_plane_normal4;
	reflection_plane_in_clipping_space.normal = inv_transpose_proj_view * src_plane.normal;
	Vector3 pos_on_plane = src_plane.normal * src_plane.dist;
	Vector3 pos_on_clipping_space_plane = inv_transpose_proj_view * pos_on_plane;
	reflection_plane_in_clipping_space.dist = Vec3Dot( pos_on_clipping_space_plane, reflection_plane_in_clipping_space.normal );

	float plane_coefficients[] =
	{
		reflection_plane_in_clipping_space.normal.x,
		reflection_plane_in_clipping_space.normal.y,
		reflection_plane_in_clipping_space.normal.z,
		reflection_plane_in_clipping_space.dist * (-1.0f)
	};

	// For fixed function pipeline
//	float plane_coefficients[] =
//	{
//		reflection_plane.normal.x,
//		reflection_plane.normal.y,
//		reflection_plane.normal.z,
//		reflection_plane.dist * (-1.0f)
//	};

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;
	hr = pd3dDev->SetClipPlane( 0, plane_coefficients );
	hr = pd3dDev->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
}
*/


void PlanarReflectionDemo::RenderMirroredSceneToTextureRenderTarget()
{
	m_pTextureRenderTarget->SetRenderTarget();

	Plane reflection_plane( Vector3(0,1,0), 0 );
	Matrix44 mirror = Matrix44Mirror( reflection_plane );

	Matrix44 view = GetCurrentCamera().GetCameraMatrix();
	GetShaderManagerHub().PushViewAndProjectionMatrices( view * mirror, GetCurrentCamera().GetProjectionMatrix() );

/*	ShaderManager& ffp_mgr = FixedFunctionPipelineManager();
	Matrix44 view = ffp_mgr.GetViewTransform();
	ffp_mgr.SetViewTransform( view * mirror );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr )
	{
		view = pShaderMgr->GetViewTransform();
		pShaderMgr->SetViewTransform( view * mirror );
	}*/

	GraphicsDevice().SetClipPlane( 0, reflection_plane );
	GraphicsDevice().EnableClipPlane( 0 );
	GraphicsDevice().UpdateViewProjectionTransformsForClipPlane(
		0,
		GetCurrentCamera().GetCameraMatrix(),
		GetCurrentCamera().GetProjectionMatrix()
		);

//	GraphicsDevice().SetCullingMode( CullingMode::CLOCKWISE );

//	RenderReflectionClipPlane( /*reflection_plane*/ );
/*	SetClipPlane( GetCurrentCamera(), reflection_plane );
	SetClipPlaneViaD3DXFunctions( GetCurrentCamera() );*/

//	RenderReflectionSourceMeshes( GetCurrentCamera().GetPose(), CullingMode::CLOCKWISE );
//	RenderReflectionSourceMeshes( GetMirroredPose( Plane(Vector3(0,1,0),0), GetCurrentCamera().GetPose() ), CullingMode::CLOCKWISE );
//	RenderReflectionSourceMeshes( GetMirroredPosition( Plane(Vector3(0,1,0),0), GetCurrentCamera().GetPosition() ), CullingMode::CLOCKWISE );
//	RenderReflectionSourceMeshes( GetCameraPoseFromCameraMatrix( mirror ), CullingMode::CLOCKWISE );
	Matrix34 camera_pose = GetCurrentCamera().GetPose();
	RenderReflectionSourceMeshes( Matrix34( GetMirroredPosition(Plane(Vector3(0,1,0),0),camera_pose.vPosition), camera_pose.matOrient ), CullingMode::CLOCKWISE );

//	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
//	HRESULT hr = pd3dDev->SetRenderState( D3DRS_CLIPPLANEENABLE, 0 );

	GraphicsDevice().DisableClipPlane( 0 );

	GetShaderManagerHub().PopViewAndProjectionMatrices();

	m_pTextureRenderTarget->ResetRenderTarget();
}


void PlanarReflectionDemo::Render()
{
	// Render mirrored scene to the texture render target
	if( m_RenderMirroredScene )
		RenderMirroredSceneToTextureRenderTarget();

	// Render the scene that has planar reflection
//	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );
	RenderReflectionSourceMeshes( GetCurrentCamera().GetPose(), CullingMode::COUNTERCLOCKWISE );

	// Render surface that does planar reflection
	RenderReflectionSurface();

	// lens flares

//	pShaderMgr->SetTechnique( m_DefaultTechnique );

//	rect.Draw();
}


void PlanarReflectionDemo::UpdateViewTransform( const Matrix44& matView )
{
}


void PlanarReflectionDemo::UpdateProjectionTransform( const Matrix44& matProj )
{
}


void PlanarReflectionDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fReflection += 0.05f;
			UpdateReflection();
		}
		break;
	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fReflection -= 0.05f;
			UpdateReflection();
		}
		break;
	case 'M':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_RenderMirroredScene = !m_RenderMirroredScene;
		}
		break;
	case 'V':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_RenderSkybox = !m_RenderSkybox;
		}
		break;
	case 'T':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_pTextureRenderTarget )
				m_pTextureRenderTarget->GetRenderTargetTexture().SaveTextureToImageFile( ".debug/mirrored_scene.png" );
		}
		break;
	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}
