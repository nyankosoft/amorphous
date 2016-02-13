#ifndef  __GLCustomMeshRenderer_HPP__
#define  __GLCustomMeshRenderer_HPP__


#include "amorphous/Graphics/Mesh/CustomMeshRenderer.hpp"


namespace amorphous
{


class GLCustomMeshRenderer : public CustomMeshRenderer
{
	void RenderMeshWithSpecifiedProgram( CustomMesh& mesh, ShaderManager& shader_mgr );

public:

	void RenderMesh( CustomMesh& mesh );

	void RenderSubset( CustomMesh& mesh, int subset_index );

	void RenderZSortedMesh( CustomMesh& mesh ) { RenderMesh( mesh ); }

	void RenderMesh( CustomMesh& mesh, ShaderManager& shader_mgr );

	void RenderSubset( CustomMesh& mesh, ShaderManager& shader_mgr, int subset_index );

	void RenderZSortedMesh( CustomMesh& mesh, ShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr ); }

	static GLCustomMeshRenderer ms_Instance;
};

} // namespace amorphous



#endif /* __GLCustomMeshRenderer_HPP__ */
