#include "AsyncLoadingTest.hpp"
#include "3DMath/Matrix34.hpp"
#include "Graphics/all.hpp"
#include "Graphics/AsyncResourceLoader.hpp"
#include "Support/Timer.hpp"
#include "Support/Profile.hpp"
#include "Support/Macro.h"
#include "GUI.hpp"

using namespace std;
using namespace boost;


static int gs_TextureMipLevels = 1;


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
	return new CAsyncLoadingTest();
}


extern const std::string GetAppTitle()
{
	return string("AsyncLoadingTest");
}


CAsyncLoadingTest::CAsyncLoadingTest()
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_TestAsyncLoading = true;

	GraphicsResourceManager().AllowAsyncLoading( m_TestAsyncLoading );


	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CAsyncLoadingTest::~CAsyncLoadingTest()
{
}


void CAsyncLoadingTest::CreateSampleUI()
{
}


bool CAsyncLoadingTest::InitShader()
{
	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/AsyncLoadingTest.fx" );
	
	if( !shader_loaded )
		return false;

	CShaderLightManager *pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager().get();

	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

//	pShaderLightMgr->SetLight( 0, light );
//	pShaderLightMgr->SetDirectionalLightOffset( 0 );
//	pShaderLightMgr->SetNumDirectionalLights( 1 );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );
	m_Shader.GetShaderManager()->SetProjectionTransform( matProj );

	return true;
}


void CAsyncLoadingTest::CreateCachedResources()
{
	CTextureResourceDesc tex_desc;
	tex_desc.Format    = TextureFormat::A8R8G8B8;
	tex_desc.Width     = 1024;
	tex_desc.Height    = 1024;

	tex_desc.MipLevels = gs_TextureMipLevels;

	const int num_textures_to_preload = 8;
	for( int i=0; i<num_textures_to_preload; i++ )
	{
		GraphicsResourceCacheManager().AddCache( tex_desc );
	}
}


int CAsyncLoadingTest::Init()
{
	m_pFont = shared_ptr<CFontBase>( new CFont( "ÇlÇr ÉSÉVÉbÉN", 6, 12 ) );
//	m_pFont = shared_ptr<CFontBase>( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );

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

	m_vecMesh.push_back( CTestMeshHolder( "./models/sample_level_00.msh",   CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/FlakySlate.msh",        CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1, 100), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/HighAltitude.msh",      CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY,   Matrix34( Vector3(-25,1, 100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/RustPeel.msh",          CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1,-100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/SmashedGrayMarble.msh", CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3(-25,1,-100), Matrix33Identity() ) ) );

	InitShader();

	if( m_TestAsyncLoading )
		CreateCachedResources();

	return 0;
}


//void CAsyncLoadingTest::LoadTexturesAsync()
void CAsyncLoadingTest::LoadResourcesAsync( CTestMeshHolder& holder )
{
	const Vector3 vCamPos = g_Camera.GetPosition();

//	if( m_vecMesh.size() == 0 )
//		return;

//	CD3DXMeshObjectBase *pMesh = m_vecMesh[0].GetMesh().get();

	if( holder.m_LoadingStyle == CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER
	 && Vec3Length( holder.m_Pose.vPosition - vCamPos ) < 25.0f
	 && holder.m_Handle.GetEntryState() == GraphicsResourceState::RELEASED )
	{
		// asynchronously load the mesh and its texture(s)
		holder.m_Handle.Load( holder.m_MeshDesc );
	}

	if( holder.m_LoadingStyle == CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER )
		return;

	CD3DXMeshObjectBase *pMesh = holder.m_Handle.GetMesh().get();
	if( !pMesh )
		return;

	const int num_materials = pMesh->GetNumMaterials();
	for( int i=0; i<num_materials; i++ )
	{
		AABB3 aabb = pMesh->GetAABB(i);

		AABB3 world_aabb;
		Vector3 vWorldPos = holder.m_Pose.vPosition;
		world_aabb.vMin = vWorldPos + aabb.vMin - Vector3(1,1,1) * 3.0f;
		world_aabb.vMax = vWorldPos + aabb.vMax + Vector3(1,1,1) * 3.0f;

		Vector3 vDist = world_aabb.GetCenterPosition() - vCamPos;
		const float fDist = Vec3Length(vDist);
		CTextureHandle& tex = pMesh->GetTexture( i, 0 );

		if( fDist < 25.0f // dist between cam position and the triangle set is less than 25m - for small parts
		 || world_aabb.IsPointInside(vCamPos) ) // cam position is included in the AABB of the triangle set - for large parts
		{
//			if( tex.IsLoaded() == false )
			if( tex.GetEntryState() == GraphicsResourceState::RELEASED
			 && !tex.GetEntry() )
			{
				CTextureResourceDesc desc = pMesh->GetMaterial(i).TextureDesc[0];
				desc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
				desc.Format = TextureFormat::A8R8G8B8;
				desc.MipLevels = gs_TextureMipLevels;

				tex.Load( desc );
			}
		}
		else if( 30.0f < fDist 
		 && !world_aabb.IsPointInside(vCamPos) )
		{
			if( tex.GetEntryState() == GraphicsResourceState::LOADED )
				tex.Release();
		}
	}
}


void CAsyncLoadingTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );

	if( true /*m_TestAsyncLoading*/ )
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
	}
}


void CAsyncLoadingTest::RenderMeshes()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	pShaderManager->SetTechnique( m_MeshTechnique );
//	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecMesh )
	BOOST_FOREACH( CTestMeshHolder& holder, m_vecMesh )
	{
//		CD3DXMeshObjectBase *pMesh = mesh.GetMesh().get();

		if( holder.m_Handle.GetEntryState() == GraphicsResourceState::LOADED )
		{
			// set world transform
			holder.m_Pose.GetRowMajorMatrix44( matWorld );
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			pShaderManager->SetWorldTransform( matWorld );

			CD3DXMeshObjectBase *pMesh = holder.m_Handle.GetMesh().get();

			if( pMesh )
				pMesh->Render( *pShaderManager );
		}
	}

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CAsyncLoadingTest::Render()
{
	PROFILE_FUNCTION();

//	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

	RenderMeshes();

	if( m_pSampleUI )
		m_pSampleUI->Render();

	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

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


void CAsyncLoadingTest::SaveTexturesAsImageFiles()
{
	CMeshObjectHandle& mesh_handle = m_vecMesh[0].m_Handle;//m_vecMesh[0];
	if( m_vecMesh.size() == 0 || !mesh_handle.GetMesh() )
		return;

	CD3DXMeshObjectBase *pMesh = mesh_handle.GetMesh().get();

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


void CAsyncLoadingTest::HandleInput( const SInputData& input )
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
			SaveTexturesAsImageFiles();
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


void CAsyncLoadingTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CAsyncLoadingTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
