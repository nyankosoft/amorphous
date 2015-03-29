#ifndef __amorphous_EmbeddedPostProcessEffectShader_HPP__
#define __amorphous_EmbeddedPostProcessEffectShader_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
#include "amorphous/base.hpp"
#include "amorphous/Graphics/Shader/ShaderGenerator.hpp"


namespace amorphous
{


class EmbeddedPostProcessEffectShader
{
public:
	EmbeddedPostProcessEffectShader() {}
	virtual ~EmbeddedPostProcessEffectShader() {}

	/// Returns true if the class supports a HLSL/Cg-style shader, a shader compiled
	/// from a single file which contains multiple vertex & pixel shaders.
	virtual bool SupportsCombinedShader() const = 0;

	virtual Result::Name GenerateShader( const std::string& effect_name, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name GenerateVertexShader( const std::string& effect_name, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name GenerateFragmentShader( const std::string& effect_name, std::string& shader ) { return Result::UNKNOWN_ERROR; }
};


inline boost::shared_ptr<EmbeddedPostProcessEffectShader>& GetEmbeddedPostProcessEffectShader()
{
	static boost::shared_ptr<EmbeddedPostProcessEffectShader> s_pPPEffectShader;
	return s_pPPEffectShader;
}


class PostProcessEffectFilterShaderGenerator : public ShaderGenerator
{

public:

	std::string m_EffectName;

	/// Multiple flags are not supported, i.e. specify only one flag.
//	CPostProcessEffect::TypeFlag m_Type;

//	PostProcessEffectFilterShaderGenerator() : m_Type(0) {}

	PostProcessEffectFilterShaderGenerator( const char *effect_name = "" )
		:
	m_EffectName(effect_name)
	{}

	void GetShader( std::string& shader )
	{
		GetEmbeddedPostProcessEffectShader()->GenerateShader( m_EffectName.c_str(), shader );
	}

	void GetVertexShader( std::string& shader )
	{
		GetEmbeddedPostProcessEffectShader()->GenerateVertexShader( m_EffectName.c_str(), shader );
	}

	void GetPixelShader( std::string& shader )
	{
		GetEmbeddedPostProcessEffectShader()->GenerateFragmentShader( m_EffectName.c_str(), shader );
	}
};


} // namespace amorphous



#endif /* __amorphous_EmbeddedPostProcessEffectShader_HPP__ */
