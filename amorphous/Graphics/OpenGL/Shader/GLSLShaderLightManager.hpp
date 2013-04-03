#ifndef __GLSLShaderLightManager_HPP__
#define __GLSLShaderLightManager_HPP__


#include "GLFixedPipelineLightManager.hpp"
#include <GL/gl.h>


namespace amorphous
{


class CGLSLShaderLightManager : public CGLFixedPipelineLightManager
{
	int m_VertexShader;
	int m_FragmentShader;

	int m_VSLowerLightColor[8];
	int m_FSLowerLightColor[8];

	void SetHSDiffuseColors( const HemisphericLightAttribute& hs_light, GLenum light_id );

public:

	CGLSLShaderLightManager();
	~CGLSLShaderLightManager() {}

	void Init();

	void SetAmbientLight( const AmbientLight& light );

	// Use the implementation of CGLFixedPipelineLightManager
//	virtual void SetDirectionalLight( const DirectionalLight& light );
//	virtual void SetPointLight( const PointLight& light );

	virtual void SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light );
	virtual void SetHemisphericPointLight( const HemisphericPointLight& light );

//	virtual void SetTriDirectionalLight( const TriDirectionalLight& light ) {}
//	virtual void SetTriPointLight( const TriPointLight& light ) {}

//	virtual void ClearLights();

	void SetVertexShader( int vs ) { m_VertexShader = vs; }
	void SetFragmentShader( int fs ) { m_FragmentShader = fs; }

	virtual void CommitChanges();

};


} // namespace amorphous



#endif	/*  __GLSLShaderLightManager_HPP__  */
