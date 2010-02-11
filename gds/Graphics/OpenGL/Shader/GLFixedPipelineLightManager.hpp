#ifndef __GLFixedPipelineLightManager_HPP__
#define __GLFixedPipelineLightManager_HPP__

#include "../../Shader/ShaderLightManager.hpp"


class CGLFixedPipelineLightManager : public CShaderLightManager
{
protected:

	int m_NumCurrentLights;

public:

	CGLFixedPipelineLightManager();

	virtual ~CGLFixedPipelineLightManager() {}

	virtual void SetAmbientLight( const CAmbientLight& light );

	void SetDirectionalLight( const CDirectionalLight& light );
	void SetPointLight( const CPointLight& light );

	virtual void SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light );
	virtual void SetHemisphericPointLight( const CHemisphericPointLight& light );
//	virtual void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	virtual void SetTriPointLight( const CTriPointLight& light ) {}

	virtual void ClearLights();

	virtual void CommitChanges() {}

};



#endif /* __GLFixedPipelineLightManager_HPP__ */
