#ifndef  __CustomMeshRenderer_HPP__
#define  __CustomMeshRenderer_HPP__


namespace amorphous
{


class CustomMesh;
class ShaderManager;


class CustomMeshRenderer
{
public:

	virtual ~CustomMeshRenderer() {}

	virtual void RenderMesh( CustomMesh& mesh ) = 0;

	virtual void RenderSubset( CustomMesh& mesh, int subset_index ) = 0;

	virtual void RenderZSortedMesh( CustomMesh& mesh ) = 0;

	virtual void RenderMesh( CustomMesh& mesh, ShaderManager& shader_mgr ) = 0;

	virtual void RenderSubset( CustomMesh& mesh, ShaderManager& shader_mgr, int subset_index ) = 0;

	/// NOTE: only works with the mesh that has only one subset.
	virtual void RenderZSortedMesh( CustomMesh& mesh, ShaderManager& shader_mgr ) = 0;

	static CustomMeshRenderer *ms_pInstance;

	static CustomMeshRenderer *GetInstance() { return ms_pInstance; }
};


inline CustomMeshRenderer& GetCustomMeshRenderer()
{
	return *(CustomMeshRenderer::GetInstance());
}


} // namespace amorphous


#endif /* __CustomMeshRenderer_HPP__ */
