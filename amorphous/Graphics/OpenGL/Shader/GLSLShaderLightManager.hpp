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

	GLSLLight()
	{
		for( int i=0; i<numof(m_DiffuseColors); i++ )
			m_DiffuseColors[i] = -1;
	}
};


class GLSLDirectionalLight : public GLSLLight
{
public:
	GLint m_Direction;

	GLSLDirectionalLight()
		:
	m_Direction(-1)
	{}
};


class GLSLPointLight : public GLSLLight
{
public:
	GLint m_Position;

	GLint m_Attenuation;

	GLSLPointLight()
		:
	m_Position(-1),
	m_Attenuation(-1)
	{}
};


class CGLSLShaderLightManager : public CGLFixedPipelineLightManager
{
	GLuint m_Program;

	int m_VertexShader;
	int m_FragmentShader;

	int m_VSLowerLightColor[8];
	int m_FSLowerLightColor[8];

	HemisphericDirectionalLight m_HSDirLight;

	std::vector<GLSLDirectionalLight> m_HSDirLightUniforms;

	std::vector<GLSLPointLight> m_HSPointLightUniforms;

	std::vector<HemisphericDirectionalLight> m_HSDirectionalLights;
	std::vector<HemisphericPointLight> m_HSPointLights;
	std::vector<HemisphericSpotlight> m_HSSpotlights;

//	std::vector<Vector3> m_HSDL_Directions;
//	std::vector<SFloatRGBAColor> m_HSDL_UpperDiffuseColors;
//	std::vector<SFloatRGBAColor> m_HSDL_LowerDiffuseColors;

//	std::vector<Vector3> m_HSPL_Positions;
//	std::vector<SFloatRGBAColor> m_HSPL_UpperDiffuseColors;
//	std::vector<SFloatRGBAColor> m_HSPL_LowerDiffuseColors;
//	std::vector<Vector3> m_HSPL_Attenuations;

//	std::vector<Vector3> m_HSSL_Directions;
//	std::vector<SFloatRGBAColor> m_HSSL_UpperDiffuseColors;
//	std::vector<SFloatRGBAColor> m_HSSL_LowerDiffuseColors;

	uint m_NumMaxHSDirectionalLights;
	uint m_NumMaxHSPointLights;
	uint m_NumMaxHSSpotlights;

	enum MiscUniforms
	{
		NUM_HS_DIRECTIONAL_LIGHTS,
		NUM_HS_POINT_LIGHTS,
		NUM_HS_SPOTLIGHTS,
		NUM_MISC_UNIFORMS
	};

	GLint m_MiscUniforms[NUM_MISC_UNIFORMS];

//	uint m_NumCurrentHSDirectionalLights;
//	uint m_NumCurrentHSPointLights;
//	uint m_NumCurrentHSSpotlights;

	Matrix44 m_ViewMatrix;

private:

	void SetHSDiffuseColors( const HemisphericLightAttribute& hs_light, GLenum light_id );

	void UpdateHSDirLightUniformVariables( const HemisphericDirectionalLight& light, const GLSLDirectionalLight& glsl_light );

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

	void SetViewTransform( const Matrix44& view ) { m_ViewMatrix = view; }
};


} // namespace amorphous



#endif	/*  __GLSLShaderLightManager_HPP__  */
