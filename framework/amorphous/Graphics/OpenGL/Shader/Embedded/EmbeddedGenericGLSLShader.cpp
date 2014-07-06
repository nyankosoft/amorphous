#include "EmbeddedGenericGLSLShader.hpp"
#include "Graphics/Shader/GenericShaderDesc.hpp"
#include "Graphics/Shader/Generic2DShaderDesc.hpp"
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
	"out vec4 pos_ps;\n"\
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
		"pos_ps = ProjViewWorld * position;"\
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


static void GeneratePlanarReflectionFragmentShader( const GenericShaderDesc& desc, const char *&planar_reflection_fs_dec, const char *&planar_reflection_fs_code )
{
//	static const char *planar_reflection_fs_dec = "";
//	static const char *planar_reflection_fs_code = "";

	switch( desc.PlanarReflection )
//	switch( PlanarReflectionOption::NONE )
	{
	case PlanarReflectionOption::FLAT:
		planar_reflection_fs_dec =
			"uniform sampler2D PlanarMirrorTextureSampler;\n"\
			"uniform float g_fPlanarReflection = 0.5f;\n";
		planar_reflection_fs_code =
			"vec2 pr_tex = vec2( pos_ps.x / pos_ps.w, pos_ps.y / pos_ps.w ) * 0.5 + 0.5;\n"\
			"vec4 pr_tex_color = vec4( texture(T1/*PlanarMirrorTextureSampler*/, pr_tex).xyz, 1 );\n"\
			/*"vec4 pr_tex_color = vec4( texture(T1, pr_tex).xyz, 1 );\n"\*/
			"fc = fc * (1-g_fPlanarReflection) + pr_tex_color * g_fPlanarReflection;\n";
		break;

	case PlanarReflectionOption::PERTURBED:
		planar_reflection_fs_dec =
			"uniform sampler2D PlanarMirrorTextureSampler;\n"\
			"uniform sampler2D PerturbationTextureSampler;\n"\
			"uniform float g_fPlanarReflection = 1.0f;\n"\
			"vec2 g_vPerturbationTextureUVShift = vec2(0,0);\n";
		planar_reflection_fs_code =
			"vec2 pr_tex = vec2( pos_ps.x, -pos_ps.y ) * 0.5 + 0.5;"\
			"vec2 perturbation = ( tex2D(PerturbationTextureSampler, t0 + g_vPerturbationTextureUVShift).xy - vec2(0.5,0.5) ) * 0.1;"\
			"pr_tex += perturbation;"\
			"t0 += perturbation;"\
			"vec4 pr_tex_color = vec4( tex2D(PlanarMirrorTextureSampler, pr_tex).xyz, 1 );\n"\
			"#define PLANAR_REFLECTION_COLOR_UPDATE color = color * (1-g_fPlanarReflection) + pr_tex_color * g_fPlanarReflection;\n";
		break;

	case PlanarReflectionOption::NONE:
	default:
		break;
	}
}


static Result::Name GenerateLightingFragmentShader( const GenericShaderDesc& desc, std::string& fragment_shader )
{
	static const char *fs_test =
	"#version 330\n"\
	"layout(location = 0) out vec4 fc;\n"\
	"void main(){fc = vec4(0,1,0,1);}\n";
	/*"out vec4 o_color;\n"\*/
	/*"void main(){o_color = vec4(1,1,1,1);}\n";*/

	// pos_vc: vertex position in view space
	// pos_vc: vertex position in projection space
	static const char *hs_lighting_fs_ins =
	"#version 330\n"\
	"in vec3 pos_vs;\n"\
	"in vec4 pos_ps;\n"\
	"in vec3 n_vs;\n"\
	"in vec4 dc;\n"\
	"in vec2 t0;\n";

	// fc: output fragment color
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

	const char *planar_reflection_fs_dec = "";
	const char *planar_reflection_fs_code = "";
	GeneratePlanarReflectionFragmentShader( desc, planar_reflection_fs_dec, planar_reflection_fs_code );

	fragment_shader += hs_lighting_fs_ins;

	fragment_shader += hs_lighting_fs;

	fragment_shader += planar_reflection_fs_dec;

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
			"for(int i=0;i<NumHSDLs;i++)"\
			"{"\
				"vec3 dir_to_light = -HSDL_Dirs[i];"\
				"float hsd = (dot(normal_vs,dir_to_light)+1)*0.5;"\
				"br += HSDL_UDCs[i] * hsd + HSDL_LDCs[i] * (1-hsd);"\
			"}\n";

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
		/*"		bt = pow(bt,HSDL_Gs[i]);"\*/
		"		bt = pow(bt,25);"\
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
			"for(int i=0;i<NumHSPLs;i++)"\
			"{"\
				"vec3 to_light = HSPL_Positions[i] - pos_vs;"\
				"float dist = length(to_light);"\
				"vec3 dir_to_light = to_light / dist;"\
				"float att = 1.0f / (HSPL_Atts[i].x + HSPL_Atts[i].y*dist + HSPL_Atts[i].z*dist*dist);"\
				"float hsd = (dot(normal_vs,dir_to_light)+1)*0.5;"\
				"br += ( HSPL_UDCs[i] * hsd + HSPL_LDCs[i] * (1-hsd) ) * att;"\
			"}\n";

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
		"		vec3 dir_vs = HSSL_Dirs[i];"\
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

	const char *fs_fragment_color =
	"	vec4 tc = texture(T0,t0);"\
	"	fc = br * dc * tc;"\
	"	fc.a = dc.a * tc.a;\n";
	/*"	fc = br;"\*/
	/*"	fc = vec4(0,1,1,1);"\*/
	/*"	fc = vec4(normal_vs.x,normal_vs.y,normal_vs.z,1);"*/

	fragment_shader += fs_fragment_color;

	// Planar reflection shader (calculations)
	fragment_shader += planar_reflection_fs_code;

	const char *fs_end = "}\n";

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
	Result::Name res = Result::UNKNOWN_ERROR;

	if( desc.Lighting )
	{
		res = GenerateLightingFragmentShader( desc, fragment_shader );
	}
	else
	{
		res = GenerateNoLightingFragmentShader( desc, fragment_shader );
	}

	{
		static int s_counter = 0;
		FILE *fp = fopen(fmt_string(".debug/fragment_shader_%02d.frag",s_counter).c_str(),"w");
		s_counter += 1;
		if(fp)
		{
			fprintf(fp,fragment_shader.c_str());
			fclose(fp);
		}
	}

	return res;
}


