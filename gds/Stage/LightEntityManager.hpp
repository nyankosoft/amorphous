#ifndef  __LIGHTENTITYMANAGER_H__
#define  __LIGHTENTITYMANAGER_H__

#include <vector>
using namespace std;

#include "../Graphics/GraphicsDevice.hpp"
#include "../Stage/CopyEntity.hpp"

#include "../Graphics/Shader/ShaderLightManager.hpp"

#include "LightEntity.hpp"


namespace amorphous
{


extern int g_iNumDynamicLights;

// control the lighting for entities

#define CLEM_NUM_MAX_STATIC_LIGHTS	128
#define CLEM_NUM_MAX_DYNAMIC_LIGHTS	32	// this is also used as an offset value for indeices to dynamic light entities


#define CLEM_POINT_LIGHT_OFFSET	1

/**
 * manages 2 types of lights, static lights & dynamic lights
 *
 * static light
 *  - cannot move
 *  - applied to entities, but not to static geometry
 *  - static lighting to static geometry is suppoed to be done through lightmapping
 *  - cannot not be removed once registered
 *  - usually created when the stage is loaded and stays at the original position until the player leaves the stage
 *
 * dynamic light
 *  - movable
 *  - applied to both entities and static geometry
 *  - can be created & destroyed at any time in the stage
 */
class CLightEntityManager : public CLightEntity
{
private:

	CEntitySet *m_pEntitySet;

	ShaderLightManager *m_pShaderLightManager;

	LPD3DXEFFECT m_pEffect;

//	vector<CLightEntity> m_vecStaticLightEntity;
	CLightEntity m_aStaticLightEntity[CLEM_NUM_MAX_STATIC_LIGHTS];
	int m_iNumStaticLights;

//	vector<CLightEntity> m_vecDynamicLightEntity;
	CLightEntity m_aDynamicLightEntity[CLEM_NUM_MAX_DYNAMIC_LIGHTS];
	int m_iNumDynamicLights;

	CLightEntity m_DynamicLightHead;

	int m_iNumPrevLights;


private:

	void InitShaderLightManager();

	/// find a new light entity
	/// and returns the index and the pointer to the light entity
	void GetNewLightEntity( int light_type, int& index, CLightEntity*& pLightEntity );

public:

	CLightEntityManager();
	~CLightEntityManager();

	void Init( CEntitySet* pEntitySet );

	/// add new light in the stage
	/// Light properties such as position, direction & color are taken from copy entity
	/// given as an the argument
	/// TOOD: see to it hat v1 & v2 of the copy entity will be used as upper & lower colors of the hemispheric light
	/// returns index to the registered light
	int RegisterLight( CCopyEntity& rLightEntity, int light_type );

	int RegisterHemisphericPointLight( HemisphericPointLight& light, int light_type );

	int RegisterHemisphericDirLight( HemisphericDirectionalLight& dir_light, int light_type );

	/// updates the position of the dynamic point light
	void UpdateLightPosition( int light_index, Vector3& rvNewPosition );

	/// remove a dynamic light from the scene
	inline void DeleteLight( int light_index, int light_type );

	inline void SetLightsForEntity( CCopyEntity *pEntity );

	inline CLightEntity *GetLightEntity( int light_index );

	int GetNumDynamicLights() { return m_iNumDynamicLights; }
//	CLightEntity *GetDynamicLight( int iIndex ) { return &m_aDynamicLightEntity[iIndex]; }
	CLightEntity *GetDynamicLight() { return m_DynamicLightHead.GetNextLight(); }

	/// unlink & re-link all the static & dynamic light entities
	/// to the entity tree. called when the entity tree is re-created
	void RelinkLightEntities();

	inline void EnableLightForEntity();
	inline void DisableLightForEntity();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources();
};


// ================================ inline implementations ================================ 


inline void CLightEntityManager::EnableLightForEntity()
{
	GraphicsDevice().Enable( RenderStateType::LIGHTING );
}


inline void CLightEntityManager::DisableLightForEntity()
{
	GraphicsDevice().Disable( RenderStateType::LIGHTING );

//	int i;
//	for( i=0; i<m_iNumPrevLights; i++ )
//		DIRECT3D9.GetDevice()->LightEnable(i, FALSE);
}


inline void CLightEntityManager::SetLightsForEntity(CCopyEntity *pEntity)
{/*
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	int num_pnt_lights = 0, num_dir_lights = 0;
//	int light_index;
	int i, iNumCurrentLights = pEntity->GetNumLights();
	CLightEntity *pLightEntity;

	for( i=0; i<iNumCurrentLights; i++ )
	{
//		light_index = pEntity->GetLightIndex(i);
//
//		if( light_index < CLEM_NUM_MAX_STATIC_LIGHTS )
//			pLightEntity = &m_aStaticLightEntity[ light_index ];
//		else
//			pLightEntity = &m_aDynamicLightEntity[ light_index - CLEM_NUM_MAX_STATIC_LIGHTS ];

		pLightEntity = GetLightEntity( pEntity->GetLightIndex(i) );

		if( !pLightEntity->IsInUse() )
			continue;

		if( m_pShaderLightManager )
		{
			if( pLightEntity->GetLightType() == D3DLIGHT_DIRECTIONAL )
			{
				num_dir_lights++;
				m_pShaderLightManager->SetLight( i, pLightEntity->GetHemisphericDirLight() );
			}
			else if( pLightEntity->GetLightType() == D3DLIGHT_POINT )
			{
				num_pnt_lights++;
				m_pShaderLightManager->SetLight( i, pLightEntity->GetHemisphericLight() );
			}

//			m_pShaderLightManager->UpdateLightPositionInViewSpace( i, matView );
		}
		else
		{
			pd3dDev->SetLight( i, &pLightEntity->GetLight() );
		}
	}

	if( m_pShaderLightManager )
	{
		m_pShaderLightManager->SetDirectionalLightOffset( 0 );
		m_pShaderLightManager->SetNumDirectionalLights( num_dir_lights );
		m_pShaderLightManager->SetPointLightOffset( num_dir_lights );
		m_pShaderLightManager->SetNumPointLights( num_pnt_lights );
		m_pShaderLightManager->CommitChanges();
//		m_pEffect->CommitChanges();
	}
	else
	{
		if( m_iNumPrevLights < iNumCurrentLights )
		{
			for( i=m_iNumPrevLights; i<iNumCurrentLights; i++ )
				pd3dDev->LightEnable(i, TRUE);
		}
		else if( iNumCurrentLights < m_iNumPrevLights )
		{
			for( i=iNumCurrentLights; i<m_iNumPrevLights; i++ )
				pd3dDev->LightEnable(i, FALSE);
		}
	}

	m_iNumPrevLights = iNumCurrentLights;*/
}


inline void CLightEntityManager::DeleteLight( int light_index, int light_type )
{/*
	if( light_type == CLE_LIGHT_DYNAMIC )
	{
		m_aDynamicLightEntity[light_index - CLEM_NUM_MAX_STATIC_LIGHTS].Invalidate();
		m_iNumDynamicLights--;

		g_iNumDynamicLights = m_iNumDynamicLights;
	}*/
}


inline CLightEntity *CLightEntityManager::GetLightEntity( int light_index )
{/*
	if( CLEM_NUM_MAX_STATIC_LIGHTS <= light_index )
	{	// dynamic light
		return &m_aDynamicLightEntity[light_index - CLEM_NUM_MAX_STATIC_LIGHTS];
	}
	else
	{	// static light
		return &m_aStaticLightEntity[light_index];
	}*/
}
} // amorphous



#endif		/*  __LIGHTENTITYMANAGER_H__  */
