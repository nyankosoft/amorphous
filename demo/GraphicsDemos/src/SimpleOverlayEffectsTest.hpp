#ifndef  __SimpleOverlayEffectsTest_H__
#define  __SimpleOverlayEffectsTest_H__


#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/PseudoNoiseEffect.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/GUI/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CSimpleOverlayEffectsTest : public CGraphicsTestBase, public GraphicsComponent
{
	PseudoNoiseEffect m_PseudoNoiseEffect;

	bool m_EnableNoiseEffect;

	TextureHandle m_StripeTexture;

	bool m_EnableStripeEffect;

	std::vector<MeshHandle> m_Meshes;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

private:

	void SetLights();

	bool InitShader();

	void RenderMeshes();

public:

	CSimpleOverlayEffectsTest();

	~CSimpleOverlayEffectsTest();

	const char *GetAppTitle() const { return "SimpleOverlayEffectsTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );
};


#endif /* __SimpleOverlayEffectsTest_H__ */
