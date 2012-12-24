#include "LensFlareTest.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/LensFlare.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Graphics/TextureGenerators/GradationTextureGenerators.hpp"
#include "gds/Graphics/TextureGenerators/TCBSplineGradationTextureGenerators.hpp"
#include "gds/Utilities/TextFileScannerExtensions.hpp"
#include "gds/Support/ParamLoader.hpp"

using std::string;
using namespace boost;


CLensFlareTest::CLensFlareTest()
{
}


CLensFlareTest::~CLensFlareTest()
{
}


void CLensFlareTest::InitSkyTexture()
{
	SFloatRGBAColor top_color, mid_color, bottom_color;
	LoadParamFromFile( "params.txt", "bg_color_top",    top_color );
	LoadParamFromFile( "params.txt", "bg_color_mid",    mid_color );
	LoadParamFromFile( "params.txt", "bg_color_bottom", bottom_color );

//	m_SkyTexture = CreateHorizontalGradationTexture( 256, 256, TextureFormat::A8R8G8B8, top_color, mid_color, bottom_color );
	m_SkyTexture = CreateTCBSplineHorizontalGradationTexture( 256, 256, TextureFormat::A8R8G8B8, top_color, mid_color, bottom_color );

	m_SkyTexture.SaveTextureToImageFile( "sky_texture.png" );
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
//	m_pLensFlare->AddTexture( "./textures/flare01.dds", 0, 1, 1 );
	m_pLensFlare->AddTexture( "./textures/ring.png",    0, 1, 1 );
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

	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/LensFlareTest.fx" );

	// load skybox mesh
//	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );
	m_SkyboxMesh = CreateSkyboxMesh( "./textures/dark_chrome.jpg" );

	InitSkyTexture();

	// load the terrain mesh
	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/terrain06.msh";
	mesh_desc.MeshType     = CMeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	m_TestTexture.Load( "./textures/flare02.dds" );

	CMeshResourceDesc sphere_mesh_desc;
	CSphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = 2.0f;
	sphere_mesh_desc.pMeshGenerator.reset( new CSphereMeshGenerator(sphere_desc) );
	m_LightPosIndicator.Load( sphere_mesh_desc );

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
//	if( !pShaderMgr )
//		return;

	CShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

//	RenderAsSkybox( m_SkyboxMesh, GetCurrentCamera().GetPose() );
	RenderSkybox( m_SkyTexture, GetCurrentCamera().GetPose() );
//	RenderSkyCylinder( m_SkyTexture, GetCurrentCamera().GetPosition() );

	shader_mgr.SetWorldTransform( matWorld );

	shader_mgr.SetTechnique( m_MeshTechnique );

	shared_ptr<CBasicMesh> pTerrainMesh = m_TerrainMesh.GetMesh();
	if( pTerrainMesh )
		pTerrainMesh->Render( shader_mgr );

	// display light position
	Matrix34 light_pose( m_pLensFlare->GetLightPosition(), Matrix33Identity() );
//	light_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );
	shader_mgr.SetWorldTransform( light_pose );
	FixedFunctionPipelineManager().SetWorldTransform( light_pose );
	shared_ptr<CBasicMesh> pLightPosIndicator = m_LightPosIndicator.GetMesh();
	if( pLightPosIndicator )
		pLightPosIndicator->Render();

	// lens flares

	shader_mgr.SetTechnique( m_DefaultTechnique );

	rect.Draw();

	m_pLensFlare->Render( shader_mgr );
}


void CLensFlareTest::UpdateViewTransform( const Matrix44& matView )
{
	m_pLensFlare->UpdateViewTransform( matView );
}


void CLensFlareTest::UpdateProjectionTransform( const Matrix44& matProj )
{
	m_pLensFlare->UpdateProjectionTransform( matProj );
}
