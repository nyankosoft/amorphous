
#include "PointLight.h"
#include "../Graphics/Direct3D9.h"

void CPointLight::SetLight(int i)
{
	DIRECT3D9.GetDevice()->SetLight(i, &m_PointLight);
}

/*
void CPointLight::Enable();
{
	DIRECT3D9.GetDevice()->LightEnable( index, &m_PointLight);
}*/