#ifndef  __HLSLLightManager_H__
#define  __HLSLLightManager_H__


#include <d3dx9.h>
#include "Graphics/Shader/ShaderLightManager.hpp"


class CHLSLShaderLightManager : public CShaderLightManager
{
//	static const int m_iNumMaxLights = 6;	error

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
		NUM_LIGHTING_PROPERTIES
	};

	enum eNumLights
	{
		NUM_MAX_LIGHTS = 8
	};

public:

	CHLSLShaderLightManager();

	CHLSLShaderLightManager( LPD3DXEFFECT pEffect );

	~CHLSLShaderLightManager();

	bool Init();

	bool SetShaderHandles();

	void CommitChanges();

	inline void SetAmbientLight( const CAmbientLight& light );
	inline void SetDirectionalLight( const CDirectionalLight& light );
	inline void SetPointLight( const CPointLight& light );
	inline void SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light );
	inline void SetHemisphericPointLight( const CHemisphericPointLight& light );
//	inline void SetTriDirectionalLight( const CTriDirectionalLight& light );
//	inline void SetTriPointLight( const CTriPointLight& light );

	/// set light to shader variables
	/// user is responsible for calling CommitChanges() after setting the light
	inline void SetLight( const int index, const D3DLIGHT9& rLight );

	/// set light to shader variables
	/// user is responsible for calling CommitChanges() after setting the light
	inline void SetLight( const int index, const CHemisphericPointLight& rLight );

	inline void SetLight( const int index, const CHemisphericDirectionalLight& rLight );

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
	D3DXHANDLE m_aPropertyHandle[NUM_LIGHTING_PROPERTIES];

	/// controls properties of each light
	D3DXHANDLE m_aHandle[NUM_MAX_LIGHTS][NUM_LIGHT_PROPERTY_HANDLES];

	/// copied from shader manager
	/// must be updated when the shader is reloaded
	LPD3DXEFFECT	m_pEffect;

	CLightCache m_LightCache;
};


//================================= inline implementations =================================


inline void CHLSLShaderLightManager::SetLight( const int index, const D3DLIGHT9& rLight )
{
	m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.Ambient, sizeof(D3DCOLORVALUE) );
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


inline void CHLSLShaderLightManager::SetLight( const int index, const CHemisphericPointLight& rLight )
{
	HRESULT hr;
	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_UPPER_DIFFUSE_COLOR], &rLight.Attribute.UpperDiffuseColor, sizeof(float) * 4 );
	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_LOWER_DIFFUSE_COLOR], &rLight.Attribute.LowerDiffuseColor, sizeof(float) * 4 );

	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_POSITION], &rLight.vPosition, sizeof(float) * 3 );

	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_ATTENUATION], rLight.fAttenuation, sizeof(float) * 3 );

	float ambient[4] = {0.02f, 0.02f, 0.02f, 1.00f};
	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &ambient, sizeof(float) * 4 );
//	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.LowerColor, sizeof(float) * 4 );

	float range = 100.0f;
	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_RANGE], &range, sizeof(float) );
}


inline void CHLSLShaderLightManager::SetLight( const int index, const CHemisphericDirectionalLight& rLight )
{
	HRESULT hr;
	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_UPPER_DIFFUSE_COLOR], &rLight.Attribute.UpperDiffuseColor, sizeof(float) * 4 );
	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_LOWER_DIFFUSE_COLOR], &rLight.Attribute.LowerDiffuseColor, sizeof(float) * 4 );

	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_DIRECTION], &rLight.vDirection, sizeof(float) * 3 );

	float ambient[4] = {0.02f, 0.02f, 0.02f, 1.00f};
	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &ambient, sizeof(float) * 4 );
//	hr = m_pEffect->SetValue( m_aHandle[index][LIGHT_AMBIENT_COLOR], &rLight.LowerColor, sizeof(float) * 4 );
}


inline void CHLSLShaderLightManager::SetAmbientLight( const CAmbientLight& light )
{
}


inline void CHLSLShaderLightManager::SetDirectionalLight( const CDirectionalLight& light )
{
	m_LightCache.vecDirecitonalLight.push_back( light );
}


inline void CHLSLShaderLightManager::SetPointLight( const CPointLight& light )
{
	m_LightCache.vecPointLight.push_back( light );
}


inline void CHLSLShaderLightManager::SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light )
{
	m_LightCache.vecHSDirecitonalLight.push_back( light );
}


inline void CHLSLShaderLightManager::SetHemisphericPointLight( const CHemisphericPointLight& light )
{
	m_LightCache.vecHSPointLight.push_back( light );
}


#endif /* __HLSLLightManager_H__ */
