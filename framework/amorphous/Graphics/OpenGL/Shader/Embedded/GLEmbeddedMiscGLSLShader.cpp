#include "GLEmbeddedMiscGLSLShader.hpp"


namespace amorphous
{


void GLEmbeddedMiscGLSLShader::GetSingleDiffuseColorVertexShader( std::string& shader )
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


void GLEmbeddedMiscGLSLShader::GetVertexWeightMapDisplayVertexShader( std::string& shader )
{
	// Adapted from GenerateNoLightingVertexShader()
	static const char *vs =
	"#version 330\n"\
	"#define NUM_COLORS 22\n"\
	"uniform vec4 colors[NUM_COLORS] ="\
	"{"\
		"vec4(1.00,0.25,0.25,1),"\
		"vec4(0.25,1.00,0.25,1),"\
		"vec4(0.25,0.25,1.00,1),"\
		"vec4(1.00,0.25,1.00,1),"\
		"vec4(0.25,1.00,1.00,1),"\
		"vec4(1.00,0.25,1.00,1),"\
		"vec4(0.25,0.25,0.25,1),"\
		"vec4(1.00,0.50,0.50,1),"\
		"vec4(0.50,1.00,0.50,1),"\
		"vec4(0.50,0.50,1.00,1),"\
		"vec4(1.00,0.50,1.00,1),"\
		"vec4(0.50,1.00,1.00,1),"\
		"vec4(1.00,0.50,1.00,1),"\
		"vec4(0.50,0.50,0.50,1),"\
		"vec4(1.00,0.75,0.75,1),"\
		"vec4(0.75,1.00,0.75,1),"\
		"vec4(0.75,0.75,1.00,1),"\
		"vec4(1.00,0.75,1.00,1),"\
		"vec4(0.75,1.00,1.00,1),"\
		"vec4(1.00,0.75,1.00,1),"\
		"vec4(0.75,0.75,0.75,1),"\
		"vec4(1,1,1,1)"\
	"};\n"\
	"layout(location = 0) in vec4 position;\n"\
	"layout(location = 1) in vec4 blend_weights;\n"\
	"layout(location = 2) in ivec4 blend_indices;\n"\
	"layout(location = 3) in vec3 normal;\n"\
	"layout(location = 4) in vec4 _dc;\n"\
	"out vec4 dc;\n"\
	"uniform mat4 ProjViewWorld;\n"\
	"void main()"\
	"{"\
		"gl_Position=ProjViewWorld*position;"\
		"dc=colors[ blend_indices[0] % NUM_COLORS ];"\
	"}\n";

	shader = vs;
}


void GLEmbeddedMiscGLSLShader::GetSingleDiffuseColorFragmentShader( std::string& shader )
{
	static const char *fs =
	"#version 330\n"\
	"in vec4 dc;\n"\
	"layout(location = 0) out vec4 fc;\n"\
	"void main(){"\
	"	fc = dc;\n";
	"}\n";

	shader = fs;
}


void GLEmbeddedMiscGLSLShader::GetVertexWeightMapDisplayFragmentShader( std::string& shader )
{
	static const char *fs =
	"#version 330\n"\
	"in vec4 dc;\n"\
	"layout(location = 0) out vec4 fc;\n"\
	"void main(){"\
		"fc = dc;\n"\
	"}\n";

	shader = fs;
}

Result::Name GLEmbeddedMiscGLSLShader::GetVertexShader( MiscShader::ID shader_id, std::string& shader )
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


Result::Name GLEmbeddedMiscGLSLShader::GetFragmentShader( MiscShader::ID shader_id, std::string& shader )
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


} // namespace amorphous
