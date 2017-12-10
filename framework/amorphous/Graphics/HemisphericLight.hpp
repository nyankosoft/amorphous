#ifndef  __HEMISPHERICLIGHT_H__
#define  __HEMISPHERICLIGHT_H__

#include "../3DMath/Vector3.hpp"
#include "FloatRGBColor.hpp"
#include "LightStructs.hpp"


namespace amorphous
{

inline SFloatRGBColor ToRGBColor( const SFloatRGBAColor& src )
{
	return SFloatRGBColor( src.red, src.green, src.blue );
}


class HemisphericLightAttribute : public IArchiveObjectBase
{
public:

	SFloatRGBAColor UpperDiffuseColor;
	SFloatRGBAColor LowerDiffuseColor;

public:

	HemisphericLightAttribute()
		:
	UpperDiffuseColor( SFloatRGBAColor::Black() ),
	LowerDiffuseColor( SFloatRGBAColor::Black() )
	{}

	HemisphericLightAttribute( const SFloatRGBAColor& upper_diffuse_color, const SFloatRGBAColor& lower_diffuse_color )
		:
	UpperDiffuseColor( upper_diffuse_color ),
	LowerDiffuseColor( lower_diffuse_color )
	{}

	/**
	 * \brief 
	 *
	 * Alpha values are set to 1.0f for both upper and lower colors.
	 */
	inline void SetColorsGrayscale( float upper, float lower )
	{
		UpperDiffuseColor = SFloatRGBAColor(upper,upper,upper,1.0f);
		LowerDiffuseColor = SFloatRGBAColor(lower,lower,lower,1.0f);
	}

	inline SFloatRGBColor CalcHSLightAmount( float d )
	{
		return ToRGBColor( UpperDiffuseColor * d + LowerDiffuseColor * ( 1.0f - d ) );
	}

	virtual void Serialize( IArchive& ar, const unsigned int version ) { ar & UpperDiffuseColor & LowerDiffuseColor; }
};

/*
class HemisphericLight : public Light
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

	HemisphericLight() {}
	virtual ~HemisphericLight() {}
};
*/

class HemisphericPointLight : public PointLight
{
public:

	HemisphericLightAttribute Attribute;

public:

//	HemisphericPointLight();
//	~HemisphericPointLight();

	Type GetLightType() const { return Light::HEMISPHERIC_POINT; }

	SFloatRGBColor CalcPointLightFactor( float NdotL )
	{
		return Attribute.CalcHSLightAmount( ( NdotL + 1.0f ) * 0.5f );
	}

	void SetDiffuseColor( int index, SFloatRGBColor& diffuse_color ) { SetDiffuseColor( index, diffuse_color ); }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( LightVisitor& visitor ) { visitor.VisitHemisphericPointLight( *this ); }

	SFloatRGBAColor& UpperColor() { return Attribute.UpperDiffuseColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerDiffuseColor; }
};



class HemisphericDirectionalLight : public DirectionalLight
{
public:

	HemisphericLightAttribute Attribute;

public:

	HemisphericDirectionalLight() {}

	HemisphericDirectionalLight( const Vector3& direction, const SFloatRGBAColor& upper_diffuse_color, const SFloatRGBAColor& lower_diffuse_color )
	{
		vDirection = direction;
		Attribute.UpperDiffuseColor = upper_diffuse_color;
		Attribute.LowerDiffuseColor = lower_diffuse_color;
	}

//	~HemisphericDirectionalLight();

	Type GetLightType() const { return Light::HEMISPHERIC_DIRECTIONAL; }

	inline SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal )
	{
		float d = ( Vec3Dot( -vDirection, normal ) + 1.0f ) * 0.5f;
		return Attribute.CalcHSLightAmount( d );
	}

	void SetDiffuseColor( int index, SFloatRGBColor& diffuse_color ) { SetDiffuseColor( index, diffuse_color ); }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( LightVisitor& visitor ) { visitor.VisitHemisphericDirectionalLight( *this ); }

	SFloatRGBAColor& UpperColor() { return Attribute.UpperDiffuseColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerDiffuseColor; }
};



class HemisphericSpotlight : public Spotlight
{
public:

	HemisphericLightAttribute Attribute;

public:

	Type GetLightType() const { return Light::HEMISPHERIC_SPOTLIGHT; }

	SFloatRGBColor CalcSpotlightColor( float NdotL )
	{
		return Attribute.CalcHSLightAmount( ( NdotL + 1.0f ) * 0.5f );
	}

	void SetDiffuseColor( int index, SFloatRGBColor& diffuse_color ) { SetDiffuseColor( index, diffuse_color ); }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( LightVisitor& visitor ) { visitor.VisitHemisphericSpotlight( *this ); }

	SFloatRGBAColor& UpperColor() { return Attribute.UpperDiffuseColor; }
	SFloatRGBAColor& LowerColor() { return Attribute.LowerDiffuseColor; }
};


// ================================ inline implementations ================================ 

inline void HemisphericPointLight::Serialize( IArchive& ar, const unsigned int version )
{
	PointLight::Serialize( ar, version );

	ar & Attribute;
}


inline void HemisphericDirectionalLight::Serialize( IArchive& ar, const unsigned int version )
{
	DirectionalLight::Serialize( ar, version );

	ar & Attribute;
}


inline void HemisphericSpotlight::Serialize( IArchive& ar, const unsigned int version )
{
	Spotlight::Serialize( ar, version );

	ar & Attribute;
}

} // namespace amorphous



#endif		/*  __HEMISPHERICLIGHT_H__  */
