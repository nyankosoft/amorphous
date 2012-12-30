#ifndef  __CustomMeshRenderer_HPP__
#define  __CustomMeshRenderer_HPP__


namespace amorphous
{


class CCustomMesh;
class CShaderManager;


class CCustomMeshRenderer
{
public:

	virtual ~CCustomMeshRenderer() {}

	virtual void RenderMesh( CCustomMesh& mesh ) = 0;

	virtual void RenderSubset( CCustomMesh& mesh, int subset_index ) = 0;

	virtual void RenderZSortedMesh( CCustomMesh& mesh ) = 0;

	virtual void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) = 0;

	virtual void RenderSubset( CCustomMesh& mesh, CShaderManager& shader_mgr, int subset_index ) = 0;

	/// NOTE: only works with the mesh that has only one subset.
	virtual void RenderZSortedMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) = 0;

	static CCustomMeshRenderer *ms_pInstance;

	static CCustomMeshRenderer *GetInstance() { return ms_pInstance; }
};


inline CCustomMeshRenderer& GetCustomMeshRenderer()
{
	return *(CCustomMeshRenderer::GetInstance());
}


} // namespace amorphous


#endif /* __CustomMeshRenderer_HPP__ */
