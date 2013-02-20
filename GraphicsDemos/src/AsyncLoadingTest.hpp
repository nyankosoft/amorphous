#ifndef  __AsyncLoadingTest_H__
#define  __AsyncLoadingTest_H__


#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"

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


class CAsyncLoadingTest : public CGraphicsTestBase, public GraphicsComponent
{
//	std::vector<MeshHandle> m_vecMesh;
	std::vector<CTestMeshHolder> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<FontBase> m_pFont;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	bool m_TestAsyncLoading;

	bool m_UseCachedResources;

	int m_NumTextureMipmaps;

	std::string m_TextBuffer;

private:

	void CreateSampleUI();

	bool InitShader();

	void CreateCachedResources();

	void RenderMeshes();

//	void LoadTexturesAsync();
	void LoadResourcesAsync( CTestMeshHolder& holder );
	void SaveTexturesAsImageFiles();

public:

	CAsyncLoadingTest();

	~CAsyncLoadingTest();

	const char *GetAppTitle() const { return "AsyncLoadingTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );
};


#endif /* __AsyncLoadingTest_H__ */
