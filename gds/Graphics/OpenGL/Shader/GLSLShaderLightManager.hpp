#ifndef __GLSLShaderLightManager_HPP__
#define __GLSLShaderLightManager_HPP__


#include "GLFixedPipelineLightManager.hpp"
#include <GL/gl.h>


class CGLSLShaderLightManager : public CGLFixedPipelineLightManager
{
	int m_VertexShader;
	int m_FragmentShader;

	int m_VSLowerLightColor[8];
	int m_FSLowerLightColor[8];

	void SetHSDiffuseColors( const CHemisphericLightAttribute& hs_light, GLenum light_id );

public:

	CGLSLShaderLightManager();
	~CGLSLShaderLightManager() {}

	void Init();

	void SetAmbientLight( const CAmbientLight& light );

	// Use the implementation of CGLFixedPipelineLightManager
//	virtual void SetDirectionalLight( const CDirectionalLight& light );
//	virtual void SetPointLight( const CPointLight& light );

	virtual void SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light );
	virtual void SetHemisphericPointLight( const CHemisphericPointLight& light );

//	virtual void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	virtual void SetTriPointLight( const CTriPointLight& light ) {}

//	virtual void ClearLights();

	void SetVertexShader( int vs ) { m_VertexShader = vs; }
	void SetFragmentShader( int fs ) { m_FragmentShader = fs; }

	virtual void CommitChanges();

};



#endif	/*  __GLSLShaderLightManager_HPP__  */
