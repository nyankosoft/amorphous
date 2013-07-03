#include "LightingTest.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/VertexFormat.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Input.hpp"

using std::string;
using namespace boost;


CLightingTest::CLightingTest()
:
m_RandomLightColors(true),
m_NumLightsX(4),
m_NumLightsZ(3)
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0.0f, 1.6f, -10.0f ) );

	InitRand( (unsigned long)timeGetTime() );
}


CLightingTest::~CLightingTest()
{
}


inline SFloatRGBAColor GetRangedRandFRGBA1( float min, float max )
{
	return SFloatRGBAColor(
		RangedRand( min, max ),
		RangedRand( min, max ),
		RangedRand( min, max ),
		1.0f
		);
}


static const SFloatRGBAColor s_rand_colors[] = {
	GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), 
	GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), 
	GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), 
	GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), GetRangedRandFRGBA1(0.6f,1.0f), 
};


void CLightingTest::SetHSDirectionalLights( ShaderLightManager& shader_light_mgr, bool random_color )
{
	// Always use hemispheric lights
	const bool use_hemespheric_light = true;

	if( use_hemespheric_light )
	{
//		HemisphericDirectionalLight light;
//		light.Attribute.UpperDiffuseColor.SetRGBA( 0.0f, 1.0f, 1.0f, 1.0f );
//		light.Attribute.LowerDiffuseColor.SetRGBA( 1.0f, 0.1f, 0.1f, 1.0f );
//		light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );
		HemisphericDirectionalLight light;
//		light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
//		light.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
		light.Attribute.UpperDiffuseColor = SFloatRGBAColor::Red();
		light.Attribute.LowerDiffuseColor = SFloatRGBAColor::Blue();
		light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.5f, -0.9f ) );
		shader_light_mgr.SetHemisphericDirectionalLight( light );
	}
	else
	{
		DirectionalLight dir_light;
		dir_light.DiffuseColor = SFloatRGBColor(1,1,1);
		dir_light.fIntensity = 1.0f;
		dir_light.vDirection = Vec3GetNormalized( Vector3( 1.2f, -1.8f, 1.0f ) );
		shader_light_mgr.SetDirectionalLight( dir_light );
	}
}


void CLightingTest::SetHSPointLights( ShaderLightManager& shader_light_mgr, bool random_color )
{
	int x = m_NumLightsX;
	int z = m_NumLightsZ;
	float span_x = 32.0f;
	float span_z = 32.0f;
//	int num_point_lights = 4;
//	for( int i=0; i<num_point_lights; i++ )
	float interval_x = (2 <= x) ? span_x / (float)(x-1) : 0.0f;
	float interval_z = (2 <= z) ? span_z / (float)(z-1) : 0.0f;
	int num_point_lights = z * x;
	int light_index = 0;
	for( int i=0; i<z; i++ )
	{
		for( int j=0; j<x; j++ )
		{
			HemisphericPointLight light;
//			light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
			int color_index = light_index % numof(s_rand_colors);
			light_index++;
			light.Attribute.UpperDiffuseColor = random_color ? s_rand_colors[color_index] : SFloatRGBAColor::White();
			light.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
			light.vPosition = Vector3(
				(float)j * interval_x - span_x * 0.5f,
				1.0f,
				(float)i * interval_z - span_z * 0.5f );
			light.fAttenuation[0] = 0.2f;
			light.fAttenuation[1] = 0.1f;
			light.fAttenuation[2] = 0.1f;
			shader_light_mgr.SetHemisphericPointLight( light );
		}
	}
}


