#include "PrimitiveShapeMeshesTest.hpp"
#include <gds/3DMath/Matrix34.hpp>
#include <gds/Graphics.hpp>
#include <gds/Graphics/AsyncResourceLoader.hpp>
#include <gds/Graphics/MeshGenerators.hpp>
#include <gds/Graphics/Shader/ShaderLightManager.hpp>
#include <gds/Support/Timer.hpp>
#include <gds/Support/Profile.hpp>
#include <gds/Support/Macro.h>
#include <gds/GUI.hpp>

using namespace std;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CPrimitiveShapeMeshesTest();
}


extern const std::string GetAppTitle()
{
	return string("PrimitiveShapeMeshesTest");
}


CPrimitiveShapeMeshesTest::CPrimitiveShapeMeshesTest()
:
m_MeshIndex(0),
m_NumPrimitiveMeshes(0)
{
//	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_MeshTechnique.SetTechniqueName( "SingleHSDL_Specular_CTS" );
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_TestAsyncLoading = false;


	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CPrimitiveShapeMeshesTest::~CPrimitiveShapeMeshesTest()
{
}


void CPrimitiveShapeMeshesTest::CreateGUIControls()
{
}


bool CPrimitiveShapeMeshesTest::InitShader()
{
	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/PrimitiveShapeMeshesTest.fx" );
	
	if( !shader_loaded )
		return false;

	shared_ptr<CShaderLightManager> pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager();

	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

	shared_ptr<CShaderLightManager> pHLSLShaderLightMgr
		= dynamic_pointer_cast<CShaderLightManager,CShaderLightManager>(pShaderLightMgr);

//	pHLSLShaderLightMgr->SetDirectionalLightOffset( 0 );
//	pHLSLShaderLightMgr->SetNumDirectionalLights( 1 );
	pHLSLShaderLightMgr->SetHemisphericDirectionalLight( light );

	return true;
}


int CPrimitiveShapeMeshesTest::Init()
{
	m_pFont = shared_ptr<CFontBase>( new CFont( "ÇlÇr ÉSÉVÉbÉN", 6, 12 ) );
//	m_pFont = shared_ptr<CFontBase>( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );

	m_ConeTexture.Load( "./textures/white.bmp" );
/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

	CMeshResourceDesc mesh_desc[3];
	m_vecMesh.resize( numof(mesh_desc) );

	shared_ptr<CBoxMeshGenerator> pBoxGenerator( new CBoxMeshGenerator() );
	pBoxGenerator->SetEdgeLengths( Vector3( 1.0f, 1.0f, 1.0f ) );
	mesh_desc[0].pMeshGenerator = pBoxGenerator;
	mesh_desc[0].pMeshGenerator->SetTexturePath( "./textures/AshySandstone.jpg" );
	mesh_desc[0].ResourcePath = "BoxMesh";
	m_vecMesh[0].Load( mesh_desc[0] );

	CConeDesc cone_desc;
	cone_desc.cone_height = 4.0f;
	cone_desc.body_height = 4.0f;
	mesh_desc[1].pMeshGenerator = shared_ptr<CMeshGenerator>( new CConeMeshGenerator( cone_desc ) );
	mesh_desc[1].ResourcePath = "ConeMesh";
	mesh_desc[1].pMeshGenerator->SetTexturePath( "./textures/AshySandstone.jpg" );
	m_vecMesh[1].Load( mesh_desc[1] );

	CSphereDesc shpere_desc;
	shared_ptr<CSphereMeshGenerator> pSphereGenerator( new CSphereMeshGenerator(shpere_desc) );
	mesh_desc[2].pMeshGenerator = pSphereGenerator;
	mesh_desc[2].pMeshGenerator->SetTexturePath( "./textures/AshySandstone.jpg" );
	mesh_desc[2].ResourcePath = "SphereMesh";
	m_vecMesh[2].Load( mesh_desc[2] );

	m_NumPrimitiveMeshes = 2;

	InitShader();

	return 0;
}


void CPrimitiveShapeMeshesTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );
}


void CPrimitiveShapeMeshesTest::RenderMeshes()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	Matrix44 matWorld( Matrix44Identity() );

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	pShaderManager->SetTechnique( m_MeshTechnique );
	pShaderManager->GetShaderLightManager()->CommitChanges();

//	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecMesh )
//	{
		shared_ptr<CBasicMesh> pMesh = m_vecMesh[m_MeshIndex].GetMesh();//mesh.GetMesh();

		if( pMesh )
		{
			// set world transform
//			holder.m_Pose.GetRowMajorMatrix44( matWorld );
			FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
			pShaderManager->SetWorldTransform( Matrix44Identity() );

			pShaderManager->SetTexture( 0, m_ConeTexture );
			pShaderManager->GetEffect()->CommitChanges();
			pMesh->RenderSubset( *pShaderManager, 0 );
//			pMesh->Render( *pShaderManager );
		}
//	}

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CPrimitiveShapeMeshesTest::Render()
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
/*	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();
*/
	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = g_Camera.GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
}


void CPrimitiveShapeMeshesTest::HandleInput( const SInputData& input )
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
			m_MeshIndex = (m_MeshIndex + 1) % m_NumPrimitiveMeshes;
		}
		break;
	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_MeshIndex = (m_MeshIndex + m_NumPrimitiveMeshes - 1) % m_NumPrimitiveMeshes;
		}
		break;
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		break;
	}
}


void CPrimitiveShapeMeshesTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CPrimitiveShapeMeshesTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateGUIControls();
}
