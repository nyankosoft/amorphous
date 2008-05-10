#ifndef  __HEMISPHERICLIGHT_H__
#define  __HEMISPHERICLIGHT_H__

#include "3DMath/Vector3.h"
#include "FloatRGBColor.h"
#include "LightStructs.h"


class CHemisphericLight : public CLight
{
public:

	SFloatRGBColor UpperColor;
	SFloatRGBColor LowerColor;

protected:

	inline SFloatRGBColor CalcHSLightAmount( float d )
	{
		return UpperColor * d + LowerColor * ( 1.0f - d );
	}

public:

	CHemisphericLight() {}
	virtual ~CHemisphericLight() {}
};


class CHemisphericPointLight : public CHemisphericLight
{
public:

	Vector3 vPosition;

	float fAttenuation[3];

public:

//	CHemisphericPointLight();
//	~CHemisphericPointLight();

	inline void SetAttenuation( float a0, float a1, float a2 )
	{
		fAttenuation[0] = a0;
		fAttenuation[1] = a1;
		fAttenuation[2] = a2;
	}

	Type GetLightType() const { return CLight::HEMISPHERIC_POINT; }

	inline SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal )
	{
		return SFloatRGBColor(0,0,0);
	}
};



class CHemisphericDirectionalLight : public CHemisphericLight
{
public:

	Vector3 vDirection;

public:

//	CHemisphericDirLight();
//	~CHemisphericDirLight();

	Type GetLightType() const { return CLight::HEMISPHERIC_DIRECTIONAL; }

	inline SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal )
	{
		float d = ( Vec3Dot( -vDirection, normal ) + 1.0f ) * 0.5f;
		return CalcHSLightAmount( d );
	}
};


#endif		/*  __HEMISPHERICLIGHT_H__  */
