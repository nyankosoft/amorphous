#include "HLSLEffectDemo.hpp"
#include "amorphous/3DMath/MatrixConversions.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Utilities/TextFileScannerExtensions.hpp"
#include "amorphous/Input.hpp"

using std::string;
using std::vector;
using namespace boost;


HLSLEffectDemo::HLSLEffectDemo()
:
m_CurrentShaderIndex( 0 ),
m_CurrentMeshIndex( 0 ),
m_DisplayDebugInfo(false)
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	if( CameraController() )
		CameraController()->SetPosition( Vector3( 0, 1, -30 ) );

	for( int i=0; i<numof(m_EnableLight); i++ )
		m_EnableLight[i] = 1;
}


HLSLEffectDemo::~HLSLEffectDemo()
{
}


bool HLSLEffectDemo::SetShader( int index )
{
	if( index < 0 || (int)m_Shaders.size() <= index )
		return false;

	// initialize shader
/*	bool shader_loaded = m_Shaders[index].Load( "./shaders/HLSLEffectDemo.fx" );

	if( !shader_loaded )
		return false;
*/
	if( !m_Shaders[index].GetShaderManager() )
		return false;

	ShaderManager& shader_mgr = *(m_Shaders[index].GetShaderManager());

	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();

	// Set lights

	pShaderLightMgr->ClearLights();

	if( m_EnableLight[0] )
	{
		HemisphericDirectionalLight dir_light;
		dir_light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
		dir_light.Attribute.LowerDiffuseColor.SetRGBA( 0.3f, 0.3f, 0.3f, 1.0f );
		dir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

		pShaderLightMgr->SetHemisphericDirectionalLight( dir_light );
	}

	if( m_EnableLight[1] )
	{
		HemisphericPointLight pnt_light;
		pnt_light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
		pnt_light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
		pnt_light.vPosition = Vector3( 10.0f, 3.0f, -15.0f );
		pnt_light.fAttenuation[0] = 0.1f;
		pnt_light.fAttenuation[1] = 0.1f;
		pnt_light.fAttenuation[2] = 0.1f;
		ParamLoader loader( "params.txt" );
		if( loader.IsReady() )
		{
			loader.LoadParam( "pnt_light_position",      pnt_light.vPosition );
//			loader.LoadParam( "pnt_light_diffuse_upper", pnt_light.Attribute.UpperDiffuseColor );
//			loader.LoadParam( "pnt_light_diffuse_lower", pnt_light.Attribute.LowerDiffuseColor );
			float *att = pnt_light.fAttenuation;

			loader.LoadParam( "pnt_light_attenuations", att[0], att[1], att[2] );
		}
		pShaderLightMgr->SetHemisphericPointLight( pnt_light );
	}

	pShaderLightMgr->CommitChanges();

//	shader_mgr.SetParam( "g_vEyeVS", GetCurrentCamera().GetCameraMatrix() * GetCurrentCamera().GetPosition() );
	shader_mgr.SetParam( "g_vEyePos", GetCurrentCamera().GetPosition() );

	// for bright rim lights
	shader_mgr.SetParam( "g_vEyeDir", GetCurrentCamera().GetPosition() );
	shader_mgr.SetTexture( 3, m_LookupTextureForLighting );

	m_CurrentShaderIndex = index;

	return true;
}


bool HLSLEffectDemo::InitShaders()
{
	vector<string> shaders;
	vector<string> techs;
	shaders.resize( 4 );
	techs.resize( shaders.size() );

	shaders[0] = "shaders/PerVertexHSLighting.fx";             techs[0] = "PerVertexHSLighting";
//	shaders[?] = "shaders/PerPixelHSLighting.fx";              techs[?] = "PVL_HSLs_Specular";
	shaders[1] = "shaders/PerPixelHSLighting.fx";              techs[1] = "PPL_HSLs";
	shaders[2] = "shaders/PerPixelHSLightingWithSpecular.fx";  techs[2] = "PPL_HSLs_Specular";
	shaders[3] = "shaders/PerPixelHSLighting_2DLUT.fx";        techs[3] = "HSLs_2DLUT";

	m_Shaders.resize( shaders.size() );
	m_Techniques.resize( shaders.size() );
	for( size_t i=0; i<shaders.size(); i++ )
	{
		bool loaded = m_Shaders[i].Load( shaders[i] );

		m_Techniques[i].SetTechniqueName( techs[i].c_str() );
	}

	return true;
}


