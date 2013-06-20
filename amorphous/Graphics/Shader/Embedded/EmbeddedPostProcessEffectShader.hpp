#ifndef __EmbeddedPostProcessEffectShader_HPP__
#define __EmbeddedPostProcessEffectShader_HPP__


#include <string>


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

	virtual void GetShader( const char *effect_name, std::string& shader ) {}

	virtual void GetVertexShader( const char *effect_name, std::string& vertex_shader ) {}

	virtual void GetFragmentShader( const char *effect_name, std::string& fragment_shader ) {}
};


} // namespace amorphous



#endif /* __EmbeddedPostProcessEffectShader_HPP__ */
