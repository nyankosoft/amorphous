#include "HLSLEffectTest.hpp"
#include "gds/3DMath/MatrixConversions.hpp"
#include "gds/Graphics.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/Macro.h"
#include "gds/Utilities/TextFileScannerExtensions.hpp"
#include "gds/GUI.hpp"

using std::string;
using std::vector;
using namespace boost;


extern CPlatformDependentCameraController g_CameraController;


CTestMeshHolder::CTestMeshHolder( const std::string& filepath, LoadingStyleName loading_style, const Matrix34& pose )
:
m_LoadingStyle(loading_style),
m_Pose(pose),
m_fScale(1.0f)
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
	return new CHLSLEffectTest();
}


extern const std::string GetAppTitle()
{
	return string("HLSL Effect Test");
}


CHLSLEffectTest::CHLSLEffectTest()
:
m_CurrentShaderIndex( 0 ),
m_CurrentMeshIndex( 0 ),
m_DisplayDebugInfo(false)
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	g_CameraController.SetPosition( Vector3( 0, 1, -30 ) );

	for( int i=0; i<numof(m_EnableLight); i++ )
		m_EnableLight[i] = 1;
}


CHLSLEffectTest::~CHLSLEffectTest()
{
}


void CHLSLEffectTest::CreateSampleUI()
{
}


bool CHLSLEffectTest::SetShader( int index )
{
	if( index < 0 || (int)m_Shaders.size() <= index )
		return false;

	// initialize shader
/*	bool shader_loaded = m_Shaders[index].Load( "./shaders/HLSLEffectTest.fx" );

	if( !shader_loaded )
		return false;
*/
	if( !m_Shaders[index].GetShaderManager() )
		return false;

	CShaderManager& shader_mgr = *(m_Shaders[index].GetShaderManager());

	CShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();

	// Set lights

	pShaderLightMgr->ClearLights();

	if( m_EnableLight[0] )
	{
		CHemisphericDirectionalLight dir_light;
		dir_light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
		dir_light.Attribute.LowerDiffuseColor.SetRGBA( 0.3f, 0.3f, 0.3f, 1.0f );
		dir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

		pShaderLightMgr->SetHemisphericDirectionalLight( dir_light );
	}

	if( m_EnableLight[1] )
	{
		CHemisphericPointLight pnt_light;
		pnt_light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
		pnt_light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
		pnt_light.vPosition = Vector3( 10.0f, 3.0f, -15.0f );
		pnt_light.fAttenuation[0] = 0.1f;
		pnt_light.fAttenuation[1] = 0.1f;
		pnt_light.fAttenuation[2] = 0.1f;
		CParamLoader loader( "params.txt" );
		if( loader.IsReady() )
		{
			loader.LoadParam( "pnt_light_position",      pnt_light.vPosition );
//			loader.LoadParam( "pnt_light_diffuse_upper", pnt_light.Attribute.UpperDiffuseColor );
//			loader.LoadParam( "pnt_light_diffuse_lower", pnt_light.Attribute.LowerDiffuseColor );
			float *att = pnt_light.fAttenuation;

			loader.LoadParam( "pnt_light_attenuations", att[0], att[1], att[2] );
		}
		pShaderLightMgr->SetHemisphericPointLight( pnt_light );
	}

	pShaderLightMgr->CommitChanges();

	Matrix44 proj = Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );
	m_Shaders[index].GetShaderManager()->SetProjectionTransform( proj );

//	shader_mgr.SetParam( "g_vEyeVS", g_Camera.GetCameraMatrix() * g_Camera.GetPosition() );
	shader_mgr.SetParam( "g_vEyePos", g_Camera.GetPosition() );

	// for bright rim lights
	shader_mgr.SetParam( "g_vEyeDir", g_Camera.GetPosition() );
	shader_mgr.SetTexture( 3, m_LookupTextureForLighting );

	m_CurrentShaderIndex = index;

	return true;
}


bool CHLSLEffectTest::InitShaders()
{
	vector<string> shaders;
	vector<string> techs;
	shaders.resize( 4 );
	techs.resize( shaders.size() );

	shaders[0] = "shaders/PerVertexHSLighting.fx";             techs[0] = "PerVertexHSLighting";
//	shaders[?] = "shaders/PerPixelHSLighting.fx";              techs[?] = "PVL_HSLs_Specular";
	shaders[1] = "shaders/PerPixelHSLighting.fx";              techs[1] = "PPL_HSLs";
	shaders[2] = "shaders/PerPixelHSLightingWithSpecular.fx";  techs[2] = "PPL_HSLs_Specular";
	shaders[3] = "shaders/PerPixelHSLighting_2DLUT.fx";        techs[3] = "HSLs_2DLUT";

	m_Shaders.resize( shaders.size() );
	m_Techniques.resize( shaders.size() );
	for( size_t i=0; i<shaders.size(); i++ )
	{
		bool loaded = m_Shaders[i].Load( shaders[i] );

		m_Techniques[i].SetTechniqueName( techs[i].c_str() );
	}

	return true;
}


