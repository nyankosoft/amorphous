#include "PlanarReflectionTest.hpp"
#include "3DMath/Matrix34.hpp"
//#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/Font/BuiltinFonts.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Graphics/SkyboxMisc.hpp"
#include "Graphics/TextureRenderTarget.hpp"
#include "Support/ParamLoader.hpp"
#include "Support/CameraController_Win32.hpp"
#include "Support/Timer.hpp"

using namespace std;
using namespace boost;


extern CPlatformDependentCameraController g_CameraController;


Vector3 GetMirroredPosition( const Plane& plane, const Vector3& pos )
{
	float d = plane.GetDistanceFromPoint( pos );
	return pos - plane.normal * d * 2.0f;
}



extern CGraphicsTestBase *CreateTestInstance()
{
	return new CPlanarReflectionTest();
}


extern const std::string GetAppTitle()
{
	return string("Planar Reflection Test");
}



CPlanarReflectionTest::CPlanarReflectionTest()
{
}


CPlanarReflectionTest::~CPlanarReflectionTest()
{
}


int CPlanarReflectionTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
//	string shader_path = "./shaders/PlanarReflectionTest.fx";
	string shader_path = "./shaders/PerPixelSingleHSDirectionalLight.fx";
	bool shader_loaded = m_Shader.Load( shader_path );

	string pr_shader_path = "./shaders/PerPixelSingleHSDirectionalLight_PR.fx";
	LoadParamFromFile( "params.txt", "planar_reflection_shader", pr_shader_path );
	shader_loaded = m_PlanarReflectionShader.Load( pr_shader_path );
	
	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/terrain06.msh";
	mesh_desc.MeshType     = CMeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	m_ReflectionSourceMeshes.resize( 1 );
	m_ReflectionSourceMeshes[0].Load( "./models/wall_and_ceiling.msh" );

	m_ReflectiveSurfaceMeshes.resize( 1 );
	m_ReflectiveSurfaceMeshes[0].Load( "./models/floor.msh" );

//	m_TestTexture.Load( "./textures/flare02.dds" );

	m_pTextureRenderTarget = CTextureRenderTarget::Create();
	m_pTextureRenderTarget->InitScreenSizeRenderTarget();

	m_PerturbationTexture.Load( "./textures/watersurf_nmap.jpg" );

	return 0;
}


void CPlanarReflectionTest::Update( float dt )
{
}


void CPlanarReflectionTest::RenderReflectionSourceMeshes( const Vector3& camera_pos )
{
	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

	RenderAsSkybox( m_SkyboxMesh, camera_pos );

	Matrix44 matWorld = Matrix44Identity();
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	CShaderManager& shader_mgr = *pShaderMgr;

//	Matrix44 matMirror = CreateMirrorMatrix( SPlane( Vector3(0,1,0), 0 ) );
//	matWorld = matMirror * matWorld;

	shader_mgr.SetWorldTransform( matWorld );

	shader_mgr.SetTechnique( m_MeshTechnique );

//	m_TerrainMesh.GetMesh().get()->Render( *pShaderMgr );

	CShaderTechniqueHandle tech;
	tech.SetTechniqueName( "Default" );
	Result::Name res = shader_mgr.SetTechnique( tech );

	for( size_t i=0; i<m_ReflectionSourceMeshes.size(); i++ )
	{
		shared_ptr<CBasicMesh> pMesh = m_ReflectionSourceMeshes[i].GetMesh();
		
		if( !pMesh )
			continue;
		
		pMesh->Render( shader_mgr );
	}
}


void CPlanarReflectionTest::RenderReflectionSurface()
{
	CShaderManager *pShaderMgr = m_PlanarReflectionShader.GetShaderManager();
	if( !pShaderMgr )
		return;

	CShaderManager& shader_mgr = *pShaderMgr;

	shader_mgr.SetTexture( 2, m_PerturbationTexture );

	CShaderTechniqueHandle tech;
	tech.SetTechniqueName( "Default" );
	shader_mgr.SetTechnique( tech );

	shader_mgr.SetTexture( 1, m_pTextureRenderTarget->GetRenderTargetTexture() );

	// shift UV of perturbation texture to make the reflection look like water surface with waves
	vector<float> uv_shift;
	uv_shift.resize( 2 );
	uv_shift[0] = (float)GlobalTimer().GetTime() * 0.05f;
	uv_shift[1] = 0;

	shader_mgr.SetParam( "g_vPerturbationTextureUVShift", uv_shift );

	for( size_t i=0; i<m_ReflectiveSurfaceMeshes.size(); i++ )
	{
		shared_ptr<CBasicMesh> pMesh = m_ReflectiveSurfaceMeshes[i].GetMesh();
		
		if( !pMesh )
			continue;
		
		pMesh->Render( shader_mgr );
	}
}


void CPlanarReflectionTest::Render()
{
	// Render mirrored scene to the texture render target

	m_pTextureRenderTarget->SetRenderTarget();

	Matrix44 mirror = Matrix44Mirror( SPlane( Vector3(0,1,0), 0 ) );

	Matrix44 view = g_Camera.GetCameraMatrix();
	ShaderManagerHub.PushViewAndProjectionMatrices( view * mirror, g_Camera.GetProjectionMatrix() );

/*	CShaderManager& ffp_mgr = FixedFunctionPipelineManager();
	Matrix44 view = ffp_mgr.GetViewTransform();
	ffp_mgr.SetViewTransform( view * mirror );

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr )
	{
		view = pShaderMgr->GetViewTransform();
		pShaderMgr->SetViewTransform( view * mirror );
	}*/

	GraphicsDevice().SetCullingMode( CullingMode::CLOCKWISE );
	RenderReflectionSourceMeshes( GetMirroredPosition( Plane(Vector3(0,1,0),0), g_Camera.GetPosition() ) );

	ShaderManagerHub.PopViewAndProjectionMatrices();

	m_pTextureRenderTarget->ResetRenderTarget();

	// Render the scene that has planar reflection
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );
	RenderReflectionSourceMeshes( g_Camera.GetPosition() );

	// Render surface that does planar reflection
	RenderReflectionSurface();

	// lens flares

//	pShaderMgr->SetTechnique( m_DefaultTechnique );

//	rect.Draw();
}


void CPlanarReflectionTest::UpdateViewTransform( const Matrix44& matView )
{
}


void CPlanarReflectionTest::UpdateProjectionTransform( const Matrix44& matProj )
{
}
