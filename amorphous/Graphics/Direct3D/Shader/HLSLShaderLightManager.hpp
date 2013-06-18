#ifndef  __HLSLLightManager_H__
#define  __HLSLLightManager_H__


#include <d3dx9.h>
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Macro.h"


namespace amorphous
{


class HLSLLight
{
public:

	D3DXHANDLE m_DiffuseColors[3];

	D3DXHANDLE m_AmbientColor;

	HLSLLight()
		:
	m_AmbientColor(0)
	{
		for( int i=0; i<numof(m_DiffuseColors); i++ )
			m_DiffuseColors[i] = 0;
	}

	virtual ~HLSLLight() {}
};


class HLSLDirectionalLight : public HLSLLight
{
public:
	D3DXHANDLE m_Direction;

	HLSLDirectionalLight()
		:
	m_Direction(0)
	{}
};


class HLSLPointLight : public HLSLLight
{
public:
	D3DXHANDLE m_Position;

	D3DXHANDLE m_Attenuation;

	HLSLPointLight()
		:
	m_Position(0),
	m_Attenuation(0)
	{}
};


class CHLSLShaderLightManager : public ShaderLightManager
{
	enum eHandleType
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

	enum eLightPropertyHandle
	{
		LPH_NUM_DIRECTIONAL_LIGHTS,
		LPH_DIRECTIONAL_LIGHT_OFFSET,
		LPH_NUM_POINT_LIGHTS,
		LPH_POINT_LIGHT_OFFSET,
		LPH_NUM_SPOTLIGHTS,
		LPH_SPOTLIGHT_OFFSET,
		NUM_LIGHTING_PROPERTIES
	};

public:

	CHLSLShaderLightManager();

	CHLSLShaderLightManager( LPD3DXEFFECT pEffect );

	~CHLSLShaderLightManager();

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
	inline void SetLight( const int index, const D3DLIGHT9& rLight );

	/// set light to shader variables
	/// user is responsible for calling CommitChanges() after setting the light
	inline void SetLight( const int index, const HemisphericPointLight& rLight );

	inline void SetLight( const int index, const HemisphericDirectionalLight& rLight );

	inline void SetLight( const int index, const HemisphericSpotlight& rLight );

	/// set the number of directional lights
	/// user is responsible for calling CommitChanges() after this call
	inline void SetNumDirectionalLights( const int iNumDirectionalLights );

	inline void SetDirectionalLightOffset( const int iDirectionalLightOffset );

	/// set the number of point lights
	/// user is responsible for calling CommitChanges() after this call
	inline void SetNumPointLights( const int iNumPointLights );

	inline void SetPointLightOffset( const int iPointLightOffset );

	void LoadGraphicsResources( const GraphicsParameters& rParam );

	void ReleaseGraphicsResources();

	void ClearLights() { m_LightCache.Clear(); }

	friend class ShaderLightManager;

private:

	/// controls overall configurations of lights (e.g. the number of directional/point lights)
	D3DXHANDLE m_aPropertyHandle[NUM_LIGHTING_PROPERTIES];

	/// \brief controls properties of each light
	/// e.g. m_aHandle[i][LIGHT_POSITION] == the position of the i-th light.
//	std::vector<D3DXHANDLE> m_aHandle[NUM_LIGHT_PROPERTY_HANDLES];

	std::vector<HLSLDirectionalLight> m_HSDirectionalLights;

	std::vector<HLSLPointLight> m_HSPointLights;

	/// copied from shader manager
	/// must be updated when the shader is reloaded
	LPD3DXEFFECT	m_pEffect;

