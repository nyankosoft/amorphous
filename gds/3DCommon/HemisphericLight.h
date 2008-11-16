#ifndef  __HEMISPHERICLIGHT_H__
#define  __HEMISPHERICLIGHT_H__

#include "3DMath/Vector3.h"
#include "FloatRGBColor.h"
#include "LightStructs.h"

inline SFloatRGBColor ToRGBColor( const SFloatRGBAColor& src )
{
	return SFloatRGBColor( src.fRed, src.fGreen, src.fBlue );
}


class CHemisphericLightAttribute : public IArchiveObjectBase
{
public:

	SFloatRGBAColor UpperColor;
	SFloatRGBAColor LowerColor;

public:

	inline SFloatRGBColor CalcHSLightAmount( float d )
	{
		return ToRGBColor( UpperColor * d + LowerColor * ( 1.0f - d ) );
	}

	virtual void Serialize( IArchive& ar, const unsigned int version ) { ar & UpperColor & LowerColor; }
};

/*
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
*/

class CHemisphericPointLight : public CPointLight
{
public:

	CHemisphericLightAttribute Attribute;

public:

//	CHemisphericPointLight();
//	~CHemisphericPointLight();

	Type GetLightType() const { return CLight::HEMISPHERIC_POINT; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitHemisphericPointLight( *this ); }

	SFloatRGBColor CalcPointLightFactor( float NdotL )
	{
		return Attribute.CalcHSLightAmount( ( NdotL + 1.0f ) * 0.5f );
	}

	SFloatRGBAColor& UpperColor() { return Attribute.UpperColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerColor; }
};



class CHemisphericDirectionalLight : public CDirectionalLight
{
public:

	CHemisphericLightAttribute Attribute;

public:

//	CHemisphericDirectionalLight();
//	~CHemisphericDirectionalLight();

	Type GetLightType() const { return CLight::HEMISPHERIC_DIRECTIONAL; }

	inline SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal )
	{
		float d = ( Vec3Dot( -vDirection, normal ) + 1.0f ) * 0.5f;
		return Attribute.CalcHSLightAmount( d );
	}

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitHemisphericDirectionalLight( *this ); }

	SFloatRGBAColor& UpperColor() { return Attribute.UpperColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerColor; }
};


// ================================ inline implementations ================================ 

inline void CHemisphericPointLight::Serialize( IArchive& ar, const unsigned int version )
{
	CPointLight::Serialize( ar, version );

	ar & Attribute;
}


inline void CHemisphericDirectionalLight::Serialize( IArchive& ar, const unsigned int version )
{
	CDirectionalLight::Serialize( ar, version );

	ar & Attribute;
}


#endif		/*  __HEMISPHERICLIGHT_H__  */
