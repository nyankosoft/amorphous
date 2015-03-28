#ifndef __EmbeddedPostProcessEffectHLSLShader_HPP__
#define __EmbeddedPostProcessEffectHLSLShader_HPP__


#include "amorphous/Graphics/PostProcessEffectFilter.hpp"
#include "amorphous/Graphics/Shader/ShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/Embedded/EmbeddedPostProcessEffectShader.hpp"


namespace amorphous
{


class CEmbeddedPostProcessEffectHLSLShader : public EmbeddedPostProcessEffectShader
{
	static const char *m_pTextureSamplers;

	static const char *m_pSampleOffsetsAndWeights;

	static const char *m_pBloom;

	static const char *m_pDownScale2x2;

	static const char *m_pDownScale4x4;

	static const char *m_pGaussBlur5x5;

	static const char *m_pMonochrome;

	static const char *m_pHDR;

	void GetCombinedShader( std::string& shader );

public:

	CEmbeddedPostProcessEffectHLSLShader(){}
	~CEmbeddedPostProcessEffectHLSLShader(){}

	bool SupportsCombinedShader() const { return true; }

	Result::Name GenerateShader( const std::string& effect_name, std::string& shader );
};


class CPostProcessEffectFilterShaderGenerator : public ShaderGenerator
{

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

	void GetVertexShader( std::string& shader );

	void GetPixelShader( std::string& shader );
};


} // namespace amorphous



#endif /* __EmbeddedPostProcessEffectHLSLShader_HPP__ */
