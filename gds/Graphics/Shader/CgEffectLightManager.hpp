#ifndef  __CgEffectLightManager_HPP__
#define  __CgEffectLightManager_HPP__


#include "Graphics/Shader/ShaderLightManager.hpp"
#include <Cg/cg.h>


namespace amorphous
{


class CCgEffectLightManager : public CShaderLightManager
{
//	static const int m_iNumMaxLights = 6;	error

	enum HandleType
	{
		LIGHT_POSITION = 0,
		LIGHT_DIRECTION,
		LIGHT_AMBIENT_COLOR,
		LIGHT_DIFFUSE_COLOR,
		LIGHT_RANGE,
		LIGHT_ATTENUATION,
		LIGHT_ATTENUATION0,
		LIGHT_ATTENUATION1,
		LIGHT_ATTENUATION2,

		LIGHT_UPPER_DIFFUSE_COLOR,
		LIGHT_LOWER_DIFFUSE_COLOR,

		NUM_LIGHT_PROPERTY_HANDLES
	};

	enum LightPropertyHandle
	{
		LPH_NUM_DIRECTIONAL_LIGHTS,
		LPH_DIRECTIONAL_LIGHT_OFFSET,
		LPH_NUM_POINT_LIGHTS,
		LPH_POINT_LIGHT_OFFSET,
		NUM_LIGHTING_PROPERTIES
	};

	enum NumLights
	{
		NUM_MAX_LIGHTS = 8
	};

public:

	CCgEffectLightManager();

	CCgEffectLightManager( CGeffect cg_effect );

	~CCgEffectLightManager();

	bool Init();

	bool SetShaderHandles();

	void CommitChanges();

	inline void SetAmbientLight( const AmbientLight& light );
	inline void SetDirectionalLight( const DirectionalLight& light );
	inline void SetPointLight( const PointLight& light );
	inline void SetSpotlight( const Spotlight& light );
	inline void SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light );
	inline void SetHemisphericPointLight( const HemisphericPointLight& light );
	inline void SetHemisphericSpotlight( const HemisphericSpotlight& light );
//	inline void SetTriDirectionalLight( const TriDirectionalLight& light );
//	inline void SetTriPointLight( const TriPointLight& light );

	/// set light to shader variables
	/// user is responsible for calling CommitChanges() after setting the light
//	inline void SetLight( const int index, const D3DLIGHT9& rLight );

	/// set light to shader variables
	/// user is responsible for calling CommitChanges() after setting the light
	inline void SetLight( const int index, const HemisphericPointLight& rLight );

	inline void SetLight( const int index, const HemisphericDirectionalLight& rLight );

	/// set the number of directional lights
	/// user is responsible for calling CommitChanges() after this call
	inline void SetNumDirectionalLights( const int iNumDirectionalLights );

	inline void SetDirectionalLightOffset( const int iDirectionalLightOffset );

	/// set the number of point lights
	/// user is responsible for calling CommitChanges() after this call
	inline void SetNumPointLights( const int iNumPointLights );

	inline void SetPointLightOffset( const int iPointLightOffset );

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void ReleaseGraphicsResources();

	void ClearLights() { m_LightCache.Clear(); }

	friend class CShaderLightManager;

private:

	/// controls overall configurations of lights (e.g. the number of directional/point lights)
	CGparameter m_aPropertyHandle[NUM_LIGHTING_PROPERTIES];

	/// controls properties of each light
	CGparameter m_aHandle[NUM_MAX_LIGHTS][NUM_LIGHT_PROPERTY_HANDLES];

	/// copied from shader manager
	/// must be updated when the shader is reloaded
	CGeffect m_CgEffect;

	CLightCache m_LightCache;
};


//================================= inline implementations =================================


//inline void CCgEffectLightManager::SetLight( const int index, const D3DLIGHT9& rLight ) {}


