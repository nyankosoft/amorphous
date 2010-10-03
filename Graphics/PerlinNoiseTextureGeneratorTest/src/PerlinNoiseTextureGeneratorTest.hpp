#ifndef  __PerlinNoiseTextureGeneratorTest_H__
#define  __PerlinNoiseTextureGeneratorTest_H__


#include "3DMath/Matrix34.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input/fwd.hpp"
#include "Input.hpp"
#include "GUI/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CPerlinNoiseTextureGeneratorTest : public CGraphicsTestBase, public CGraphicsComponent
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

//	CShaderHandle m_Shader;

	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	boost::shared_ptr<CTimer> m_pTimer;

	CTextureHandle m_PerlinNoiseTexture;

	CTextureHandle m_PerlinNoiseNormalMap;

	int m_ImageWidth;

	float m_fTextureRepeats;

	char m_TextBuffer[TEXT_BUFFER_SIZE];

private:

	void CreateSampleUI();

	bool InitShader();

	void CreatePerlinNoiseTexture();

	void SaveTexturesAsImageFiles();

public:

	CPerlinNoiseTextureGeneratorTest();

	~CPerlinNoiseTextureGeneratorTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __PerlinNoiseTextureGeneratorTest_H__ */
