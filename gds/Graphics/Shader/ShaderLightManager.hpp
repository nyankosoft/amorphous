#ifndef  __ShaderLightManager_H__
#define  __ShaderLightManager_H__


#include "../fwd.hpp"
#include "../HemisphericLight.hpp"
#include "../../Support/FixedVector.hpp"


namespace amorphous
{


class CLightCache
{
public:
	fixed_vector<DirectionalLight,8> vecDirecitonalLight;
	fixed_vector<PointLight,8> vecPointLight;
	fixed_vector<Spotlight,8> vecSpotlight;
	fixed_vector<HemisphericDirectionalLight,8> vecHSDirecitonalLight;
	fixed_vector<HemisphericPointLight,8> vecHSPointLight;
	fixed_vector<HemisphericSpotlight,8> vecHSSpotlight;

public:

	void Clear()
	{
		vecDirecitonalLight.resize(0);
		vecPointLight.resize(0);
		vecSpotlight.resize(0);
		vecHSDirecitonalLight.resize(0);
		vecHSPointLight.resize(0);
		vecHSSpotlight.resize(0);
	}
};


class ShaderLightManager
{
public:

	ShaderLightManager() {}
	virtual ~ShaderLightManager() {}

	virtual void SetAmbientLight( const AmbientLight& light ) {}
	virtual void SetDirectionalLight( const DirectionalLight& light ) {}
	virtual void SetPointLight( const PointLight& light ) {}
	virtual void SetSpotlight( const Spotlight& light ) {}
	virtual void SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light ) {}
	virtual void SetHemisphericPointLight( const HemisphericPointLight& light ) {}
	virtual void SetHemisphericSpotlight( const HemisphericSpotlight& light ) {}
//	virtual void SetTriDirectionalLight( const TriDirectionalLight& light ) {}
//	virtual void SetTriPointLight( const TriPointLight& light ) {}

	virtual void ClearLights() {}

	virtual void CommitChanges() {}

};


class ShaderLightParamsWriter : public LightVisitor
{
	ShaderLightManager *m_pShaderLightManager;

public:

	ShaderLightParamsWriter( ShaderLightManager *pMgr )
	:
	m_pShaderLightManager(pMgr)
	{}

//	void VisitLight( Light& light ) {}
	void VisitAmbientLight( AmbientLight& ambient_light ) { m_pShaderLightManager->SetAmbientLight( ambient_light ); }
	void VisitPointLight( PointLight& point_light )       { m_pShaderLightManager->SetPointLight( point_light ); }
	void VisitDirectionalLight( DirectionalLight& directional_light )        { m_pShaderLightManager->SetDirectionalLight( directional_light ); }
	void VisitHemisphericPointLight( HemisphericPointLight& hs_point_light ) { m_pShaderLightManager->SetHemisphericPointLight( hs_point_light ); }
	void VisitHemisphericDirectionalLight( HemisphericDirectionalLight& hs_directional_light ) { m_pShaderLightManager->SetHemisphericDirectionalLight(  hs_directional_light ); }
//	void VisitTriPointLight( TriPointLight& tri_point_light ) { m_pShaderLightManager->Set?Light(  ); }
//	void VisitTriDirectionalLight( TriDirectionalLight& tri_directional_light ) { m_pShaderLightManager->Set?Light(  ); }
};

} // namespace amorphous



#endif		/*  __ShaderLightManager_H__  */
