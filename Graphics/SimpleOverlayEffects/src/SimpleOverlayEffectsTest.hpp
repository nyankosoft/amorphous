#ifndef  __SimpleOverlayEffectsTest_H__
#define  __SimpleOverlayEffectsTest_H__


#include "3DMath/Matrix34.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Graphics/PseudoNoiseEffect.hpp"
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


class CSimpleOverlayEffectsTest : public CGraphicsTestBase, public CGraphicsComponent
{
	enum UIID
	{
		UIID_DLG_ROOT = 1000,
		UIID_OTHER
	};

	enum Params
	{
		TEXT_BUFFER_SIZE = 4096
	};

	CPseudoNoiseEffect m_PseudoNoiseEffect;

	bool m_EnableNoiseEffect;

	CTextureHandle m_StripeTexture;

	bool m_EnableStripeEffect;

//	std::vector<CMeshObjectHandle> m_vecMesh;
	std::vector<CTestMeshHolder> m_vecMesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	char m_TextBuffer[TEXT_BUFFER_SIZE];

	bool m_DisplayResourceInfo;

private:

	void CreateSampleUI();

	bool InitShader();

	void RenderMeshes();

	void DisplayResourceInfo();

public:

	CSimpleOverlayEffectsTest();

	~CSimpleOverlayEffectsTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __SimpleOverlayEffectsTest_H__ */
