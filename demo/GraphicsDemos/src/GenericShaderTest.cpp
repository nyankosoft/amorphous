#include "GenericShaderTest.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Support/Macro.h"

using std::string;
using std::vector;
using namespace boost;


CGenericShaderTest::CGenericShaderTest()
:
m_DisplayDebugInfo(false)
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	if( CameraController() )
		CameraController()->SetPosition( Vector3( 0, 1, -50 ) );
}


CGenericShaderTest::~CGenericShaderTest()
{
}


void CGenericShaderTest::CreateSampleUI()
{
}


bool CGenericShaderTest::SetShaderParams( ShaderManager& shader_mgr )
{
	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();

	if( !pShaderLightMgr )
		return false;

	HemisphericDirectionalLight dir_light_0, dir_light_1;

	dir_light_0.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 0.8f, 0.1f, 1.0f );
	dir_light_0.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
	dir_light_0.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

	dir_light_1.Attribute.UpperDiffuseColor.SetRGBA( 0.8f, 0.8f, 1.0f, 1.0f );
	dir_light_1.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
	dir_light_1.vDirection = Vec3GetNormalized( Vector3( 1.0f, -1.5f, -0.6f ) );

	HemisphericPointLight pnt_light_0, pnt_light_1;

	pnt_light_0.Attribute.UpperDiffuseColor.SetRGBA( 0.1f, 0.6f, 0.1f, 1.0f );
	pnt_light_0.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
	pnt_light_0.vPosition = Vector3( -9.0f, 9.0f, 11.0f );

	pnt_light_1.Attribute.UpperDiffuseColor.SetRGBA( 0.6f, 0.6f, 0.1f, 1.0f );
	pnt_light_1.Attribute.LowerDiffuseColor.SetRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
	pnt_light_1.vPosition = Vector3( 12.0f, 12.5f, -0.6f );

	pShaderLightMgr->ClearLights();
	pShaderLightMgr->SetHemisphericDirectionalLight( dir_light_0 );
//	pShaderLightMgr->SetHemisphericDirectionalLight( dir_light_1 );
	pShaderLightMgr->SetHemisphericPointLight( pnt_light_0 );
//	pShaderLightMgr->SetHemisphericPointLight( pnt_light_1 );
	pShaderLightMgr->CommitChanges();

//	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );
	shader_mgr.SetParam( "g_vEyeVS",  GetCurrentCamera().GetCameraMatrix() * GetCurrentCamera().GetPosition() );
	shader_mgr.SetParam( "g_vEyePos", GetCurrentCamera().GetPosition() );

	return true;
}


bool CGenericShaderTest::InitShader()
{
	ShaderResourceDesc shader_desc;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(m_GenericShaderDesc) );

	bool loaded = m_Shader.Load( shader_desc );

	m_Technique = ShaderTechniqueHandle();
	m_Technique.SetTechniqueName( "Default" );

	return loaded;
}


int CGenericShaderTest::Init()
{
/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

	m_Meshes.resize( 1 );
	m_Meshes[0].Load( "./models/fw43.msh" );
//	m_Meshes[1].Load( "./models/FlakySlate.msh" );
//	m_Meshes[2].Load( "./models/HighAltitude.msh" );

	InitShader();

	return 0;
}


void CGenericShaderTest::Update( float dt )
{
//	if( m_pSampleUI )
//		m_pSampleUI->Update( dt );
}


void CGenericShaderTest::RenderMeshes()
{
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	ShaderManager *pShaderManager = m_Shader.GetShaderManager();

	ShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	SetShaderParams( shader_mgr );

	// render the scene

	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	Result::Name res = pShaderManager->SetTechnique( m_Technique );
	for( size_t i=0; i<m_Meshes.size(); i++ )
	{
		boost::shared_ptr<BasicMesh> pMesh = m_Meshes[i].GetMesh();

		if( !pMesh )
			continue;

		// set world transform
		FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
		shader_mgr.SetWorldTransform( Matrix44Identity() );

		pMesh->Render( shader_mgr );
	}

	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void CGenericShaderTest::RenderDebugInfo()
{
//	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

//	Vector2 vTopLeft(     (float)GetWindowWidth() / 4.0f,  16.0f );
//	Vector2 vBottomRight( (float)GetWindowWidth() - 16.0f, (float)GetWindowHeight() * 3.0f / 2.0f );
//	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

//	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	int ch = m_pFont->GetFontHeight();
	int rh = (int)(ch * 1.2);// row height

	Vector3 vCamPos = GetCurrentCamera().GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
}


void CGenericShaderTest::RenderShaderInfo()
{
	const std::string shader_info[] =
	{
		fmt_string( "[H] lighting (normal/hemishperic):   %d", (int)m_GenericShaderDesc.LightingTechnique ),
		fmt_string( "[L] per-vertx or per-pixel lighting: %d", (int)m_GenericShaderDesc.LightingType ),
		fmt_string( "[G] specular:                        %d", (int)m_GenericShaderDesc.Specular ),
		fmt_string( "[I] the number directional lights:   %d", (int)m_GenericShaderDesc.NumDirectionalLights ),
		fmt_string( "[P] the number point lights:         %d", (int)m_GenericShaderDesc.NumPointLights ),
	};

	for( int i=0; i<numof(shader_info); i++ )
	{
		m_pFont->DrawText( shader_info[i], Vector2( 20, (float)(400 + i * 16) ) );
	}
}


void CGenericShaderTest::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

//	if( m_pSampleUI )
//		m_pSampleUI->Render();

	if( m_DisplayDebugInfo )
		RenderDebugInfo();

	RenderShaderInfo();
}


void CGenericShaderTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case 'G':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			int num_specular_types = CSpecularSource::NUM_TYPES;
			int num_specular_types = 3;
			m_GenericShaderDesc.Specular = (SpecularSource::Name)( (m_GenericShaderDesc.Specular + 1) % num_specular_types );
			InitShader();
		}
		break;

	case 'I':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			int num_max_directional_lights = 4;
			m_GenericShaderDesc.NumDirectionalLights += 1;
			if( num_max_directional_lights <= m_GenericShaderDesc.NumDirectionalLights )
				m_GenericShaderDesc.NumDirectionalLights = -1; // -1: an arbitrary number of lights.
			InitShader();
		}
		break;

	case 'P':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			int num_max_point_lights = 4;
			m_GenericShaderDesc.NumPointLights += 1;
			if( num_max_point_lights <= m_GenericShaderDesc.NumPointLights )
				m_GenericShaderDesc.NumPointLights = -1; // -1: an arbitrary number of lights.
			InitShader();
		}
		break;

	case GIC_F1:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_DisplayDebugInfo = !m_DisplayDebugInfo;
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
