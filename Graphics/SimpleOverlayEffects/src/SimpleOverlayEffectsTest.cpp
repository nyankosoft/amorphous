#include "SimpleOverlayEffectsTest.hpp"
#include <boost/foreach.hpp>
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics.hpp"
#include "gds/Graphics/NoiseTextureGenerators.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Input.hpp"
#include "gds/GUI.hpp"

using namespace std;
using namespace boost;


extern CPlatformDependentCameraController g_CameraController;


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

extern CGraphicsTestBase *CreateTestInstance()
{
	return new CSimpleOverlayEffectsTest();
}


extern const std::string GetAppTitle()
{
	return string("SimpleOverlayEffectsTest");
}


CSimpleOverlayEffectsTest::CSimpleOverlayEffectsTest()
:
m_EnableNoiseEffect(true),
m_EnableStripeEffect(true),
m_DisplayResourceInfo(false)
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	g_CameraController.SetPosition( Vector3( 0, 1, -30 ) );
	g_Camera.SetPosition( Vector3( 0, 1, -30 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CSimpleOverlayEffectsTest::~CSimpleOverlayEffectsTest()
{
}


void CSimpleOverlayEffectsTest::CreateSampleUI()
{
}


bool CSimpleOverlayEffectsTest::InitShader()
{
	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/SimpleOverlayEffectsTest.fx" );
	
	CShaderManager& shader_mgr
		= (shader_loaded && m_Shader.GetShaderManager()) ? *(m_Shader.GetShaderManager()) : FixedFunctionPipelineManager();

//	if( !shader_loaded )
//		return false;

	CShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();

	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

//	pShaderLightMgr->SetLight( 0, light );
//	pShaderLightMgr->SetDirectionalLightOffset( 0 );
//	pShaderLightMgr->SetNumDirectionalLights( 1 );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	Matrix44 proj = Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );
	shader_mgr.SetProjectionTransform( proj );

	return true;
}


int CSimpleOverlayEffectsTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 6, 12 );

	m_PseudoNoiseEffect.Init( 0.5f, 2 );

	// stripe texture
	CTextureResourceDesc desc;
	desc.Width  = 16;
	desc.Height = 16;
	desc.Format = TextureFormat::A8R8G8B8;
	shared_ptr<CStripeTextureGenerator> pGenerator( new CStripeTextureGenerator );
	pGenerator->m_StripeWidth = 1;
	int w = 0;
	LoadParamFromFile( "params.txt", "stripe_width", w );
	if( 0 < w )
		pGenerator->m_StripeWidth = w;
	// opacity adjust by vertex diffuse color of the fullscreen rects
//	float alpha = 0.5f;
//	LoadParamFromFile( "params.txt", "stripe_opacity", alpha );
//	pGenerator->m_Color0 = SFloatRGBAColor(0,0,0,alpha);
//	pGenerator->m_Color0 = SFloatRGBAColor(0,0,0,1);
	desc.pLoader = pGenerator;
	bool loaded = m_StripeTexture.Load( desc );
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

//	m_vecMesh.push_back( CTestMeshHolder( "./models/sample_level_00.msh",   CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/fw43.msh",              CTestMeshHolder::LOAD_SYNCHRONOUSLY,   Matrix34Identity() ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/HighAltitude.msh",      CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY,   Matrix34( Vector3(-25,1, 100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/RustPeel.msh",          CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1,-100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/SmashedGrayMarble.msh", CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3(-25,1,-100), Matrix33Identity() ) ) );

	for( size_t i=0; i<m_vecMesh.size(); i++ )
	{
		m_vecMesh[i].m_Handle.Load( m_vecMesh[i].m_MeshDesc );
	}

	InitShader();

	return 0;
}


void CSimpleOverlayEffectsTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );
}


void CSimpleOverlayEffectsTest::RenderMeshes()
{
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();

	CShaderManager& shader_mgr = pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( g_Camera.GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	shader_mgr.SetTechnique( m_MeshTechnique );
//	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecMesh )
	BOOST_FOREACH( CTestMeshHolder& holder, m_vecMesh )
	{
//		CBasicMesh *pMesh = mesh.GetMesh().get();

		if( holder.m_Handle.GetEntryState() == GraphicsResourceState::LOADED )
		{
			// set world transform
//			FixedFunctionPipelineManager().SetWorldTransform( holder.m_Pose );
			shader_mgr.SetWorldTransform( holder.m_Pose );

			CBasicMesh *pMesh = holder.m_Handle.GetMesh().get();

			if( pMesh )
				pMesh->Render( shader_mgr );
		}
	}

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CSimpleOverlayEffectsTest::DisplayResourceInfo()
{
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = g_Camera.GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
}


void CSimpleOverlayEffectsTest::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

	m_PseudoNoiseEffect.SetNoiseTexture();

	if( m_EnableNoiseEffect )
		m_PseudoNoiseEffect.RenderNoiseEffect();

	if( m_EnableStripeEffect )
	{
		CViewport vp;
		GraphicsDevice().GetViewport( vp );
		C2DRect rect;
		static SFloatRGBAColor color( SFloatRGBAColor::White() );
		UPDATE_PARAM( "params.txt", "stripe_opacity", color.fAlpha );
		rect.SetColor( color );
		rect.SetPositionLTRB( 0, 0, (int)vp.Width-1, (int)vp.Height-1 );
//		rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2((float)vp.Width/(float)16,(float)vp.Height/(float)16) );
		rect.Draw( m_StripeTexture );
	}

	if( m_pSampleUI )
		m_pSampleUI->Render();

	if( m_DisplayResourceInfo )
		DisplayResourceInfo();
}


void CSimpleOverlayEffectsTest::HandleInput( const SInputData& input )
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
	case 'N':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_EnableNoiseEffect = !m_EnableNoiseEffect;
		break;

	case 'H':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_EnableStripeEffect = !m_EnableStripeEffect;
		break;

//	case GIC_F12:
//		if( input.iType == ITYPE_KEY_PRESSED )
//			SaveTexturesAsImageFiles();
//		break;

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


void CSimpleOverlayEffectsTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CSimpleOverlayEffectsTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
