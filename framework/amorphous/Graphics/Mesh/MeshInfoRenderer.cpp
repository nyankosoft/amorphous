#include "MeshInfoRenderer.hpp"
#include "BasicMesh.hpp"
#include "../Shader/ShaderManager.hpp"
#include "../Shader/CommonShaders.hpp"
#include "../Shader/FixedFunctionPipelineManager.hpp"
#include "../PrimitiveRenderer.hpp"


namespace amorphous
{

using namespace std;


Result::Name MeshInfoRenderer::RenderNormals( const BasicMesh& mesh, const Matrix44& world_transform, float normal_length, const SFloatRGBAColor& color )
{
	vector<Vector3> positions, normals;

	mesh.GetVertexPositions( positions );
	mesh.GetVertexNormals( normals );

	if( positions.size() != normals.size() )
		return Result::UNKNOWN_ERROR;

	ShaderManager *pShaderMgr = GetNoLightingShader().GetShaderManager();
	if( !pShaderMgr )
		return Result::UNKNOWN_ERROR;

	ShaderManager& shader_mgr = *pShaderMgr;

	shader_mgr.SetWorldTransform( world_transform );

	ShaderTechniqueHandle technique;
	technique.SetTechniqueName( "Default" );
	shader_mgr.SetTechnique( technique );

//	shader_mgr

	GraphicsDevice().Disable( RenderStateType::LIGHTING );
	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );

	FixedFunctionPipelineManager().SetWorldTransform( world_transform );

	const size_t num_vertices = positions.size();
	for( size_t i=0; i<num_vertices; i++ )
	{
		const Vector3 start = positions[i];
		const Vector3 end   = (positions[i] + normals[i] * normal_length);
		GetPrimitiveRenderer().DrawLine( start, end, color );
	}

	return Result::SUCCESS;
}


Result::Name MeshInfoRenderer::RenderNormals( const BasicMesh& mesh, const Matrix34& mesh_pose, float normal_length, const SFloatRGBAColor& color )
{
	return RenderNormals( mesh, ToMatrix44(mesh_pose), normal_length, color );
}


} // namespace amorphous
