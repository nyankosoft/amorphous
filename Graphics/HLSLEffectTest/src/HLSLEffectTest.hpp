#ifndef  __HLSLEffectTest_H__
#define  __HLSLEffectTest_H__


#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include "3DMath/Matrix34.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input/fwd.hpp"
#include "Input.hpp"
#include "GUI/fwd.hpp"

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

	CTestMeshHolder( const std::string& filepath, LoadingStyleName loading_style, const Matrix34& pose );
};


class CHLSLEffectTest : public CGraphicsTestBase, public CGraphicsComponent
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

	std::vector<CShaderHandle> m_Shaders;

	std::vector<CShaderTechniqueHandle> m_Techniques;

	int m_CurrentShaderIndex;

	CShaderTechniqueHandle m_MeshTechnique;

	shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CInputHandlerSharedPtr m_pInputHandler;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	char m_TextBuffer[TEXT_BUFFER_SIZE];

	bool m_DisplayDebugInfo;

private:

	void CreateSampleUI();

	bool SetShader( int index );

	bool InitShaders();

	void CreateCachedResources();

	void RenderMeshes();

	void RenderDebugInfo();

public:

	CHLSLEffectTest();

	~CHLSLEffectTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __HLSLEffectTest_H__ */
