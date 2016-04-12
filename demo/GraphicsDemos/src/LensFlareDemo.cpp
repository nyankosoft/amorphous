#include "LensFlareDemo.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/LensFlare.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/TextureGenerators/GradationTextureGenerators.hpp"
#include "amorphous/Graphics/TextureGenerators/TCBSplineGradationTextureGenerators.hpp"
#include "amorphous/Utilities/TextFileScannerExtensions.hpp"
#include "amorphous/Support/ParamLoader.hpp"

using std::string;
using namespace boost;


LensFlareDemo::LensFlareDemo()
{
}


LensFlareDemo::~LensFlareDemo()
{
}


void LensFlareDemo::InitSkyTexture()
{
	SFloatRGBAColor top_color    = SFloatRGBAColor(0.596f, 0.773f, 0.988f, 1.000f);
	SFloatRGBAColor mid_color    = SFloatRGBAColor(0.894f, 0.941f, 0.996f, 1.000f);
	SFloatRGBAColor bottom_color = SFloatRGBAColor(0.894f, 0.941f, 0.996f, 1.000f);
//	LoadParamFromFile( "LensFlareDemo/params.txt", "bg_color_top",    top_color );
//	LoadParamFromFile( "LensFlareDemo/params.txt", "bg_color_mid",    mid_color );
//	LoadParamFromFile( "LensFlareDemo/params.txt", "bg_color_bottom", bottom_color );

	m_SkyTexture = CreateHorizontalGradationTexture( 256, 256, TextureFormat::A8R8G8B8, top_color, mid_color, bottom_color );
//	m_SkyTexture = CreateTCBSplineHorizontalGradationTexture( 256, 256, TextureFormat::A8R8G8B8, top_color, mid_color, bottom_color );

	m_SkyTexture.SaveTextureToImageFile( ".debug/sky_texture.png" );
}


static inline SFloatRGBAColor ARGB32toFloatRGBA( U32 argb )
{
	SFloatRGBAColor dest;
	dest.SetARGB32( argb );
	return dest;
}


void LensFlareDemo::InitLensFlare( const string& strPath )
{
	m_pLensFlare = shared_ptr<LensFlare>( new LensFlare() );

	string texture_directory = "LensFlareDemo/textures/";
//	m_pLensFlare->AddTexture( texture_directory + "flare00.dds", 0, 1, 1 );
//	m_pLensFlare->AddTexture( texture_directory + "flare01.dds", 0, 1, 1 );
	m_pLensFlare->AddTexture( texture_directory + "ring.png",    0, 1, 1 );
	m_pLensFlare->AddTexture( texture_directory + "flare02.png", 1, 1, 1 );
	m_pLensFlare->AddTexture( texture_directory + "flare06.png", 2, 1, 1 );
//	m_pLensFlare->AddTexture( texture_directory + "LensFlareTex/flare07.dds", 2, 1, 1 );

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


int LensFlareDemo::Init()
{
	InitLensFlare( "" );

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "Default" );
	m_DefaultTechnique.SetTechniqueName( "Default" );

	// initialize shader
//	bool shader_loaded = m_Shader.Load( "LensFlareDemo/shaders/LensFlareDemo.fx" );

	ShaderResourceDesc shader_desc;
	GenericShaderDesc gs_desc;
	gs_desc.Lighting = false;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gs_desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

	// load skybox mesh
//	m_SkyboxMesh = CreateSkyboxMesh( "LensFlareDemo/textures/skygrad_slim_01.jpg" );
	m_SkyboxMesh = CreateSkyboxMesh( "LensFlareDemo/textures/dark_chrome.jpg" );

	InitSkyTexture();

	// load the terrain mesh
	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "LensFlareDemo/models/terrain06.msh";
	mesh_desc.MeshType     = MeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	m_TestTexture.Load( "LensFlareDemo/textures/flare02.png" );

	MeshResourceDesc sphere_mesh_desc;
	SphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = 2.0f;
	sphere_mesh_desc.pMeshGenerator.reset( new SphereMeshGenerator(sphere_desc) );
	m_LightPosIndicator.Load( sphere_mesh_desc );

	return 0;
}


void LensFlareDemo::Update( float dt )
{
	m_pLensFlare->UpdateLensFlares();

	m_pLensFlare->SetLightPosition( Vector3( 170, 250, 200 ) );
}


void LensFlareDemo::Render()
{
	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

	Matrix44 matWorld = Matrix44Identity();
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

//	RenderAsSkybox( m_SkyboxMesh, GetCurrentCamera().GetPose() );
	RenderSkybox( m_SkyTexture, GetCurrentCamera().GetPose() );
//	RenderSkyCylinder( m_SkyTexture, GetCurrentCamera().GetPosition() );

	shader_mgr.SetWorldTransform( matWorld );

	shader_mgr.SetTechnique( m_MeshTechnique );

	shared_ptr<BasicMesh> pTerrainMesh = m_TerrainMesh.GetMesh();
	if( pTerrainMesh )
		pTerrainMesh->Render( shader_mgr );

	// display light position
	Matrix34 light_pose( m_pLensFlare->GetLightPosition(), Matrix33Identity() );
//	light_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );
	shader_mgr.SetWorldTransform( light_pose );
	shared_ptr<BasicMesh> pLightPosIndicator = m_LightPosIndicator.GetMesh();
	if( pLightPosIndicator )
		pLightPosIndicator->Render();

	// lens flares

	shader_mgr.SetTechnique( m_DefaultTechnique );

	rect.Draw();

	m_pLensFlare->Render( shader_mgr );
}


void LensFlareDemo::UpdateViewTransform( const Matrix44& matView )
{
	m_pLensFlare->UpdateViewTransform( matView );
}


void LensFlareDemo::UpdateProjectionTransform( const Matrix44& matProj )
{
	m_pLensFlare->UpdateProjectionTransform( matProj );
}
