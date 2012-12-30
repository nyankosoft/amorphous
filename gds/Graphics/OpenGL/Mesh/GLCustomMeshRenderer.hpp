#ifndef  __GLCustomMeshRenderer_HPP__
#define  __GLCustomMeshRenderer_HPP__


#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


namespace amorphous
{


class CGLCustomMeshRenderer : public CCustomMeshRenderer
{
public:

	void RenderMesh( CCustomMesh& mesh );

	void RenderSubset( CCustomMesh& mesh, int subset_index );

	void RenderZSortedMesh( CCustomMesh& mesh ) { RenderMesh( mesh ); }

	void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr );

	void RenderSubset( CCustomMesh& mesh, CShaderManager& shader_mgr, int subset_index );

	void RenderZSortedMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr ); }

	static CGLCustomMeshRenderer ms_Instance;
};

} // namespace amorphous



#endif /* __GLCustomMeshRenderer_HPP__ */
