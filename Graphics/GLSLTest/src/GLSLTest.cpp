#include "GLSLTest.hpp"
#include <gds/Graphics.hpp>
#include <gds/Graphics/AsyncResourceLoader.hpp>
#include <gds/Graphics/Font/BitstreamVeraSansMono_Bold_256.hpp>
#include <gds/Support/Timer.hpp>
#include <gds/Support/Profile.hpp>
#include <gds/Support/ParamLoader.hpp>
#include <gds/Support/Macro.h>
#include <gds/GUI.hpp>

using namespace std;
using namespace boost;


static int gs_TextureMipLevels = 1;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CGLSLTest();
}


extern const std::string GetAppTitle()
{
	return string("GLSLTest");
}


static const CSimpleBitmapFontData sg_NullFontData;

const CSimpleBitmapFontData& GetBuiltinFontData( const string& builtin_font_name )
{
	if( builtin_font_name == "BitstreamVeraSansMono-Bold-256" )
	{
		return g_BitstreamVeraSansMono_Bold_256;
	}
	else
		return sg_NullFontData;
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

//	m_TestAsyncLoading = true;

//	GraphicsResourceManager().AllowAsyncLoading( m_TestAsyncLoading );


	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CGLSLTest::~CGLSLTest()
{
}


void CGLSLTest::CreateSampleUI()
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
	dir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );
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

	Matrix44 matProj
		= Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );

	if( m_Shader.GetShaderManager() )
		m_Shader.GetShaderManager()->SetProjectionTransform( matProj );

	if( m_pGLProgram )
		m_pGLProgram->SetProjectionTransform( matProj );

	return true;
}


int CGLSLTest::Init()
{
//	m_pFont = shared_ptr<CFontBase>( new CFont( "ÇlÇr ÉSÉVÉbÉN", 6, 12 ) );
//	m_pFont = shared_ptr<CFontBase>( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );
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

//	if( m_TestAsyncLoading )
//		CreateCachedResources();

	return 0;
}


void CGLSLTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );

/*
	if( true //m_TestAsyncLoading/ )
	{
//		LoadTexturesAsync();

		BOOST_FOREACH( CTestMeshHolder& holder, m_vecMesh )
		{
			//if( holder.m_LoadingStyle != CTestMeshHolder::LOAD_SYNCHRONOUSLY )
				LoadResourcesAsync( holder );
		}
	}

	if( !GraphicsResourceManager().IsAsyncLoadingAllowed() )
	{
		// async loading is not enabled
		// - The primary thread (this thread) loads the resources from the disk/memory.
		AsyncResourceLoader().ProcessResourceLoadRequest();
	}*/
}


void CGLSLTest::RenderMeshes()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

//	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );

	CShaderManager *pShaderManager = m_pGLProgram.get();
//	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

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

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CGLSLTest::Render()
{
	PROFILE_FUNCTION();

//	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

	RenderMeshes();

	if( m_pSampleUI )
		m_pSampleUI->Render();

//	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

//	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     (float)GetWindowWidth() / 4,  (float)16 );
	Vector2 vBottomRight( (float)GetWindowWidth() - 16, (float)GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = g_Camera.GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
}




void CGLSLTest::HandleInput( const SInputData& input )
{
	if( m_pUIInputHandler )
	{
//		CInputHandler::ProcessInput() does not take const SInputData&
		SInputData input_copy = input;
		m_pUIInputHandler->ProcessInput( input_copy );

		if( m_pUIInputHandler->PrevInputProcessed() )
			return;
	}

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
//			m_pSampleUI->GetDialog(UIID_DLG_RESOLUTION)->Open();
		}
		break;
	default:
		break;
	}
}


void CGLSLTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CGLSLTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