static const char *sg_2d_glsl_vs =
	"#version 330\n"\
	"layout(location = 0) in vec4 position;\n"\
	"layout(location = 1) in vec4 diffuse_color;\n"\
	"layout(location = 2) in vec2 tex0;\n"\
	"layout(location = 3) in vec2 tex1;\n"\
	"layout(location = 4) in vec2 tex2;\n"\
	"layout(location = 5) in vec2 tex3;\n"\
	"out vec4 dc;\n"\
	"out vec2 t0;\n"\
	"out vec2 t1;\n"\
	"out vec2 t2;\n"\
	"out vec2 t3;\n"\
	"void main(){"\
		/*"gl_Position = position;"\*/
		"float vpw = 1280;"\
		"float vph =  720;"\
		"float x = ( position.x / vpw - 0.5) * 2.0;"\
		"float y = (-position.y / vph + 0.5) * 2.0;"\
		"gl_Position = vec4(x,y,position.z,1);"\
		"dc = diffuse_color;"\
		"t0 = tex0;"\
		"t1 = tex1;"\
		"t2 = tex2;"\
		"t3 = tex3;"\
	"}\n";

static const char *sg_2d_glsl_fs =
	"#version 330\n"\
	"in vec4 dc;\n"\
	"in vec2 t0;\n"\
	"in vec2 t1;\n"\
	"in vec2 t2;\n"\
	"in vec2 t3;\n"\
	/*"layout(location = 0) out vec4 fc;\n"\*/
	"out vec4 fc;\n"\
	"uniform sampler2D T0;\n"\
	"uniform sampler2D T1;\n"\
	"uniform sampler2D T2;\n"\
	"uniform sampler2D T3;\n"\
	"void main(){"\
		/*"vec4 tc = texture(T0,t0);"\*/
		/*"vec4 tc = texture(T0,t0) * texture(T1,t0);"\*/
		/*"fc = dc * tc;"\*/
		"vec3 trgb = texture(T0,t0).rgb * texture(T1,t0).rgb;"\
		"fc.rgb = dc.rgb * trgb;"\
		"fc.a = dc.a;"\
		/*"fc = vec4(1,1,0,1);"\*/
	"}\n";


Result::Name EmbeddedGenericGLSLShader::Generate2DVertexShader( const Generic2DShaderDesc& desc, std::string& shader )
{
	shader = sg_2d_glsl_vs;
	return Result::SUCCESS;
}


Result::Name EmbeddedGenericGLSLShader::Generate2DFragmentShader( const Generic2DShaderDesc& desc, std::string& shader )
{
//	shader = sg_2d_glsl_fs;

	string& fs = shader;

	fs =
	"#version 330\n"\
	"in vec4 dc;\n"\
	"in vec2 t0;\n"\
	"in vec2 t1;\n"\
	"in vec2 t2;\n"\
	"in vec2 t3;\n"\
	/*"layout(location = 0) out vec4 fc;\n"\*/
	"out vec4 fc;\n"\
	"uniform sampler2D T0;\n"\
	"uniform sampler2D T1;\n"\
	"uniform sampler2D T2;\n"\
	"uniform sampler2D T3;\n"\
	"void main(){";

	for( int i=0; i<numof(desc.textures); i++ )
	{
		const texture_sample_params& sample_params = desc.textures[i];

		if( !sample_params.is_valid() )
			break;

		fs += fmt_string( "vec4 tc%d = texture(T%d,t%d);\n", i, sample_params.sampler, sample_params.coord );
	}

	fs += "vec3 rgb=";
	AppendBlendCalculations(
		desc,
		"rgb",
		desc.diffuse_color_and_tex0_blend.rgb,
		desc.tex0_and_tex1_blend.rgb,
		desc.tex1_and_tex2_blend.rgb,
		desc.tex2_and_tex3_blend.rgb,
		fs
		);
	fs += ";\n";

	fs += "float a=";
	AppendBlendCalculations(
		desc,
		"a",
		desc.diffuse_color_and_tex0_blend.alpha,
		desc.tex0_and_tex1_blend.alpha,
		desc.tex1_and_tex2_blend.alpha,
		desc.tex2_and_tex3_blend.alpha,
		fs
		);
	fs += ";\n";

	fs += "fc = vec4(rgb,a);}\n";

	{
		static int s_counter = 0;
		FILE *fp = fopen(fmt_string(".debug/tl_fragment_shader_%02d.frag",s_counter).c_str(),"w");
		s_counter += 1;
		if(fp)
		{
			fprintf(fp,fs.c_str());
			fclose(fp);
		}
	}

	return Result::SUCCESS;
}


} // namespace amorphous
