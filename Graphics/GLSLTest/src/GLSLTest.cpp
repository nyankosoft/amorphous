#include "GLSLTest.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/MeshGenerators.hpp"
#include "gds/Graphics/MeshUtilities.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Input.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"

using std::string;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CGLSLTest();
}


extern const std::string GetAppTitle()
{
	return string("GLSLTest");
}


CGLSLTest::CGLSLTest()
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CGLSLTest::~CGLSLTest()
{
}


void CGLSLTest::SetLights()
{
	PROFILE_FUNCTION();

//	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	CShaderManager *pShaderMgr = m_pGLProgram.get();
	if( !pShaderMgr )
		return;

	CShaderLightManager *pShaderLightMgr = pShaderMgr->GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

	CDirectionalLight dir_light;
	dir_light.DiffuseColor = SFloatRGBColor(1,1,1);
	dir_light.fIntensity = 1.0f;
	dir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, 0.9f ) );
	pShaderLightMgr->SetDirectionalLight( dir_light );

	bool set_pnt_light = false;
	if( set_pnt_light )
	{
		CPointLight pnt_light;
		pnt_light.DiffuseColor = SFloatRGBColor(1,1,1);
		pnt_light.fIntensity = 1.0f;
		pnt_light.vPosition = Vector3( 2.0f, 2.8f, -1.9f );
		pnt_light.fAttenuation[0] = 1.0f;
		pnt_light.fAttenuation[1] = 1.0f;
		pnt_light.fAttenuation[2] = 1.0f;
		pShaderLightMgr->SetPointLight( pnt_light );
	}

/*	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );
*/}


bool CGLSLTest::InitShader()
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


int CGLSLTest::Init()
{
	shared_ptr<CTextureFont> pTexFont( new CTextureFont );
	pTexFont->InitFont( GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" ) );
	pTexFont->SetFontSize( 8, 16 );
	m_pFont = pTexFont;

	m_Meshes.push_back( CMeshObjectHandle() );
	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator() );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetDiffuseColor( SFloatRGBAColor::White() );
	CMeshResourceDesc mesh_desc;
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


void CGLSLTest::Update( float dt )
{
}


void CGLSLTest::RenderMeshes()
{
	PROFILE_FUNCTION();

	if( m_Meshes.empty() )
		return;

	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,   true );

	CShaderManager *pShaderManager = m_pGLProgram.get();
//	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
//	CShaderManager *pShaderManager = NULL;
	CShaderManager &shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( g_Camera.GetPosition() );

//	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	shader_mgr.SetTechnique( m_MeshTechnique );

	int mesh_index = 0;
	for( int x=-1; x<=1; x++ )
	{
		PROFILE_SCOPE( "the mesh rendering loop" );
		for( int z=-1; z<=1; z++ )
		{
			const Matrix34 mesh_world_pose = Matrix34( Vector3((float)x,0,(float)z) * 2.0f, Matrix33Identity() );
			shader_mgr.SetWorldTransform( mesh_world_pose );
			shared_ptr<CBasicMesh> pMesh = m_Meshes[mesh_index].GetMesh();

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
//		shared_ptr<CBasicMesh> pMesh = m_Meshes[i].GetMesh();
//
//		if( pMesh )
//			pMesh->Render( shader_mgr );
//	}

//	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();

//	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, true );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING,    false );
}


void CGLSLTest::Render()
{
	PROFILE_FUNCTION();

	SetLights();

	RenderMeshes();

	SetRenderStatesForTextureFont( AlphaBlend::InvSrcAlpha );

//	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

//	Vector2 vTopLeft(     (float)GetWindowWidth() / 4,  (float)16 );
//	Vector2 vBottomRight( (float)GetWindowWidth() - 16, (float)GetWindowHeight() * 3 / 2 );
//	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

//	m_pFont->DrawText( m_TextBuffer, vTopLeft );
}




void CGLSLTest::HandleInput( const SInputData& input )
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


void CGLSLTest::ReleaseGraphicsResources()
{
}


void CGLSLTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}
