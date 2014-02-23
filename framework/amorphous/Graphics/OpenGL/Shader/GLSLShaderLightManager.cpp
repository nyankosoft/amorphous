#include "GLSLShaderLightManager.hpp"
#include "Graphics/OpenGL/GLExtensions.hpp"
#include "Support/StringAux.hpp"
#include "Support/Macro.h"


namespace amorphous
{

using namespace std;
//using namespace boost;


CGLSLShaderLightManager::CGLSLShaderLightManager()
:
m_Program(0),
m_VertexShader(0),
m_FragmentShader(0)
//m_NumCurrentHSPointLights(0)
{
	memset( m_VSLowerLightColor, 0, sizeof(m_VSLowerLightColor) );
	memset( m_FSLowerLightColor, 0, sizeof(m_FSLowerLightColor) );

	for( int i=0; i<NUM_MISC_UNIFORMS; i++ )
		m_MiscUniforms[i] = -1;

	m_NumMaxHSDirectionalLights = 0xFF;
	m_NumMaxHSPointLights       = 0xFF;
	m_NumMaxHSSpotlights        = 0xFF;
}


void CGLSLShaderLightManager::Init( GLuint program )
{
	if( program == 0 )
		return;

	m_Program = program;

	glUseProgram( m_Program );

	char var_name[16];
	memset( var_name, 0, sizeof(var_name) );

//	GLint dirs[2];

//	for( int i=0; i<numof(m_HSDirLights); i++ )
//	{
//		sprintf( var_name, "HSDL_Dir[%d]", i );
//		dirs[i] = glGetUniformLocation( program,   var_name );
//	}

	m_MiscUniforms[NUM_HS_DIRECTIONAL_LIGHTS] = glGetUniformLocation( program, "NumHSDLs" );
	m_MiscUniforms[NUM_HS_POINT_LIGHTS]       = glGetUniformLocation( program, "NumHSPLs" );
	m_MiscUniforms[NUM_HS_SPOTLIGHTS]         = glGetUniformLocation( program, "NumHSSLs" );

	m_HSDirLightUniforms.resize( 2 );
	for( int i=0; i<(int)m_HSDirLightUniforms.size(); i++ )
	{
		m_HSDirLightUniforms[i].m_DiffuseColors[0] = glGetUniformLocation( program, fmt_string( "HSDL_UDCs[%d]", i ).c_str() );
		m_HSDirLightUniforms[i].m_DiffuseColors[1] = glGetUniformLocation( program, fmt_string( "HSDL_LDCs[%d]", i ).c_str() );
		m_HSDirLightUniforms[i].m_Direction        = glGetUniformLocation( program, fmt_string( "HSDL_Dirs[%d]", i ).c_str() );
	}

	m_HSDirLight.Attribute.UpperDiffuseColor = SFloatRGBAColor::White();
	m_HSDirLight.Attribute.LowerDiffuseColor = SFloatRGBAColor(0,0,0,1);
	m_HSDirLight.vDirection = Vec3GetNormalized( Vector3(-1,-3,1) );

//	UpdateHSDirLightUniformVariables( m_HSDirLight, m_HSDirLightUniforms );

	m_HSPointLightUniforms.resize( 16 );

//	for( int i=0; i<numof(m_HSPointLights); i++ )
	for( int i=0; i<(int)m_HSPointLightUniforms.size(); i++ )
	{
		m_HSPointLightUniforms[i].m_DiffuseColors[0] = glGetUniformLocation( program, fmt_string( "HSPL_UDCs[%d]"     , i ).c_str() );
		m_HSPointLightUniforms[i].m_DiffuseColors[1] = glGetUniformLocation( program, fmt_string( "HSPL_LDCs[%d]"     , i ).c_str() );
		m_HSPointLightUniforms[i].m_Position         = glGetUniformLocation( program, fmt_string( "HSPL_Positions[%d]", i ).c_str() );
		m_HSPointLightUniforms[i].m_Attenuation      = glGetUniformLocation( program, fmt_string( "HSPL_Atts[%d]"     , i ).c_str() );
	}
}


void CGLSLShaderLightManager::SetHSDiffuseColors( const HemisphericLightAttribute& hs_light, GLenum light_id )
{
	const SFloatRGBAColor& ldc = hs_light.LowerDiffuseColor;
	glUniform4f( m_VSLowerLightColor[light_id], ldc.red, ldc.green, ldc.blue, ldc.alpha );
	glUniform4f( m_FSLowerLightColor[light_id], ldc.red, ldc.green, ldc.blue, ldc.alpha );

//	float rgba[4];
//	FRGBtoFloat4( hs_light.UpperDiffuseColor, rgba );

//	glLightfv( light_id, GL_DIFFUSE,  (GLfloat *)rgba ); // 4 floats
	glLightfv( light_id, GL_DIFFUSE,  (GLfloat *)&(hs_light.UpperDiffuseColor) ); // 4 floats
}


void CGLSLShaderLightManager::UpdateHSDirLightUniformVariables( const HemisphericDirectionalLight& light, const GLSLDirectionalLight& glsl_light )
{
	if( m_Program == 0 )
		return;

	glUseProgram( m_Program );

	glUniform4fv( glsl_light.m_DiffuseColors[0], 1, (GLfloat *)&light.Attribute.UpperDiffuseColor );
	glUniform4fv( glsl_light.m_DiffuseColors[1], 1, (GLfloat *)&light.Attribute.LowerDiffuseColor );
	glUniform3fv( glsl_light.m_Direction,        1, (GLfloat *)&light.vDirection );
}


void CGLSLShaderLightManager::UpdateHSPointLightUniformVariables( const HemisphericPointLight& light, const GLSLLight& glsl_light )
{
	if( m_Program == 0 )
		return;

	glUseProgram( m_Program );

	glUniform4fv( glsl_light.m_DiffuseColors[0], 1, (GLfloat *)&light.Attribute.UpperDiffuseColor );
	glUniform4fv( glsl_light.m_DiffuseColors[1], 1, (GLfloat *)&light.Attribute.LowerDiffuseColor );
//	glUniform3fv( glsl_light.m_Position,         1, (GLfloat *)&light.vPosition );
//	glUniform3fv( glsl_light.m_Attenuation,      1, (GLfloat *)&light.fAttenuation );
}


void CGLSLShaderLightManager::SetAmbientLight( const AmbientLight& light )
{
}


void CGLSLShaderLightManager::SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light )
{
	if( (int)m_HSDirectionalLights.size() == m_NumMaxHSDirectionalLights )
		return;

	m_HSDirectionalLights.push_back( light );

//	m_HSDirLight = light;

//	UpdateHSDirLightUniformVariables( light, m_HSDirLightUniforms );

//	GLenum light_id = m_NumCurrentLights;

//	SetHSDiffuseColors( light.Attribute, light_id );
}


