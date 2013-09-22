#include "ShadowMapTest.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/MeshModel/PrimitiveShapeMeshes.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/PrimitiveShapeRenderer.hpp"
#include "amorphous/Graphics/TextureRenderTarget.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Support/CameraController_Win32.hpp"
#include "amorphous/Support/ParamLoader.hpp"

using std::string;
using std::vector;
using namespace boost;


static int sg_LightID = 0;


CShadowMapTest::CShadowMapTest()
{
	if( CameraController() )
		CameraController()->SetPosition( Vector3( 0, 2, 0 ) );
}


CShadowMapTest::~CShadowMapTest()
{
}


int CShadowMapTest::Init()
{
//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "Default" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
/*	string shader_path = "./shaders/ShadowMapTest.fx";
//	string shader_path = "./shaders/PerPixelSingleHSDirectionalLight.fx";
	bool shader_loaded = m_Shader.Load( shader_path );
*/
	GenericShaderDesc desc;

	desc.LightingType = ShaderLightingType::PER_PIXEL;
	desc.Specular = SpecularSource::NONE;

	ShaderResourceDesc shader_desc;

//	m_Techniques.resize( shader_descs.size() );
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

//	if( shader_loaded )
//		m_Technique.SetTechniqueName( "Default" );

	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( "ShadowMapDemo/textures/skygrad_slim_01.jpg" );

	string model = "models/shadow_map_test.msh";
	LoadParamFromFile( "ShadowMapDemo/params.txt", "model", model );
	model = "ShadowMapDemo/" + model;
	m_Mesh.Load( model );

	// load the terrain mesh
	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "ShadowMapDemo/models/floor.msh";
	mesh_desc.MeshType     = MeshType::BASIC;
	m_FloorMesh.Load( mesh_desc );
//	m_FloorMesh = CreateBoxMesh();

	m_pShadowMapManager.reset( new ShadowMapManager );
	m_pShadowMapManager->SetShadowMapShaderFilename( "ShadowMapDemo/shaders/SimpleShadowMap.fx" );
	bool initialized = m_pShadowMapManager->Init();

	// Create a light
	shared_ptr<HemisphericDirectionalLight> pLight;
	pLight.reset( new HemisphericDirectionalLight );
	pLight->vDirection = Vec3GetNormalized( Vector3( -1.0f, -3.0f, 1.0f ) );
	pLight->Attribute.UpperDiffuseColor = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	pLight->Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f );
	m_pLight = pLight;

	// Set the light to the shader
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	ShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();
	shared_ptr<ShaderLightManager> pLightMgr = shader_mgr.GetShaderLightManager();
	if( pLightMgr )
	{
		pLightMgr->SetHemisphericDirectionalLight( *pLight );
	}

	m_pShadowMapManager->CreateShadowMap( sg_LightID, *pLight );

	m_pShadowMapSceneRenderer.reset( new CShadowMapTestSceneRenderer(this) );

	m_pShadowMapManager->SetSceneRenderer( m_pShadowMapSceneRenderer );

	return 0;
}


void CShadowMapTest::Update( float dt )
{
}


void CShadowMapTest::RenderScene( ShaderManager& shader_mgr )
{
//	m_pShadowMapManager->ShaderTechniqueForShadowCaster();

	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, false );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,    true );

	shared_ptr<ShaderLightManager> pLightMgr = shader_mgr.GetShaderLightManager();
	if( pLightMgr )
		pLightMgr->CommitChanges();

	int num_mesh_rows = 5;
	for( int i=0; i<num_mesh_rows; i++ )
	{
		Matrix34 pose( Matrix34Identity() );
		pose.vPosition = Vector3( 0.0f, (float)i * 1.25f, (float)i * 5.0f + 2.0f );

		shader_mgr.SetWorldTransform( pose );

		shared_ptr<BasicMesh> pMesh = m_Mesh.GetMesh();
		if( pMesh )
			pMesh->Render( shader_mgr );
	}

	shader_mgr.SetWorldTransform( Matrix44Identity() );
	shared_ptr<BasicMesh> pFloor = m_FloorMesh.GetMesh();
	if( pFloor )
		pFloor->Render( shader_mgr );

//	PrimitiveShapeRenderer renderer;
//	renderer.SetShader(  );
//	renderer.RenderBox( Vector3( 100.0f, 0.1f, 100.0f ), Matrix34( Vector3(0.0f,-0.05f,0.0f), Matrix33Identity() ) );
}


void CShadowMapTest::Render()
{
	if( m_pLight )
		m_pShadowMapManager->UpdateLightForShadow( sg_LightID, *m_pLight );

	m_pShadowMapManager->SetSceneCamera( GetCurrentCamera() );

	m_pShadowMapManager->RenderShadowCasters( Camera() );

	m_pShadowMapManager->RenderShadowReceivers( Camera() );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	ShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();

	m_pShadowMapManager->BeginScene();

	RenderScene( shader_mgr );

	m_pShadowMapManager->EndScene();

	m_pShadowMapManager->RenderSceneWithShadow();


//	m_pShadowMapManager->RenderSceneWithShadow( 0, 0, GetWindowWidth() - 1, GetWindowHeight() - 1 );

//	C2DRect scene_shadow_rect;
//	scene_shadow_rect.SetPositionLTWH( 0, 0, GetWindowWidth() - 1, GetWindowHeight() - 1 );
//	scene_shadow_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
//	scene_shadow_rect.Draw( m_pShadowMapManager->GetSceneShadowTexture() );
}


void CShadowMapTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F11:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_pShadowMapManager )
			{
				TextureHandle tex = m_pShadowMapManager->GetSceneShadowTexture();
				tex.SaveTextureToImageFile( "scene_shadow_texture.png" );

//				shared_ptr<TextureRenderTarget> pTexRenderTarget
//					= m_pShadowMapManager->GetSceneShadowTexture();
//
//				if( pTexRenderTarget )
//					pTexRenderTarget->GetRenderTargetTexture().SaveTextureToImageFile( "scene_shadow_texture.png" );
			}
		}
		break;
	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}


void CShadowMapTest::RenderShadowCasters( Camera& camera )
{
	ShaderManager *pShaderMgr = m_pShadowMapManager->GetShader().GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	ShaderTechniqueHandle tech = m_pShadowMapManager->ShaderTechniqueForShadowCaster();
	Result::Name res = shader_mgr.SetTechnique( tech );

	RenderScene( shader_mgr );
}


void CShadowMapTest::RenderShadowReceivers( Camera& camera )
{
	ShaderManager *pShaderMgr = m_pShadowMapManager->GetShader().GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	ShaderTechniqueHandle tech = m_pShadowMapManager->ShaderTechniqueForShadowReceiver();
	Result::Name res = shader_mgr.SetTechnique( tech );

	RenderScene( shader_mgr );
}