int HLSLEffectDemo::Init()
{
	m_Meshes.resize( 6 );
	m_MeshScales.resize( m_Meshes.size() );

	m_Meshes[0].Load( "./models/fw43.msh" );             m_MeshScales[0] =  0.0f;
	m_Meshes[1].Load( "./models/primitive_meshes.msh" ); m_MeshScales[1] =  0.0f;
	m_Meshes[2].Load( "./models/9x19mm.msh" );           m_MeshScales[2] = 20.0f;
	m_Meshes[3].Load( "./models/5.56x45mm.msh" );        m_MeshScales[3] = 20.0f;
	m_Meshes[4].Load( "./models/cz75-1st.msh" );         m_MeshScales[4] = 20.0f;
	m_Meshes[5].Load( "./models/male.msh" );             m_MeshScales[5] =  0.0f;

	InitShaders();

//	string tex_file( "textures/normal_hs_light.jpg" );
//	string tex_file( "textures/spectrum-vertical-s256x256.jpg" );
	string tex_file( "textures/textured_spectrum.jpg" );
	bool tex_loaded = m_LookupTextureForLighting.Load( tex_file );

	return 0;
}


void HLSLEffectDemo::Update( float dt )
{
}


void HLSLEffectDemo::RenderMesh()
{
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	if( m_Shaders.empty()
	 || m_CurrentShaderIndex < 0
	 || (int)m_Shaders.size() <= m_CurrentShaderIndex )
	{
		return;
	}

	SetShader( m_CurrentShaderIndex );

	ShaderManager *pShaderManager = m_Shaders[m_CurrentShaderIndex].GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	pShaderManager->SetViewerPosition( GetCurrentCamera().GetPosition() );

	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	if( m_CurrentMeshIndex < 0 || (int)m_Meshes.size() <= m_CurrentMeshIndex )
		return;

	Result::Name res = pShaderManager->SetTechnique( m_Techniques[m_CurrentShaderIndex] );

	MeshHandle& holder = m_Meshes[m_CurrentMeshIndex];

	if( holder.GetEntryState() == GraphicsResourceState::LOADED )
	{
		float s = m_MeshScales[m_CurrentMeshIndex];
//		Matrix44 world = ToMatrix44(holder.m_Pose) * Matrix44Scaling(s,s,s);
		Matrix44 world = Matrix44Identity() * Matrix44Scaling(s,s,s);
		// set world transform
		FixedFunctionPipelineManager().SetWorldTransform( world );
		pShaderManager->SetWorldTransform( world );

		BasicMesh *pMesh = holder.GetMesh().get();

		if( pMesh )
			pMesh->Render( *pShaderManager );
	}

	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void HLSLEffectDemo::RenderDebugInfo()
{
	if( 0 <= m_CurrentShaderIndex && m_CurrentShaderIndex < (int)m_Shaders.size() )
	{
		ShaderHandle& shader = m_Shaders[m_CurrentShaderIndex];
		if( shader.GetEntry()
		 && shader.GetEntry()->GetResource() )
		{
			string shader_path = 
				shader.GetEntry()->GetResource()->GetDesc().ResourcePath;

			m_pFont->DrawText( shader_path.c_str(), Vector2( 20, 50 ) );
		}
	}
}


void HLSLEffectDemo::Render()
{
	PROFILE_FUNCTION();

	RenderMesh();

	if( m_DisplayDebugInfo )
		RenderDebugInfo();
}


void HLSLEffectDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_Shaders.empty() )
				return;

			m_CurrentShaderIndex = (m_CurrentShaderIndex+1) % (int)m_Shaders.size();
		}
		break;
	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentShaderIndex = (m_CurrentShaderIndex + (int)m_Shaders.size() - 1) % (int)m_Shaders.size(); 
		}
		break;

	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_Meshes.empty() )
				return;

			m_CurrentMeshIndex = (m_CurrentMeshIndex+1) % (int)m_Meshes.size();
		}
		break;

	case GIC_PAGE_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentMeshIndex = (m_CurrentMeshIndex + (int)m_Meshes.size() - 1) % (int)m_Meshes.size(); 
		}
		break;

	case '1':
	case '2':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			bool& toggle = m_EnableLight[ input.iGICode - '1' ];
			toggle = !toggle;
		}
		break;

	case GIC_F1:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_DisplayDebugInfo = !m_DisplayDebugInfo;
		}
		break;
	case GIC_F5:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			GraphicsResourceManager().Refresh();
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
