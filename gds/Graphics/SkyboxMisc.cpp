#include "SkyboxMisc.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
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


void RenderAsSkybox( CMeshObjectHandle& mesh, const Vector3& vCamPos )
{
	Result::Name res;
//	HRESULT hr;

	res = GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );
	res = GraphicsDevice().Disable( RenderStateType::ALPHA_TEST );
	res = GraphicsDevice().Disable( RenderStateType::LIGHTING );
	res = GraphicsDevice().Disable( RenderStateType::DEPTH_TEST );
	res = GraphicsDevice().Disable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
	res = GraphicsDevice().Enable(  RenderStateType::FACE_CULLING );

	// Commented out: don't change the "CullingMode::CLOCKWISE" mode when rendering the scene for a planar reflection texture.
//	res = GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	boost::shared_ptr<CBasicMesh> pMesh = mesh.GetMesh();

	CShaderManager& ffp_mgr = FixedFunctionPipelineManager();

	Matrix44 matWorld = Matrix44Identity();
//	const Vector3 vCamPos = g_CameraController.GetPosition();
	matWorld(0,3) = vCamPos.x;
	matWorld(1,3) = vCamPos.y;
	matWorld(2,3) = vCamPos.z;
	ffp_mgr.SetWorldTransform( matWorld );

	if( pMesh )
		pMesh->Render();

	res = GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	res = GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
}
