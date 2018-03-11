#ifndef  __AsyncLoadingDemo_H__
#define  __AsyncLoadingDemo_H__


#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
//#include "amorphous/GUI/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CTestMeshHolder
{
public:
	MeshHandle m_Handle;
	Matrix34 m_Pose;

	float m_fScale;

	MeshResourceDesc m_MeshDesc;

	enum LoadingStyleName
	{
		LOAD_SYNCHRONOUSLY,
		LOAD_MESH_AND_TEX_TOGETHER,
		LOAD_MESH_AND_TEX_SEPARATELY,
		NUM_LOADING_STYLES
	};

	LoadingStyleName m_LoadingStyle;

	CTestMeshHolder( const std::string& filepath, LoadingStyleName loading_style, const Matrix34& pose );
};


class AsyncLoadingDemo : public CGraphicsTestBase, public GraphicsComponent
{
//	std::vector<MeshHandle> m_vecMesh;
	std::vector<CTestMeshHolder> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	//std::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	//CGM_DialogManagerSharedPtr m_pSampleUI;

	bool m_TestAsyncLoading;

	bool m_UseCachedResources;

	int m_NumTextureMipmaps;

//	std::string m_TextBuffer;

private:

	void CreateSampleUI();

	bool InitShader();

	void CreateCachedResources();

	void RenderMeshes();

//	void LoadTexturesAsync();
	void LoadResourcesAsync( CTestMeshHolder& holder );
	void SaveTexturesAsImageFiles();

public:

	AsyncLoadingDemo();

	~AsyncLoadingDemo();

	const char *GetAppTitle() const { return "AsyncLoadingDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );
};


#endif /* __AsyncLoadingDemo_H__ */
