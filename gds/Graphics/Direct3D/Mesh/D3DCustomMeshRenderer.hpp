#ifndef __D3DCustomMeshRenderer_HPP__
#define __D3DCustomMeshRenderer_HPP__


#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


class CD3DCustomMeshRenderer : public CCustomMeshRenderer
{
	void DrawPrimitives( const CCustomMesh& mesh, int subset_index, bool use_zsorted_indices );

	void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr, bool use_zsorted_indices );

	void RenderMesh( CCustomMesh& mesh, bool use_zsorted_indices );

public:

	void RenderMesh( CCustomMesh& mesh ) { RenderMesh( mesh, false ); }

	void RenderZSortedMesh( CCustomMesh& mesh ) { RenderMesh( mesh, true ); }

	void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr, false ); }

	void RenderZSortedMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr, true ); }

	static CD3DCustomMeshRenderer ms_Instance;
};


#endif /* __D3DCustomMeshRenderer_HPP__ */
