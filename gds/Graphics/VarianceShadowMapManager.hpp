#ifndef  __VarianceShadowMapManager_H__
#define  __VarianceShadowMapManager_H__


#include "ShadowMapManager.hpp"
#include "Shader/ShaderParameter.hpp"


typedef CShaderParameter< std::vector<float> > CShaderParamFloatArray;


/**
 */
class CVarianceShadowMapManager : public CShadowMapManager
{
//	LPDIRECT3DTEXTURE9 m_pHBlurredShadowMap;

//	LPDIRECT3DTEXTURE9 m_pBlurredShadowMap;

	boost::shared_ptr<CTextureRenderTarget> m_pHBlurredShadowMap;

	boost::shared_ptr<CTextureRenderTarget> m_pBlurredShadowMap;

	CShaderHandle m_BlurShader;

	CShaderParamFloatArray m_SampleOffsetsH;
	CShaderParamFloatArray m_SampleOffsetsV;
	CShaderParamFloatArray m_SampleWeights;

	enum Params
	{
		NUM_GAUSSIAN_SAMPLES = 15
	};

	Vector2 m_avSampleOffsetH[NUM_GAUSSIAN_SAMPLES];
	Vector2 m_avSampleOffsetV[NUM_GAUSSIAN_SAMPLES];

	float m_afSampleWeights[NUM_GAUSSIAN_SAMPLES];

private:

	TextureFormat::Format GetShadowMapTextureFormat() { return TextureFormat::G16R16F; }

public:

	CVarianceShadowMapManager();

	~CVarianceShadowMapManager();

	/// returns true on success
	bool Init();

//	void BeginSceneShadowMap();

	/// Creates the blurred shadowmap from the shadowmap
	void EndSceneShadowMap();


	void BeginSceneDepthMap();

	void PostProcessShadowMap( CShadowMap& shadow_map );

//	void EndSceneDepthMap();

	void UpdateLightPositionAndDirection();

//	void PostProcessDirectionalLightShadowMap( CDirectionalLightShadowMap& shadow_map );
	void PostProcessDirectionalLightShadowMap( COrthoShadowMap& shadow_map );

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif		/*  __VarianceShadowMapManager_H__  */
