#include "GLSLTest.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/MeshGenerators.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"
#include <boost/foreach.hpp>

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


CTestMeshHolder::CTestMeshHolder()
:
m_LoadingStyle(LOAD_SYNCHRONOUSLY),
m_Pose(Matrix34Identity())
{
}


CTestMeshHolder::CTestMeshHolder( const std::string& filepath, LoadingStyleName loading_style, const Matrix34& pose )
:
m_LoadingStyle(loading_style),
m_Pose(pose)
{
	m_MeshDesc.ResourcePath = filepath;

	if( loading_style == LOAD_MESH_AND_TEX_TOGETHER )
	{
		m_MeshDesc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
		m_MeshDesc.LoadOptionFlags = MeshLoadOption::LOAD_TEXTURES_ASYNC;
	}
	else if( loading_style == LOAD_MESH_AND_TEX_SEPARATELY )
	{
		m_MeshDesc.LoadingMode = CResourceLoadingMode::SYNCHRONOUS;
		m_MeshDesc.LoadOptionFlags = MeshLoadOption::DO_NOT_LOAD_TEXTURES;
		m_Handle.Load( m_MeshDesc );
	}
	else if( loading_style == LOAD_SYNCHRONOUSLY )
	{
		m_MeshDesc.LoadingMode = CResourceLoadingMode::SYNCHRONOUS;
		m_Handle.Load( m_MeshDesc );
	}
//	else
//	{
//	}
}


void CTestMeshHolder::Load()
{
	m_Handle.Load( m_MeshDesc );
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

	m_pGLProgram = shared_ptr<CGLProgram>( new CGLProgram );
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

	m_vecMesh.push_back( CTestMeshHolder() );
	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator() );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetDiffuseColor( SFloatRGBAColor::White() );
	m_vecMesh.back().m_MeshDesc.pMeshGenerator = pBoxMeshGenerator;
//	m_vecMesh.back().m_MeshDesc.OptionFlags |= GraphicsResourceOption::DONT_SHARE;
	m_vecMesh.back().Load();

/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

/*	string mesh_file[] =
	{
		"./models/sample_level_00.msh", // manually load textures
		"./models/FlakySlate.msh",      // load mesh and texture asnchronously
		"./models/HighAltitude.msh",
		"./models/RustPeel.msh",
		"./models/SmashedGrayMarble.msh"
	};

	BOOST_FOREACH( const string& filepath, mesh_file )
	{
		m_vecMesh.push_back( CMeshObjectHandle() );

		CMeshResourceDesc desc;
		desc.ResourcePath = filepath;

		if( m_TestAsyncLoading )
			desc.LoadOptionFlags = MeshLoadOption::DO_NOT_LOAD_TEXTURES;

		m_vecMesh.back().Load( desc );
	}
*/
/*
	m_vecMesh.push_back( CTestMeshHolder( "./models/sample_level_00.msh",   CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/FlakySlate.msh",        CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1, 100), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/HighAltitude.msh",      CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY,   Matrix34( Vector3(-25,1, 100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/RustPeel.msh",          CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1,-100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/SmashedGrayMarble.msh", CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3(-25,1,-100), Matrix33Identity() ) ) );
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
	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );

	CShaderManager *pShaderManager = m_pGLProgram.get();
//	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );

//	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	pShaderManager->SetTechnique( m_MeshTechnique );
//	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecMesh )
	BOOST_FOREACH( CTestMeshHolder& holder, m_vecMesh )
	{
		if( holder.m_Handle.GetEntryState() == GraphicsResourceState::LOADED )
		{
			// set world transform
			const Matrix34 mesh_world_pose = holder.m_Pose;
//			FixedPipelineManager().SetWorldTransform( mesh_world_pose );
			pShaderManager->SetWorldTransform( mesh_world_pose );

			shared_ptr<CBasicMesh> pMesh = holder.m_Handle.GetMesh();

			if( pMesh )
				pMesh->Render( *pShaderManager );
		}
	}

//	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CGLSLTest::Render()
{
	PROFILE_FUNCTION();

	SetLights();

	RenderMeshes();

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
