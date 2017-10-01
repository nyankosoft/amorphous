#ifndef  __PostProcessEffectDemo_HPP__
#define  __PostProcessEffectDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/PostProcessEffectManager.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class PostProcessEffectDemo : public CGraphicsTestBase
{
	std::shared_ptr<PostProcessEffectManager> m_pPostProcessEffectManager;

	unsigned int m_BlurStrength; // blur strength x 10

	static const float m_fBlurFactor;

	float m_fLuminanceAdaptationRate;

	U32 m_PPEffectFlags;

	std::vector<MeshHandle> m_vecMesh;

	std::vector<std::string> m_vecMeshFilepath;

	ShaderTechniqueHandle m_MeshTechnique;
	ShaderTechniqueHandle m_SkyboxTechnique;
	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

//	float m_fKeyValue;

	bool m_EnablePostProcessEffects;

	HDRLightingParams m_HDRLightingParams;

//	float m_fBlurStrength;

	bool m_RenderSky;

	bool m_DisplayDebugInfo;

protected:

	void RenderMeshes();

	void UpdateShaderParams();

	bool LoadModels();

public:

	PostProcessEffectDemo();

	~PostProcessEffectDemo();

	const char *GetAppTitle() const { return "PostProcessEffectDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void RenderScene();

	void HandleInput( const InputData& input );

	void LoadHDRParamValues();
};


#endif /* __PostProcessEffectDemo_HPP__ */
