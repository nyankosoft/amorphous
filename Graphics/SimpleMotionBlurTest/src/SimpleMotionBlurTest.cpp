#include "SimpleMotionBlurTest.hpp"
#include "3DMath/Matrix34.hpp"
#include "Graphics/SimpleMotionBlur.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/Font/BuiltinFonts.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/SkyboxMisc.hpp"
#include "Support/CameraController_Win32.hpp"

using namespace boost;


extern CPlatformDependentCameraController g_CameraController;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CSimpleMotionBlurTest();
}


extern const std::string GetAppTitle()
{
	return std::string("Simple Motion Blur Test");
}



CSimpleMotionBlurTest::CSimpleMotionBlurTest()
{
}


CSimpleMotionBlurTest::~CSimpleMotionBlurTest()
{
}


int CSimpleMotionBlurTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/SimpleMotionBlurTest.fx" );

	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/terrain06.msh";
	mesh_desc.MeshType     = CMeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	g_CameraController.SetPosition( Vector3( 0, 3, 0 ) );

	return 0;
}


void CSimpleMotionBlurTest::Update( float dt )
{
}


void CSimpleMotionBlurTest::RenderScene()
{
	Matrix44 matWorld = Matrix44Identity();
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();

	GraphicsDevice().Enable( RenderStateType::FACE_CULLING );
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	CShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	RenderAsSkybox( m_SkyboxMesh, g_CameraController.GetPosition() );

	shader_mgr.SetWorldTransform( matWorld );

	shader_mgr.SetTechnique( m_MeshTechnique );

	shared_ptr<CBasicMesh> pTerrainMesh = m_TerrainMesh.GetMesh();
	if( pTerrainMesh )
		pTerrainMesh->Render( shader_mgr );
}


void CSimpleMotionBlurTest::Render()
{
	if( !m_pSimpleMotionBlur )
	{
		m_pSimpleMotionBlur.reset( new CSimpleMotionBlur );
		m_pSimpleMotionBlur->InitForScreenSize();
		m_pSimpleMotionBlur->SetBlurWeight( 0.1f );
	}

	m_pSimpleMotionBlur->Begin();

	RenderScene();

	m_pSimpleMotionBlur->End();

	m_pSimpleMotionBlur->Render();
}