inline void CCgEffectLightManager::SetNumDirectionalLights( const int iNumDirectionalLights )
{
	cgSetParameter1i( m_aPropertyHandle[LPH_NUM_DIRECTIONAL_LIGHTS], iNumDirectionalLights );
}


inline void CCgEffectLightManager::SetDirectionalLightOffset( const int iDirectionalLightOffset )
{
	cgSetParameter1i( m_aPropertyHandle[LPH_DIRECTIONAL_LIGHT_OFFSET], iDirectionalLightOffset );
}


inline void CCgEffectLightManager::SetNumPointLights( const int iNumPointLights )
{
	cgSetParameter1i( m_aPropertyHandle[LPH_NUM_POINT_LIGHTS], iNumPointLights );
}


inline void CCgEffectLightManager::SetPointLightOffset( const int iPointLightOffset )
{
	cgSetParameter1i( m_aPropertyHandle[LPH_POINT_LIGHT_OFFSET], iPointLightOffset );
}


inline void CCgEffectLightManager::SetLight( const int index, const HemisphericPointLight& rLight )
{
	cgSetParameter4fv( m_aHandle[index][LIGHT_UPPER_DIFFUSE_COLOR], (const float *)&rLight.Attribute.UpperDiffuseColor );
	cgSetParameter4fv( m_aHandle[index][LIGHT_LOWER_DIFFUSE_COLOR], (const float *)&rLight.Attribute.LowerDiffuseColor );

	cgSetParameter3fv( m_aHandle[index][LIGHT_POSITION], (const float *)&rLight.vPosition );

	cgSetParameter3fv( m_aHandle[index][LIGHT_ATTENUATION], rLight.fAttenuation );

	float ambient[4] = {0.02f, 0.02f, 0.02f, 1.00f};
	cgSetParameter4fv( m_aHandle[index][LIGHT_AMBIENT_COLOR], ambient );
//	cgSetParameter4fv( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.LowerColor );

	float range = 100.0f;
	cgSetParameter1f( m_aHandle[index][LIGHT_RANGE], range );
}


inline void CCgEffectLightManager::SetLight( const int index, const HemisphericDirectionalLight& rLight )
{
	cgSetParameter4fv( m_aHandle[index][LIGHT_UPPER_DIFFUSE_COLOR], (const float *)&rLight.Attribute.UpperDiffuseColor );
	cgSetParameter4fv( m_aHandle[index][LIGHT_LOWER_DIFFUSE_COLOR], (const float *)&rLight.Attribute.LowerDiffuseColor );

	cgSetParameter3fv( m_aHandle[index][LIGHT_DIRECTION], (const float *)&rLight.vDirection );

	float ambient[4] = {0.02f, 0.02f, 0.02f, 1.00f};
	cgSetParameter4fv( m_aHandle[index][LIGHT_AMBIENT_COLOR], ambient );
//	cgSetParameter4fv( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.LowerColor );
}


inline void CCgEffectLightManager::SetAmbientLight( const AmbientLight& light )
{
}


inline void CCgEffectLightManager::SetDirectionalLight( const DirectionalLight& light )
{
	m_LightCache.vecDirecitonalLight.push_back( light );
}


inline void CCgEffectLightManager::SetSpotlight( const Spotlight& light )
{
	m_LightCache.vecSpotlight.push_back( light );
}


inline void CCgEffectLightManager::SetPointLight( const PointLight& light )
{
	m_LightCache.vecPointLight.push_back( light );
}


inline void CCgEffectLightManager::SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light )
{
	m_LightCache.vecHSDirecitonalLight.push_back( light );
}


inline void CCgEffectLightManager::SetHemisphericPointLight( const HemisphericPointLight& light )
{
	m_LightCache.vecHSPointLight.push_back( light );
}


inline void CCgEffectLightManager::SetHemisphericSpotlight( const HemisphericSpotlight& light )
{
	m_LightCache.vecHSSpotlight.push_back( light );
}


} // namespace amorphous



#endif /* __CgEffectLightManager_HPP__ */
