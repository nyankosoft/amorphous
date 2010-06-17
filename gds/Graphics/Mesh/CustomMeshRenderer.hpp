#ifndef  __CustomMeshRenderer_HPP__
#define  __CustomMeshRenderer_HPP__


class CCustomMesh;
class CShaderManager;


class CCustomMeshRenderer
{
public:

	virtual ~CCustomMeshRenderer() {}

	virtual void RenderMesh( CCustomMesh& mesh ) = 0;

	virtual void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) = 0;

	static CCustomMeshRenderer *ms_pInstance;

	static CCustomMeshRenderer *GetInstance() { return ms_pInstance; }
};


inline CCustomMeshRenderer& GetCustomMeshRenderer()
{
	return *(CCustomMeshRenderer::GetInstance());
}



#endif /* __CustomMeshRenderer_HPP__ */