	CLightCache m_LightCache;
};


//================================= inline implementations =================================


inline void CHLSLShaderLightManager::SetLight( const int index, const D3DLIGHT9& rLight )
{
/*	m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.Ambient, sizeof(D3DCOLORVALUE) );
	m_pEffect->SetValue( m_aHandle[index][LIGHT_DIFFUSE_COLOR], &rLight.Diffuse, sizeof(D3DCOLORVALUE) );

	if( rLight.Type == D3DLIGHT_POINT )
	{
		m_pEffect->SetValue( m_aHandle[index][LIGHT_RANGE], &rLight.Range, sizeof(float) );
		m_pEffect->SetValue( m_aHandle[index][LIGHT_POSITION], &rLight.Position, sizeof(D3DVECTOR) );

		float attenuation[3];
		attenuation[0] = rLight.Attenuation0;
		attenuation[1] = rLight.Attenuation1;
		attenuation[2] = rLight.Attenuation2;
		m_pEffect->SetValue( m_aHandle[index][LIGHT_ATTENUATION], attenuation, sizeof(float) * 3 );

//		m_pEffect->SetValue( m_aHandle[index][LIGHT_ATTENUATION0], &rLight.Attenuation0, sizeof(float) );
//		m_pEffect->SetValue( m_aHandle[index][LIGHT_ATTENUATION1], &rLight.Attenuation1, sizeof(float) );
//		m_pEffect->SetValue( m_aHandle[index][LIGHT_ATTENUATION2], &rLight.Attenuation2, sizeof(float) );
	}
	else if( rLight.Type == D3DLIGHT_DIRECTIONAL )
	{
		m_pEffect->SetValue( m_aHandle[index][LIGHT_DIRECTION], &rLight.Direction, sizeof(D3DVECTOR) );
	}
*/
}


inline void CHLSLShaderLightManager::SetNumDirectionalLights( const int iNumDirectionalLights )
{
	HRESULT hr;
	hr = m_pEffect->SetValue( m_aPropertyHandle[LPH_NUM_DIRECTIONAL_LIGHTS], &iNumDirectionalLights, sizeof(int) );
}


inline void CHLSLShaderLightManager::SetDirectionalLightOffset( const int iDirectionalLightOffset )
{
	HRESULT hr;
	hr = m_pEffect->SetValue( m_aPropertyHandle[LPH_DIRECTIONAL_LIGHT_OFFSET], &iDirectionalLightOffset, sizeof(int) );
}


inline void CHLSLShaderLightManager::SetNumPointLights( const int iNumPointLights )
{
	HRESULT hr;
	hr = m_pEffect->SetValue( m_aPropertyHandle[LPH_NUM_POINT_LIGHTS], &iNumPointLights, sizeof(int) );
}


inline void CHLSLShaderLightManager::SetPointLightOffset( const int iPointLightOffset )
{
	HRESULT hr;
	hr = m_pEffect->SetValue( m_aPropertyHandle[LPH_POINT_LIGHT_OFFSET], &iPointLightOffset, sizeof(int) );
}


inline void CHLSLShaderLightManager::SetLight( const int index, const HemisphericPointLight& rLight )
{
	if( (int)m_HSPointLights.size() <= index )
		return;

	HRESULT hr;
	hr = m_pEffect->SetValue( m_HSPointLights[index].m_DiffuseColors[0], &rLight.Attribute.UpperDiffuseColor, sizeof(float) * 4 );
	hr = m_pEffect->SetValue( m_HSPointLights[index].m_DiffuseColors[1], &rLight.Attribute.LowerDiffuseColor, sizeof(float) * 4 );

	hr = m_pEffect->SetValue( m_HSPointLights[index].m_Position, &rLight.vPosition, sizeof(float) * 3 );

	hr = m_pEffect->SetValue( m_HSPointLights[index].m_Attenuation, rLight.fAttenuation, sizeof(float) * 3 );

	float ambient[4] = {0.02f, 0.02f, 0.02f, 1.00f};
	hr = m_pEffect->SetFloatArray( m_HSPointLights[index].m_AmbientColor, ambient, 4 );
//	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.LowerColor, sizeof(float) * 4 );

//	float range = 100.0f;
//	hr = m_pEffect->SetFloat( m_HSPointLights[index][LIGHT_RANGE], range );
}


inline void CHLSLShaderLightManager::SetLight( const int index, const HemisphericDirectionalLight& rLight )
{
	if( (int)m_HSDirectionalLights.size() <= index )
		return;

	HRESULT hr;
	hr = m_pEffect->SetValue( m_HSDirectionalLights[index].m_DiffuseColors[0], &rLight.Attribute.UpperDiffuseColor, sizeof(float) * 4 );
	hr = m_pEffect->SetValue( m_HSDirectionalLights[index].m_DiffuseColors[1], &rLight.Attribute.LowerDiffuseColor, sizeof(float) * 4 );

	hr = m_pEffect->SetValue( m_HSDirectionalLights[index].m_Direction, &rLight.vDirection, sizeof(float) * 3 );

	float ambient[4] = {0.02f, 0.02f, 0.02f, 1.00f};
	hr = m_pEffect->SetFloatArray( m_HSDirectionalLights[index].m_AmbientColor, ambient, 4 );
//	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.LowerColor, sizeof(float) * 4 );
}


inline void CHLSLShaderLightManager::SetLight( const int index, const HemisphericSpotlight& rLight )
{
	LOG_PRINT_ERROR( " Not implemented yet." );

/*	if( (int)m_HSSpotlight.size() <= index )
		return;

	HRESULT hr;
	hr = m_pEffect->SetValue( m_HSSpotlight[index].m_DiffuseColors[0], &rLight.Attribute.UpperDiffuseColor, sizeof(float) * 4 );
	hr = m_pEffect->SetValue( m_HSSpotlight[index].m_DiffuseColors[1], &rLight.Attribute.LowerDiffuseColor, sizeof(float) * 4 );

	hr = m_pEffect->SetValue( m_HSSpotlight[index][LIGHT_DIRECTION], &rLight.vDirection, sizeof(float) * 3 );

	float ambient[4] = {0.02f, 0.02f, 0.02f, 1.00f};
	hr = m_pEffect->SetFloatArray( m_HSSpotlight[index].m_AmbientColor, ambient, 4 );
//	hr = m_pEffect->SetValue( m_HSSpotlight[index].m_AmbientColor, &rLight.LowerColor, sizeof(float) * 4 );
*/
}


inline void CHLSLShaderLightManager::SetAmbientLight( const AmbientLight& light )
{
}


inline void CHLSLShaderLightManager::SetDirectionalLight( const DirectionalLight& light )
{
	m_LightCache.vecDirecitonalLight.push_back( light );
}


inline void CHLSLShaderLightManager::SetPointLight( const PointLight& light )
{
	m_LightCache.vecPointLight.push_back( light );
}


inline void CHLSLShaderLightManager::SetSpotlight( const Spotlight& light )
{
	m_LightCache.vecSpotlight.push_back( light );
}


inline void CHLSLShaderLightManager::SetHemisphericDirectionalLight( const HemisphericDirectionalLight& light )
{
	m_LightCache.vecHSDirecitonalLight.push_back( light );
}


inline void CHLSLShaderLightManager::SetHemisphericPointLight( const HemisphericPointLight& light )
{
	m_LightCache.vecHSPointLight.push_back( light );
}


inline void CHLSLShaderLightManager::SetHemisphericSpotlight( const HemisphericSpotlight& light )
{
	m_LightCache.vecHSSpotlight.push_back( light );
}


} // namespace amorphous



#endif /* __HLSLLightManager_H__ */