int CHLSLEffectTest::Init()
{
//	shared_ptr<CTextureFont> pTexFont( new CTextureFont );
//	pTexFont->InitFont( GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" ) );
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 6, 12 );

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

	m_vecMesh.push_back( CTestMeshHolder( "./models/fw43.msh",              CTestMeshHolder::LOAD_SYNCHRONOUSLY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/primitive_meshes.msh",  CTestMeshHolder::LOAD_SYNCHRONOUSLY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/9x19mm.msh",            CTestMeshHolder::LOAD_SYNCHRONOUSLY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.back().m_fScale = 20.0f;
	m_vecMesh.push_back( CTestMeshHolder( "./models/5.56x45mm.msh",         CTestMeshHolder::LOAD_SYNCHRONOUSLY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.back().m_fScale = 20.0f;
	m_vecMesh.push_back( CTestMeshHolder( "./models/cz75-1st.msh",          CTestMeshHolder::LOAD_SYNCHRONOUSLY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.back().m_fScale = 20.0f;
	m_vecMesh.push_back( CTestMeshHolder( "./models/male.msh",              CTestMeshHolder::LOAD_SYNCHRONOUSLY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );

	InitShaders();

//	string tex_file( "textures/normal_hs_light.jpg" );
//	string tex_file( "textures/spectrum-vertical-s256x256.jpg" );
	string tex_file( "textures/textured_spectrum.jpg" );
	bool tex_loaded = m_LookupTextureForLighting.Load( tex_file );

	return 0;
}


void CHLSLEffectTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );
}


void CHLSLEffectTest::RenderMesh()
{
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	if( m_Shaders.empty()
	 || m_CurrentShaderIndex < 0
	 || (int)m_Shaders.size() <= m_CurrentShaderIndex )
	{
		return;
	}

	SetShader( m_CurrentShaderIndex );

	CShaderManager *pShaderManager = m_Shaders[m_CurrentShaderIndex].GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	if( m_CurrentMeshIndex < 0 || (int)m_vecMesh.size() <= m_CurrentMeshIndex )
		return;

	Result::Name res = pShaderManager->SetTechnique( m_Techniques[m_CurrentShaderIndex] );

	CTestMeshHolder& holder = m_vecMesh[m_CurrentMeshIndex];

	if( holder.m_Handle.GetEntryState() == GraphicsResourceState::LOADED )
	{
		float s = holder.m_fScale;
		Matrix44 world = ToMatrix44(holder.m_Pose) * Matrix44Scaling(s,s,s);
		// set world transform
		FixedFunctionPipelineManager().SetWorldTransform( world );
		pShaderManager->SetWorldTransform( world );

		CBasicMesh *pMesh = holder.m_Handle.GetMesh().get();

		if( pMesh )
			pMesh->Render( *pShaderManager );
	}

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CHLSLEffectTest::RenderDebugInfo()
{
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     (float)GetWindowWidth() / 4.0f,  16.0f );
	Vector2 vBottomRight( (float)GetWindowWidth() - 16.0f, (float)GetWindowHeight() * 3.0f / 2.0f );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = g_Camera.GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );

	if( 0 <= m_CurrentShaderIndex && m_CurrentShaderIndex < (int)m_Shaders.size() )
	{
		CShaderHandle& shader = m_Shaders[m_CurrentShaderIndex];
		if( shader.GetEntry()
		 && shader.GetEntry()->GetResource() )
		{
			string shader_path = 
				shader.GetEntry()->GetResource()->GetDesc().ResourcePath;

			m_pFont->DrawText( shader_path.c_str(), Vector2( 20, 50 ) );
		}
	}
}


void CHLSLEffectTest::Render()
{
	PROFILE_FUNCTION();

	RenderMesh();

	if( m_pSampleUI )
		m_pSampleUI->Render();

//	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

	if( m_DisplayDebugInfo )
		RenderDebugInfo();
}


void CHLSLEffectTest::HandleInput( const SInputData& input )
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
	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_Shaders.empty() )
				return;

			m_CurrentShaderIndex = (m_CurrentShaderIndex+1) % (int)m_Shaders.size();
		}
		break;
	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentShaderIndex = (m_CurrentShaderIndex + (int)m_Shaders.size() - 1) % (int)m_Shaders.size(); 
		}
		break;

	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_vecMesh.empty() )
				return;

			m_CurrentMeshIndex = (m_CurrentMeshIndex+1) % (int)m_vecMesh.size();
		}
		break;

	case GIC_PAGE_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentMeshIndex = (m_CurrentMeshIndex + (int)m_vecMesh.size() - 1) % (int)m_vecMesh.size(); 
		}
		break;

	case '1':
	case '2':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			bool& toggle = m_EnableLight[ input.iGICode - '1' ];
			toggle = !toggle;
		}
		break;

	case GIC_F1:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_DisplayDebugInfo = !m_DisplayDebugInfo;
		}
		break;
	case GIC_F5:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			GraphicsResourceManager().Refresh();
		}
		break;
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
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


void CHLSLEffectTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CHLSLEffectTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
