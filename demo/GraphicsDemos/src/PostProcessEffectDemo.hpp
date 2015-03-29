#ifndef  __PostProcessEffectDemo_HPP__
#define  __PostProcessEffectDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/PostProcessEffectManager.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"

/*
enum ePPEffectFile
{
	PP_COLOR_GBLUR_H,
	PP_COLOR_GBLUR_V,
    PP_COLOR_INVERSE,
	PP_COLOR_MONOCHROME,
	PP_COLOR_DOWNFILTER4,
	PP_COLOR_UPFILTER4,
	PP_COLOR_BLOOM_H,
	PP_COLOR_BLOOM_V,
	PP_COLOR_BRIGHTPASS,
	PP_COLOR_COMBINE4,
	NUM_PP_EFFECT_FILES
};

enum ePPEffect
{
	PP_GBLUR,
	PP_GBLUR2,
	PP_COARSE,
	PP_NEGATIVE,
	PP_BLOOM,
	NUM_PP_EFFECTS
};

enum eSizeFilterType
{
	SF_DOWNFILTER_FOR_BLUR,
	SF_UPFILTER_FOR_BLUR,
	SF_DOWNFILTER_FOR_BLOOM,
	SF_UPFILTER_FOR_BLOOM,
	SF_BRIGHTPASS,
	NUM_SIZE_FILTERS,
};
*/


class PostProcessEffectDemo : public CGraphicsTestBase
{
	boost::shared_ptr<PostProcessEffectManager> m_pPostProcessEffectManager;

//	int m_aPPEffectIndex[NUM_PP_EFFECT_FILES];

//	int m_aPostProcessEffect[NUM_PP_EFFECTS];

	unsigned int m_BlurStrength; // blur strength x 10

	static const float m_fBlurFactor;

//	int m_aFilterIndex[NUM_SIZE_FILTERS];

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
