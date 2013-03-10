#include "BrickWallTextureGeneratorTest.hpp"
#include "gds/Graphics.hpp"
#include "gds/Graphics/VertexFormat.hpp"
#include "gds/Graphics/MeshGenerators/BrickWallMeshGenerator.hpp"
#include "gds/Graphics/Shader/GenericShaderDesc.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"
#include "gds/Input.hpp"

using std::string;
using namespace boost;


CBrickWallTextureGeneratorTest::CBrickWallTextureGeneratorTest()
{
	m_MeshTechnique.SetTechniqueName( "Default" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.6f, 1.0f ) );

	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0, 1, -2 ) );
}


CBrickWallTextureGeneratorTest::~CBrickWallTextureGeneratorTest()
{
}


void CBrickWallTextureGeneratorTest::SetLights( bool use_hemespheric_light )
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


bool CBrickWallTextureGeneratorTest::InitShader()
{
	ShaderResourceDesc shader_desc;
	GenericShaderDesc gen_shader_desc;
	gen_shader_desc.LightingTechnique = ShaderLightingTechnique::HEMISPHERIC;
//	gen_shader_desc.Specular = SpecularSource::NONE;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

	return shader_loaded;
}


int CBrickWallTextureGeneratorTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 8, 16 );

	bool loaded = false;
	string mesh_file_pathname;
	LoadParamFromFile( "BrickWallTextureGeneratorDemo/params.txt", "model", mesh_file_pathname );
	mesh_file_pathname = "BrickWallTextureGeneratorDemo/" + mesh_file_pathname;

	BrickPanelDesc desc;
	desc.per_brick_color_variations.min = SFloatRGBAColor( 0.5f, 0.5f, 0.5f, 1.0f );
	desc.per_brick_color_variations.max = SFloatRGBAColor( 0.9f, 0.9f, 0.9f, 1.0f );
//	desc.color_variations.min = SFloatRGBAColor( 0.9f, 0.9f, 0.9f, 1.0f );
//	desc.color_variations.max = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	desc.grayscale.set( 0.5f, 1.0f );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new BrickWallMeshGenerator(desc) );
	bool mesh_loaded = m_BrickWallMesh.Load( mesh_desc );

	InitShader();

//	SetLights( true );

	return 0;
}


void CBrickWallTextureGeneratorTest::Update( float dt )
{
}


void CBrickWallTextureGeneratorTest::RenderMeshes()
{
	GraphicsDevice().Enable( RenderStateType::LIGHTING );
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	ShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

	SetLights( pShaderMgr ? true : false );

	shader_mgr.SetWorldTransform( Matrix44Identity() );

//	GetShaderManagerHub().PushViewAndProjectionMatrices( g_Camera );

	Result::Name res = shader_mgr.SetTechnique( m_MeshTechnique );

	shared_ptr<BasicMesh> pBrickWallMesh = m_BrickWallMesh.GetMesh();

	if( pBrickWallMesh )
		pBrickWallMesh->Render( shader_mgr );

//	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void CBrickWallTextureGeneratorTest::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

//	m_TextBuffer.clear();
//	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	Vector2 vTopLeft(     (float)GetWindowWidth() / 4,  (float)16 );
//	Vector2 vBottomRight( (float)GetWindowWidth() - 16, (float)GetWindowHeight() * 3 / 2 );
//	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );
}


void CBrickWallTextureGeneratorTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case 'M':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			BrickWallMeshGenerator mesh_generator;
			mesh_generator.Generate();
			C3DMeshModelArchive mesh_archive = mesh_generator.GetMeshArchive();
			mesh_archive.SaveToFile( "brick_wall.msh" );
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