void CLightingTest::SetHSSpotights( ShaderLightManager& shader_light_mgr, bool random_color )
{
	int x = m_NumLightsX;
	int z = m_NumLightsZ;
	float span_x = 32.0f;
	float span_z = 32.0f;
//	int num_point_lights = 4;
//	for( int i=0; i<num_point_lights; i++ )
	float interval_x = (2 <= x) ? span_x / (float)(x-1) : 0.0f;
	float interval_z = (2 <= z) ? span_z / (float)(z-1) : 0.0f;
	int num_point_lights = z * x;
	int light_index = 0;
	for( int i=0; i<z; i++ )
	{
		for( int j=0; j<x; j++ )
		{
			HemisphericSpotlight light;
//			light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
			int color_index = light_index % numof(s_rand_colors);
			light_index++;
			light.Attribute.UpperDiffuseColor = s_rand_colors[color_index];
			light.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
			light.vPosition = Vector3(
				(float)j * interval_x - span_x * 0.5f,
				1.0f,
				(float)i * interval_z - span_z * 0.5f );
//			light.fAttenuation[0] = 0.1f;
//			light.fAttenuation[1] = 0.1f;
//			light.fAttenuation[2] = 0.1f;
			shader_light_mgr.SetHemisphericSpotlight( light );
		}
	}
}


void CLightingTest::SetLights( bool use_hemespheric_light )
{
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

//	ShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();
	ShaderManager& shader_mgr = *pShaderMgr;

	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	ShaderLightManager& shader_light_mgr = *pShaderLightMgr;

	shader_light_mgr.ClearLights();

	bool random_color = m_RandomLightColors;

	bool hs_directinal_lights = true;
	bool hs_point_lights = true;
	LoadParamFromFile( "LightingDemo/params.txt", "hs_directinal_lights", hs_directinal_lights );
	LoadParamFromFile( "LightingDemo/params.txt", "hs_point_lights",      hs_point_lights );

	LoadParamFromFile( "LightingDemo/params.txt", "num_lights_x_z",       m_NumLightsX, m_NumLightsZ );

	if( hs_directinal_lights )
		SetHSDirectionalLights( shader_light_mgr, random_color );

	if( hs_point_lights )
		SetHSPointLights( shader_light_mgr, random_color );

//	SetHSSpotights( shader_light_mgr, random_color );

	shader_light_mgr.CommitChanges();
}


bool CLightingTest::InitShader()
{
	bool specular = true;
	LoadParamFromFile( "LightingDemo/params.txt", "specular", specular );

	ShaderResourceDesc shader_desc;
	GenericShaderDesc gen_shader_desc;
	gen_shader_desc.LightingTechnique = ShaderLightingTechnique::HEMISPHERIC;
	gen_shader_desc.NumPointLights = 16;
	gen_shader_desc.Specular = specular ? SpecularSource::UNIFORM : SpecularSource::NONE;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

	return shader_loaded;
}


int CLightingTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 8, 16 );
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
	LoadParamFromFile( "LightingDemo/params.txt", "model", mesh_file_pathname );
	if( 0 < mesh_file_pathname.length() )
	{
		mesh_file_pathname = "LightingDemo/" + mesh_file_pathname;
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

	InitShader();

	LoadParamFromFile( "LightingDemo/params.txt", "random_light_colors", m_RandomLightColors );

//	MeshResourceDesc mesh_desc;
//	mesh_desc.pMeshGenerator.reset( new BoxMeshGenerator );
//	m_RegularMesh.Load( mesh_desc );

//	SetLights();

	return 0;
}


void CLightingTest::Update( float dt )
{
}


void CLightingTest::RenderMeshes()
{
	GraphicsDevice().Enable( RenderStateType::LIGHTING );
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Enable( RenderStateType::FACE_CULLING );
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

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


void CLightingTest::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

//	if( m_pSampleUI )
//		m_pSampleUI->Render();

//	m_TextBuffer.clear();
//	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	Vector2 vTopLeft(     (float)GetWindowWidth() / 4,  (float)16 );
//	Vector2 vBottomRight( (float)GetWindowWidth() - 16, (float)GetWindowHeight() * 3 / 2 );
//	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );

//	Vector3 vCamPos = GetCurrentCamera().GetPosition();
//	m_pFont->DrawText( string("camera: ") + to_string( vCamPos ), Vector2( 20, 300 ) );
}


void CLightingTest::HandleInput( const InputData& input )
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
