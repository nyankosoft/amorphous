#ifndef  __HEMISPHERICLIGHT_H__
#define  __HEMISPHERICLIGHT_H__

#include "3DMath/Vector3.h"
#include "FloatRGBAColor.h"


class CHemisphericLight
{
public:

	Vector3 vPosition;

	SFloatRGBAColor UpperColor;
	SFloatRGBAColor LowerColor;

	float fAttenuation[3];

public:

//	CHemisphericLight();
//	~CHemisphericLight();

	inline void SetAttenuation( float a0, float a1, float a2 )
	{
		fAttenuation[0] = a0;
		fAttenuation[1] = a1;
		fAttenuation[2] = a2;
	}
};



class CHemisphericDirLight
{
public:

	Vector3 vDirection;

	SFloatRGBAColor UpperColor;
	SFloatRGBAColor LowerColor;

public:

//	CHemisphericDirLight();
//	~CHemisphericDirLight();

};


#endif		/*  __HEMISPHERICLIGHT_H__  */
