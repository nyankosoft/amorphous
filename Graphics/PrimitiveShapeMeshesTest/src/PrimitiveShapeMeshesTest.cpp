#include "PrimitiveShapeMeshesTest.hpp"
#include "gds/Graphics.hpp"
#include "gds/Graphics/Meshgenerators/PipeMeshGenerator.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/Macro.h"
#include "gds/Support/ParamLoader.hpp"

using std::string;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CPrimitiveShapeMeshesTest();
}


extern const std::string GetAppTitle()
{
	return string("PrimitiveShapeMeshesTest");
}


CPrimitiveShapeMeshesTest::CPrimitiveShapeMeshesTest()
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


CPrimitiveShapeMeshesTest::~CPrimitiveShapeMeshesTest()
{
}


bool CPrimitiveShapeMeshesTest::InitShader()
{
	// initialize shader
//	bool shader_loaded = m_Shader.Load( "./shaders/PrimitiveShapeMeshesTest.fx" );

	// For now, use the fixed function pipeline.
	CShaderResourceDesc shader_desc;
	int use_fixed_function_pipeline = 1;
	LoadParamFromFile( "params.txt", "use_fixed_function_pipeline", use_fixed_function_pipeline );
	if( use_fixed_function_pipeline )
	{
		return true;
//		shader_desc.ShaderType = CShaderType::NON_PROGRAMMABLE;
	}
	else
	{
		CGenericShaderDesc gen_shader_desc;
		shader_desc.pShaderGenerator.reset( new CGenericShaderGenerator(gen_shader_desc) );

		bool shader_loaded = m_Shader.Load( shader_desc );
	
		if( !shader_loaded )
			return false;
	}

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	CShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();

	shared_ptr<CShaderLightManager> pShaderLightMgr = shader_mgr.GetShaderLightManager();
	if( !pShaderLightMgr )
		return false;

	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	return true;
}


int CPrimitiveShapeMeshesTest::CreatePrimitiveShapeMeshes()
{
	int show_texture = 0;
	LoadParamFromFile( "params.txt", "show_texture", show_texture );
	string texture_pathname = show_texture ? "./textures/AshySandstone.jpg" : "./textures/white.png";

	CMeshResourceDesc mesh_desc[6];
	m_vecMesh.resize( numof(mesh_desc) );

	shared_ptr<CBoxMeshGenerator> pBoxGenerator( new CBoxMeshGenerator() );
	pBoxGenerator->SetEdgeLengths( Vector3( 1.0f, 1.0f, 1.0f ) );
	mesh_desc[0].pMeshGenerator = pBoxGenerator;
	mesh_desc[0].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[0].ResourcePath = "BoxMesh";
	m_vecMesh[0].Load( mesh_desc[0] );

	CConeDesc cone_desc;
	cone_desc.cone_height = 4.0f;
	cone_desc.body_height = 4.0f;
	mesh_desc[1].pMeshGenerator = shared_ptr<CMeshGenerator>( new CConeMeshGenerator( cone_desc ) );
	mesh_desc[1].ResourcePath = "ConeMesh";
	mesh_desc[1].pMeshGenerator->SetTexturePath( texture_pathname );
	m_vecMesh[1].Load( mesh_desc[1] );

	CCapsuleDesc capsule_desc;
	shared_ptr<CCapsuleMeshGenerator> pCapsuleGenerator( new CCapsuleMeshGenerator(capsule_desc) );
	mesh_desc[2].pMeshGenerator = pCapsuleGenerator;
	mesh_desc[2].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[2].ResourcePath = "CapsuleMesh";
	m_vecMesh[2].Load( mesh_desc[2] );

	CSphereDesc shpere_desc;
	shpere_desc.num_sides = 32;
	shpere_desc.num_segments = 16;
//	shpere_desc.poly_dir = MeshPolygonDirection::INWARD;
	shared_ptr<CSphereMeshGenerator> pSphereGenerator( new CSphereMeshGenerator(shpere_desc) );
	mesh_desc[3].pMeshGenerator = pSphereGenerator;
	mesh_desc[3].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[3].ResourcePath = "SphereMesh";
	m_vecMesh[3].Load( mesh_desc[3] );

	CCylinderDesc cylinder_desc;
	cylinder_desc.num_sides = 24;
	shared_ptr<CCylinderMeshGenerator> pCylinderGenerator( new CCylinderMeshGenerator(cylinder_desc) );
	mesh_desc[4].pMeshGenerator = pCylinderGenerator;
	mesh_desc[4].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[4].ResourcePath = "CylinderMesh";
	m_vecMesh[4].Load( mesh_desc[4] );

	shared_ptr<PipeMeshGenerator> pPipeGenerator( new PipeMeshGenerator );
	mesh_desc[5].pMeshGenerator = pPipeGenerator;
	mesh_desc[5].pMeshGenerator->SetTexturePath( texture_pathname );
	mesh_desc[5].ResourcePath = "PipeMesh";
	m_vecMesh[5].Load( mesh_desc[5] );

	m_NumPrimitiveMeshes = 6;

	return 0;
}


int CPrimitiveShapeMeshesTest::Init()
{
	shared_ptr<CTextureFont> pTexFont( new CTextureFont() );
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


void CPrimitiveShapeMeshesTest::Update( float dt )
{
}


void CPrimitiveShapeMeshesTest::RenderMeshes()
{
	CShaderManager *pShaderManager = m_Shader.GetShaderManager();

	CShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	// render the scene

	Matrix44 matWorld( Matrix44Identity() );

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( GetCurrentCamera() );

	shader_mgr.SetTechnique( m_MeshTechnique );
	if( shader_mgr.GetShaderLightManager() )
		shader_mgr.GetShaderLightManager()->CommitChanges();

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

//	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecMesh )
//	{
		shared_ptr<CBasicMesh> pMesh = m_vecMesh[m_MeshIndex].GetMesh();//mesh.GetMesh();

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

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CPrimitiveShapeMeshesTest::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     640,       16 );
	Vector2 vBottomRight( 640 + 256, 16 + 400 );
/*	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();
*/
	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = GetCurrentCamera().GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
}


void CPrimitiveShapeMeshesTest::HandleInput( const SInputData& input )
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
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	case 'L':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_Lighting = !m_Lighting;
		}
		break;
	default:
		break;
	}
}
