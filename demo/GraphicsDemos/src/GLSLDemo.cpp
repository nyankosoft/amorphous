#include "GLSLDemo.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/MeshUtilities.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"

using std::string;
using namespace boost;


GLSLDemo::GLSLDemo()
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0, 2, -20 ) );
}


GLSLDemo::~GLSLDemo()
{
}


void GLSLDemo::SetLights()
{
	PROFILE_FUNCTION();

//	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	ShaderManager *pShaderMgr = m_pGLProgram.get();
	if( !pShaderMgr )
		return;

	ShaderLightManager *pShaderLightMgr = pShaderMgr->GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

	DirectionalLight dir_light;
	dir_light.DiffuseColor = SFloatRGBColor(1,1,1);
	dir_light.fIntensity = 1.0f;
	dir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, 0.9f ) );
	pShaderLightMgr->SetDirectionalLight( dir_light );

	bool set_pnt_light = false;
	if( set_pnt_light )
	{
		PointLight pnt_light;
		pnt_light.DiffuseColor = SFloatRGBColor(1,1,1);
		pnt_light.fIntensity = 1.0f;
		pnt_light.vPosition = Vector3( 2.0f, 2.8f, -1.9f );
		pnt_light.fAttenuation[0] = 1.0f;
		pnt_light.fAttenuation[1] = 1.0f;
		pnt_light.fAttenuation[2] = 1.0f;
		pShaderLightMgr->SetPointLight( pnt_light );
	}

/*	HemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );
*/}


bool GLSLDemo::InitShader()
{
	// initialize shader
/*	bool shader_loaded = m_Shader.Load( "shaders/glsl_test.vert|shaders/glsl_test.frag" );

	if( !shader_loaded )
		return false;
*/
//	string shader_filepath = "shaders/shader.vert|shaders/shader.frag";
	string shader_filepath = LoadParamFromFile<string>( "config", "Shader" );

	m_pGLProgram.reset( new CGLProgram );
	m_pGLProgram->LoadShaderFromFile( shader_filepath );

//	SetLights();

	const float aspect_ratio = (float)GetWindowWidth() / (float)GetWindowHeight();
	Matrix44 matProj = Matrix44PerspectiveFoV_LH( (float)PI / 4, aspect_ratio, 0.1f, 500.0f );

	if( m_Shader.GetShaderManager() )
		m_Shader.GetShaderManager()->SetProjectionTransform( matProj );

	if( m_pGLProgram )
		m_pGLProgram->SetProjectionTransform( matProj );

	return true;
}


int GLSLDemo::Init()
{
	m_Meshes.push_back( MeshHandle() );
	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator() );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetDiffuseColor( SFloatRGBAColor::White() );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;
//	mesh_desc.OptionFlags |= GraphicsResourceOption::DONT_SHARE;
	m_Meshes.back().Load( mesh_desc );

	m_Meshes.push_back( CreateSphereMesh( 0.5f ) );
//	m_Meshes.push_back( CreateConeMesh() );
//	m_Meshes.push_back( CreateCylinderMesh() );

/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

	// init shader
	InitShader();

	return 0;
}


void GLSLDemo::Update( float dt )
{
}


void GLSLDemo::RenderMeshes()
{
	PROFILE_FUNCTION();

	if( m_Meshes.empty() )
		return;

	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,   true );

	ShaderManager *pShaderManager = m_pGLProgram.get();
//	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
//	ShaderManager *pShaderManager = NULL;
	ShaderManager &shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

//	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	shader_mgr.SetTechnique( m_MeshTechnique );

	int mesh_index = 0;
	for( int x=-1; x<=1; x++ )
	{
		PROFILE_SCOPE( "the mesh rendering loop" );
		for( int z=-1; z<=1; z++ )
		{
			const Matrix34 mesh_world_pose = Matrix34( Vector3((float)x,0,(float)z) * 2.0f, Matrix33Identity() );
			shader_mgr.SetWorldTransform( mesh_world_pose );
			shared_ptr<BasicMesh> pMesh = m_Meshes[mesh_index].GetMesh();

			if( pMesh )
				pMesh->Render( shader_mgr );

			mesh_index = (mesh_index+1) % (int)m_Meshes.size();
		}
	}


//	Vector3 mesh_positions[] =
//	{
//		Vector3(-1,0,2),
//		Vector3( 1,0,2),
//		Vector3( 1,0,1),
//		Vector3(-1,0,1)
//	};

//	const int num_meshes_to_render = take_min( (int)numof(mesh_positions), (int)m_Meshes.size() );
//	for( int i=0; i<num_meshes_to_render; i++ )
//	{
//		// set world transform
//		const Matrix34 mesh_world_pose = Matrix34( mesh_positions[i], Matrix33Identity() );
//		shader_mgr.SetWorldTransform( mesh_world_pose );
//
//		shared_ptr<BasicMesh> pMesh = m_Meshes[i].GetMesh();
//
//		if( pMesh )
//			pMesh->Render( shader_mgr );
//	}

//	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();

//	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, true );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,    false );
}


void GLSLDemo::Render()
{
	PROFILE_FUNCTION();

	SetLights();

	RenderMeshes();
}


void GLSLDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case 'L':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			const std::vector<string>& buffer = GetProfileText();
			FILE *fp = fopen( "profile.txt", "w" );
			if( !fp )
				break;
			for( size_t i=0; i<buffer.size(); i++ )
				fprintf( fp, "%s", buffer[i].c_str() );

			fclose(fp);
		}
		break;
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			SaveTexturesAsImageFiles();
		}
		break;
	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		break;
	}
}
