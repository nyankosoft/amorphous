#ifndef  __PostProcessEffectTest_HPP__
#define  __PostProcessEffectTest_HPP__


#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/PostProcessEffectManager.hpp"
#include "gds/Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


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

/*
class CPostProcessEffectTest;


class CPostProcessEffectTestInputHandler : public CInputHandler
{
	CPostProcessEffectTest* m_pTest;

public:

	CPostProcessEffectTestInputHandler( CPostProcessEffectTest* pTest )
		:
	m_pTest(pTest) {}

	virtual void ProcessInput(SInputData& input);
};
*/

class CPostProcessEffectTest : public CGraphicsTestBase
{
//	shared_ptr<CPostProcessManager> m_pPPManager;
	boost::shared_ptr<CPostProcessEffectManager> m_pPostProcessEffectManager;

	int m_aPPEffectIndex[NUM_PP_EFFECT_FILES];

	int m_aPostProcessEffect[NUM_PP_EFFECTS];

	float m_fBlurFactor;

	int m_aFilterIndex[NUM_SIZE_FILTERS];

	float m_fLuminance;

	U32 m_PPEffectFlags;

	std::vector<CMeshObjectHandle> m_vecMesh;

	std::vector<std::string> m_vecMeshFilepath;

	CShaderTechniqueHandle m_MeshTechnique;
	CShaderTechniqueHandle m_SkyboxTechnique;
	CShaderTechniqueHandle m_DefaultTechnique;

	CShaderHandle m_Shader;

//	shared_ptr<CShaderManager> m_pShaderManager;

//	CShaderLightManager m_ShaderLightManager;

//	CUnitSphere m_LightPosIndicator;

//	CShaderLightManager m_ShaderLightManager;

//	Vector3 m_vLightPosition;

	CFontSharedPtr m_pFont;

//	CInputHandlerSharedPtr m_pInputHandler;

//	float m_fKeyValue;

	CHDRLightingParams m_HDRLightingParams;

	float m_fBlurStrength;

protected:

	void RenderMeshes();

	void UpdateShaderParams();

	bool LoadModels();

public:

	CPostProcessEffectTest();

	~CPostProcessEffectTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void RenderScene();

//	void UpdateViewTransform( const D3DXMATRIX& matView );

//	void UpdateProjectionTransform( const D3DXMATRIX& matProj );

	void HandleInput( const SInputData& input );
};


#endif /* __PostProcessEffectTest_HPP__ */
