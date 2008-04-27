#ifndef  __LIGHTENTITY_H__
#define  __LIGHTENTITY_H__


#include "3DMath/3DStructs.h"

#include "3DCommon/HemisphericLight.h"
#include "3DCommon/FloatRGBColor.h"

#include <d3d9.h>
#include <d3dx9.h>


#define CLE_LIGHT_STATIC	1
#define CLE_LIGHT_DYNAMIC	2


class CLightEntity
{
	bool in_use;

	D3DLIGHT9 m_Light;

	CHemisphericLight m_HemisphericLight;
	CHemisphericDirLight m_HemisphericDirLight;

	int m_LightType;
	int m_Index;

	Vector3 m_vPosition;

	/// radius of the sphere which represents the range of point light
	float m_fRadius;

	AABB3 m_aabb;

public:
	CLightEntity *m_pPrevLight;	// used by the link list of 'CLightEntityManager'
	CLightEntity *m_pNextLight;

	CLightEntity *m_pPrev;	// used by the link list attached to entity tree
	CLightEntity *m_pNext;

public:

	inline CLightEntity();
	//~CLightEntity();

	inline D3DLIGHT9& GetLight() { return m_Light; }

	inline const Vector3& GetPosition() const { return m_vPosition; }

	inline float GetRadius() const { return m_fRadius; }

	inline AABB3& GetLocalAABB() { return m_aabb; }

	inline float *GetColor() { return (float *)&m_Light.Diffuse; }

	inline void SetPosition( const Vector3& rvPosition );
	inline void SetDiffuseColor( float *pafColor );	// specify an RGB color with a pointer to an array of 3 floating-point values

	inline  bool IsInUse() { return in_use; }
	inline void UnlinkFromEntityTree();
	inline void Invalidate();

	inline int GetIndex() const { return m_Index; }
	inline void SetIndex(int index) { m_Index = index; }

	///< returns light type (point/directional))
	inline int GetLightType() const { return m_Light.Type; } 

	///< returns light entity type (static/dynamic)
	inline int GetLightEntityType() const { return m_LightType; }

	/*inline*/ void SetLightFrom( CCopyEntity& rEntity, int light_type	);

	/*inline*/ void SetLight( const CHemisphericLight& point_light, int light_type );
	/*inline*/ void SetLight( const CHemisphericDirLight& dir_light, int light_type );

	inline const CHemisphericLight& GetHemisphericLight() { return m_HemisphericLight; }
	inline const CHemisphericDirLight& GetHemisphericDirLight() { return m_HemisphericDirLight; }

	inline void SetColor( int index, const SFloatRGBAColor& color );
	inline void SetColor( int index, const SFloatRGBColor& color );

	inline void SetAttenuationFactors( float a0, float a1, float a2 );

	inline CLightEntity *GetNext() { return m_pNext; }
	inline CLightEntity *GetPrev() { return m_pPrev; }
	inline void SetNext( CLightEntity *pNext ) { m_pNext = pNext; }
	inline void SetPrev( CLightEntity *pPrev ) { m_pPrev = pPrev; }

	inline CLightEntity *GetNextLight() { return m_pNextLight; }
};


// ================================ inline implementations ================================ 

inline CLightEntity::CLightEntity()
{
	in_use = false;

	m_fRadius = 0.0f;
	m_aabb.Nullify();

	m_pPrevLight = NULL;
	m_pNextLight = NULL;

	m_pPrev = NULL;
	m_pNext = NULL;
}


inline void CLightEntity::UnlinkFromEntityTree()
{
	if( m_pNext )
		m_pNext->m_pPrev = m_pPrev;

	if( m_pPrev )
		m_pPrev->m_pNext = m_pNext;

	m_pNext = NULL;
	m_pPrev = NULL;
}


inline void CLightEntity::Invalidate()
{
	in_use = false;
	m_Index = -1;

	UnlinkFromEntityTree();

	// unlink from the link list in light entity manager
	if( m_pNextLight )
		m_pNextLight->m_pPrevLight = m_pPrevLight;

	if( m_pPrevLight )
		m_pPrevLight->m_pNextLight = m_pNextLight;

	m_pNextLight = NULL;
	m_pPrevLight = NULL;

}


inline void CLightEntity::SetPosition( const Vector3& rvPosition )
{
	m_vPosition = rvPosition;
	m_Light.Position = (D3DVECTOR)m_vPosition;

	m_HemisphericLight.vPosition = m_vPosition;
}


inline void CLightEntity::SetDiffuseColor( float *pafColor )
{
	m_Light.Diffuse.r = pafColor[0];
	m_Light.Diffuse.g = pafColor[1];
	m_Light.Diffuse.b = pafColor[2];
	m_Light.Diffuse.a = 1;

	m_HemisphericLight.UpperColor.SetRGB( pafColor[0], pafColor[1], pafColor[2] );
	m_HemisphericLight.LowerColor = m_HemisphericLight.UpperColor * 0.2f;
}


inline void CLightEntity::SetAttenuationFactors( float a0, float a1, float a2 )
{
	m_HemisphericLight.fAttenuation[0] = a0;
	m_HemisphericLight.fAttenuation[1] = a1;
	m_HemisphericLight.fAttenuation[2] = a2;
}


inline void CLightEntity::SetColor( int index, const SFloatRGBColor& color )
{
	SetColor( index, SFloatRGBAColor( color.fRed, color.fGreen, color.fBlue, 1.0f ) );
}


inline void CLightEntity::SetColor( int index, const SFloatRGBAColor& color )
{
	switch( index )
	{
	case 0:
		m_HemisphericDirLight.UpperColor = color;
		m_HemisphericLight.UpperColor = color;
		break;
	case 1:
		break;
	case 2:
		m_HemisphericDirLight.LowerColor = color;
		m_HemisphericLight.LowerColor = color;
		break;
	default:
		break;
	}

}


#endif		/*  __LIGHTENTITY_H__  */
