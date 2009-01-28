#ifndef  __LightEntity_H__
#define  __LightEntity_H__


#include "Support/prealloc_pool.h"
#include "3DMath/3DStructs.h"
#include "Graphics/HemisphericLight.h"
#include "Graphics/FloatRGBColor.h"
#include "CopyEntity.h"
#include "CopyEntityDesc.h"


class CBE_Light;


class CLightHolder : public pooled_object
{
public:

	CLightHolder() : pLight(NULL) {}

	~CLightHolder() { SafeDelete( pLight ); }

	CLight *pLight;
};


template<class TLight>
class CLightHolderInitializer
{
public:

	/// Called in prealloc_pool<CLightHolder>::init()
	/// to create a pool of light objects.
	void operator()( CLightHolder *pHolder )
	{
		pHolder->pLight = new TLight;
	};
};


class CLightColorVisitor : public CLightVisitor
{
	int m_Index;
	SFloatRGBAColor m_Color;

	void SetHSLightColor( CHemisphericLightAttribute& dest )
	{
		switch( m_Index )
		{
		case 0: dest.UpperDiffuseColor = m_Color; break;
		case 2: dest.LowerDiffuseColor = m_Color; break;
		default:
			break;
		}
	}

public:

	CLightColorVisitor( int index, const SFloatRGBAColor& color )
	:
	m_Index(index),
	m_Color(color)
	{}

//	void VisitLight( CLight& light ) {}
	void VisitAmbientLight( CAmbientLight& ambient_light ) { ambient_light.DiffuseColor = m_Color.GetRGBColor(); }
	void VisitPointLight( CPointLight& point_light ) { point_light.DiffuseColor = m_Color.GetRGBColor(); }
	void VisitDirectionalLight( CDirectionalLight& directional_light ) { directional_light.DiffuseColor = m_Color.GetRGBColor(); }
	void VisitHemisphericPointLight( CHemisphericPointLight& hs_point_light ) { SetHSLightColor( hs_point_light.Attribute ); }
	void VisitHemisphericDirectionalLight( CHemisphericDirectionalLight& hs_directional_light ) { SetHSLightColor( hs_directional_light.Attribute ); }
//	void VisitTriPointLight( CTriPointLight& tri_point_light ) {}
//	void VisitTriDirectionalLight( CTriDirectionalLight& tri_directional_light ) {}
};


class CLightDesc
{
public:

	CLight::Type LightType;

	SFloatRGBAColor aColor[3];

	float fIntensity;

	/// Used by the point lights.
	/// Directional lights also use this to 
	Vector3 vPosition;

	float afAttenuation[3];

	Vector3 vDirection;
};


class CLightEntityDesc : public CCopyEntityDesc
{
public:

	boost::shared_ptr<CLight> pLight;

//	or 

//	CLight *pLightRawPtr;

// or

	// use pose of CCopyEntityDesc to specify light position, direction, etc.
//	CLightDesc Desc;

	CLight::Type LightType;

	float fIntensity;

	SFloatRGBAColor aColor[3];

	float afAttenuation[3]; /// for point lights

	int LightGroup;

public:

	CLightEntityDesc( CLight::Type light_type = CLight::DIRECTIONAL )
		:
	LightType(light_type),
	LightGroup(0),
	fIntensity(1.0f)
	{
		TypeID = CCopyEntityTypeID::LIGHT_ENTITY;
		afAttenuation[0] = 0.1f;
		afAttenuation[1] = 0.1f;
		afAttenuation[2] = 0.1f;
	}
};


class CLightEntity : public CCopyEntity
{
	/// Use 1 or 2 to store the light object

	/// 1. owned reference of light
	boost::shared_ptr<CLight> m_pLight;

	/// 2. hold light object taken from pooled resources
	CLightHolder *m_pLightHolder;

//	CLight::Type m_LightType;

	boost::weak_ptr<CLightEntity> m_pLightEntitySelf;

	CBE_Light *m_pLightBaseEntity;

	float m_fTimeLeft;

//	CLightEntity *m_pNextLight;
//	CLightEntity *m_pPrevLight;
	CLinkNode<CLightEntity> m_LightEntityLink;

//	int m_LightType;
//	int m_Index;

private:

