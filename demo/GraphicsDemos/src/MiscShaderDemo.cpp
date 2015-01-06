#include "MiscShaderDemo.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/VertexFormat.hpp"
#include "amorphous/Graphics/Shader/MiscShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/MeshUtilities.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Input.hpp"

using std::string;
using namespace boost;


MiscShaderDemo::MiscShaderDemo()
:
m_CurrentShader(0)
{
	m_MeshTechnique.SetTechniqueName( "Default" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	InitRand( (unsigned long)timeGetTime() );
}


MiscShaderDemo::~MiscShaderDemo()
{
}


bool MiscShaderDemo::InitShaders()
{
//	bool specular = true;
//	LoadParamFromFile( "MiscShaderDemo/params.txt", "specular", specular );

//	ShaderResourceDesc shader_desc;
//	GenericShaderDesc gen_shader_desc;
//	gen_shader_desc.LightingTechnique = ShaderLightingTechnique::HEMISPHERIC;
//	gen_shader_desc.NumPointLights = 16;
//	gen_shader_desc.Specular = specular ? SpecularSource::UNIFORM : SpecularSource::NONE;
//	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );
//	bool shader_loaded = m_Shader.Load( shader_desc );

//	return shader_loaded;

	m_Shaders.resize( MiscShader::NUM_IDS );
	for( int i=0; i<MiscShader::NUM_IDS; i++ )
	{
		m_Shaders[i] = CreateMiscShader( (MiscShader::ID)i );
	}

	return true;
}


int MiscShaderDemo::Init()
{
//f	CreateParamFileIfNotFound( "MiscShaderDemo/params.txt",
//f		"model models/shapes.msh\n"\
//f		"random_light_colors true\n"\
//f		"specular false\n"\
//f		"hs_directinal_lights false\n"\
//f		"hs_point_lights true\n"\
//f		"num_lights_x_z 3 3\n" );

	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0.0f, 1.6f, -5.0f ) );
/*
	bool loaded = false;
	string mesh_file_pathname;
	LoadParamFromFile( "MiscShaderDemo/params.txt", "model", mesh_file_pathname );
	if( 0 < mesh_file_pathname.length() )
	{
		mesh_file_pathname = "MiscShaderDemo/" + mesh_file_pathname;
		loaded = m_Mesh.LoadFromFile( mesh_file_pathname );
	}
	else
	{
		BoxMeshGenerator box_mesh_generator;
//		box_mesh_generator.Generate( Vector3(1,1,1) );
		box_mesh_generator.SetPolygonDirection( MeshPolygonDirection::INWARD );
		box_mesh_generator.Generate( Vector3(50,4,50), MeshGenerator::DEFAULT_VERTEX_FLAGS, SFloatRGBAColor::White() );
		C3DMeshModelArchive ar = box_mesh_generator.GetMeshArchive();
		loaded = m_Mesh.LoadFromArchive( ar );
	}
*/
	m_Meshes.resize(3);
	m_Meshes[0] = CreateBoxMesh( Vector3(1,1,1) );
	m_Meshes[1] = CreateSphereMesh( 0.5f );
	m_Meshes[2].Load( "Common/models/bunny.msh" );

	InitShaders();

//	LoadParamFromFile( "MiscShaderDemo/params.txt", "random_light_colors", m_RandomLightColors );

//	SetLights();

	return 0;
}


void MiscShaderDemo::Update( float dt )
{
}


ShaderHandle MiscShaderDemo::GetCurrentShader()
{
//	return ShaderHandle();
	return m_Shaders[MiscShader::SINGLE_COLOR_MEMBRANE];
}


void MiscShaderDemo::RenderMeshes()
{
	GraphicsDevice().Enable( RenderStateType::LIGHTING );
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Enable( RenderStateType::FACE_CULLING );
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );

	ShaderHandle shader = GetCurrentShader();
	ShaderManager *pShaderMgr = shader.GetShaderManager();

	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	// render the scene

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

//	SetLights( pShaderMgr ? true : false );

	shader_mgr.SetWorldTransform( Matrix44Identity() );

//	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	if( 2 < m_Meshes.size() )
	{
		boost::shared_ptr<BasicMesh> pMesh = m_Meshes[2].GetMesh();
		if( pMesh )
			pMesh->Render( shader_mgr );
	}

//	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void MiscShaderDemo::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

	GraphicsDevice().Disable( RenderStateType::LIGHTING );
}


void MiscShaderDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
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
