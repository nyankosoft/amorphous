#ifndef  __POINTLIGHT_H__
#define  __POINTLIGHT_H__

#include <vector>
using namespace std;

#include <d3d9.h>
#include <d3dx9.h>


class CPointLight
{
	D3DLIGHT9 m_PointLight;

public:
	CPointLight()
	{
		ZeroMemory(&m_PointLight,sizeof(D3DLIGHT9));
		m_PointLight.Type = D3DLIGHT_POINT;
		m_PointLight.Attenuation1 = 1.0f;
	}

	void SetPosition(D3DXVECTOR3 vPosition) { m_PointLight.Position = vPosition; }
	void SetRange(float fRange) { m_PointLight.Range = fRange; }
	void SetDiffuseColor(float r, float g, float b) { m_PointLight.Diffuse.r = r; m_PointLight.Diffuse.g = g; m_PointLight.Diffuse.b = b; }
	void SetAmbientColor(float r, float g, float b) { m_PointLight.Ambient.r = r; m_PointLight.Ambient.g = g; m_PointLight.Ambient.b = b; }
	void SetAttenuationConstants(float fAttenuation0, float fAttenuation1, float fAttenuation2)
	{
		m_PointLight.Attenuation0 = fAttenuation0;
		m_PointLight.Attenuation1 = fAttenuation1;
		m_PointLight.Attenuation2 = fAttenuation2;
	}

	void SetPositionRangeAttenuation();

	void SetLight(int i);

};


#endif		/*  __POINTLIGHT_H__  */