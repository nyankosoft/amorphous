#ifndef  __GLSLTest_H__
#define  __GLSLTest_H__


#include <vector>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include <gds/3DMath/Matrix34.hpp>
#include <gds/Graphics/fwd.hpp>
#include <gds/Graphics/GraphicsComponentCollector.hpp>
#include <gds/Graphics/MeshObjectHandle.hpp>
#include <gds/Graphics/ShaderHandle.hpp>
#include <gds/Graphics/Shader/ShaderTechniqueHandle.hpp>
#include <gds/Input/fwd.hpp>
#include <gds/Input.hpp>
#include <gds/GUI/fwd.hpp>
#include <gds/Graphics/OpenGL/Shader/GLShader.hpp>

#include "../../../_Common/GraphicsTestBase.hpp"


class CTestMeshHolder
{
public:
	CMeshObjectHandle m_Handle;
	Matrix34 m_Pose;

	CMeshResourceDesc m_MeshDesc;

	enum LoadingStyleName
	{
		LOAD_SYNCHRONOUSLY,
		LOAD_MESH_AND_TEX_TOGETHER,
		LOAD_MESH_AND_TEX_SEPARATELY,
		NUM_LOADING_STYLES
	};

	LoadingStyleName m_LoadingStyle;

	CTestMeshHolder();

	CTestMeshHolder( const std::string& filepath, LoadingStyleName loading_style, const Matrix34& pose );

	void Load();
};


class CGLSLTest : public CGraphicsTestBase, public CGraphicsComponent
{
	enum UIID
	{
		UIID_DLG_ROOT = 1000,
		UIID_DLG_SLIDERS,
		UIID_DLG_RESOLUTION,
		UIID_LBX_RESOLUTION,
		UIID_DLG_LISTBOXGROUP,
		UIID_OTHER
	};

	enum Params
	{
		TEXT_BUFFER_SIZE = 4096
	};

//	std::vector<CMeshObjectHandle> m_vecMesh;
	std::vector<CTestMeshHolder> m_vecMesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CInputHandlerSharedPtr m_pInputHandler;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	boost::shared_ptr<CGLProgram> m_pGLProgram;

//	bool m_TestAsyncLoading;

	char m_TextBuffer[TEXT_BUFFER_SIZE];

private:

	void CreateSampleUI();

	bool InitShader();

	void CreateCachedResources();

	void RenderMeshes();

	void SetLights();

public:

	CGLSLTest();

	~CGLSLTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __GLSLTest_H__ */
