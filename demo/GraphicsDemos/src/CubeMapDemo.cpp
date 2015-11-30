#include "CubeMapDemo.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/MeshUtilities.hpp"
#include "amorphous/Support/Profile.hpp"

using std::string;
using std::vector;
using namespace boost;


CubeMapDemo::CubeMapDemo()
:
m_UseEmbeddedShader(true),
m_MeshIndex(1)
{
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
}


CubeMapDemo::~CubeMapDemo()
{
}


int CubeMapDemo::Init()
{
	// Load 6 image files as a cubemap texture
	TextureResourceDesc tex_desc;
	tex_desc.TypeFlags += TextureTypeFlag::CUBE_MAP;
	tex_desc.ResourcePath = "CubeMapDemo/textures/Teide_%u.jpg";

	m_CubeMapTextures.resize( 1 );
	bool texture_loaded = m_CubeMapTextures.back().Load( tex_desc );

	if( !texture_loaded )
		LOG_PRINT_ERROR( "Failed to load cube map textures: " + tex_desc.ResourcePath );

	m_Mesh = CreateSphereMesh( 0.25f );

	// initialize shader
//	bool loaded = false;
//	if( m_UseEmbeddedShader )
//	{
//		GenericShaderDesc gs_desc;
//		gs_desc.EnvMap = EnvMapOption::ENABLED;
//		gs_desc.Specular = SpecularSource::NONE;
//		ShaderResourceDesc shader_desc;
//		shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gs_desc) );
//
//		loaded = m_EnvMappedMeshShader.Load( shader_desc );
//
//		gs_desc.EnvMap = EnvMapOption::NONE;
//		gs_desc.Specular = SpecularSource::NONE;
//		shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gs_desc) );
//
//		loaded = m_NonEnvMappedMeshShader.Load( shader_desc );
//	}
//	else
//	{
//		loaded = m_EnvMappedMeshShader.Load( "./shaders/CubeMapDemo.fx" );
//	}

	ShaderManager *pShaderManager = m_EnvMappedMeshShader.GetShaderManager();
//	if( !pShaderManager )
//		return -1;

	ShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	// load models

	m_vecEnvMappedMesh.resize( 2 );
	m_vecEnvMappedMesh[0] = CreateBoxMesh( Vector3(1,1,1), SFloatRGBAColor::White() );
	m_vecEnvMappedMesh[1] = CreateSphereMesh( 0.5f );

//	shared_ptr<ShaderLightManager> pShaderLightMgr = shader_mgr.GetShaderLightManager();
	shared_ptr<ShaderLightManager> pShaderLightMgrs[] =
	{
		FixedFunctionPipelineManager().GetShaderLightManager(),
		shader_mgr.GetShaderLightManager()
	};

	return 0;
}


void CubeMapDemo::Update( float dt )
{
}


void CubeMapDemo::Render()
{
	if( m_CubeMapTextures.empty() )
		return;

	ShaderManager *pShaderManager = m_EnvMappedMeshShader.GetShaderManager();
//	if( !pShaderManager )
//		return;

//	ShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	bool use_ffp = false;
	if( use_ffp )
	{
		ShaderManager& shader_mgr = FixedFunctionPipelineManager();

		shader_mgr.SetTexture( 0, m_CubeMapTextures[0] );

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable(GL_NORMALIZE);
	}
	else
	{
	}

//	for( size_t i=0; i<m_vecEnvMappedMesh.size(); i++ )
	if( m_MeshIndex < (uint)m_vecEnvMappedMesh.size() )
	{
//		shared_ptr<BasicMesh> pMesh = m_vecEnvMappedMesh[i].GetMesh();
		shared_ptr<BasicMesh> pMesh = m_vecEnvMappedMesh[m_MeshIndex].GetMesh();

		if( pMesh )
		{
			if( 0 < pMesh->GetNumMaterials() )
			{
				if( 0 < pMesh->Material(0).Texture.size() )
				{
					pMesh->Material(0).Texture[0] = m_CubeMapTextures[0];
				}
			}

			pMesh->Render();
		}
	}
}

//void CubeMapDemo::HandleInput( const InputData& input )
//{
//	switch( input.iGICode )
//	{
//	case '?':
//		if( input.iType == ITYPE_KEY_PRESSED )
//		{
//		}
//		break;
//	default:
//		break;
//	}
//}

void CubeMapDemo::UpdateViewTransform( const Matrix44& matView )
{
//	m_pCubeMapManager->UpdateViewTransform( matView );
}


void CubeMapDemo::UpdateProjectionTransform( const Matrix44& matProj )
{
//	m_pCubeMapManager->UpdateProjectionTransform( matProj );
}
