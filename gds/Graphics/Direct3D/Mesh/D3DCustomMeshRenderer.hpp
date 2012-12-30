#ifndef __D3DCustomMeshRenderer_HPP__
#define __D3DCustomMeshRenderer_HPP__


#include <vector>
#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


namespace amorphous
{


class CD3DCustomMeshRenderer : public CCustomMeshRenderer
{
	std::vector<int> m_SubsetIndices;

public:

	const std::vector<int>& GetAllSubsetIndices( const CCustomMesh& mesh );

	void DrawPrimitives( const CCustomMesh& mesh, int subset_index, bool use_zsorted_indices );

	void RenderMesh(
		CCustomMesh& mesh,
		CShaderManager& shader_mgr,
		const std::vector<int> subsets_to_render,
		bool use_zsorted_indices
		);

	void RenderMesh(
		CCustomMesh& mesh,
		const std::vector<int> subsets_to_render,
		bool use_zsorted_indices
		);

public:

	void RenderMesh( CCustomMesh& mesh ) { RenderMesh( mesh, GetAllSubsetIndices(mesh), false ); }

	void RenderSubset( CCustomMesh& mesh, int subset_index );

	void RenderZSortedMesh( CCustomMesh& mesh ) { RenderMesh( mesh, GetAllSubsetIndices(mesh), true ); }

	void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr, GetAllSubsetIndices(mesh), false ); }

	void RenderSubset( CCustomMesh& mesh, CShaderManager& shader_mgr, int subset_index );

	void RenderZSortedMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr, GetAllSubsetIndices(mesh), true ); }

	static CD3DCustomMeshRenderer ms_Instance;
};

} // namespace amorphous



#endif /* __D3DCustomMeshRenderer_HPP__ */
