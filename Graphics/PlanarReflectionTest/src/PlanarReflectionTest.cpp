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
#include "Support/CameraController_Win32.hpp"

using namespace std;
using namespace boost;


extern CPlatformDependentCameraController g_CameraController;


inline Matrix44 CreateMirrorMatrix( const SPlane& plane )
{
	Matrix44 out;
	const Vector3& n = plane.normal;
	const float k = plane.dist;

	out(0,0) = 1-2*n.x*n.x;  out(0,1) =  -2*n.x*n.y;  out(0,2) =  -2*n.x*n.z;  out(0,3) = 2*n.x*k;
	out(1,0) =  -2*n.y*n.x;  out(1,1) = 1-2*n.y*n.y;  out(1,2) =  -2*n.y*n.z;  out(1,3) = 2*n.y*k;
	out(2,0) =  -2*n.z*n.x;  out(2,1) =  -2*n.z*n.y;  out(2,2) = 1-2*n.z*n.z;  out(2,3) = 2*n.z*k;
	out(3,0) = 0;  out(3,1) = 0;  out(3,2) = 0;  out(3,3) = 1;

	return out;
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

	shader_loaded = m_PlanarReflectionShader.Load( "./shaders/PerPixelSingleHSDirectionalLight_PR.fx" );
	
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

	return 0;
}


void CPlanarReflectionTest::Update( float dt )
{
}


void CPlanarReflectionTest::RenderReflectionSourceMeshes()
{
	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

	Matrix44 matWorld = Matrix44Identity();
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	CShaderManager& shader_mgr = *pShaderMgr;

//	RenderAsSkybox( m_SkyboxMesh, g_CameraController.GetPosition() );

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

	CShaderTechniqueHandle tech;
	tech.SetTechniqueName( "Default" );
	shader_mgr.SetTechnique( tech );

	shader_mgr.SetTexture( 1, m_pTextureRenderTarget->GetRenderTargetTexture() );

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
	RenderAsSkybox( m_SkyboxMesh, g_CameraController.GetPosition() );

	// Render mirrored scene to the texture render target

	m_pTextureRenderTarget->SetRenderTarget();

	Matrix44 mirror = CreateMirrorMatrix( SPlane( Vector3(0,1,0), 0 ) );

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
	RenderReflectionSourceMeshes();

	ShaderManagerHub.PopViewAndProjectionMatrices();

	m_pTextureRenderTarget->ResetRenderTarget();

	// Render the scene that has planar reflection
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );
	RenderReflectionSourceMeshes();

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
