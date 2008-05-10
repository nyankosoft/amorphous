#ifndef __LIGHTSTRUCTS_H__
#define __LIGHTSTRUCTS_H__


#include "3DMath/Vector3.h"
#include "FloatRGBColor.h"

#include <string>

class CLight
{
public:

	enum Type
	{
		INVALID = -1,
		AMBIENT = 0,
		ZONE_AMBIENT,
		POINT,
		DIRECTIONAL,
		HEMISPHERIC_POINT,
		HEMISPHERIC_DIRECTIONAL,
		TRI_POINT,
		TRI_DIRECTIONAL,
		NUM_LIGHT_TYPES
	};

public:

	std::string strName;

	SFloatRGBColor Color;

	float fIntensity;

	float fRange;

public:

	inline CLight();

	virtual ~CLight() {}

	virtual Type GetLightType() const = 0;

	virtual SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal ) { return SFloatRGBColor(0,0,0); }
};


class CAmbientLight : public CLight
{
public:
	int iAmbientIdentifier;

	SFloatRGBColor Color;

	CAmbientLight() { iAmbientIdentifier = 0; }

	Type GetLightType() const { return CLight::AMBIENT; }

	virtual SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal ) { return Color; }
};


class CDirectionalLight : public CLight
{
public:
	Vector3 vPseudoPosition;
	Vector3 vDirection;

public:

	inline CDirectionalLight();

	CLight::Type GetLightType() const { return CLight::DIRECTIONAL; }

	virtual SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal )
	{
		float d = Vec3Dot( -vDirection, normal );

		if( 0 < d )
			return Color * d;
		else
			return SFloatRGBColor(0,0,0);
	}
};


class CPointLight : public CLight
{
public:

	Vector3 vPosition;
	char cFallOffType;
	float fAttenuation0;
	float fAttenuation1;
	float fAttenuation2;

	inline CPointLight();

	CLight::Type GetLightType() const { return CLight::POINT; }
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

inline CLight::CLight()
:
Color(SFloatRGBColor(0,0,0)),
fIntensity(0),
fRange(0)
{
}


inline CDirectionalLight::CDirectionalLight()
{
	vDirection      = Vector3(0,0,0);
	vPseudoPosition = Vector3(0,0,0);
}


inline CPointLight::CPointLight()
{
	fRange        = 100;
	fAttenuation0 = 0.01f;
	fAttenuation1 = 0.01f;
	fAttenuation2 = 0.01f;
}


#endif  /*  __LIGHTSTRUCTS_H__  */
