#include "BE_Skybox.hpp"
#include "BaseEntity_Draw.hpp"

//#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/MiscShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
//#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/BuiltinSkyTextures.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/TextureGenerators/BuiltinImageTextureLoader.hpp"
#include "amorphous/Graphics/TextureGenerators/GradationTextureGenerators.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Macro.h"


namespace amorphous
{

using namespace std;


CBE_Skybox::CBE_Skybox()
{
	/// must be linked to the root node of the entity tree
	/// TODO: guarantee this
	m_aabb = AABB3( Vector3( -1200000, -1200000, -1200000 ),
		            Vector3(  1200000,  1200000,  1200000 ) );

	m_fRadius = 1800000;

	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	m_BoundingVolumeType = BVTYPE_AABB;

//	m_MeshProperty.m_ShaderDesc.ResourcePath = "Shader/HighAltCamSkybox.fx";
//	m_MeshProperty.m_ShaderTechnique.resize(1,1);
//	m_MeshProperty.m_ShaderTechnique(0,0).SetTechniqueName( "SkySphereFG" );
}


CBE_Skybox::~CBE_Skybox()
{
}


void CBE_Skybox::Init()
{
	MeshResourceDesc& mesh_desc = m_MeshProperty.m_MeshDesc;
	if( !lfs::file_exists( mesh_desc.ResourcePath ) )
	{
		// The default mesh for skybox, which is currently "./Model/skybox.msh", was not found;
		// create a skysphere mesh with sphere mesh generator.
//		SphereDesc sphere_desc;
//		sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = 5.0f;
//		sphere_desc.axis = 1; // shift texture coords along y-axis
//		sphere_desc.poly_dir = MeshPolygonDirection::INWARD;
//		mesh_desc.pMeshGenerator.reset( new SphereMeshGenerator(sphere_desc) );
	}

	if( m_strName == "skybox" )
	{
		GenericShaderDesc desc;
		desc.Lighting = false;
		std::shared_ptr<GenericShaderGenerator> p;
		p.reset( new GenericShaderGenerator(desc) );
		m_MeshProperty.m_ShaderDesc.pShaderGenerator = p;

		m_MeshProperty.m_ShaderTechnique.resize(1,1);
	}
	else if( m_strName == "high_altitude_range_skybox" )
	{
		m_MeshProperty.m_ShaderDesc.ResourcePath = "Shader/HighAltCamSkybox.fx";
		m_MeshProperty.m_ShaderTechnique.resize(1,1);
		m_MeshProperty.m_ShaderTechnique(0,0).SetTechniqueName( "SkySphereFG" );
	}

	Init3DModel();
}


void CBE_Skybox::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	Vector3 vCenterPos = pCopyEnt->GetWorldPosition();
}


void CBE_Skybox::Act(CCopyEntity* pCopyEnt)
{
}


void CBE_Skybox::Draw(CCopyEntity* pCopyEnt)
{
	Camera* pCamera = m_pStage->GetCurrentCamera();
	BasicMesh* pMeshObject = m_MeshProperty.m_MeshObjectHandle.GetMesh().get();

	if( !pMeshObject )
	{
//		ONCE( LOG_PRINT_WARNING( " An invlid mesh object: base entity '%s'", m_strName.c_str() ) );
		RenderSkybox( m_SkyboxTexture, pCamera ? pCamera->GetPose() : Matrix34Identity() );
		return;
	}

	BasicMesh& mesh = *pMeshObject;

	// set the world transform
	Matrix44 world( Matrix44Scaling( 10.0f, 10.0f, 10.0f ) );

	Vector3 vPos;
	if( pCamera )
		vPos = pCamera->GetPosition();
	else
		vPos = Vector3(0,0,0);

	world(0,3) = vPos.x;
	world(1,3) = vPos.y;
	world(2,3) = vPos.z;
	world(3,3) = 1;

//	FixedFunctionPipelineManager().SetWorldTransform( world );


	pMeshObject->SetVertexDeclaration();

	int num_materials = pMeshObject->GetNumMaterials();

	bool shift_camera_height = true;

//	ShaderParameter<float> cam_height_param( "g_CameraHeight" );

	// Disable depth-test and writing to depth buffer.
	// These 2 settings and restored after rendering skybox because they are changed only when necessary.
	// This is not required if you are using HLSL effect of Direct3D, and the technique to render
	// the skybox defines the same render states.
	// If you are using OpenGL, you have to do this whether you are using GLSL or not?
	GraphicsDevice().Disable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Disable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );

	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	// save the original texture and temporarily overwrite it with the sky texture
	TextureHandle orig_tex;
	if( 0 < num_materials )
	{
		if( mesh.Material(0).Texture.empty() )
			mesh.Material(0).Texture.resize( 1 );

		orig_tex = mesh.Material(0).Texture[0];
		mesh.Material(0).Texture[0] = m_SkyboxTexture;
	}

	ShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();

	if( m_strName == "skybox" && pCamera )
	{
//		//RenderAsSkybox( m_MeshProperty.m_MeshObjectHandle, vPos );
		mesh.Render();

	}
	else if( m_strName == "high_altitude_range_skybox" && pShaderManager )
	{
		if( shift_camera_height )
		{
			float fCamHeight;
			if( pCamera )
				fCamHeight = pCamera->GetPosition().y;
			else
				fCamHeight = 5.0f;

//			pShaderManager->GetEffect()->SetFloat( "g_CameraHeight", fCamHeight );
			pShaderManager->SetParam( "g_CameraHeight", fCamHeight );

//			pEffect->SetFloat( "g_TexVShiftFactor", 0.000005f );
//			pShaderManager->SetParam( "g_TexVShiftFactor", 0.000005f );

//			cam_height_param.Parameter() = fCamHeight;
//			pShaderManager->SetParam( cam_height_param );
		}

		// render the skybox mesh with an HLSL shader

		pShaderManager->SetWorldTransform( world );

		Result::Name res = pShaderManager->SetTechnique( m_MeshProperty.m_ShaderTechnique(0,0) );

		// Meshes are divided into subsets by materials. Render each subset in a loop
		mesh.Render( *pShaderManager );
	}

	if( 0 < num_materials
	 && 0 < mesh.Material(0).Texture.size() )
	{
		// restore the original texture
		mesh.Material(0).Texture[0] = orig_tex;
	}

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
}

