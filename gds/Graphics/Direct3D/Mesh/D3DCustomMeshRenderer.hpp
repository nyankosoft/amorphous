#ifndef __D3DCustomMeshRenderer_HPP__
#define __D3DCustomMeshRenderer_HPP__


#include <vector>
#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


namespace amorphous
{


class CD3DCustomMeshRenderer : public CustomMeshRenderer
{
	std::vector<int> m_SubsetIndices;

public:

	const std::vector<int>& GetAllSubsetIndices( const CustomMesh& mesh );

	void DrawPrimitives( const CustomMesh& mesh, int subset_index, bool use_zsorted_indices );

	void RenderMesh(
		CustomMesh& mesh,
		ShaderManager& shader_mgr,
		const std::vector<int> subsets_to_render,
		bool use_zsorted_indices
		);

	void RenderMesh(
		CustomMesh& mesh,
		const std::vector<int> subsets_to_render,
		bool use_zsorted_indices
		);

public:

	void RenderMesh( CustomMesh& mesh ) { RenderMesh( mesh, GetAllSubsetIndices(mesh), false ); }

	void RenderSubset( CustomMesh& mesh, int subset_index );

	void RenderZSortedMesh( CustomMesh& mesh ) { RenderMesh( mesh, GetAllSubsetIndices(mesh), true ); }

	void RenderMesh( CustomMesh& mesh, ShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr, GetAllSubsetIndices(mesh), false ); }

	void RenderSubset( CustomMesh& mesh, ShaderManager& shader_mgr, int subset_index );

	void RenderZSortedMesh( CustomMesh& mesh, ShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr, GetAllSubsetIndices(mesh), true ); }

	static CD3DCustomMeshRenderer ms_Instance;
};

} // namespace amorphous



#endif /* __D3DCustomMeshRenderer_HPP__ */
