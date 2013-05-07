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
	"uniform mat4 World;\n"\
	"uniform mat4 View;\n"\
	"uniform mat4 Proj;\n"\
	"uniform mat4 ViewWorld;\n"\
	"uniform mat4 ProjViewWorld;\n"\
	"uniform sampler2D Tex0;\n"\
	"uniform sampler2D Tex1;\n"\
	"uniform vec3 HSDL_Dir;\n"\
	"void main(){gl_Position = ProjViewWorld * position;}\n";

	// test: hemispheric directional light
	// Note that the actual light calculation is done in FS
	static const char *hs_dl_vs =
	"#version 330\n"\
	"layout(location = 0) in vec4 position;\n"\
	"layout(location = 1) in vec3 normal;\n"\
	"layout(location = 2) in vec4 diffuse_color;\n"\
	"layout(location = 3) in vec2 tex0;\n"\
	"out vec3 n_vs;\n"\
	"out vec4 dc;\n"\
	"out vec2 t0;\n"\
	"uniform mat4 World;\n"\
	"uniform mat4 View;\n"\
	"uniform mat4 Proj;\n"\
	"uniform mat4 ViewWorld;\n"\
	"uniform mat4 ProjViewWorld;\n"\
	"void main(){"\
		"gl_Position = ProjViewWorld * position;"\
		"n_vs = mat3(ViewWorld) * normal;"\
		"t0 = tex0;"\
		"dc = diffuse_color;"\
	"}\n";

	vertex_shader = hs_dl_vs;
	
	return Result::SUCCESS;
}


Result::Name EmbeddedGenericGLSLShader::GenerateFragmentShader( const GenericShaderDesc& desc, std::string& fragment_shader )
{
	static const char *fs =
	"#version 330\n"\
	"void main(){gl_FragColor = vec4(0,1,0,1);}\n";
	/*"out vec4 o_color;\n"\*/
	/*"void main(){o_color = vec4(1,1,1,1);}\n";*/

	static const char *hs_dl_fs =
	"#version 330\n"\
	"in vec3 n_vs;\n"\
	"in vec4 dc;\n"\
	"in vec2 t0;\n"\
	"uniform mat4 View;\n"\
	"uniform vec4 HSDL_UDC;\n"\
	"uniform vec4 HSDL_LDC;\n"\
	"uniform vec3 HSDL_Dir;\n"\
	"uniform sampler2D T0;\n"\
	"uniform sampler2D T1;\n"\
	"uniform sampler2D T2;\n"\
	"uniform sampler2D T3;\n"\
	"void main(){"\
	"	vec3 normal_vs = normalize(n_vs);"\
	"	vec3 dir_vs = mat3(View) * HSDL_Dir;"\
	"	float hsd = (dot(normal_vs,-dir_vs)+1)*0.5;"\
	"	vec4 br = HSDL_UDC * hsd + HSDL_LDC * (1-hsd);"\
	/*"	gl_FragColor = vec4( normal_vs.x, normal_vs.y, normal_vs.z, 1 );"\*/
	"	vec4 tc = texture(T0,t0);"\
	"	gl_FragColor = br * dc * tc;"\
	/*"	gl_FragColor = br;"\*/
	/*"	gl_FragColor = vec4(0,1,1,1);"\*/
	"	gl_FragColor.a = dc.a * tc.a;"
	"}\n";

	static const char *no_lighting_fs =
	"#version 330\n"\
	"in vec4 dc;\n"\
	"in vec2 t0;\n"\
	"uniform sampler2D T0;\n"\
	"uniform sampler2D T1;\n"\
	"uniform sampler2D T2;\n"\
	"uniform sampler2D T3;\n"\
	"void main(){"\
	"	vec4 tc = texture(T0,t0);"\
	"	gl_FragColor = dc * tc;"\
	"}\n";

	fragment_shader = desc.Lighting ? hs_dl_fs : no_lighting_fs;

	return Result::SUCCESS;
}


} // namespace amorphous
