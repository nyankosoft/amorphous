#include "GLSLShaderLightManager.hpp"
#include "Graphics/OpenGL/GLExtensions.hpp"


namespace amorphous
{
//#include <boost/filesystem.hpp>

using namespace std;
//using namespace boost;


CGLSLShaderLightManager::CGLSLShaderLightManager()
:
m_VertexShader(0),
m_FragmentShader(0)
{
	memset( m_VSLowerLightColor, 0, sizeof(m_VSLowerLightColor) );
	memset( m_FSLowerLightColor, 0, sizeof(m_FSLowerLightColor) );
}


void CGLSLShaderLightManager::Init()
{
	char var_name[16];
	memset( var_name, 0, sizeof(var_name) );

	int num_lights = 4;
	for( int i=0; i<num_lights; i++ )
	{
		sprintf( var_name, "g_LowerColor%d", i );
//		sprintf( var_name, "g_LowerColor[%d]", i );
		m_VSLowerLightColor[i] = glGetUniformLocation( m_VertexShader,   var_name );
		m_FSLowerLightColor[i] = glGetUniformLocation( m_FragmentShader, var_name );
	}
}


void CGLSLShaderLightManager::SetHSDiffuseColors( const CHemisphericLightAttribute& hs_light, GLenum light_id )
{
	const SFloatRGBAColor& ldc = hs_light.LowerDiffuseColor;
	glUniform4f( m_VSLowerLightColor[light_id], ldc.red, ldc.green, ldc.blue, ldc.alpha );
	glUniform4f( m_FSLowerLightColor[light_id], ldc.red, ldc.green, ldc.blue, ldc.alpha );

//	float rgba[4];
//	FRGBtoFloat4( hs_light.UpperDiffuseColor, rgba );

//	glLightfv( light_id, GL_DIFFUSE,  (GLfloat *)rgba ); // 4 floats
	glLightfv( light_id, GL_DIFFUSE,  (GLfloat *)&(hs_light.UpperDiffuseColor) ); // 4 floats
}


void CGLSLShaderLightManager::SetAmbientLight( const CAmbientLight& light )
{
}


void CGLSLShaderLightManager::SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light )
{
	GLenum light_id = m_NumCurrentLights;

	CGLFixedPipelineLightManager::SetDirectionalLight( light );

	SetHSDiffuseColors( light.Attribute, light_id );
}


void CGLSLShaderLightManager::SetHemisphericPointLight( const CHemisphericPointLight& light )
{
	GLenum light_id = m_NumCurrentLights;

	CGLFixedPipelineLightManager::SetPointLight( light );
	
	SetHSDiffuseColors( light.Attribute, light_id );
}

//	void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	void SetTriPointLight( const CTriPointLight& light ) {}
/*
void CGLSLShaderLightManager::ClearLights(){}
*/

void CGLSLShaderLightManager::CommitChanges()
{
}


} // namespace amorphous
