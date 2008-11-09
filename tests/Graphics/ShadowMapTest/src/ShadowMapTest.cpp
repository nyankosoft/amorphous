#include "ShadowMapTest.h"
#include "3DMath/Matrix34.h"
#include "3DCommon/all.h"
#include "3DCommon/ShadowMapManager.h"
#include "3DCommon/AsyncResourceLoader.h"
#include "GameCommon/Timer.h"
#include "Support/Profile.h"
#include "Support/Macro.h"
#include "UI.h"

using namespace std;
using namespace boost;


static int gs_TextureMipLevels = 1;


class CShadowMapTestSceneRenderer : public CShadowMapSceneRenderer
{
	CShadowMapTest *m_pTest;

public:

	CShadowMapTestSceneRenderer( CShadowMapTest *pTest )
		:
	m_pTest(pTest)
	{}

	void RenderSceneToShadowMap( CCamera& camera )
	{
		m_pTest->RenderShadowCasters();
	}

	/// render objects which are cast shadows by others
	void RenderShadowReceivers( CCamera& camera )
	{
		m_pTest->RenderShadowReceivers();
	}
};


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CShadowMapTest();
}


extern const std::string GetAppTitle()
{
	return string("ShadowMapTest");
}


CShadowMapTest::CShadowMapTest()
{
//	string tech = "NoLighting";
	string tech = "QuickTest";
	m_MeshTechnique.SetTechniqueName( tech.c_str() );
/*
	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_pShadowMapManager = shared_ptr<CShadowMapManager>( new CShadowMapManager() );

	m_pShadowMapManager->SetShadowMapShaderFilename( "../../../Shaders/ShadowMap.fx" );

	bool initialized = m_pShadowMapManager->Init();
}


CShadowMapTest::~CShadowMapTest()
{
}


void CShadowMapTest::CreateLights()
{
	CDirectionalLight dir_light;
//	dir_light.vDirection = Vec3GetNormalized( Vector3( 1.0f, -5.0f, -2.0f ) );
	dir_light.vDirection = Vec3GetNormalized( Vector3( 1.0f, -5.0f, -6.0f ) );
	m_pShadowMapManager->AddShadowForLight( dir_light );
}


bool CShadowMapTest::InitShader()
{
	// initialize shader
	bool shader_loaded = m_Shader.Load( "./data/shaders/ShadowMapTest.fx" );
	
	if( !shader_loaded )
		return false;

	CShaderLightManager *pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager().get();

	CHemisphericDirectionalLight light;
	light.Attribute.UpperColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

	pShaderLightMgr->SetLight( 0, light );
	pShaderLightMgr->SetDirectionalLightOffset( 0 );
	pShaderLightMgr->SetNumDirectionalLights( 1 );

	return true;
}


int CShadowMapTest::Init()
{
	m_pFont = shared_ptr<CFontBase>( new CFont( "ÇlÇr ÉSÉVÉbÉN", 6, 12 ) );
//	m_pFont = shared_ptr<CFontBase>( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );

	string mesh_file[] =
	{
		"./data/models/shadow_map_test.msh"
	};

	BOOST_FOREACH( const string& filepath, mesh_file )
	{
		m_vecMesh.push_back( CMeshObjectHandle() );

		CMeshResourceDesc desc;
		desc.ResourcePath = filepath;

		m_vecMesh.back().Load( desc );
	}

	InitShader();

	CreateLights();

	return 0;
}


void CShadowMapTest::Update( float dt )
{
//	if( m_pSampleUI )
//		m_pSampleUI->Update( dt );
}


void CShadowMapTest::RenderShadowCasters()
{
	m_pShadowMapManager->SetShaderTechniqueForShadowCaster();
	RenderMeshes( m_pShadowMapManager->GetShader() );
}


void CShadowMapTest::RenderShadowReceivers()
{
	m_pShadowMapManager->SetShaderTechniqueForShadowReceiver();
	RenderMeshes( m_pShadowMapManager->GetShader() );
}


void CShadowMapTest::RenderMeshes( CShaderHandle& shader_handle )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	CShaderManager *pShaderManager = shader_handle.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pShaderManager->SetWorldTransform( matWorld );

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );

//	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecMesh )
	{
		CD3DXMeshObjectBase *pMesh = mesh.GetMesh().get();

		pMesh->Render( *pShaderManager );
	}
}


void CShadowMapTest::Render()
{
	PROFILE_FUNCTION();

	CShadowMapTestSceneRenderer scene_renderer(this);
	m_pShadowMapManager->SetSceneRenderer( &scene_renderer );

	m_pShadowMapManager->SetSceneCamera( g_Camera );

	// render objects that cast shadow to others

	m_pShadowMapManager->RenderShadowCasters( g_Camera );

	// render objects that receive shadow

	m_pShadowMapManager->RenderShadowReceivers( g_Camera );

	// render the scene

	m_pShadowMapManager->BeginScene();

	m_Shader.GetShaderManager()->SetTechnique( m_MeshTechnique );

	RenderMeshes( m_Shader );

	m_pShadowMapManager->EndScene();


	m_pShadowMapManager->RenderSceneWithShadow();

//	if( m_pSampleUI )
//		m_pSampleUI->Render();

/*	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );
*/
}

/*
void CShadowMapTest::SaveTexturesAsImageFiles()
{
	if( m_vecMesh.size() == 0 || !m_vecMesh[0].GetMesh() )
		return;

	CD3DXMeshObjectBase *pMesh = m_vecMesh[0].GetMesh().get();

	const int num_materials = pMesh->GetNumMaterials();
	for( int i=0; i<num_materials; i++ )
	{
//		for(each texture)
//		{
			CTextureHandle& tex = pMesh->GetTexture( i, 0 );
			if( !tex.GetEntry() )
				continue;

			shared_ptr<CTextureResource> pTexture = tex.GetEntry()->GetTextureResource();
			if( !pTexture )
				continue;

			pTexture->SaveTextureToImageFile( fmt_string( "./debug/mat%02d.dds", i ) );
//		}
	}
}
*/

void CShadowMapTest::HandleInput( const SInputData& input )
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


void CShadowMapTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CShadowMapTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
