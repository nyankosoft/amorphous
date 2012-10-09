#include "SkyboxMisc.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"

using namespace std;
using namespace boost;


boost::shared_ptr<CBoxMeshGenerator> CreateSkyboxMeshGenerator( const std::string& texture_filepath )
{
	boost::shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator );

	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetTexCoordStyleFlags( TexCoordStyle::LINEAR_SHIFT_INV_Y );
	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pBoxMeshGenerator->SetTexturePath( texture_filepath );

	return pBoxMeshGenerator;
}


CMeshObjectHandle CreateSkyboxMesh( const std::string& texture_filepath )
{
	CMeshResourceDesc skybox_mesh_desc;
	skybox_mesh_desc.pMeshGenerator = CreateSkyboxMeshGenerator( texture_filepath );

	CMeshObjectHandle skybox_mesh;
	bool loaded = skybox_mesh.Load( skybox_mesh_desc );

	return skybox_mesh;
}


template<class MeshClass>
void RenderAsSkybox( MeshClass& mesh, const Vector3& vCamPos )
{
	Result::Name res;

	res = GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );
	res = GraphicsDevice().Disable( RenderStateType::ALPHA_TEST );
	res = GraphicsDevice().Disable( RenderStateType::LIGHTING );
	res = GraphicsDevice().Disable( RenderStateType::DEPTH_TEST );
	res = GraphicsDevice().Disable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
	res = GraphicsDevice().Enable(  RenderStateType::FACE_CULLING );
//	res = GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	// Commented out: don't change the "CullingMode::CLOCKWISE" mode when rendering the scene for a planar reflection texture.
//	res = GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	CShaderManager& ffp_mgr = FixedFunctionPipelineManager();

	Matrix44 matWorld = Matrix44Identity();
	matWorld(0,3) = vCamPos.x;
	matWorld(1,3) = vCamPos.y;
	matWorld(2,3) = vCamPos.z;
	ffp_mgr.SetWorldTransform( matWorld );

	mesh.Render();

	res = GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	res = GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
}


void RenderAsSkybox( CMeshObjectHandle& mesh, const Vector3& vCamPos )
{
	boost::shared_ptr<CBasicMesh> pMesh = mesh.GetMesh();

	if( pMesh )
		RenderAsSkybox( *pMesh, vCamPos );
}


void RenderSkybox( CTextureHandle& sky_texture, const Vector3& vCamPos )
{
	static CCustomMesh s_SkyboxMesh;

	if( !s_SkyboxMesh.IsValid() )
	{
		boost::shared_ptr<CBoxMeshGenerator> pSkyboxMeshGenerator
			= CreateSkyboxMeshGenerator( "" );
		if( pSkyboxMeshGenerator )
		{
			pSkyboxMeshGenerator->Generate();
			C3DMeshModelArchive mesh_archive = pSkyboxMeshGenerator->GetMeshArchive();
			s_SkyboxMesh.LoadFromArchive( mesh_archive, "static_skybox_mesh", 0 );
		}
	}

	if( 0 < s_SkyboxMesh.GetNumMaterials() )
	{
		if( s_SkyboxMesh.Material(0).Texture.empty() )
			s_SkyboxMesh.Material(0).Texture.resize(1);

		s_SkyboxMesh.Material(0).Texture[0] = sky_texture;
	}

	RenderAsSkybox( s_SkyboxMesh, vCamPos );
}
