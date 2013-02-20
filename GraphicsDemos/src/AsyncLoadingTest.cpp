#include "AsyncLoadingTest.hpp"
#include <boost/foreach.hpp>
#include "gds/Graphics.hpp"
#include "gds/Graphics/AsyncResourceLoader.hpp"
//#include "gds/Graphics/GraphicsResourceCacheManager.hpp"
//#include "gds/Graphics/Camera.hpp"
//#include "gds/Graphics/HemisphericLight.hpp"
//#include "gds/Graphics/2DPrimitive/2DRect.hpp"
//#include "gds/Graphics/Mesh/BasicMesh.hpp"
//#include "gds/Graphics/Shader/ShaderManager.hpp"
//#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
//#include "gds/Graphics/Shader/ShaderLightManager.hpp"
//#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"
#include "gds/GUI.hpp"

using std::string;
using namespace boost;


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


CAsyncLoadingTest::CAsyncLoadingTest()
:
m_NumTextureMipmaps( 1 )
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_TestAsyncLoading = true;

	int enabled = 1;
	LoadParamFromFile( "AsyncLoadingDemo/params.txt", "cached_resource", enabled );
	m_UseCachedResources = (enabled == 1);

	LoadParamFromFile( "AsyncLoadingDemo/params.txt", "num_texture_mipmaps", m_NumTextureMipmaps );

	GraphicsResourceManager().AllowAsyncLoading( m_TestAsyncLoading );


//	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0, 1, -50 ) );
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
	bool shader_loaded = m_Shader.Load( "AsyncLoadingDemo/shaders/AsyncLoadingTest.fx" );
	
	if( !shader_loaded )
		return false;

	ShaderLightManager *pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager().get();

	HemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

//	pShaderLightMgr->SetLight( 0, light );
//	pShaderLightMgr->SetDirectionalLightOffset( 0 );
//	pShaderLightMgr->SetNumDirectionalLights( 1 );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	Matrix44 proj = Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );
	m_Shader.GetShaderManager()->SetProjectionTransform( proj );

	return true;
}


void CAsyncLoadingTest::CreateCachedResources()
{
	TextureResourceDesc tex_desc;
	tex_desc.Format    = TextureFormat::A8R8G8B8;
	tex_desc.Width     = 1024;
	tex_desc.Height    = 1024;

	tex_desc.MipLevels = m_NumTextureMipmaps;

	const int num_textures_to_preload = 8;
	for( int i=0; i<num_textures_to_preload; i++ )
	{
		GraphicsResourceCacheManager().AddCache( tex_desc );
	}
}


int CAsyncLoadingTest::Init()
{
	shared_ptr<TextureFont> pTexFont( new TextureFont );
	pTexFont->InitFont( GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" ) );
	pTexFont->SetFontSize( 6, 12 );
	m_pFont = pTexFont;

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
		m_vecMesh.push_back( MeshHandle() );

		MeshResourceDesc desc;
		desc.ResourcePath = filepath;

		if( m_TestAsyncLoading )
			desc.LoadOptionFlags = MeshLoadOption::DO_NOT_LOAD_TEXTURES;

		m_vecMesh.back().Load( desc );
	}
*/

	m_vecMesh.push_back( CTestMeshHolder( "AsyncLoadingDemo/models/sample_level_00.msh",   CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "AsyncLoadingDemo/models/FlakySlate.msh",        CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1, 100), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "AsyncLoadingDemo/models/HighAltitude.msh",      CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY,   Matrix34( Vector3(-25,1, 100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/RustPeel.msh",          CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1,-100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/SmashedGrayMarble.msh", CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3(-25,1,-100), Matrix33Identity() ) ) );

	InitShader();

	if( m_TestAsyncLoading )
	{
		if( m_UseCachedResources )
		CreateCachedResources();
	}

	return 0;
}


//void CAsyncLoadingTest::LoadTexturesAsync()
void CAsyncLoadingTest::LoadResourcesAsync( CTestMeshHolder& holder )
{
	const Vector3 vCamPos = GetCurrentCamera().GetPosition();

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

	BasicMesh *pMesh = holder.m_Handle.GetMesh().get();
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
		TextureHandle& tex = pMesh->GetTexture( i, 0 );

		if( fDist < 25.0f // dist between cam position and the triangle set is less than 25m - for small parts
		 || world_aabb.IsPointInside(vCamPos) ) // cam position is included in the AABB of the triangle set - for large parts
		{
//			if( tex.IsLoaded() == false )
			if( tex.GetEntryState() == GraphicsResourceState::RELEASED
			 && !tex.GetEntry() )
			{
				TextureResourceDesc desc = pMesh->GetMaterial(i).TextureDesc[0];
				desc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
				desc.Format = TextureFormat::A8R8G8B8;
				desc.MipLevels = m_NumTextureMipmaps;

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
	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );

	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	pShaderManager->SetViewerPosition( GetCurrentCamera().GetPosition() );

	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	pShaderManager->SetTechnique( m_MeshTechnique );
//	BOOST_FOREACH( MeshHandle& mesh, m_vecMesh )
	BOOST_FOREACH( CTestMeshHolder& holder, m_vecMesh )
	{
//		BasicMesh *pMesh = mesh.GetMesh().get();

		if( holder.m_Handle.GetEntryState() == GraphicsResourceState::LOADED )
		{
			// set world transform
			FixedFunctionPipelineManager().SetWorldTransform( holder.m_Pose );
			pShaderManager->SetWorldTransform( holder.m_Pose );

			BasicMesh *pMesh = holder.m_Handle.GetMesh().get();

			if( pMesh )
				pMesh->Render( *pShaderManager );
		}
	}

	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void CAsyncLoadingTest::Render()
{
	PROFILE_FUNCTION();

//	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

	RenderMeshes();

	if( m_pSampleUI )
		m_pSampleUI->Render();

	AsyncResourceLoader().ProcessGraphicsDeviceRequests();

	m_TextBuffer.resize( 0 );
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = GetCurrentCamera().GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
}


void CAsyncLoadingTest::SaveTexturesAsImageFiles()
{
	MeshHandle& mesh_handle = m_vecMesh[0].m_Handle;//m_vecMesh[0];
	if( m_vecMesh.size() == 0 || !mesh_handle.GetMesh() )
		return;

	BasicMesh *pMesh = mesh_handle.GetMesh().get();

	const int num_materials = pMesh->GetNumMaterials();
	for( int i=0; i<num_materials; i++ )
	{
//		for(each texture)
//		{
			TextureHandle& tex = pMesh->GetTexture( i, 0 );
			if( !tex.GetEntry() )
				continue;

			shared_ptr<TextureResource> pTexture = tex.GetEntry()->GetTextureResource();
			if( !pTexture )
				continue;

			pTexture->SaveTextureToImageFile( fmt_string( "./debug/mat%02d.dds", i ) );
//		}
	}
}


void CAsyncLoadingTest::HandleInput( const InputData& input )
{
	if( m_pUIInputHandler )
	{
//		InputHandler::ProcessInput() does not take const InputData&
		InputData input_copy = input;
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


void CAsyncLoadingTest::LoadGraphicsResources( const GraphicsParameters& rParam )
{
//	CreateSampleUI();
}
