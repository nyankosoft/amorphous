#include "PrimitiveShapeMeshesDemo.hpp"
#include "amorphous/Graphics.hpp"
#include "amorphous/Graphics/Meshgenerators/PipeMeshGenerator.hpp"
#include "amorphous/Graphics/Meshgenerators/GridMeshGenerator.hpp"
#include "amorphous/Graphics/MeshGenerators/HelixMeshGenerator.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/ParamLoader.hpp"

using std::string;
using namespace boost;


PrimitiveShapeMeshesDemo::PrimitiveShapeMeshesDemo()
:
m_MeshIndex(0),
m_NumPrimitiveMeshes(0),
m_Lighting(false)
{
//	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_MeshTechnique.SetTechniqueName( "Default" );
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	if( CameraController() )
		CameraController()->SetPosition( Vector3( 0, 1, -3 ) );
}


PrimitiveShapeMeshesDemo::~PrimitiveShapeMeshesDemo()
{
}


bool PrimitiveShapeMeshesDemo::InitShader()
{
	// initialize shader
//	bool shader_loaded = m_Shader.Load( "./shaders/PrimitiveShapeMeshesDemo.fx" );

	// For now, use the fixed function pipeline.
	ShaderResourceDesc shader_desc;
	int use_fixed_function_pipeline = 1;
	LoadParamFromFile( "PrimitiveShapeMeshesDemo/params.txt", "use_fixed_function_pipeline", use_fixed_function_pipeline );
	if( use_fixed_function_pipeline )
	{
		return true;
//		shader_desc.ShaderType = ShaderType::NON_PROGRAMMABLE;
	}
	else
	{
		GenericShaderDesc gen_shader_desc;
		shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );

		bool shader_loaded = m_Shader.Load( shader_desc );
	
		if( !shader_loaded )
			return false;
	}

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	ShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();

	shared_ptr<ShaderLightManager> pShaderLightMgr = shader_mgr.GetShaderLightManager();
	if( !pShaderLightMgr )
		return false;

	HemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	return true;
}


int PrimitiveShapeMeshesDemo::CreatePrimitiveShapeMeshes()
{
	int show_texture = 0;
	LoadParamFromFile( "PrimitiveShapeMeshesDemo/params.txt", "show_texture", show_texture );
	string texture_file_pathname = show_texture ? "AshySandstone.jpg" : "white.png";
	string texture_pathname = "PrimitiveShapeMeshesDemo/textures/" + texture_file_pathname;

	MeshResourceDesc mesh_desc[8];
	m_vecMesh.resize( numof(mesh_desc) );

	shared_ptr<BoxMeshGenerator> pBoxGenerator( new BoxMeshGenerator() );
	pBoxGenerator->SetEdgeLengths( Vector3( 1.0f, 1.0f, 1.0f ) );
	mesh_desc[0].pMeshGenerator = pBoxGenerator;
	mesh_desc[0].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[0].ResourcePath = "BoxMesh";
	m_vecMesh[0].Load( mesh_desc[0] );

	ConeDesc cone_desc;
	cone_desc.cone_height = 4.0f;
	cone_desc.body_height = 4.0f;
	mesh_desc[1].pMeshGenerator.reset( new ConeMeshGenerator( cone_desc ) );
	mesh_desc[1].ResourcePath = "ConeMesh";
	mesh_desc[1].pMeshGenerator->SetTexturePath( texture_pathname );
	m_vecMesh[1].Load( mesh_desc[1] );

	CapsuleDesc capsule_desc;
	shared_ptr<CapsuleMeshGenerator> pCapsuleGenerator( new CapsuleMeshGenerator(capsule_desc) );
	mesh_desc[2].pMeshGenerator = pCapsuleGenerator;
	mesh_desc[2].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[2].ResourcePath = "CapsuleMesh";
	m_vecMesh[2].Load( mesh_desc[2] );

	SphereDesc shpere_desc;
	shpere_desc.num_sides = 32;
	shpere_desc.num_segments = 16;
//	shpere_desc.poly_dir = MeshPolygonDirection::INWARD;
	shared_ptr<SphereMeshGenerator> pSphereGenerator( new SphereMeshGenerator(shpere_desc) );
	mesh_desc[3].pMeshGenerator = pSphereGenerator;
	mesh_desc[3].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[3].ResourcePath = "SphereMesh";
	m_vecMesh[3].Load( mesh_desc[3] );

	CylinderDesc cylinder_desc;
	cylinder_desc.num_sides = 24;
	shared_ptr<CylinderMeshGenerator> pCylinderGenerator( new CylinderMeshGenerator(cylinder_desc) );
	mesh_desc[4].pMeshGenerator = pCylinderGenerator;
	mesh_desc[4].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[4].ResourcePath = "CylinderMesh";
	m_vecMesh[4].Load( mesh_desc[4] );

	shared_ptr<PipeMeshGenerator> pPipeGenerator( new PipeMeshGenerator );
	mesh_desc[5].pMeshGenerator = pPipeGenerator;
	mesh_desc[5].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[5].ResourcePath = "PipeMesh";
	m_vecMesh[5].Load( mesh_desc[5] );

	shared_ptr<GridMeshGenerator> pGMGenerator( new GridMeshGenerator(
		1, 1, 10, 10, 1, 1, false ) );
	mesh_desc[6].pMeshGenerator = pGMGenerator;
	mesh_desc[6].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[6].ResourcePath = "GridMesh";
	m_vecMesh[6].Load( mesh_desc[6] );

	shared_ptr<HelixMeshGenerator> pHelixMeshGenerator( new HelixMeshGenerator );
	mesh_desc[7].pMeshGenerator = pHelixMeshGenerator;
	mesh_desc[7].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[7].ResourcePath = "HelixMesh";
	m_vecMesh[7].Load( mesh_desc[7] );

	m_NumPrimitiveMeshes = 8;

	return 0;
}


