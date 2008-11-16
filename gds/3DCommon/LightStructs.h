#ifndef __LIGHTSTRUCTS_H__
#define __LIGHTSTRUCTS_H__


#include <float.h>
#include <string>

#include "fwd.h"
#include "3DMath/Vector3.h"
#include "FloatRGBColor.h"

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
#include "Support/Serialization/Serialization_Color.h"
using namespace GameLib1::Serialization;


class CLightVisitor
{
public:

	virtual void VisitLight( CLight& light ) {}
	virtual void VisitAmbientLight( CAmbientLight& ambient_light ) {}
	virtual void VisitPointLight( CPointLight& point_light ) {}
	virtual void VisitDirectionalLight( CDirectionalLight& directional_light ) {}
	virtual void VisitHemisphericPointLight( CHemisphericPointLight& hs_point_light ) {}
	virtual void VisitHemisphericDirectionalLight( CHemisphericDirectionalLight& hs_directional_light ) {}
//	virtual void VisitTriPointLight( CTriPointLight& tri_point_light ) {}
//	virtual void VisitTriDirectionalLight( CTriDirectionalLight& tri_directional_light ) {}
};


class CLight : public IArchiveObjectBase
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

	virtual Vector3 GetPosition() const { return Vector3(0,0,0); }

	virtual Vector3 GetDirection() const { return Vector3(0,-1,0); }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitLight( *this ); }
};


class CAmbientLight : public CLight
{
public:

//	int iAmbientIdentifier;

public:

	CAmbientLight() { /*iAmbientIdentifier = 0;*/ }

	Type GetLightType() const { return CLight::AMBIENT; }

	virtual SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal ) { return Color; }

//	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitAmbientLight( *this ); }
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

	virtual Vector3 GetPosition() const { return vPseudoPosition; }

	virtual Vector3 GetDirection() const { return vDirection; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitDirectionalLight( *this ); }
};


class CPointLight : public CLight
{
public:

	Vector3 vPosition;
	float fAttenuation[3];

	int FalloffType;

public:

	inline CPointLight();

	virtual CLight::Type GetLightType() const { return CLight::POINT; }

	virtual SFloatRGBColor CalcLightAmount( const Vector3& pos, const Vector3& normal )
	{
		Vector3 vToLight = vPosition - pos;
		float dist_to_light = Vec3Length( vToLight );
		Vector3 vDirToLight = vToLight / dist_to_light;

		if( fRange < dist_to_light )
			return SFloatRGBColor(0,0,0);

		float NdotL = Vec3Dot( normal, vDirToLight );

		return CalcPointLightFactor( NdotL ) * fIntensity * CalcAttenuation( dist_to_light );
	}

	virtual Vector3 GetPosition() const { return vPosition; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );


	virtual SFloatRGBColor CalcPointLightFactor( float NdotL )
	{
		if( 0.0f < NdotL )
		{
			return Color * NdotL;
		}
		else
			return SFloatRGBColor(0,0,0);
	}

	inline float CalcAttenuation( float dist_to_light )
	{
		return 1.0f / ( fAttenuation[0] + fAttenuation[1] * dist_to_light + fAttenuation[2] * dist_to_light * dist_to_light );
	}

	inline void SetAttenuation( float a0, float a1, float a2 )
	{
		fAttenuation[0] = a0;
		fAttenuation[1] = a1;
		fAttenuation[2] = a2;
	}

	virtual void Accept( CLightVisitor& visitor ) { visitor.VisitPointLight( *this ); }
};



//------------------------------- ZoneAmbientLight (experimental) -------------------------------
// A light which is applied to objects included in an aabb
// - introduced experimentally to adjust the indoor brightness

#include "3DMath/AABB3.h"

class CZoneAmbientLight : public CLight
{
public:

	AABB3 m_AAABB;
};



//========================== inline implementations ==========================

inline CLight::CLight()
:
Color(SFloatRGBColor(0,0,0)),
fIntensity(1.0f),
fRange(FLT_MAX)
{
}


inline void CLight::Serialize( IArchive& ar, const unsigned int version )
{
	ar & strName;
	ar & Color;
	ar & fIntensity;
	ar & fRange;
}


inline CDirectionalLight::CDirectionalLight()
{
	vDirection      = Vector3(0,-1,0);
	vPseudoPosition = Vector3(0,0,0);
}


inline void CDirectionalLight::Serialize( IArchive& ar, const unsigned int version )
{
	CLight::Serialize( ar, version );

	ar & vPseudoPosition;
	ar & vDirection;
}


inline CPointLight::CPointLight()
:
vPosition(Vector3(0,0,0))
{
	FalloffType     = 0;
	fAttenuation[0] = 0.00f;
	fAttenuation[1] = 0.01f;
	fAttenuation[2] = 0.00f;
}


inline void CPointLight::Serialize( IArchive& ar, const unsigned int version )
{
	CLight::Serialize( ar, version );

	ar & vPosition;
	ar & FalloffType;

	for( int i=0; i<3; i++ )
		ar & fAttenuation[i];
}


#endif  /*  __LIGHTSTRUCTS_H__  */
