#ifndef  __EmbeddedGenericGLSLShader_HPP__
#define  __EmbeddedGenericGLSLShader_HPP__


#include "amorphous/Graphics/Shader/Embedded/EmbeddedGenericShader.hpp"


namespace amorphous
{


class EmbeddedGenericGLSLShader : public EmbeddedGenericShader
{
public:

	EmbeddedGenericGLSLShader() {}

	~EmbeddedGenericGLSLShader() {}

	Result::Name GenerateVertexShader( const GenericShaderDesc& desc, std::string& vertex_shader );

	Result::Name GenerateFragmentShader( const GenericShaderDesc& desc, std::string& fragment_shader );
};


} // namespace amorphous



#endif		/*  __EmbeddedGenericGLSLShader_HPP__  */
