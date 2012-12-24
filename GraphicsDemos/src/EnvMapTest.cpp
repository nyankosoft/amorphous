#include "EnvMapTest.hpp"
#include "gds/Graphics.hpp"
#include "gds/Graphics/Direct3D/Direct3D9.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/MeshUtilities.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include <boost/foreach.hpp>

using std::string;
using std::vector;
using namespace boost;


void CCubeMapTestRenderer::RenderSceneToCubeMap( CCamera& camera )
{
	m_pEnvMapTest->RenderNonEnvMappedMeshes();
}


CEnvMapTest::CEnvMapTest()
:
m_UseEmbeddedShader(true)
{
	SetUseRenderBase( true );

	if( m_UseEmbeddedShader )
	{
		m_EnvMappedMesh.SetTechniqueName( "Default" );
		m_NonEnvMappedMesh.SetTechniqueName( "Default" );
	}
	else
	{
//		m_EnvMappedMesh.SetTechniqueName( "QuickTest" );
//		m_EnvMappedMesh.SetTechniqueName( "SingleHSDL_Specular_CTS_EM" );
		m_EnvMappedMesh.SetTechniqueName( "SingleHSPL_Specular_CTS_EM" );

//		m_NonEnvMappedMesh.SetTechniqueName( "QuickTest" );
//		m_NonEnvMappedMesh.SetTechniqueName( "SingleHSDL_Specular" );
//		m_NonEnvMappedMesh.SetTechniqueName( "SingleHSDL_Specular_CTS" );
		m_NonEnvMappedMesh.SetTechniqueName( "SingleHSPL_Specular_CTS" );
	}

	m_CurrentEnvMapTarget = -1;
}


CEnvMapTest::~CEnvMapTest()
{
}


int CEnvMapTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 6, 12 );

	// initialize shader
	bool loaded = false;
	if( m_UseEmbeddedShader )
	{
		CGenericShaderDesc gs_desc;
		gs_desc.EnvMap = CEnvMapOption::ENABLED;
		gs_desc.Specular = CSpecularSource::NONE;
		CShaderResourceDesc shader_desc;
		shader_desc.pShaderGenerator.reset( new CGenericShaderGenerator(gs_desc) );

		loaded = m_EnvMappedMeshShader.Load( shader_desc );

		gs_desc.EnvMap = CEnvMapOption::NONE;
		gs_desc.Specular = CSpecularSource::NONE;
		shader_desc.pShaderGenerator.reset( new CGenericShaderGenerator(gs_desc) );

		loaded = m_NonEnvMappedMeshShader.Load( shader_desc );
	}
	else
	{
		loaded = m_EnvMappedMeshShader.Load( "./shaders/EnvMapTest.fx" );
	}

	CShaderManager *pShaderManager = m_EnvMappedMeshShader.GetShaderManager();
//	if( !pShaderManager )
//		return -1;

	CShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	// init cube map manager and envmap scene renderer

	m_pCubeMapManager.reset( new CCubeMapManager() );

	m_pCubeMapManager->Init();

	m_pCubeMapSceneRenderer.reset( new CCubeMapTestRenderer(this) ); 

	m_pCubeMapManager->SetCubeMapSceneRenderer( m_pCubeMapSceneRenderer.get() );

	// load models

	m_vecEnvMappedMesh.resize( 2 );
	m_vecEnvMappedMesh[0] = CreateBoxMesh( Vector3(1,1,1), SFloatRGBAColor::White() );
	m_vecEnvMappedMesh[1] = CreateSphereMesh( 0.5f );
//	m_vecEnvMappedMesh[2] = CreateConeMesh( 0.5f, 1.0f );

	m_vecNonEnvMappedMesh.resize( 1 );
	m_vecNonEnvMappedMesh[0].Load( "models/env_map_test_room.msh" );

/*	string env_mapped_mesh_file[] =
	{
		"./models/F14s_display_primitives.msh"
	};

	BOOST_FOREACH( const string& filepath, env_mapped_mesh_file )
	{
		m_vecEnvMappedMesh.push_back( CMeshObjectHandle() );
//		m_vecEnvMappedMesh.back().filename = filepath;
		m_vecEnvMappedMesh.back().Load( filepath );
	}
*/

/*	string non_env_mapped_mesh_file[] =
	{
		"./models/F14s_display_floor.msh",
		"./models/F14s_display_rest.msh",
	};

	BOOST_FOREACH( const string& filepath, non_env_mapped_mesh_file )
	{
		m_vecNonEnvMappedMesh.push_back( CMeshObjectHandle() );
//		m_vecNonEnvMappedMesh.back().filename = filepath;
		m_vecNonEnvMappedMesh.back().Load( filepath );
	}
*/
//	shared_ptr<CShaderLightManager> pShaderLightMgr = shader_mgr.GetShaderLightManager();
	shared_ptr<CShaderLightManager> pShaderLightMgrs[] =
	{
		FixedFunctionPipelineManager().GetShaderLightManager(),
		shader_mgr.GetShaderLightManager()
	};

	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 0.7f, 0.7f, 0.7f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

	CHemisphericPointLight pnt_light;
	pnt_light.Attribute.UpperDiffuseColor.SetRGBA( 0.6f, 0.6f, 0.6f, 1.0f );
	pnt_light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	pnt_light.vPosition = Vector3( -0.2f, 2.8f, -0.1f );

	for( int i=0; i<numof(pShaderLightMgrs); i++ )
	{
		if( !pShaderLightMgrs[i] )
			continue;

		pShaderLightMgrs[i]->ClearLights();
		pShaderLightMgrs[i]->SetHemisphericDirectionalLight( light );
//		pShaderLightMgrs[i]->SetHemisphericPointLight( pnt_light );
		pShaderLightMgrs[i]->CommitChanges();

	}
