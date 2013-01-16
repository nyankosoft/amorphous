#ifndef  __GLCustomMeshRenderer_HPP__
#define  __GLCustomMeshRenderer_HPP__


#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


namespace amorphous
{


class CGLCustomMeshRenderer : public CustomMeshRenderer
{
public:

	void RenderMesh( CustomMesh& mesh );

	void RenderSubset( CustomMesh& mesh, int subset_index );

	void RenderZSortedMesh( CustomMesh& mesh ) { RenderMesh( mesh ); }

	void RenderMesh( CustomMesh& mesh, CShaderManager& shader_mgr );

	void RenderSubset( CustomMesh& mesh, CShaderManager& shader_mgr, int subset_index );

	void RenderZSortedMesh( CustomMesh& mesh, CShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr ); }

	static CGLCustomMeshRenderer ms_Instance;
};

} // namespace amorphous



#endif /* __GLCustomMeshRenderer_HPP__ */
