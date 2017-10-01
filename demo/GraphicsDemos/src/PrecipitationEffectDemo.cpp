#include "PrecipitationEffectTest.hpp"
#include "amorphous/Graphics/DoubleConeScrollEffect.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/TextureGenerators/GradationTextureGenerators.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/Vec3_StringAux.hpp"

using std::string;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new PrecipitationEffectDemo();
}


PrecipitationEffectDemo::PrecipitationEffectDemo()
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );
//	SetBackgroundColor( SFloatRGBAColor::Black() );

	Vector3 init_camera_position( Vector3( 0, 1, -12 ) );
//	Vector3 init_camera_position( Vector3( 0, 520, 120 ) );
	if( CameraController() )
		CameraController()->SetPosition( init_camera_position );
}


PrecipitationEffectDemo::~PrecipitationEffectDemo()
{
}


void PrecipitationEffectDemo::CreateSampleUI()
{
}


bool PrecipitationEffectDemo::InitShader()
{
	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/PrecipitationEffectTest.fx" );
	
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

	return true;
}


int PrecipitationEffectDemo::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 6, 12 );

/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

//	bool loaded = m_SkyTexture.Load( "textures/skygrad_slim_01.jpg" );
	m_SkyboxMesh = CreateSkyboxMesh( "textures/skygrad_slim_01.jpg" );

	SFloatRGBAColor top, mid, bottom;
	top.SetARGB32( 0xFFDDDFE0 );
	mid.SetARGB32( 0xFFF0F5FA );
	bottom.SetARGB32( 0xFFDDDFE0 );
	m_SkyTexture = CreateHorizontalGradationTexture( 256, 256, TextureFormat::A8R8G8B8,
		top, mid, bottom );

//	m_SkyTexture.Load( "textures/non-tilable_texture.png" );

	m_SkyTexture.SaveTextureToImageFile( "sky.png" );

	InitShader();

	m_pDoubleConeScrollEffect.reset( new CDoubleConeScrollEffect );
	string texture_pathname;
//	texture_pathname = "textures/precipitation_mid-density-512.dds";
//	texture_pathname = "textures/non-tilable_texture.png";
//	texture_pathname = "textures/4x4_flag.png";
//	texture_pathname = "textures/blunt_precipitation.png";
	texture_pathname = "textures/blunt_precipitation-s256.png";
	m_pDoubleConeScrollEffect->SetTextureFilepath( texture_pathname );
	m_pDoubleConeScrollEffect->Init();

	return 0;
}


void PrecipitationEffectDemo::Update( float dt )
{
	if( m_pDoubleConeScrollEffect )
	{
		m_pDoubleConeScrollEffect->SetCameraPose( GetCurrentCamera().GetPose() );
		m_pDoubleConeScrollEffect->Update( dt );
	}
}


void PrecipitationEffectDemo::RenderMeshes()
{
	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );

	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );

	GetShaderManagerHub().PushViewAndProjectionMatrices( g_Camera );

	// set world transform

	// render mesh

	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void PrecipitationEffectDemo::Render()
{
	PROFILE_FUNCTION();

	Result::Name res = GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	RenderSkybox( m_SkyTexture, GetCurrentCamera().GetPose() );

//	RenderAsSkybox( m_SkyboxMesh, GetCurrentCamera().GetPose() );

//	RenderMeshes();

	if( m_pDoubleConeScrollEffect )
		m_pDoubleConeScrollEffect->Render();

	m_TextBuffer.resize( 0 );
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     (float)GetWindowWidth() / 4.0f,  16.0f );
	Vector2 vBottomRight( (float)GetWindowWidth() - 16.0f, (float)GetWindowHeight() * 3.0f / 2.0f );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = GetCurrentCamera().GetPosition();
	m_pFont->DrawText( string("camera: ") + to_string(vCamPos), Vector2( 20, 300 ) );
}


void PrecipitationEffectDemo::HandleInput( const InputData& input )
{
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
			m_SkyTexture.SaveTextureToImageFile( "sky.png" );
		}
		break;
	default:
		break;
	}
}
