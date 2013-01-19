#ifndef  __LightEntity_H__
#define  __LightEntity_H__


#include "gds/Support/prealloc_pool.hpp"
#include "gds/Graphics/HemisphericLight.hpp"
#include "gds/Graphics/FloatRGBColor.hpp"
#include "CopyEntity.hpp"


namespace amorphous
{


class CBE_Light;
class CLightEntity;


class CLightEntityHandle : public CEntityHandle<CLightEntity>
{
public:

	CLightEntityHandle() {}

	CLightEntityHandle( boost::weak_ptr<CLightEntity> pLightEntity )
		:
	CEntityHandle<CLightEntity>( pLightEntity )
	{}

	// for hemisheric lights
	void SetUpperColor( const SFloatRGBAColor& color );
	void SetLowerColor( const SFloatRGBAColor& color );

	// light type     index 0          1           2
	// -----------------------------------------------------
	// regular              color      -           -
	// hemispheric          upper      lower       -
	// trilight             ?          ?           ?
	void SetColor( int index, const SFloatRGBAColor& color );

	void SetColor( const SFloatRGBAColor& color );

	void SetAttenuationFactors( float a0, float a1, float a2 );
};


class CLightHolder : public pooled_object
{
public:

	CLightHolder() : pLight(NULL) {}

	~CLightHolder() { SafeDelete( pLight ); }

	Light *pLight;
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


class CLightColorVisitor : public LightVisitor
{
	int m_Index;
	SFloatRGBAColor m_Color;

	void SetHSLightColor( HemisphericLightAttribute& dest )
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
	void VisitAmbientLight( AmbientLight& ambient_light ) { ambient_light.DiffuseColor = m_Color.GetRGBColor(); }
	void VisitPointLight( PointLight& point_light ) { point_light.DiffuseColor = m_Color.GetRGBColor(); }
	void VisitDirectionalLight( DirectionalLight& directional_light ) { directional_light.DiffuseColor = m_Color.GetRGBColor(); }
	void VisitHemisphericPointLight( HemisphericPointLight& hs_point_light ) { SetHSLightColor( hs_point_light.Attribute ); }
	void VisitHemisphericDirectionalLight( HemisphericDirectionalLight& hs_directional_light ) { SetHSLightColor( hs_directional_light.Attribute ); }
//	void VisitTriPointLight( CTriPointLight& tri_point_light ) {}
//	void VisitTriDirectionalLight( CTriDirectionalLight& tri_directional_light ) {}
};


class CLightDesc
{
public:

	Light::Type LightType;

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

	boost::shared_ptr<Light> pLight;

//	or 

//	Light *pLightRawPtr;

// or

	// use pose of CCopyEntityDesc to specify light position, direction, etc.
//	CLightDesc Desc;

	Light::Type LightType;

	float fIntensity;

	SFloatRGBAColor aColor[3];

	float afAttenuation[3]; /// (point light & spotlight) attenuation coefficients

	float fInnerConeAngle; /// (spotlight) angle of the inner cone
	float fOuterConeAngle; /// (spotlight) angle of the inner cone

	int LightGroup;

public:

	CLightEntityDesc( Light::Type light_type = Light::DIRECTIONAL )
		:
	LightType(light_type),
	LightGroup(0),
	fInnerConeAngle((float)PI * 0.25f),
	fOuterConeAngle((float)PI * 0.25f),
	fIntensity(1.0f)
	{
		TypeID = CCopyEntityTypeID::LIGHT_ENTITY;
		afAttenuation[0] = 0.1f;
		afAttenuation[1] = 0.1f;
		afAttenuation[2] = 0.1f;
	}

	void SetHSUpperColor( SFloatRGBAColor color ) { aColor[0] = color; }
	void SetHSLowerColor( SFloatRGBAColor color ) { aColor[1] = color; }
};


class CLightEntity : public CCopyEntity
{
	/// Use 1 or 2 to store the light object

	/// 1. owned reference of light
	boost::shared_ptr<Light> m_pLight;

	/// 2. hold light object taken from pooled resources
	CLightHolder *m_pLightHolder;

//	Light::Type m_LightType;

	boost::weak_ptr<CLightEntity> m_pLightEntitySelf;

	CBE_Light *m_pLightBaseEntity;

	float m_fTimeLeft;

//	CLightEntity *m_pNextLight;
//	CLightEntity *m_pPrevLight;
	CLinkNode<CLightEntity> m_LightEntityLink;

//	int m_LightType;

private:

	inline void UnlinkFromLightEntityList();

public:

	CLightEntity();

	~CLightEntity();

	boost::weak_ptr<CLightEntity>& LightEntitySelf() { return  m_pLightEntitySelf; }

	inline void SetPosition( const Vector3& rvPosition );

	void Unlink();

	void LinkDerivedEntity();

	void Init( CCopyEntityDesc& desc );

//	inline float *GetColor() { return (float *)&m_Light.Diffuse; }

	Light::Type GetLightType() const { return m_pLightHolder->pLight->GetLightType(); } 

	inline void SetColor( int index, const SFloatRGBAColor& color );
	inline void SetColor( int index, const SFloatRGBColor& color );

	inline void SetAttenuationFactors( float a0, float a1, float a2 );

//	boost::shared_ptr<Light>& GetLightObject() { return m_pLight; }
	Light *GetLightObject() { return m_pLightHolder ? m_pLightHolder->pLight : NULL; }

	bool ReachesEntity( CCopyEntity *pEntity );

	void AddLightIfReachesEntity( CCopyEntity *pEntity );

	inline CLightEntity *GetNextLight() { return m_LightEntityLink.pNext->pOwner; }

	void TerminateDerived();


	///< returns light entity type (static/dynamic)
//	inline int GetLightEntityType() const { return m_LightType; }

//	inline void SetLightFrom( CCopyEntity& rEntity, int light_type	);

	/*inline*/ void SetLight( const HemisphericPointLight& point_light, int light_type );
	/*inline*/ void SetLight( const HemisphericDirectionalLight& dir_light, int light_type );

//	inline const HemisphericPointLight& GetHemisphericLight() { return m_HemisphericLight; }
//	inline const HemisphericDirectionalLight& GetHemisphericDirLight() { return m_HemisphericDirLight; }

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
	CCopyEntity::SetWorldPosition( rvPosition );

	Light *pLight = GetLightObject();
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
	PointLight *pPointLight = dynamic_cast<PointLight *>(GetLightObject());
	if( pPointLight )
	{
		pPointLight->fAttenuation[0] = a0;
		pPointLight->fAttenuation[1] = a1;
		pPointLight->fAttenuation[2] = a2;
	}
}


inline void CLightEntity::SetColor( int index, const SFloatRGBColor& color )
{
	SetColor( index, SFloatRGBAColor( color.red, color.green, color.blue, 1.0f ) );
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


} // namespace amorphous



#endif		/*  __LightEntity_H__  */
