#include "EmbeddedGenericGLSLShader.hpp"
#include "Graphics/Shader/GenericShaderDesc.hpp"
#include "Support/Log/DefaultLog.hpp"

using std::string;


namespace amorphous
{


static Result::Name GenerateLightingVertexShader( const GenericShaderDesc& desc, std::string& vertex_shader )
{
	static const char *vs =
	"#version 330\n"\
	"layout(location = 0) in vec4 position;\n"\
	"uniform mat4 World;\n"\
	"uniform mat4 View;\n"\
	"uniform mat4 Proj;\n"\
	"uniform mat4 ViewWorld;\n"\
	"uniform mat4 ProjViewWorld;\n"\
	/*"uniform sampler2D T0;\n"\*/
	/*"uniform sampler2D T1;\n"\*/
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
	"out vec3 pos_vs;\n"\
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
		"vec4 pos4_vs = ViewWorld * position;"\
		"pos_vs = pos4_vs.xyz / pos4_vs.w;"\
		"t0 = tex0;"\
		"dc = diffuse_color;"\
	"}\n";

	vertex_shader = hs_dl_vs;

	return Result::SUCCESS;
}


static Result::Name GenerateNoLightingVertexShader( const GenericShaderDesc& desc, std::string& vertex_shader )
{
	static const char *no_lighting_vs =
	"#version 330\n"\
	"layout(location = 0) in vec4 position;\n"\
	"layout(location = 2) in vec4 diffuse_color;\n"\
	"layout(location = 3) in vec2 tex0;\n"\
	"out vec4 dc;\n"\
	"out vec2 t0;\n"\
	"uniform mat4 ProjViewWorld;\n"\
	"void main(){"\
		"gl_Position = ProjViewWorld * position;"\
		"t0 = tex0;"\
		"dc = diffuse_color;"\
	"}\n";

	vertex_shader = no_lighting_vs;

	return Result::SUCCESS;
}


Result::Name EmbeddedGenericGLSLShader::GenerateVertexShader( const GenericShaderDesc& desc, std::string& vertex_shader )
{
	if( desc.Lighting )
	{
		return GenerateLightingVertexShader( desc, vertex_shader );
	}
	else
	{
		return GenerateNoLightingVertexShader( desc, vertex_shader );
	}

	return Result::UNKNOWN_ERROR;
}


static Result::Name GenerateLightingFragmentShader( const GenericShaderDesc& desc, std::string& fragment_shader )
{
	static const char *fs =
	"#version 330\n"\
	"layout(location = 0) out vec4 fc;\n"\
	"void main(){fc = vec4(0,1,0,1);}\n";
	/*"out vec4 o_color;\n"\*/
	/*"void main(){o_color = vec4(1,1,1,1);}\n";*/

	static const char *hs_lighting_fs_ins =
	"#version 330\n"\
	"in vec3 pos_vs;\n"\
	"in vec3 n_vs;\n"\
	"in vec4 dc;\n"\
	"in vec2 t0;\n";

	static const char *hs_lighting_fs =
	"layout(location = 0) out vec4 fc;\n"\
	"uniform mat4 View;\n"\
	/*"uniform vec4 HSDL_UDC;\n"\*/
	/*"uniform vec4 HSDL_LDC;\n"\*/
	/*"uniform vec3 HSDL_Dir;\n"\*/
	"uniform sampler2D T0;\n"\
	"uniform sampler2D T1;\n"\
	"uniform sampler2D T2;\n"\
	"uniform sampler2D T3;\n";

	const char *fs_begin =
	"void main(){"\
	"	vec3 normal_vs = normalize(n_vs);"\
	"	vec4 br = vec4(0,0,0,1);\n";

	/*"	fc = vec4( normal_vs.x, normal_vs.y, normal_vs.z, 1 );"\*/

	fragment_shader += hs_lighting_fs_ins;

	fragment_shader += hs_lighting_fs;

	const char *hsdl_calc = "";
	const char *hspl_calc = "";
	const char *hssl_calc = "";

	if( desc.NumDirectionalLights != 0 )
	{
		int default_num_hsdls = 1;
		int num_hsdls = (0 < desc.NumPointLights) ? desc.NumPointLights : default_num_hsdls;
		fragment_shader += fmt_string( "#define NUM_HSDLS %d\n", num_hsdls );

		const char *hsdl_vars =
			"uniform vec4 HSDL_UDCs[NUM_HSDLS];\n"\
			"uniform vec4 HSDL_LDCs[NUM_HSDLS];\n"\
			/* directions in world space */
			"uniform vec3 HSDL_Dirs[NUM_HSDLS];\n"\
			"uniform int NumHSDLs;\n";
		fragment_shader += hsdl_vars;

		const char *non_specular_hsdl_calc =
		"	for(int i=0;i<NumHSDLs;i++)"\
		"	{"\
		"		vec3 dir_to_light = -HSDL_Dirs[i];"\
		"		float hsd = (dot(normal_vs,dir_to_light)+1)*0.5;"\
		"		br += HSDL_UDCs[i] * hsd + HSDL_LDCs[i] * (1-hsd);"\
		"	}\n";

		const char *specular_hsdl_calc =
		"	vec3 dir_to_viewer_vs = -normalize(pos_vs);"\
		"	for(int i=0;i<NumHSDLs;i++)"\
		"	{"\
		"		vec3 dir_to_light = -HSDL_Dirs[i];"\
		"		float cos_angle_incidence = dot(normal_vs,dir_to_light);"\
		"		cos_angle_incidence = clamp(cos_angle_incidence,0,1);"\
		"		vec3 half_angle = normalize(dir_to_viewer_vs+dir_to_light);"\
		/* Calculate the Blinn term */
		"		float bt = clamp(dot(half_angle,normal_vs),0,1);"\
		/* Clamp the Blinn term to 0 if the angle between the surface normal and the light direction is wider than 90 [deg] */
		"		bt = cos_angle_incidence != 0.0 ? bt : 0.0;"\
		/*"		bt = pow(bt,HSPL_ShininessFactors[i]);"\*/
		"		bt = pow(bt,3);"\
		"		float hsd = (dot(normal_vs,dir_to_light)+1)*0.5;"\
		"		br += ( HSDL_UDCs[i] * hsd + HSDL_LDCs[i] * (1-hsd) );"\
		/*"		br += HSPL_SCs[i] * bt;"\*/
		"		br += vec4(1,1,1,1) * bt;"\
		"	};\n";

		hsdl_calc = (desc.Specular == SpecularSource::NONE) ? non_specular_hsdl_calc : specular_hsdl_calc;
	}

	if( desc.NumPointLights != 0 )
//	if( false )
	{
		int default_num_hspls = 2;
		int num_hspls = (0 < desc.NumPointLights) ? desc.NumPointLights : default_num_hspls;
		fragment_shader += fmt_string( "#define NUM_HSPLS %d\n", num_hspls );

		const char *hspl_vars =
			"uniform vec4 HSPL_UDCs[NUM_HSPLS];\n"\
			"uniform vec4 HSPL_LDCs[NUM_HSPLS];\n"\
			"uniform vec4 HSPL_SCs[NUM_HSPLS];\n"\
			"uniform float HSPL_ShininessFactors[NUM_HSPLS];\n"\
			/* positions in world space */
			"uniform vec3 HSPL_Positions[NUM_HSPLS];\n"\
			"uniform vec3 HSPL_Atts[NUM_HSPLS];\n"\
			"uniform int NumHSPLs;\n";
		fragment_shader += hspl_vars;

		const char *non_specular_hspl_calc =
		"	for(int i=0;i<NumHSPLs;i++)"\
		"	{"\
		"		vec3 to_light = HSPL_Positions[i] - pos_vs;"\
		"		float dist = length(to_light);"\
		"		vec3 dir_to_light = to_light / dist;"\
		"		float att = 1.0f / (HSPL_Atts[i].x + HSPL_Atts[i].y*dist + HSPL_Atts[i].z*dist*dist);"\
		"		float hsd = (dot(normal_vs,dir_to_light)+1)*0.5;"\
		"		br += ( HSPL_UDCs[i] * hsd + HSPL_LDCs[i] * (1-hsd) ) * att;"\
		"	}\n";

		const char *specular_hspl_calc =
		"	vec3 dir_vs = normalize(pos_vs);"\
		"	for(int i=0;i<NumHSPLs;i++)"\
		"	{"\
		"		vec3 to_light = HSPL_Positions[i] - pos_vs;"\
		"		float dist = length(to_light);"\
		"		vec3 dir_to_light = to_light / dist;"\
		"		float cos_angle_incidence = dot(normal_vs,dir_to_light);"\
		"		cos_angle_incidence = clamp(cos_angle_incidence,0,1);"\
		"		vec3 half_angle = normalize(dir_vs+dir_to_light);"\
		/* Calculate the Blinn term */
		"		float bt = clamp(dot(half_angle,normal_vs),0,1);"\
		/* Clamp the Blinn term to 0 if the angle between the surface normal and the light direction is wider than 90 [deg] */
		"		bt = cos_angle_incidence != 0.0 ? bt : 0.0;"\
		/*"		bt = pow(bt,HSPL_ShininessFactors[i]);"\*/
		"		bt = pow(bt,3);"\
		"		float att = 1.0f / (HSPL_Atts[i].x + HSPL_Atts[i].y*dist + HSPL_Atts[i].z*dist*dist);"\
		"		float hsd = (dot(normal_vs,dir_to_light)+1)*0.5;"\
		"		br += ( HSPL_UDCs[i] * hsd + HSPL_LDCs[i] * (1-hsd) ) * att;"\
		/*"		br += HSPL_SCs[i] * bt * att;"\*/
		"		br += vec4(1,1,1,1) * bt * att;"\
		"	}\n";

//		hspl_calc = specular_hspl_calc;
		hspl_calc = (desc.Specular == SpecularSource::NONE) ? non_specular_hspl_calc : specular_hspl_calc;
	}

//	if( desc.NumSpotLights != 0 )
	if( false )
	{
		int default_num_hssls = 1;
		int num_hssls = (0 < desc.NumPointLights) ? desc.NumPointLights : default_num_hssls;
		fragment_shader += fmt_string( "#define NUM_HSSLS %d\n", num_hssls );

		const char *hssl_vars =
			"uniform vec4 HSSL_UDCs[NUM_HSSLS];\n"\
			"uniform vec4 HSSL_LDCs[NUM_HSSLS];\n"\
			/* directions in world space */
			"uniform vec3 HSSL_Dirs[NUM_HSSLS];\n"\
			"uniform int NumHSSLs;\n";
		fragment_shader += hssl_vars;

		const char *non_specular_hssl_calc =
		"	for(int i=0;i<NumHSSLs;i++)"\
		"	{"\
		"		vec3 dir_vs = mat3(View) * HSSL_Dirs[i];"\
		"		float hsd = (dot(normal_vs,-dir_vs)+1)*0.5;"\
		"		br += HSSL_UDCs[i] * hsd + HSSL_LDCs[i] * (1-hsd);"\
		"	}\n";
		hssl_calc = non_specular_hssl_calc;
	}

//	fragment_shader += hs_lighting_fs;

	fragment_shader += fs_begin;

	fragment_shader += hsdl_calc;
	fragment_shader += hspl_calc;
	fragment_shader += hssl_calc;

	const char *fs_end =
	"	vec4 tc = texture(T0,t0);"\
	"	fc = br * dc * tc;"\
	"	fc.a = dc.a * tc.a;"\
	/*"	fc = br;"\*/
	/*"	fc = vec4(0,1,1,1);"\*/
	/*"	fc = vec4(normal_vs.x,normal_vs.y,normal_vs.z,1);"*/
	"}\n";
	fragment_shader += fs_end;

	return Result::SUCCESS;
}


static Result::Name GenerateNoLightingFragmentShader( const GenericShaderDesc& desc, std::string& fragment_shader )
{
	static const char *no_lighting_fs =
	"#version 330\n"\
	"in vec4 dc;\n"\
	"in vec2 t0;\n"\
	"layout(location = 0) out vec4 fc;\n"\
	"uniform sampler2D T0;\n"\
	"uniform sampler2D T1;\n"\
	"uniform sampler2D T2;\n"\
	"uniform sampler2D T3;\n"\
	"void main(){"\
		"vec4 tc = texture(T0,t0);"\
		"fc = dc * tc;"\
	"}\n";

	fragment_shader = no_lighting_fs;

	return Result::SUCCESS;
}


Result::Name EmbeddedGenericGLSLShader::GenerateFragmentShader( const GenericShaderDesc& desc, std::string& fragment_shader )
{
	if( desc.Lighting )
	{
		return GenerateLightingFragmentShader( desc, fragment_shader );
	}
	else
	{
		return GenerateNoLightingFragmentShader( desc, fragment_shader );
	}

	return Result::UNKNOWN_ERROR;
}


} // namespace amorphous
