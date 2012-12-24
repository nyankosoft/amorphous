#ifndef  __SimpleOverlayEffectsTest_H__
#define  __SimpleOverlayEffectsTest_H__


#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/PseudoNoiseEffect.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CSimpleOverlayEffectsTest : public CGraphicsTestBase, public CGraphicsComponent
{
	CPseudoNoiseEffect m_PseudoNoiseEffect;

	bool m_EnableNoiseEffect;

	CTextureHandle m_StripeTexture;

	bool m_EnableStripeEffect;

	std::vector<CMeshObjectHandle> m_Meshes;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

	std::string m_TextBuffer;

	bool m_DisplayResourceInfo;

private:

	void CreateSampleUI();

	bool InitShader();

	void RenderMeshes();

	void DisplayResourceInfo();

public:

	CSimpleOverlayEffectsTest();

	~CSimpleOverlayEffectsTest();

	const char *GetAppTitle() const { return "SimpleOverlayEffectsTest"; }

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