int PrimitiveShapeMeshesDemo::Init()
{
	shared_ptr<TextureFont> pTexFont( new TextureFont() );
	pTexFont->InitFont( GetBuiltinFontData("BitstreamVeraSansMono-Bold-256") );
	pTexFont->SetFontSize( 8, 16 );
	m_pFont = pTexFont;

	m_ConeTexture.Load( "./textures/white.png" );
/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/
	InitShader();

	CreatePrimitiveShapeMeshes();

	return 0;
}


void PrimitiveShapeMeshesDemo::Update( float dt )
{
}


void PrimitiveShapeMeshesDemo::RenderMeshes()
{
	ShaderManager *pShaderManager = m_Shader.GetShaderManager();

	ShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	// render the scene

	Matrix44 matWorld( Matrix44Identity() );

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

//	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	shader_mgr.SetTechnique( m_MeshTechnique );
	if( shader_mgr.GetShaderLightManager() )
		shader_mgr.GetShaderLightManager()->CommitChanges();

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

//	BOOST_FOREACH( MeshHandle& mesh, m_vecMesh )
//	{
		shared_ptr<BasicMesh> pMesh = m_vecMesh[m_MeshIndex].GetMesh();//mesh.GetMesh();

		if( pMesh )
		{
			// set world transform
//			holder.m_Pose.GetRowMajorMatrix44( matWorld );
			FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
			shader_mgr.SetWorldTransform( Matrix44Identity() );

			shader_mgr.SetTexture( 0, m_ConeTexture );
			if( shader_mgr.GetEffect() )
				shader_mgr.GetEffect()->CommitChanges();

			// TODO: automatically detect the shader type (programmable/non-programmable) by the system.
			pMesh->Render( shader_mgr );

///			pMesh->RenderSubset( *pShaderManager, 0 );
//			pMesh->Render( *pShaderManager );
		}
//	}

//	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void PrimitiveShapeMeshesDemo::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();
}


void PrimitiveShapeMeshesDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_MeshIndex = (m_MeshIndex + 1) % m_NumPrimitiveMeshes;
		}
		break;
	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_MeshIndex = (m_MeshIndex + m_NumPrimitiveMeshes - 1) % m_NumPrimitiveMeshes;
		}
		break;
	case 'L':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_Lighting = !m_Lighting;
		}
		break;
	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}
