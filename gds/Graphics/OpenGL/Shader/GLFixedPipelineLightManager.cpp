#include "GLFixedPipelineLightManager.hpp"
#include <GL/gl.h>


namespace amorphous
{

using namespace std;


inline void FRGBtoFloat4( const SFloatRGBColor& src, float *dest )
{
	dest[0] = src.red;
	dest[1] = src.green;
	dest[2] = src.blue;
	dest[3] = 1.0f;
}


inline void FRGBAtoFloat4( const SFloatRGBAColor& src, float *dest )
{
	dest[0] = src.red;
	dest[1] = src.green;
	dest[2] = src.blue;
	dest[3] = src.alpha;
}


void SetBaseLight( const CLight& light, GLenum light_id )
{
//	SFloatRGBAColor dc( light.DiffuseColor );
//	dc.MultiplyRGB( light.fIntensity );
	const SFloatRGBColor& dc = light.DiffuseColor;
	float f = light.fIntensity;
	float rgba[4];

	FRGBtoFloat4( light.DiffuseColor, rgba );
	rgba[0] *= f;
	rgba[1] *= f;
	rgba[2] *= f;

	glLightfv( light_id, GL_DIFFUSE,  (GLfloat *)rgba ); // 4 floats


	FRGBAtoFloat4( /*light.AmbientColor*/SFloatRGBAColor::Black(), rgba );
	glLightfv( light_id, GL_AMBIENT, (GLfloat *)rgba );

	FRGBAtoFloat4( /*light.SpecularColor*/SFloatRGBAColor::Black(), rgba );
	glLightfv( light_id, GL_SPECULAR, (GLfloat *)rgba );

	glEnable( light_id );
}


CGLFixedPipelineLightManager::CGLFixedPipelineLightManager()
:
m_NumCurrentLights(0)
{
}


void CGLFixedPipelineLightManager::SetAmbientLight( const CAmbientLight& light )
{
}


void CGLFixedPipelineLightManager::SetDirectionalLight( const CDirectionalLight& light )
{
	if( GL_MAX_LIGHTS <= m_NumCurrentLights )
		return;

	GLenum light_id = GL_LIGHT0 + m_NumCurrentLights;

	SetBaseLight( light, light_id );
	float dir[4] = { light.vDirection.x, light.vDirection.y, light.vDirection.z, 0.0f };
	glLightfv( light_id, GL_POSITION, (GLfloat *)dir );

	m_NumCurrentLights++;
}


void CGLFixedPipelineLightManager::SetPointLight( const CPointLight& light )
{
	if( GL_MAX_LIGHTS <= m_NumCurrentLights )
		return;

	GLenum light_id = GL_LIGHT0 + m_NumCurrentLights;

	SetBaseLight( light, light_id );

	float pos[4] = { light.vPosition.x, light.vPosition.y, light.vPosition.z, 1.0f };
	glLightfv( light_id, GL_POSITION, (GLfloat *)pos );

	glLightf( light_id, GL_CONSTANT_ATTENUATION,  light.fAttenuation[0] );
	glLightf( light_id, GL_LINEAR_ATTENUATION,    light.fAttenuation[1] );
	glLightf( light_id, GL_QUADRATIC_ATTENUATION, light.fAttenuation[2] );	

	m_NumCurrentLights++;
}


void CGLFixedPipelineLightManager::SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light )
{
	SetDirectionalLight( light );
}


void CGLFixedPipelineLightManager::SetHemisphericPointLight( const CHemisphericPointLight& light )
{
//	SetHSLight
//	glLightfv( light, GL_DIFFUSE,  (GLfloat *)&upper_rgba ); // 4 floats
//	glLightfv( light, GL_DIFFUSE,  (GLfloat *)lower_&rgba ); // 4 floats

	SetPointLight( light );
}

//	void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	void SetTriPointLight( const CTriPointLight& light ) {}

void CGLFixedPipelineLightManager::ClearLights()
{
	m_NumCurrentLights = 0;

	// Disable all the lights
	// Lights are enabled in CGLFixedPipelineLightManager::SetBaseLight()
	// every time a new light is added.
	for( int i=0; i<GL_MAX_LIGHTS; i++ )
		glDisable( GL_LIGHT0 + i );
}


} // namespace amorphous
