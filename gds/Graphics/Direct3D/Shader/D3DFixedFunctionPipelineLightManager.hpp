#ifndef  __D3DFixedFunctionPipelineLightManager_HPP__
#define  __D3DFixedFunctionPipelineLightManager_HPP__


#include <d3d9.h>
#include "Graphics/Shader/ShaderLightManager.hpp"
#include "Graphics/Direct3D9.hpp"


class CD3DFixedFunctionPipelineLightManager : public CShaderLightManager
{
//	static const int m_iNumMaxLights = 6;	error

	enum eNumLights
	{
		NUM_MAX_LIGHTS = 8
	};

	int m_NumLights;

public:

	CD3DFixedFunctionPipelineLightManager();

	~CD3DFixedFunctionPipelineLightManager();

	bool Init();

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
//	inline void SetLight( const int index, const CHemisphericPointLight& rLight );

//	inline void SetLight( const int index, const CHemisphericDirectionalLight& rLight );

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

//	void ClearLights() { m_LightCache.Clear(); }
	void ClearLights() { m_NumLights = 0; }

	friend class CShaderLightManager;

private:

//	CLightCache m_LightCache;
};


//================================= inline implementations =================================

inline D3DVECTOR ToD3DVECTOR( const Vector3& src )
{
	D3DVECTOR dest;
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
	return dest;
}

inline D3DCOLORVALUE ToD3DCOLORVALUE( const SFloatRGBColor& src )
{
	D3DCOLORVALUE dest;
	dest.r = src.fRed;
	dest.g = src.fGreen;
	dest.b = src.fBlue;
	dest.a = 1.0f;
	return dest;
}

inline D3DCOLORVALUE ToD3DCOLORVALUE( const SFloatRGBAColor& src )
{
	D3DCOLORVALUE dest;
	dest.r = src.fRed;
	dest.g = src.fGreen;
	dest.b = src.fBlue;
	dest.a = src.fAlpha;
	return dest;
}

/*
inline void CD3DFixedFunctionPipelineLightManager::SetNumDirectionalLights( const int iNumDirectionalLights )
{
}


inline void CD3DFixedFunctionPipelineLightManager::SetDirectionalLightOffset( const int iDirectionalLightOffset )
{
}


inline void CD3DFixedFunctionPipelineLightManager::SetNumPointLights( const int iNumPointLights )
{
}


inline void CD3DFixedFunctionPipelineLightManager::SetPointLightOffset( const int iPointLightOffset )
{
}
*/
/*
inline void CD3DFixedFunctionPipelineLightManager::SetLight( const int index, const CHemisphericPointLight& rLight )
{
}


inline void CD3DFixedFunctionPipelineLightManager::SetLight( const int index, const CHemisphericDirectionalLight& rLight )
{
}
*/

inline void CD3DFixedFunctionPipelineLightManager::SetAmbientLight( const CAmbientLight& light )
{
}


inline void CD3DFixedFunctionPipelineLightManager::SetDirectionalLight( const CDirectionalLight& light )
{
//	m_LightCache.vecDirecitonalLight.push_back( light );
	D3DLIGHT9 d3d_light;
	d3d_light.Type          = D3DLIGHT_DIRECTIONAL;
	d3d_light.Diffuse       = ToD3DCOLORVALUE( light.DiffuseColor );
	d3d_light.Specular      = ToD3DCOLORVALUE( SFloatRGBColor::Black() );
	d3d_light.Ambient       = ToD3DCOLORVALUE( SFloatRGBColor::Black() );
	d3d_light.Position      = ToD3DVECTOR( Vector3(0,0,0) );
	d3d_light.Direction     = ToD3DVECTOR( light.vDirection );
	d3d_light.Range         = 100000.0f;
	d3d_light.Falloff       = 0.0f;
	d3d_light.Attenuation0  = 0.0f; 
	d3d_light.Attenuation1  = 0.0f; 
	d3d_light.Attenuation2  = 0.0f; 
	d3d_light.Theta         = 0.0f;
	d3d_light.Phi           = 0.0f;

	HRESULT hr = S_OK;
	hr = DIRECT3D9.GetDevice()->LightEnable( m_NumLights, TRUE );
	hr = DIRECT3D9.GetDevice()->SetLight( m_NumLights, &d3d_light );
	m_NumLights += 1;
}


inline void CD3DFixedFunctionPipelineLightManager::SetPointLight( const CPointLight& light )
{
//	m_LightCache.vecPointLight.push_back( light );
	D3DLIGHT9 d3d_light;
	d3d_light.Type          = D3DLIGHT_POINT;
	d3d_light.Diffuse       = ToD3DCOLORVALUE( light.DiffuseColor );
	d3d_light.Specular      = ToD3DCOLORVALUE( SFloatRGBColor::Black() );
	d3d_light.Ambient       = ToD3DCOLORVALUE( SFloatRGBColor::Black() );
	d3d_light.Position      = ToD3DVECTOR( light.vPosition );
	d3d_light.Direction     = ToD3DVECTOR( Vector3(0,0,0) );
	d3d_light.Range         = 100000.0f;
	d3d_light.Falloff       = 0.0f;
	d3d_light.Attenuation0  = light.fAttenuation[0]; 
	d3d_light.Attenuation1  = light.fAttenuation[1]; 
	d3d_light.Attenuation2  = light.fAttenuation[2]; 
	d3d_light.Theta         = 0.0f;
	d3d_light.Phi           = 0.0f;

	HRESULT hr = S_OK;
	hr = DIRECT3D9.GetDevice()->LightEnable( m_NumLights, TRUE );
	hr = DIRECT3D9.GetDevice()->SetLight( m_NumLights, &d3d_light );
	m_NumLights += 1;
}


inline void CD3DFixedFunctionPipelineLightManager::SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light )
{
//	m_LightCache.vecHSDirecitonalLight.push_back( light );
}


inline void CD3DFixedFunctionPipelineLightManager::SetHemisphericPointLight( const CHemisphericPointLight& light )
{
//	m_LightCache.vecHSPointLight.push_back( light );
}


#endif /* __D3DFixedFunctionPipelineLightManager_HPP__ */
