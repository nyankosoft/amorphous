#ifndef __LIGHTSTRUCTS_H__
#define __LIGHTSTRUCTS_H__


#include "3DMath/Vector3.h"
#include "FloatRGBColor.h"

#include <string>
using namespace std;

class CBaseLight
{
public:

	enum eLightType
	{
		INVALID = -1,
		AMBIENT_LIGHT = 0,
		POINT_LIGHT,
		DIRECTIONAL_LIGHT,
		ZONE_AMBIENT_LIGHT,
		NUM_LIGHT_TYPES
	};

protected:

	eLightType m_LightType;

public:
	string strName;

	SFloatRGBColor Color;

	float fIntensity;

	float fRange;

public:

	inline CBaseLight();

	eLightType GetLightType() const { return m_LightType; }
};


class CAmbientLight : public CBaseLight
{
public:
	int iAmbientIdentifier;

	CAmbientLight() { m_LightType = AMBIENT_LIGHT; iAmbientIdentifier = 0; }
};


class CDirectionalLight : public CBaseLight
{
public:
	Vector3 vPseudoPosition;
	Vector3 vDirection;

	inline CDirectionalLight();
};


class CPointLight : public CBaseLight
{
public:

	Vector3 vPosition;
	char cFallOffType;
	float fAttenuation0;
	float fAttenuation1;
	float fAttenuation2;

	inline CPointLight();
};



//----------------------- experimental -----------------------
// type of light which is applied to objects included in an aabb
// introduced experimentally to adjust the indoor brightness

#include "3DMath/AABB3.h"

class CZoneAmbientLight
{
public:

	AABB3 m_AAABB;
};



//========================== inline implementations ==========================

inline CBaseLight::CBaseLight()
:
Color(SFloatRGBColor(0,0,0)),
fIntensity(0),
fRange(0)
{
	m_LightType = INVALID;

}


inline CDirectionalLight::CDirectionalLight()
{
	m_LightType = DIRECTIONAL_LIGHT;

	vDirection      = Vector3(0,0,0);
	vPseudoPosition = Vector3(0,0,0);
}


inline CPointLight::CPointLight()
{
	m_LightType = POINT_LIGHT;

	fRange        = 100;
	fAttenuation0 = 0.01f;
	fAttenuation1 = 0.01f;
	fAttenuation2 = 0.01f;
}


#endif  /*  __LIGHTSTRUCTS_H__  */