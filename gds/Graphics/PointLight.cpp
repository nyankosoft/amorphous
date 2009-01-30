
#include "PointLight.hpp"
#include "../Graphics/Direct3D9.hpp"

void CPointLight::SetLight(int i)
{
	DIRECT3D9.GetDevice()->SetLight(i, &m_PointLight);
}

/*
void CPointLight::Enable();
{
	DIRECT3D9.GetDevice()->LightEnable( index, &m_PointLight);
}*/