/*
	if( pShaderLightMgr )
	{
//		pShaderLightMgr->SetHemisphericDirectionalLight( light );
		pShaderLightMgr->SetHemisphericPointLight( pnt_light );
	}
*/
/*
	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );

	// load the terrain mesh
	m_TerrainMesh.filename = "./models/terrain06.msh";
	m_TerrainMesh.SetMeshType( CBasicMesh::TYPE_MESH );
	m_TerrainMesh.Load();
*/
	return 0;
}


void CEnvMapTest::Update( float dt )
{
}


void CEnvMapTest::RenderBase()
{
	PROFILE_FUNCTION();

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	CShaderManager *pShaderManager = m_EnvMappedMeshShader.GetShaderManager();

	CShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();
//	CShaderManager& shader_mgr = FixedFunctionPipelineManager();

	// update env map texture
	// RenderNonEnvMappedMeshes() is called 6 times in each RenderToCubeMap() call
	for( size_t i=0; i<m_vecEnvMappedMesh.size(); i++ )
	{
		m_CurrentEnvMapTarget = (int)i;
		m_pCubeMapManager->RenderToCubeMap();
	}

	GraphicsDevice().Disable( RenderStateType::LIGHTING );
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	m_CurrentEnvMapTarget = -1;

	// render the final scene

	shader_mgr.SetWorldTransform( Matrix44Identity() );

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( GetCurrentCamera() );

	Matrix44 mat;
	GetCurrentCamera().GetCameraMatrix( mat );
	UpdateViewTransform( mat );
	GetCurrentCamera().GetProjectionMatrix( mat );
	UpdateProjectionTransform( mat );


    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF303030, 1.0f, 0 );

    // begin the scene
    pd3dDevice->BeginScene();

//	RenderNonEnvMappedMeshes();
	shader_mgr.SetTechnique( m_NonEnvMappedMesh );
	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecNonEnvMappedMesh )
	{
		shared_ptr<CBasicMesh> pMesh = mesh.GetMesh();
		if( !pMesh )
			continue;

//		pMesh->Render( shader_mgr ); 
		pMesh->Render( FixedFunctionPipelineManager() ); 
	}

	int i = 0;
//	RenderEnvMappedMeshes();
	Result::Name res = shader_mgr.SetTechnique( m_EnvMappedMesh );
	Vector3 positions[] = { Vector3(1,1,1), Vector3(1,1,-1), Vector3(-1,1,-1), Vector3(-1,1,1), Vector3(0,1,0) };
	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecEnvMappedMesh )
	{
		shared_ptr<CBasicMesh> pMesh = mesh.GetMesh();
		if( !pMesh )
			continue;

		HRESULT hr = shader_mgr.SetCubeTexture( 0, m_pCubeMapManager->GetCubeTexture() );

		shader_mgr.SetWorldTransform( Matrix34( positions[(i++)%numof(positions)], Matrix33Identity() ) );

		pMesh->Render( shader_mgr ); 
//		pMesh->Render( FixedFunctionPipelineManager() ); 
	}

//	C2DRect rect( 0, 0, GetWindowWidth(), GetWindowHeight(), 0xFFFFFFFF );
//	rect.Draw( m_pCubeMapManager->GetCubeTexture() );

	// display fps
	float x = 10, y = 5;
	m_pFont->DrawText( to_string(GlobalTimer().GetFPS()).c_str(), Vector2(x,y), 0xFFFFFFFF );

	i = 0;
	const vector<string>& vecProfileResults = GetProfileText();
	BOOST_FOREACH( const string& text, vecProfileResults )
	{
		y = (float)( 25 + i*16 );
		m_pFont->DrawText( text.c_str(), Vector2(x,y), 0xF0F0F0FF );
		i++;
	}

    // end the scene
    pd3dDevice->EndScene();

    // present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


void CEnvMapTest::RenderNonEnvMappedMeshes()
{
	CShaderManager *pShaderManager = m_EnvMappedMeshShader.GetShaderManager();
//	if( !pShaderManager )
//		return;

//	CShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();
	CShaderManager& shader_mgr = FixedFunctionPipelineManager();

	shader_mgr.SetTechnique( m_NonEnvMappedMesh );
	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecNonEnvMappedMesh )
	{
		shared_ptr<CBasicMesh> pMesh = mesh.GetMesh();
		if( !pMesh )
			continue;

		pMesh->Render( shader_mgr );
	}

	shader_mgr.SetTechnique( m_NonEnvMappedMesh ); // Do not use envmap texture to render the model to env map texture?
	for( size_t i=0; i<m_vecEnvMappedMesh.size(); i++ )
	{
		CMeshObjectHandle& mesh = m_vecEnvMappedMesh[i];

		if( m_CurrentEnvMapTarget == i )
			continue;

		shared_ptr<CBasicMesh> pMesh = mesh.GetMesh();
		if( !pMesh )
			continue;

		pMesh->Render( shader_mgr );
	}
}

void CEnvMapTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_pCubeMapManager )
				m_pCubeMapManager->SaveCubeTextureToFile( "cube_tex.dds" );
		}
		break;
	default:
		break;
	}
}

void CEnvMapTest::UpdateViewTransform( const Matrix44& matView )
{
//	m_pCubeMapManager->UpdateViewTransform( matView );
}


void CEnvMapTest::UpdateProjectionTransform( const Matrix44& matProj )
{
//	m_pCubeMapManager->UpdateProjectionTransform( matProj );
}