	inline void UnlinkFromLightEntityList();

public:

	CLightEntity();

	~CLightEntity();

	boost::weak_ptr<CLightEntity>& LightEntitySelf() { return  m_pLightEntitySelf; }

	inline void SetPosition( const Vector3& rvPosition );

	void Init( CCopyEntityDesc& desc );

//	inline float *GetColor() { return (float *)&m_Light.Diffuse; }

	CLight::Type GetLightType() const { return m_pLightHolder->pLight->GetLightType(); } 

	inline void SetColor( int index, const SFloatRGBAColor& color );
	inline void SetColor( int index, const SFloatRGBColor& color );

	inline void SetAttenuationFactors( float a0, float a1, float a2 );

//	boost::shared_ptr<CLight>& GetLightObject() { return m_pLight; }
	CLight *GetLightObject() { return m_pLightHolder ? m_pLightHolder->pLight : NULL; }

	bool ReachesEntity( CCopyEntity *pEntity );

	void AddLightIfReachesEntity( CCopyEntity *pEntity );

	inline CLightEntity *GetNextLight() { return m_LightEntityLink.pNext->pOwner; }

	void TerminateDerived();


	///< returns light entity type (static/dynamic)
//	inline int GetLightEntityType() const { return m_LightType; }

//	inline void SetLightFrom( CCopyEntity& rEntity, int light_type	);

	/*inline*/ void SetLight( const CHemisphericPointLight& point_light, int light_type );
	/*inline*/ void SetLight( const CHemisphericDirectionalLight& dir_light, int light_type );

//	inline const CHemisphericPointLight& GetHemisphericLight() { return m_HemisphericLight; }
//	inline const CHemisphericDirectionalLight& GetHemisphericDirLight() { return m_HemisphericDirLight; }

	friend class CEntityFactory;
	friend class CEntityNode;
	friend class CBE_Light;
};


// ================================ inline implementations ================================ 


inline void CLightEntity::UnlinkFromLightEntityList()
{
	m_LightEntityLink.Unlink();
}

/*
inline void CLightEntity::Invalidate()
{
	UnlinkFromLightEntityList();
}
*/

inline void CLightEntity::SetPosition( const Vector3& rvPosition )
{
//	CCopyEntity::SetPosition( rvPosition );
	CCopyEntity::Position() = rvPosition;

	CLight *pLight = GetLightObject();
	if( pLight )
		pLight->SetPosition( rvPosition );
}

/*
inline void CLightEntity::SetDiffuseColor( float *pafColor )
{
	m_Light.Diffuse.r = pafColor[0];
	m_Light.Diffuse.g = pafColor[1];
	m_Light.Diffuse.b = pafColor[2];
	m_Light.Diffuse.a = 1;

	m_HemisphericLight.Attribute.UpperColor.SetRGB( pafColor[0], pafColor[1], pafColor[2] );
	m_HemisphericLight.Attribute.LowerColor = m_HemisphericLight.Attribute.UpperColor * 0.2f;
}

*/
inline void CLightEntity::SetAttenuationFactors( float a0, float a1, float a2 )
{
	CPointLight *pPointLight = dynamic_cast<CPointLight *>(GetLightObject());
	if( pPointLight )
	{
		pPointLight->fAttenuation[0] = a0;
		pPointLight->fAttenuation[1] = a1;
		pPointLight->fAttenuation[2] = a2;
	}
}


inline void CLightEntity::SetColor( int index, const SFloatRGBColor& color )
{
	SetColor( index, SFloatRGBAColor( color.fRed, color.fGreen, color.fBlue, 1.0f ) );
}


inline void CLightEntity::SetColor( int index, const SFloatRGBAColor& color )
{
	CLightColorVisitor visitor( index, color );
	GetLightObject()->Accept( visitor );

	// or

//	m_pLight->SetColor( color, index ); // Not implemented yet
}


/*
class CLightEntityManager
{
public:

	std::map< uint, boost::shared_ptr<CLightEntity> > m_mapEntityIDtoStaticLight;
	std::map< uint, boost::shared_ptr<CLightEntity> > m_mapEntityIDtoDynamicLight;
};
*/



#endif		/*  __LightEntity_H__  */
