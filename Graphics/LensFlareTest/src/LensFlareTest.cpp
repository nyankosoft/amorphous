#include "LensFlareTest.hpp"
#include "3DMath/Matrix34.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/LensFlare.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Font/Font.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/SkyboxMisc.hpp"
#include "Support/CameraController_Win32.hpp"

using namespace std;
using namespace boost;


extern CPlatformDependentCameraController g_CameraController;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CLensFlareTest();
}


extern const std::string GetAppTitle()
{
	return string("Lens Flare Test");
}



CLensFlareTest::CLensFlareTest()
{
}


CLensFlareTest::~CLensFlareTest()
{
}


static inline SFloatRGBAColor ARGB32toFloatRGBA( U32 argb )
{
	SFloatRGBAColor dest;
	dest.SetARGB32( argb );
	return dest;
}


void CLensFlareTest::InitLensFlare( const string& strPath )
{
	m_pLensFlare = shared_ptr<CLensFlare>( new CLensFlare() );

//	m_pLensFlare->AddTexture( "./textures/LensFlareTex/flare00.dds", 0, 1, 1 );
	m_pLensFlare->AddTexture( "./textures/flare01.dds", 0, 1, 1 );
	m_pLensFlare->AddTexture( "./textures/flare02.dds", 1, 1, 1 );
	m_pLensFlare->AddTexture( "./textures/flare06.dds", 2, 1, 1 );
//	m_pLensFlare->AddTexture( "./textures/LensFlareTex/flare07.dds", 2, 1, 1 );

	m_pLensFlare->AddLensFlareRect( 0.09f, 1.0f, 0.70f, ARGB32toFloatRGBA(0x40FF5030), 0 );
	m_pLensFlare->AddLensFlareRect( 0.08f, 1.0f, 1.50f, ARGB32toFloatRGBA(0x28FFD040), 0 );
	m_pLensFlare->AddLensFlareRect( 0.07f, 1.0f, 0.47f, ARGB32toFloatRGBA(0x30F03060), 0 );
	m_pLensFlare->AddLensFlareRect( 0.05f, 1.0f, 1.40f, ARGB32toFloatRGBA(0x202040F0), 0 );
	m_pLensFlare->AddLensFlareRect( 0.04f, 1.0f, 0.80f, ARGB32toFloatRGBA(0x3260D080), 0 );
	m_pLensFlare->AddLensFlareRect( 0.05f, 1.0f, 0.60f, ARGB32toFloatRGBA(0x10FF2020), 0 );
	m_pLensFlare->AddLensFlareRect( 0.04f, 1.0f, 0.42f, ARGB32toFloatRGBA(0x302020FF), 0 );
	m_pLensFlare->AddLensFlareRect( 0.10f, 1.0f, 0.35f, ARGB32toFloatRGBA(0x20FFFFFF), 1 );
	m_pLensFlare->AddLensFlareRect( 0.03f, 1.0f, 1.20f, ARGB32toFloatRGBA(0x20C0C0C0), 2 );
	m_pLensFlare->AddLensFlareRect( 0.04f, 1.0f, 0.19f, ARGB32toFloatRGBA(0x30F0F0F0), 2 );

//	m_pLensFlare->UpdateScreenSize( GetWindowWidth(), GetWindowHeight() );
}


int CLensFlareTest::Init()
{
	InitLensFlare( "" );

	m_pFont = CFontSharedPtr( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/LensFlareTest.fx" );

	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/terrain06.msh";
	mesh_desc.MeshType     = CMeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	m_TestTexture.Load( "./textures/flare02.dds" );

	m_LightPosIndicator.Init();

	return 0;
}


void CLensFlareTest::Update( float dt )
{
	m_pLensFlare->UpdateLensFlares();

	m_pLensFlare->SetLightPosition( Vector3( 170, 250, 200 ) );
}


void CLensFlareTest::Render()
{
	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

	Matrix44 matWorld = Matrix44Identity();
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	RenderAsSkybox( m_SkyboxMesh, g_CameraController.GetPosition() );

	pShaderMgr->SetWorldTransform( matWorld );

	pShaderMgr->SetTechnique( m_MeshTechnique );
	
	m_TerrainMesh.GetMesh().get()->Render( *pShaderMgr );

	// display light position
	Matrix34 light_pose( m_pLensFlare->GetLightPosition(), Matrix33Identity() );
//	light_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );
	pShaderMgr->SetWorldTransform( light_pose );
	FixedFunctionPipelineManager().SetWorldTransform( light_pose );
	m_LightPosIndicator.Draw();

	// lens flares

	pShaderMgr->SetTechnique( m_DefaultTechnique );

	rect.Draw();

	m_pLensFlare->Render( *pShaderMgr );
}


void CLensFlareTest::UpdateViewTransform( const Matrix44& matView )
{
	m_pLensFlare->UpdateViewTransform( matView );
}


void CLensFlareTest::UpdateProjectionTransform( const Matrix44& matProj )
{
	m_pLensFlare->UpdateProjectionTransform( matProj );
}