/*
void CBE_Skybox::MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
}
*/

bool CBE_Skybox::LoadSkyboxTexture( const std::string& texture_filename )
{
	TextureResourceDesc desc;
	if( texture_filename.length() == 0 )
	{
//		const CBuiltinImage& default_sky_image = GetDefaultBuiltinSkyTexture();
//		m_SkyboxTexture = CreateTextureFromBuiltinImage( default_sky_image );

		auto top_color    = SFloatRGBAColor(0.659f, 0.741f, 0.816f, 1.000f);
		auto mid_color    = SFloatRGBAColor(0.827f, 0.859f, 0.871f, 1.000f);
		auto bottom_color = SFloatRGBAColor(0.961f, 0.827f, 0.682f, 1.000f);
		m_SkyboxTexture = CreateHorizontalGradationTexture( 512, 512, TextureFormat::A8R8G8B8, top_color, mid_color, bottom_color );

		return m_SkyboxTexture.IsLoaded();
	}
//	else if( texture_filename.find( "Builtin::" ) == 0
//	 && strlen("Builtin::") < texture_filename.length() )
//	{
//		const CBuiltinImage& sky_image = GetBuiltinSkyTexture( texture_filename.substr(strlen("Builtin::")) );
//		m_SkyboxTexture = CreateTextureFromBuiltinImage( sky_image );
//		return m_SkyboxTexture.IsLoaded();
//	}
	else
		desc.ResourcePath = texture_filename;

	// Keep the pixel at the center of the image so that it can easily retrieved as the fog color
	BitmapImage img;
	bool res = img.LoadFromFile(texture_filename);
	if( res )
	{
		float r=0,g=0,b=0,a=0;
		img.GetPixel( img.GetWidth()/2, img.GetHeight()/2, r, g, b, a );
		m_SkyboxTextureCenterPixelColor.SetRGBA(r,g,b,a);
	}

	return m_SkyboxTexture.Load( desc );
}


bool CBE_Skybox::GetFogColor( SFloatRGBAColor& dest )
{
//	if( !GetSkyboxTexture().GetEntry() )
//		return false;
//
//	std::shared_ptr<TextureResource> pTexResource = GetSkyboxTexture().GetEntry()->GetTextureResource();
//	if( !pTexResource )
//		return false;
//
//	if( !pTexResource->Lock() )
//		return false;
//
//	std::shared_ptr<LockedTexture> pLockedTex;
//	pTexResource->GetLockedTexture( pLockedTex );
//	if( pLockedTex )
//	{
//		pLockedTex->GetPixel( pLockedTex->GetWidth() / 2, pLockedTex->GetHeight() / 2, dest );
//	}
//
//	pTexResource->Unlock();

	dest = m_SkyboxTextureCenterPixelColor;

	return true;
}


bool CBE_Skybox::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_Skybox::Serialize( IArchive& ar, const unsigned int version )
{
	BaseEntity::Serialize( ar, version );

	ar & m_SkyboxTextureFilepath;
}


} // namespace amorphous