void CGLSLShaderLightManager::SetHemisphericPointLight( const HemisphericPointLight& light )
{
	if( (int)m_HSPointLights.size() == m_NumMaxHSPointLights )
		return;

	m_HSPointLights.push_back( light );

//	if( (int)m_HSPL_Positions.size() == m_NumMaxHSPointLights )
//		return;

//	m_HSPointLights[m_NumCurrentHSPointLights] = light;

//	m_HSPL_Positions.push_back( light.vPosition );
//	m_HSPL_UpperDiffuseColors.push_back( light.Attribute.UpperDiffuseColor );
//	m_HSPL_LowerDiffuseColors.push_back( light.Attribute.LowerDiffuseColor );
//	m_HSPL_Attenuations.push_back( Vector3( light.fAttenuation[0], light.fAttenuation[1], light.fAttenuation[2] ) );

//	UpdateHSPointLightUniformVariables( light, m_HSPointLightUniforms[m_NumCurrentHSPointLights] );

//	m_NumCurrentHSPointLights += 1;

//	GLenum light_id = m_NumCurrentLights;

//	SetHSDiffuseColors( light.Attribute, light_id );
}

//	void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	void SetTriPointLight( const CTriPointLight& light ) {}

void CGLSLShaderLightManager::ClearLights()
{
	m_HSDirectionalLights.resize( 0 );
	m_HSPointLights.resize( 0 );
	m_HSSpotlights.resize( 0 );

//	m_NumCurrentHSPointLights = 0;

//	m_HSPL_Positions.clear();
//	m_HSPL_UpperDiffuseColors.clear();
//	m_HSPL_LowerDiffuseColors.clear();
//	m_HSPL_Attenuations.clear();
}


