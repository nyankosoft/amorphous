#ifndef __amorphous_GLEmbeddedMiscGLSLShader_HPP__
#define __amorphous_GLEmbeddedMiscGLSLShader_HPP__


#include "amorphous/Graphics/Shader/Embedded/EmbeddedMiscShader.hpp"


namespace amorphous
{


class GLEmbeddedMiscGLSLShader : public EmbeddedMiscShader
{
public:

	GLEmbeddedMiscGLSLShader(){}
	//GLEmbeddedMiscGLSLShader( MiscShader::ID shader_id ) : m_ShaderID(shader_id) {}
	~GLEmbeddedMiscGLSLShader(){}

	static void GetSingleDiffuseColorVertexShader( std::string& shader )
	{
		static const char *vs =
		"#version 330\n"\
		"layout(location = 0) in vec4 position;\n"\
		"layout(location = 2) in vec4 _dc;\n"\
		"out vec4 dc;\n"\
		"uniform mat4 ProjViewWorld;\n"\
		"void main(){gl_Position=ProjViewWorld*position;dc=_dc;}\n";

		shader = vs;
	}

	static void GetVertexWeightMapDisplayVertexShader( std::string& shader )
	{
		// Adapted from GenerateNoLightingVertexShader()
//		static const char *no_lighting_vs =
//		"#version 330\n"\
//		"layout(location = 0) in vec4 position;\n"\
//		"layout(location = 3) in vec2 tex0;\n"\
//		"out vec2 t0;\n"\
//		"uniform mat4 ProjViewWorld;\n"\
//		"void main(){gl_Position=ProjViewWorld*position;t0=tex0;}\n";
	}

	static void GetSingleDiffuseColorFragmentShader( std::string& shader )
	{
		static const char *fs =
		"#version 330\n"\
		"in vec4 dc;\n"\
		"layout(location = 0) out vec4 fc;\n"\
		"void main(){"\
		"	vec4 fc = dc;\n";
		"}";

		shader = fs;
	}

	static void GetVertexWeightMapDisplayFragmentShader( std::string& shader )
	{
	}

	/// vertex shader
	static Result::Name GetVertexShader( MiscShader::ID shader_id, std::string& shader )
	{
		switch( shader_id )
		{
		case MiscShader::SINGLE_DIFFUSE_COLOR:
			GetSingleDiffuseColorVertexShader(shader);
			break;
		case MiscShader::SHADED_SINGLE_DIFFUSE_COLOR:
			GetSingleDiffuseColorVertexShader(shader);
			break;
		case MiscShader::VERTEX_WEIGHT_MAP_DISPLAY:
			GetVertexWeightMapDisplayVertexShader(shader);
			break;
		default:
			return Result::UNKNOWN_ERROR;
			break;
		}

		return Result::SUCCESS;
	}

	/// pixel(fragment) shader
	static Result::Name GetFragmentShader( MiscShader::ID shader_id, std::string& shader )
	{
		switch( shader_id )
		{
		case MiscShader::SINGLE_DIFFUSE_COLOR:
			GetSingleDiffuseColorFragmentShader(shader);
			break;
		case MiscShader::SHADED_SINGLE_DIFFUSE_COLOR:
			GetSingleDiffuseColorFragmentShader(shader);
			break;
		case MiscShader::VERTEX_WEIGHT_MAP_DISPLAY:
			GetVertexWeightMapDisplayFragmentShader(shader);
			break;
		default:
			return Result::UNKNOWN_ERROR;
			break;
		}

		return Result::SUCCESS;
	}
};


} // namespace amorphous


#endif /* __amorphous_GLEmbeddedMiscGLSLShader_HPP__ */
