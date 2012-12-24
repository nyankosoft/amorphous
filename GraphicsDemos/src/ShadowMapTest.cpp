#include "ShadowMapTest.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/MeshModel/PrimitiveShapeMeshes.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Graphics/Shader/ShaderManagerHub.hpp"
#include "gds/Graphics/Shader/GenericShaderDesc.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Graphics/PrimitiveShapeRenderer.hpp"
#include "gds/Graphics/TextureRenderTarget.hpp"
#include "gds/Graphics/HemisphericLight.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/ParamLoader.hpp"

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
	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "Default" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
/*	string shader_path = "./shaders/ShadowMapTest.fx";
//	string shader_path = "./shaders/PerPixelSingleHSDirectionalLight.fx";
	bool shader_loaded = m_Shader.Load( shader_path );
*/
	CGenericShaderDesc desc;

	desc.ShaderLightingType = CShaderLightingType::PER_PIXEL;
	desc.Specular = CSpecularSource::NONE;

	CShaderResourceDesc shader_desc;

//	m_Techniques.resize( shader_descs.size() );
	shader_desc.pShaderGenerator.reset( new CGenericShaderGenerator(desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

//	if( shader_loaded )
//		m_Technique.SetTechniqueName( "Default" );

	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	string model = "models/shadow_map_test.msh";
	LoadParamFromFile( "params.txt", "model", model );
	m_Mesh.Load( model );

	// load the terrain mesh
	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/floor.msh";
	mesh_desc.MeshType     = CMeshType::BASIC;
	m_FloorMesh.Load( mesh_desc );
//	m_FloorMesh = CreateBoxMesh();

	m_pShadowMapManager.reset( new CShadowMapManager );
	m_pShadowMapManager->SetShadowMapShaderFilename( "shaders/SimpleShadowMap.fx" );
	bool initialized = m_pShadowMapManager->Init();

	// Create a light
	shared_ptr<CHemisphericDirectionalLight> pLight;
	pLight.reset( new CHemisphericDirectionalLight );
	pLight->vDirection = Vec3GetNormalized( Vector3( -1.0f, -3.0f, 1.0f ) );
	pLight->Attribute.UpperDiffuseColor = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	pLight->Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f );
	m_pLight = pLight;

	// Set the light to the shader
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	CShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();
	shared_ptr<CShaderLightManager> pLightMgr = shader_mgr.GetShaderLightManager();
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


void CShadowMapTest::RenderScene( CShaderManager& shader_mgr )
{
//	m_pShadowMapManager->ShaderTechniqueForShadowCaster();

	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, false );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,    true );

	shared_ptr<CShaderLightManager> pLightMgr = shader_mgr.GetShaderLightManager();
	if( pLightMgr )
		pLightMgr->CommitChanges();

	int num_mesh_rows = 5;
	for( int i=0; i<num_mesh_rows; i++ )
	{
		Matrix34 pose( Matrix34Identity() );
		pose.vPosition = Vector3( 0.0f, (float)i * 1.25f, (float)i * 5.0f + 2.0f );

		shader_mgr.SetWorldTransform( pose );

		shared_ptr<CBasicMesh> pMesh = m_Mesh.GetMesh();
		if( pMesh )
			pMesh->Render( shader_mgr );
	}

	shader_mgr.SetWorldTransform( Matrix44Identity() );
	shared_ptr<CBasicMesh> pFloor = m_FloorMesh.GetMesh();
	if( pFloor )
		pFloor->Render( shader_mgr );

//	CPrimitiveShapeRenderer renderer;
//	renderer.SetShader(  );
//	renderer.RenderBox( Vector3( 100.0f, 0.1f, 100.0f ), Matrix34( Vector3(0.0f,-0.05f,0.0f), Matrix33Identity() ) );
}


void CShadowMapTest::Render()
{
	if( m_pLight )
		m_pShadowMapManager->UpdateLightForShadow( sg_LightID, *m_pLight );

	m_pShadowMapManager->SetSceneCamera( g_Camera );

	m_pShadowMapManager->RenderShadowCasters( g_Camera );

	m_pShadowMapManager->RenderShadowReceivers( g_Camera );

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	CShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();

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


void CShadowMapTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_pShadowMapManager )
			{
				CTextureHandle tex = m_pShadowMapManager->GetSceneShadowTexture();
				tex.SaveTextureToImageFile( "scene_shadow_texture.png" );

//				shared_ptr<CTextureRenderTarget> pTexRenderTarget
//					= m_pShadowMapManager->GetSceneShadowTexture();
//
//				if( pTexRenderTarget )
//					pTexRenderTarget->GetRenderTargetTexture().SaveTextureToImageFile( "scene_shadow_texture.png" );
			}
		}
		break;
	}
}


void CShadowMapTest::RenderShadowCasters( CCamera& camera )
{
	CShaderManager *pShaderMgr = m_pShadowMapManager->GetShader().GetShaderManager();
	if( !pShaderMgr )
		return;

	CShaderManager& shader_mgr = *pShaderMgr;

	CShaderTechniqueHandle tech = m_pShadowMapManager->ShaderTechniqueForShadowCaster();
	Result::Name res = shader_mgr.SetTechnique( tech );

	RenderScene( shader_mgr );
}


void CShadowMapTest::RenderShadowReceivers( CCamera& camera )
{
	CShaderManager *pShaderMgr = m_pShadowMapManager->GetShader().GetShaderManager();
	if( !pShaderMgr )
		return;

	CShaderManager& shader_mgr = *pShaderMgr;

	CShaderTechniqueHandle tech = m_pShadowMapManager->ShaderTechniqueForShadowReceiver();
	Result::Name res = shader_mgr.SetTechnique( tech );

	RenderScene( shader_mgr );
}