void CGLSLShaderLightManager::CommitChanges()
{
	if( m_Program == 0 )
		return;

	glUseProgram( m_Program );

	glUniform1i( m_MiscUniforms[NUM_HS_DIRECTIONAL_LIGHTS], (GLint)m_HSDirectionalLights.size() );
	glUniform1i( m_MiscUniforms[NUM_HS_POINT_LIGHTS],       (GLint)m_HSPointLights.size() );
	glUniform1i( m_MiscUniforms[NUM_HS_SPOTLIGHTS],         (GLint)m_HSSpotlights.size() );

	for( size_t i=0; i<m_HSDirectionalLights.size(); i++ )
	{
		if( m_HSDirLightUniforms.size() <= i )
			break;

		GLSLDirectionalLight& dest = m_HSDirLightUniforms[i];
		HemisphericDirectionalLight& src = m_HSDirectionalLights[i];
//		const SFloatRGBAColor& uc = src.Attribute.UpperDiffuseColor;
//		const SFloatRGBAColor& lc = src.Attribute.LowerDiffuseColor;
		glUniform4fv( dest.m_DiffuseColors[0], 1, (GLfloat *)&(src.Attribute.UpperDiffuseColor) );
		glUniform4fv( dest.m_DiffuseColors[1], 1, (GLfloat *)&(src.Attribute.LowerDiffuseColor) );

		Matrix44 view_orientation = m_ViewMatrix;
		view_orientation.SetColumn( 3, Vector4(0,0,0,1) );
		Vector3 view_space_dir = view_orientation * src.vDirection;
		glUniform3fv( dest.m_Direction,        1, (GLfloat *)&(view_space_dir) );
	}

	for( size_t i=0; i<m_HSPointLights.size(); i++ )
	{
		if( m_HSPointLightUniforms.size() <= i )
			break;

		GLSLPointLight& dest = m_HSPointLightUniforms[i];
		HemisphericPointLight& src = m_HSPointLights[i];
		glUniform4fv( dest.m_DiffuseColors[0], 1, (GLfloat *)&(src.Attribute.UpperDiffuseColor) );
		glUniform4fv( dest.m_DiffuseColors[1], 1, (GLfloat *)&(src.Attribute.LowerDiffuseColor) );
		glUniform3fv( dest.m_Attenuation,      1, (GLfloat *)&(src.fAttenuation) );

		Vector3 view_space_pos = m_ViewMatrix * src.vPosition;
//		glUniform1fv( dest.m_Position,         3, (GLfloat *)&(view_space_pos) ); // 1fv() for vec3 uniform: Does not work.
		glUniform3fv( dest.m_Position,         1, (GLfloat *)&(view_space_pos) );
	}


//	GLSLLight& hsdl = m_HSDirLightUniforms;

//	if( !m_HSPL_Positions.empty() )
//	{
//		uint num_lights = (uint)m_HSDL_Directions.size();
//		glUniform4fv( hsdl.m_DiffuseColors[0], num_lights, (GLfloat *)&m_HSDL_UpperDiffuseColors[0] );
//		glUniform4fv( hsdl.m_DiffuseColors[1], num_lights, (GLfloat *)&m_HSDL_LowerDiffuseColors[0] );
//		glUniform3fv( hsdl.m_Direction,        num_lights, (GLfloat *)&m_HSDL_Directions[0] );
//	}

//	GLSLLight& hspl = m_HSPointLightUniforms[0];

//	if( !m_HSPL_Positions.empty() )
//	{
//		uint num_lights = (uint)m_HSPL_Positions.size();
//		glUniform4fv( hspl.m_DiffuseColors[0], num_lights, (GLfloat *)&m_HSPL_UpperDiffuseColors[0] );
//		glUniform4fv( hspl.m_DiffuseColors[1], num_lights, (GLfloat *)&m_HSPL_LowerDiffuseColors[0] );
//		glUniform3fv( hspl.m_Position,         num_lights, (GLfloat *)&m_HSPL_Positions[0] );
//		glUniform3fv( hspl.m_Attenuation,      num_lights, (GLfloat *)&m_HSPL_Attenuations[0] );
//	}
}


} // namespace amorphous
