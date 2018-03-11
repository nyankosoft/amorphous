#include "SimpleMotionBlurDemo.hpp"
#include "amorphous/Graphics/SimpleMotionBlur.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Support/CameraController.hpp"

using namespace std;


SimpleMotionBlurDemo::SimpleMotionBlurDemo()
:
m_BlurWeight(1)
{
}


SimpleMotionBlurDemo::~SimpleMotionBlurDemo()
{
}


int SimpleMotionBlurDemo::Init()
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "Default" );

	// initialize shader
	ShaderResourceDesc shader_desc;
	GenericShaderDesc gen_shader_desc;
	gen_shader_desc.LightingTechnique = ShaderLightingTechnique::HEMISPHERIC;
//	gen_shader_desc.Specular = SpecularSource::NONE;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );
//	bool shader_loaded = m_Shader.Load( "./shaders/SimpleMotionBlurDemo.fx" );

	// load skybox mesh
//	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );
	m_SkyTexture = CreateClearDaySkyTexture();
	m_SkyTexture.SaveTextureToImageFile( ".debug/SimpleMotionBlurDemo_sky.png" );

	// load the terrain mesh
	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/terrain06.msh";
	mesh_desc.MeshType     = MeshTypeName::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	if( CameraController() )
		CameraController()->SetPosition( Vector3( 0, 3, -2 ) );

	return 0;
}


void SimpleMotionBlurDemo::Update( float dt )
{
}


void SimpleMotionBlurDemo::RenderScene()
{
	Matrix44 matWorld = Matrix44Identity();
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();

	ShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	GraphicsDevice().Enable( RenderStateType::FACE_CULLING );
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

//	RenderAsSkybox( m_SkyboxMesh, GetCurrentCamera().GetPose() );
	RenderSkybox( m_SkyTexture, GetCurrentCamera().GetPose() );

	shader_mgr.SetWorldTransform( matWorld );

	shader_mgr.SetTechnique( m_MeshTechnique );

	shared_ptr<BasicMesh> pTerrainMesh = m_TerrainMesh.GetMesh();
	if( pTerrainMesh )
		pTerrainMesh->Render( shader_mgr );
}


void SimpleMotionBlurDemo::Render()
{
	if( !m_pSimpleMotionBlur )
	{
		m_pSimpleMotionBlur.reset( new SimpleMotionBlur );
		m_pSimpleMotionBlur->InitForScreenSize();
//		m_pSimpleMotionBlur->SetBlurWeight( 0.1f );
	}

	m_pSimpleMotionBlur->SetBlurWeight( (float)m_BlurWeight * 0.1f );

	m_pSimpleMotionBlur->Begin();

	RenderScene();

	m_pSimpleMotionBlur->End();

	m_pSimpleMotionBlur->Render();
}


void SimpleMotionBlurDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_BlurWeight += 1;
			clamp( m_BlurWeight, 0, 10 );
		}
		break;
	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_BlurWeight -= 1;
			clamp( m_BlurWeight, 0, 10 );
		}
		break;
	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}
