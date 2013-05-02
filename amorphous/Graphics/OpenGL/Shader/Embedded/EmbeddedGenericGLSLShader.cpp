#include "EmbeddedGenericGLSLShader.hpp"
#include "Graphics/Shader/GenericShaderDesc.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{


Result::Name EmbeddedGenericGLSLShader::GenerateVertexShader( const GenericShaderDesc& desc, std::string& vertex_shader )
{
	static const char *vs =
	"#version 330\n"\
	"layout(location = 0) in vec4 position;\n"\
	"uniform mat4 ProjViewWorld;\n"\
	"void main(){gl_Position = ProjViewWorld * position;}\n";

	vertex_shader = vs;

	return Result::SUCCESS;
}


Result::Name EmbeddedGenericGLSLShader::GenerateFragmentShader( const GenericShaderDesc& desc, std::string& fragment_shader )
{
	static const char *fs =
	"#version 330\n"\
	"out vec4 o_color;\n"\
	"void main(){o_color = vec4(1,1,1,1);}\n";

	fragment_shader = fs;

	return Result::SUCCESS;
}


} // namespace amorphous
