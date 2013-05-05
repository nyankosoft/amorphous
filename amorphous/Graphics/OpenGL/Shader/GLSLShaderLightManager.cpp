#include "GLSLShaderLightManager.hpp"
#include "Graphics/OpenGL/GLExtensions.hpp"
#include "Support/Macro.h"


namespace amorphous
{

using namespace std;
//using namespace boost;


CGLSLShaderLightManager::CGLSLShaderLightManager()
:
m_Program(0),
m_VertexShader(0),
m_FragmentShader(0),
m_NumCurrentHSPointLights(0)
{
	memset( m_VSLowerLightColor, 0, sizeof(m_VSLowerLightColor) );
	memset( m_FSLowerLightColor, 0, sizeof(m_FSLowerLightColor) );
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

	m_HSDirLightUniforms.m_DiffuseColors[0] = glGetUniformLocation( program, "HSDL_UDC" );
	m_HSDirLightUniforms.m_DiffuseColors[1] = glGetUniformLocation( program, "HSDL_LDC" );
	m_HSDirLightUniforms.m_Direction        = glGetUniformLocation( program, "HSDL_Dir" );

	m_HSDirLight.Attribute.UpperDiffuseColor = SFloatRGBAColor::White();
	m_HSDirLight.Attribute.LowerDiffuseColor = SFloatRGBAColor(0,0,0,1);
	m_HSDirLight.vDirection = Vec3GetNormalized( Vector3(-1,-3,1) );

	UpdateHSDirLightUniformVariables( m_HSDirLight, m_HSDirLightUniforms );

//	for( int i=0; i<numof(m_HSPointLights); i++ )
	for( int i=0; i<1; i++ )
	{
		m_HSPointLightUniforms[i].m_DiffuseColors[0] = glGetUniformLocation( program, "HSPL_UDC" );
		m_HSPointLightUniforms[i].m_DiffuseColors[1] = glGetUniformLocation( program, "HSPL_LDC" );
		m_HSPointLightUniforms[i].m_Direction        = glGetUniformLocation( program, "HSPL_Pos" );
	}

	int num_lights = 4;
	for( int i=0; i<num_lights; i++ )
	{
		sprintf( var_name, "g_LowerColor%d", i );
//		sprintf( var_name, "g_LowerColor[%d]", i );
		m_VSLowerLightColor[i] = glGetUniformLocation( m_VertexShader,   var_name );
		m_FSLowerLightColor[i] = glGetUniformLocation( m_FragmentShader, var_name );
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


void CGLSLShaderLightManager::UpdateHSDirLightUniformVariables( const HemisphericDirectionalLight& light, const GLSLLight& glsl_light )
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
	glUniform3fv( glsl_light.m_Position,         1, (GLfloat *)&light.vPosition );
	glUniform3fv( glsl_light.m_Attenuation,      1, (GLfloat *)&light.fAttenuation );
}


void CGLSLShaderLightManager::SetAmbientLight( const AmbientLight& light )
{
}


void CGLSLShaderLightManager::SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light )
{
	m_HSDirLight = light;

	UpdateHSDirLightUniformVariables( light, m_HSDirLightUniforms );

//	GLenum light_id = m_NumCurrentLights;

//	SetHSDiffuseColors( light.Attribute, light_id );
}


void CGLSLShaderLightManager::SetHemisphericPointLight( const HemisphericPointLight& light )
{
	if( m_NumCurrentHSPointLights == numof(m_HSPointLights) )
		return;

	m_HSPointLights[m_NumCurrentHSPointLights] = light;

	UpdateHSPointLightUniformVariables( light, m_HSPointLightUniforms[m_NumCurrentHSPointLights] );

	m_NumCurrentHSPointLights += 1;

//	GLenum light_id = m_NumCurrentLights;

//	CGLFixedPipelineLightManager::SetPointLight( light );
	
//	SetHSDiffuseColors( light.Attribute, light_id );
}

//	void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	void SetTriPointLight( const CTriPointLight& light ) {}

void CGLSLShaderLightManager::ClearLights()
{
	m_NumCurrentHSPointLights = 0;
}


void CGLSLShaderLightManager::CommitChanges()
{
}


} // namespace amorphous
