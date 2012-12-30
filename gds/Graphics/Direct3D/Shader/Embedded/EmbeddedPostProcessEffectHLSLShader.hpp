#ifndef __EmbeddedPostProcessEffectHLSLShader_HPP__
#define __EmbeddedPostProcessEffectHLSLShader_HPP__


#include "../../../Shader/ShaderGenerator.hpp"
#include "../../../PostProcessEffectFilter.hpp"


namespace amorphous
{


class CEmbeddedPostProcessEffectHLSLShader
{
public:

	CEmbeddedPostProcessEffectHLSLShader(){}
	~CEmbeddedPostProcessEffectHLSLShader(){}

	static const char *m_pTextureSamplers;

	static const char *m_pSampleOffsetsAndWeights;

	static const char *m_pBloom;

	static const char *m_pDownScale2x2;

	static const char *m_pDownScale4x4;

	static const char *m_pGaussBlur5x5;

	static const char *m_pMonochrome;

	static const char *m_pHDR;
};


class CPostProcessEffectFilterShaderGenerator : public CShaderGenerator
{
	void GetCombinedShader( std::string& shader );

public:

	std::string m_EffectName;

	/// Multiple flags are not supported, i.e. specify only one flag.
//	CPostProcessEffect::TypeFlag m_Type;

//	CPostProcessEffectFilterShaderGenerator() : m_Type(0) {}

	CPostProcessEffectFilterShaderGenerator( const char *effect_name = "" )
		:
	m_EffectName(effect_name)
	{}

	void GetShader( std::string& shader );
};


} // namespace amorphous



#endif /* __EmbeddedPostProcessEffectHLSLShader_HPP__ */
