#ifndef __GLSLShaderLightManager_HPP__
#define __GLSLShaderLightManager_HPP__


#include "GLFixedPipelineLightManager.hpp"
#include "amorphous/Support/Macro.h"
#include <GL/gl.h>


namespace amorphous
{


class GLSLLight
{
public:

	GLint m_DiffuseColors[3];

	GLint m_Position; ///< for point lights

	GLint m_Attenuation;

	GLint m_Direction; ///< for directional lights

	GLSLLight()
		:
	m_Position(-1),
	m_Attenuation(-1),
	m_Direction(-1)
	{
		for( int i=0; i<numof(m_DiffuseColors); i++ )
			m_DiffuseColors[i] = -1;
	}
};


class CGLSLShaderLightManager : public CGLFixedPipelineLightManager
{
	GLuint m_Program;

	int m_VertexShader;
	int m_FragmentShader;

	int m_VSLowerLightColor[8];
	int m_FSLowerLightColor[8];

	HemisphericDirectionalLight m_HSDirLight;

	GLSLLight m_HSDirLightUniforms;

	HemisphericPointLight m_HSPointLights[4];

	GLSLLight m_HSPointLightUniforms[4];

	uint m_NumCurrentHSPointLights;

private:

	void SetHSDiffuseColors( const HemisphericLightAttribute& hs_light, GLenum light_id );

	void UpdateHSDirLightUniformVariables( const HemisphericDirectionalLight& light, const GLSLLight& glsl_light );

	void UpdateHSPointLightUniformVariables( const HemisphericPointLight& light, const GLSLLight& glsl_light );

public:

	CGLSLShaderLightManager();

	~CGLSLShaderLightManager() {}

	void Init( GLuint program );

	void SetAmbientLight( const AmbientLight& light );

	// Use the implementation of CGLFixedPipelineLightManager
//	void SetDirectionalLight( const DirectionalLight& light );
//	void SetPointLight( const PointLight& light );

	void SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light );
	void SetHemisphericPointLight( const HemisphericPointLight& light );

//	void SetTriDirectionalLight( const TriDirectionalLight& light ) {}
//	void SetTriPointLight( const TriPointLight& light ) {}

	void ClearLights();

	void SetVertexShader( int vs ) { m_VertexShader = vs; }
	void SetFragmentShader( int fs ) { m_FragmentShader = fs; }

	virtual void CommitChanges();

};


} // namespace amorphous



#endif	/*  __GLSLShaderLightManager_HPP__  */
