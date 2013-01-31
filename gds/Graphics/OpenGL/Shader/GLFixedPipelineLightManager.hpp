#ifndef __GLFixedPipelineLightManager_HPP__
#define __GLFixedPipelineLightManager_HPP__

#include "../../Shader/ShaderLightManager.hpp"


namespace amorphous
{


class CGLFixedPipelineLightManager : public ShaderLightManager
{
protected:

	int m_NumCurrentLights;

public:

	CGLFixedPipelineLightManager();

	virtual ~CGLFixedPipelineLightManager() {}

	virtual void SetAmbientLight( const AmbientLight& light );

	void SetDirectionalLight( const DirectionalLight& light );
	void SetPointLight( const PointLight& light );

	virtual void SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light );
	virtual void SetHemisphericPointLight( const HemisphericPointLight& light );
//	virtual void SetTriDirectionalLight( const TriDirectionalLight& light ) {}
//	virtual void SetTriPointLight( const TriPointLight& light ) {}

	virtual void ClearLights();

	virtual void CommitChanges() {}

};


} // namespace amorphous



#endif /* __GLFixedPipelineLightManager_HPP__ */
