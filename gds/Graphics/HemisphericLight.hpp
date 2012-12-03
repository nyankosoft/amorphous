#ifndef  __HEMISPHERICLIGHT_H__
#define  __HEMISPHERICLIGHT_H__

#include "../3DMath/Vector3.hpp"
#include "FloatRGBColor.hpp"
#include "LightStructs.hpp"

inline SFloatRGBColor ToRGBColor( const SFloatRGBAColor& src )
{
	return SFloatRGBColor( src.red, src.green, src.blue );
}


class CHemisphericLightAttribute : public IArchiveObjectBase
{
public:

	SFloatRGBAColor UpperDiffuseColor;
	SFloatRGBAColor LowerDiffuseColor;

public:

	CHemisphericLightAttribute()
		:
	UpperDiffuseColor( SFloatRGBAColor::Black() ),
	LowerDiffuseColor( SFloatRGBAColor::Black() )
	{}

	CHemisphericLightAttribute( const SFloatRGBAColor& upper_diffuse_color, const SFloatRGBAColor& lower_diffuse_color )
		:
	UpperDiffuseColor( upper_diffuse_color ),
	LowerDiffuseColor( lower_diffuse_color )
	{}

	inline SFloatRGBColor CalcHSLightAmount( float d )
	{
		return ToRGBColor( UpperDiffuseColor * d + LowerDiffuseColor * ( 1.0f - d ) );
	}

	virtual void Serialize( IArchive& ar, const unsigned int version ) { ar & UpperDiffuseColor & LowerDiffuseColor; }
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

	SFloatRGBColor CalcPointLightFactor( float NdotL )
	{
		return Attribute.CalcHSLightAmount( ( NdotL + 1.0f ) * 0.5f );
	}

	void SetDiffuseColor( int index, SFloatRGBColor& diffuse_color ) { SetDiffuseColor( index, diffuse_color ); }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitHemisphericPointLight( *this ); }

	SFloatRGBAColor& UpperColor() { return Attribute.UpperDiffuseColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerDiffuseColor; }
};



class CHemisphericDirectionalLight : public CDirectionalLight
{
public:

	CHemisphericLightAttribute Attribute;

public:

	CHemisphericDirectionalLight() {}

	CHemisphericDirectionalLight( const Vector3& direction, const SFloatRGBAColor& upper_diffuse_color, const SFloatRGBAColor& lower_diffuse_color )
	{
		vDirection = direction;
		Attribute.UpperDiffuseColor = upper_diffuse_color;
		Attribute.LowerDiffuseColor = lower_diffuse_color;
	}

//	~CHemisphericDirectionalLight();

	Type GetLightType() const { return CLight::HEMISPHERIC_DIRECTIONAL; }

	inline SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal )
	{
		float d = ( Vec3Dot( -vDirection, normal ) + 1.0f ) * 0.5f;
		return Attribute.CalcHSLightAmount( d );
	}

	void SetDiffuseColor( int index, SFloatRGBColor& diffuse_color ) { SetDiffuseColor( index, diffuse_color ); }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitHemisphericDirectionalLight( *this ); }

	SFloatRGBAColor& UpperColor() { return Attribute.UpperDiffuseColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerDiffuseColor; }
};



class CHemisphericSpotlight : public CSpotlight
{
public:

	CHemisphericLightAttribute Attribute;

public:

	Type GetLightType() const { return CLight::HEMISPHERIC_SPOTLIGHT; }

	SFloatRGBColor CalcSpotlightColor( float NdotL )
	{
		return Attribute.CalcHSLightAmount( ( NdotL + 1.0f ) * 0.5f );
	}

	void SetDiffuseColor( int index, SFloatRGBColor& diffuse_color ) { SetDiffuseColor( index, diffuse_color ); }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitHemisphericSpotlight( *this ); }

	SFloatRGBAColor& UpperColor() { return Attribute.UpperDiffuseColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerDiffuseColor; }
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


inline void CHemisphericSpotlight::Serialize( IArchive& ar, const unsigned int version )
{
	CSpotlight::Serialize( ar, version );

	ar & Attribute;
}


#endif		/*  __HEMISPHERICLIGHT_H__  */
