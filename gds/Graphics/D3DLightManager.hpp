#ifndef  __D3DLightManager_H__
#define  __D3DLightManager_H__


#include "fwd.hpp"
#include "ShaderLightManager.hpp"


namespace amorphous
{


inline D3DVECTOR ToD3DVECTOR( const Vector3& src )
{
	return D3DVECTOR( src.x, src.y, src.z );
}

inline D3DCOLORVALUE ToD3DCOLORVALUE( const SFloatRGBAColor& src )
{
	return D3DCOLORVALUE( 
}


typedef struct D3DLIGHT9 {
    D3DLIGHTTYPE Type;
    D3DCOLORVALUE Diffuse;
    D3DCOLORVALUE Specular;
    D3DCOLORVALUE Ambient;
    D3DVECTOR Position;
    D3DVECTOR Direction;
    float Range;
    float Falloff;
    float Attenuation0;
    float Attenuation1;
    float Attenuation2;
    float Theta;
    float Phi;
} D3DLIGHT9, *LPD3DLIGHT;


class CD3DLightManager : public CShaderLightManager
{
	void SetD3DLight( const D3DLIGHT9& light );

public:

	void SetAmbientLight( const CAmbientLight& light );
	void SetDirectionalLight( const CDirectionalLight& light );
	void SetPointLight( const CPointLight& light );
//	void SetSpotlight( const CSpotlight& light )
	void SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light );
	void SetHemisphericPointLight( const CHemisphericPointLight& light );
//	void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	void SetTriPointLight( const CTriPointLight& light ) {}
	void SetLight( const CLight& light );
	
	void CommitChanges();
};

void CD3DLightManager::CommitChanges()
{/*
	int d3d_light_index = 0;
	D3DLIGHT9 light;

	for(;;)
	{
		switch(  )
		light.Position = ;
		
	}*/
}

void CD3DLightManager::SetD3DLight( const D3DLIGHT9& light )
{
}


void CD3DLightManager::SetAmbientLight( const CAmbientLight& light );
{
}

void CD3DLightManager::SetDirectionalLight( const CDirectionalLight& light );
{
	D3DLIGHT9 light;
	light.Type      = DIRECTIONAL;
	light.Direction = ToD3DVECTOR(light.vDirection);
	light.Diffuse   = ToD3DCOLORVALUE( light.Color * light.fIntensity);
//	light.Specular  = ;
//	light.Ambient   = ;

	SetD3DLight( light );
}

void CD3DLightManager::SetPointLight( const CPointLight& light );
{
	D3DLIGHT9 light;
	light.Type         = POINT;
	light.Position     = ToD3DVECTOR(light.vPosition);
	light.Diffuse      = ToD3DCOLORVALUE( light.Color * light.fIntensity );
	light.Attenuation0 = light.fAttenuation[0];
	light.Attenuation1 = light.fAttenuation[1];
	light.Attenuation2 = light.fAttenuation[2];
	light.Range        = light.fRange;
	SetD3DLight( light );
}  

/*
void CD3DLightManager::SetSpotlight( const CSpotlight& light )
{
}
*/

void CD3DLightManager::SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light );
{
}

void CD3DLightManager::SetHemisphericPointLight( const CHemisphericPointLight& light );
{
}

void CD3DLightManager::SetLight( const CLight& light );
{
}

} // amorphous



#endif /* __D3DLightManager_H__ */