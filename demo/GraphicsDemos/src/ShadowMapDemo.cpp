#include "ShadowMapDemo.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/MeshModel/PrimitiveShapeMeshes.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
//#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/GenericShaderHelpers.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/PrimitiveShapeRenderer.hpp"
#include "amorphous/Graphics/TextureRenderTarget.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Support/ParamLoader.hpp"

using std::string;
using std::vector;
using namespace boost;


static int sg_LightID = 0;


ShadowMapDemo::ShadowMapDemo()
:
m_RenderSceneWithShadow(true),
m_Lighting(false)
{
}


ShadowMapDemo::~ShadowMapDemo()
{
}


int ShadowMapDemo::Init()
{
	if( CameraController() )
		CameraController()->SetPosition( Vector3( 0, 2, -3 ) );

	m_MeshTechnique.SetTechniqueName( "Default" );

	m_NoLightingShader = CreateNoLightingShader();

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
//	m_pShadowMapManager->SetShadowMapShaderFilename( "ShadowMapDemo/shaders/SimpleShadowMap.fx" );
	bool initialized = m_pShadowMapManager->Init();

	// Create some lights
	m_pLights.resize( 2 );

	// A spotlight (enabled by default)
	shared_ptr<HemisphericSpotlight> pSpotLight;
	pSpotLight.reset( new HemisphericSpotlight );
	pSpotLight->fInnerConeAngle = 1.20f;
	pSpotLight->fOuterConeAngle = 1.21f;
	pSpotLight->vPosition = Vector3(-3,12,-1);
	pSpotLight->vDirection = Vec3GetNormalized( Vector3( 1.0f, -2.0f, 1.0f ) );
	pSpotLight->Attribute.UpperDiffuseColor = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	pSpotLight->Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f );
	m_pLights[0].first  = 1;
	m_pLights[0].second = pSpotLight;

	// A directional light (disabled by default)
	shared_ptr<HemisphericDirectionalLight> pDirLight;
	pDirLight.reset( new HemisphericDirectionalLight );
	pDirLight->vDirection = Vec3GetNormalized( Vector3( -1.0f, -3.0f, 1.0f ) );
	pDirLight->Attribute.UpperDiffuseColor = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	pDirLight->Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f );
	m_pLights[1].first  = 0;
	m_pLights[1].second = pDirLight;

	// Set the light to the shader
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
//	ShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();
	if( !pShaderMgr )
		return 0;
	ShaderManager& shader_mgr = *pShaderMgr;

	shared_ptr<ShaderLightManager> pLightMgr = shader_mgr.GetShaderLightManager();
	if( pLightMgr )
	{
		pLightMgr->SetHemisphericSpotlight( *pSpotLight );
//		pLightMgr->SetHemisphericDirectionalLight( *pDirLight );
	}

	m_pShadowMapManager->CreateShadowMap( sg_LightID, *pSpotLight );
//	m_pShadowMapManager->CreateShadowMap( sg_AnotherLightID, *pDirLight );

	m_pShadowMapSceneRenderer.reset( new ShadowMapDemoSceneRenderer(this) );

	m_pShadowMapManager->SetSceneRenderer( m_pShadowMapSceneRenderer );

	return 0;
}


void ShadowMapDemo::Update( float dt )
{
}


void ShadowMapDemo::RenderScene( ShaderManager& shader_mgr )
{
//	m_pShadowMapManager->ShaderTechniqueForShadowCaster();

	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, false );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,    m_Lighting );

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


void ShadowMapDemo::RenderSceneWithShadow()
{
	if( !m_pShadowMapManager )
		return;

	for( size_t i=0; i<m_pLights.size(); i++ )
	{
		if( m_pLights[i].first == 0 )
			continue;

		shared_ptr<Light> pLight = m_pLights[i].second;
		if( !pLight )
			continue;

		m_pShadowMapManager->UpdateLightForShadow( sg_LightID, *pLight );
	}

	m_pShadowMapManager->SetSceneCamera( GetCurrentCamera() );

	m_pShadowMapManager->RenderShadowCasters( Camera() );

	m_pShadowMapManager->RenderShadowReceivers( Camera() );

	ShaderHandle shader_to_use = m_Lighting ? m_Shader : m_NoLightingShader;

	ShaderManager *pShaderMgr = shader_to_use.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

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


void ShadowMapDemo::Render()
{
	if( m_RenderSceneWithShadow )
	{
		RenderSceneWithShadow();
	}
	else
	{
		ShaderHandle shader_to_use = m_Lighting ? m_Shader : m_NoLightingShader;
		ShaderManager *pShaderMgr = shader_to_use.GetShaderManager();
		if( pShaderMgr )
			RenderScene( *pShaderMgr );
	}
}


void ShadowMapDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case 'T':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_RenderSceneWithShadow = !m_RenderSceneWithShadow;
		}
		break;

	case 'L':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_Lighting = !m_Lighting;
		}
		break;

	case GIC_F11:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_pShadowMapManager )
			{
				TextureHandle tex = m_pShadowMapManager->GetSceneShadowTexture();
				tex.SaveTextureToImageFile( ".debug/shadow_maps/scene_shadow_texture.png" );

				m_pShadowMapManager->SaveShadowMapTexturesToImageFiles( ".debug/shadow_maps" );

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


void ShadowMapDemo::RenderShadowCasters( Camera& camera, ShaderHandle *shaders, ShaderTechniqueHandle *shader_techniques )
{
//	ShaderManager *pShaderMgr = m_pShadowMapManager->GetShader().GetShaderManager();
	ShaderHandle shader = shaders[VertexBlendType::NONE];
	ShaderManager *pShaderMgr = shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	ShaderTechniqueHandle tech = m_pShadowMapManager->ShaderTechniqueForShadowCaster();
	Result::Name res = shader_mgr.SetTechnique( tech );

	RenderScene( shader_mgr );
}


void ShadowMapDemo::RenderShadowReceivers( Camera& camera, ShaderHandle *shaders, ShaderTechniqueHandle *shader_techniques )
{
//	ShaderManager *pShaderMgr = m_pShadowMapManager->GetShader().GetShaderManager();
	ShaderHandle shader = shaders[VertexBlendType::NONE];
	ShaderManager *pShaderMgr = shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	ShaderTechniqueHandle tech = m_pShadowMapManager->ShaderTechniqueForShadowReceiver();
	Result::Name res = shader_mgr.SetTechnique( tech );

	RenderScene( shader_mgr );
}
