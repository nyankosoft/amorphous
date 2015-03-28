#ifndef __EmbeddedPostProcessEffectShader_HPP__
#define __EmbeddedPostProcessEffectShader_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
#include "amorphous/base.hpp"


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


} // namespace amorphous



#endif /* __EmbeddedPostProcessEffectShader_HPP__ */
