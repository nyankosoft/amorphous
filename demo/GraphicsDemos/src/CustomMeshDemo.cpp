#include "CustomMeshDemo.hpp"
#include "amorphous/Graphics.hpp"
#include "amorphous/Graphics/VertexFormat.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Input.hpp"

using std::string;


CCustomMeshDemo::CCustomMeshDemo()
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );
}


CCustomMeshDemo::~CCustomMeshDemo()
{
}


void CCustomMeshDemo::SetLights( bool use_hemespheric_light )
{
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
//	if( !pShaderMgr )
//		return;

	ShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

	if( use_hemespheric_light )
	{
//		HemisphericDirectionalLight light;
//		light.Attribute.UpperDiffuseColor.SetRGBA( 0.0f, 1.0f, 1.0f, 1.0f );
//		light.Attribute.LowerDiffuseColor.SetRGBA( 1.0f, 0.1f, 0.1f, 1.0f );
//		light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );
		HemisphericDirectionalLight light;
		light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
		light.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
		light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.5f, -0.9f ) );
		pShaderLightMgr->SetHemisphericDirectionalLight( light );
	}
	else
	{
		DirectionalLight dir_light;
		dir_light.DiffuseColor = SFloatRGBColor(1,1,1);
		dir_light.fIntensity = 1.0f;
		dir_light.vDirection = Vec3GetNormalized( Vector3( 1.2f, -1.8f, 1.0f ) );
		pShaderLightMgr->SetDirectionalLight( dir_light );
	}

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

	pShaderLightMgr->CommitChanges();
}


bool CCustomMeshDemo::InitShader()
{
	ShaderResourceDesc shader_desc;
	GenericShaderDesc gen_shader_desc;
	gen_shader_desc.LightingTechnique = ShaderLightingTechnique::HEMISPHERIC;
//	gen_shader_desc.Specular = SpecularSource::NONE;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

	return shader_loaded;
}


int CCustomMeshDemo::Init()
{
	CreateParamFileIfNotFound( "LightingDemo/params.txt",
		"model  ../Common/models/Chevelle.msh\n" );

	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0, 2, -12 ) );
/*
	m_vecMesh.push_back( CTestMeshHolder() );
	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator() );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetDiffuseColor( SFloatRGBAColor::White() );
	m_vecMesh.back().m_MeshDesc.pMeshGenerator = pBoxMeshGenerator;
//	m_vecMesh.back().m_MeshDesc.OptionFlags |= GraphicsResourceOption::DONT_SHARE;
	m_vecMesh.back().Load();
*/
	bool loaded = false;
	string mesh_file_pathname;
	LoadParamFromFile( "CustomMeshDemo/params.txt", "model", mesh_file_pathname );
	if( 0 < mesh_file_pathname.length() )
	{
		mesh_file_pathname = "CustomMeshDemo/" + mesh_file_pathname;
		loaded = m_Mesh.LoadFromFile( mesh_file_pathname );
	}
	else
	{
/*		BoxMeshGenerator box_mesh_generator;
//		box_mesh_generator.Generate( Vector3(1,1,1) );
		box_mesh_generator.Generate( Vector3(1,1,1), MeshGenerator::DEFAULT_VERTEX_FLAGS, SFloatRGBAColor::White() );
		C3DMeshModelArchive ar = box_mesh_generator.GetMeshArchive();*/
		SphereDesc sphere_desc;
		sphere_desc.num_segments = 12;
		sphere_desc.num_sides    = 36;
		SphereMeshGenerator sphere_mesh_generator(sphere_desc);;
		sphere_mesh_generator.Generate();
		C3DMeshModelArchive ar = sphere_mesh_generator.GetMeshArchive();
		loaded = m_Mesh.LoadFromArchive( ar );
	}
/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

	int use_programmable_shader = 1;
	LoadParamFromFile( "params.txt", "use_programmable_shader", use_programmable_shader );
	if( use_programmable_shader )
		InitShader();

	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new BoxMeshGenerator );
	m_RegularMesh.Load( mesh_desc );

//	SetLights();

	return 0;
}


void CCustomMeshDemo::Update( float dt )
{
}


void CCustomMeshDemo::RenderMeshes()
{
	GraphicsDevice().Enable( RenderStateType::LIGHTING );
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	ShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

	SetLights( pShaderMgr ? true : false );

	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

	shader_mgr.SetWorldTransform( Matrix44Identity() );

//	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	m_Mesh.Render( shader_mgr );

//	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void CCustomMeshDemo::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

//	if( m_pSampleUI )
//		m_pSampleUI->Render();

	GraphicsDevice().Disable( RenderStateType::LIGHTING );
}


void CCustomMeshDemo::HandleInput( const InputData& input )
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
