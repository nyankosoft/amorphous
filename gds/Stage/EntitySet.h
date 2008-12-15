#ifndef __ENTITYSET_H__
#define __ENTITYSET_H__


#include <boost/shared_ptr.hpp>

#include "fwd.h"
#include "Physics/fwd.h"

#include "3DMath/aabb3.h"
#include "CopyEntity.h"
#include "EntityGroupHandle.h"
#include "EntityNode.h"
#include "EntityFactory.h"
#include "BaseEntityHandle.h"
#include "EntityRenderManager.h"
#include "OverlapTestAABB.h"

#include "3DCommon/Camera.h"

class CBSPTree;




/**
 * manages base entities and copy entities
 *
 */
class CEntitySet
{
private:

	enum params { NUM_MAX_ENTITY_GROUP_IDS = ENTITY_GROUP_MAX_ID + 1 };

	U32 m_EntityIDConter;

	boost::shared_ptr<CCopyEntity> m_pEntityInUse; ///< list of copy-entities currently in use

	CEntityFactory *m_pEntityFactory;

	/// list of base entities
	std::vector<CBaseEntity *> m_vecpBaseEntity;

	int m_NumEntityNodes;
	CEntityNode* m_paEntityTree;	///< pointer to the first node of a binary tree that holds copy-entities

	CCopyEntity *m_pCameraEntity;

//	CLightEntityManager *m_pLightEntityManager;

	CEntityRenderManager *m_pRenderManager;

	float m_PhysTimestep;
	float m_PhysOverlapTime;

	/// holds string names for each entity group
	/// - used when the collision groups are specified through scripts
	std::string m_EntityGroupName[NUM_MAX_ENTITY_GROUP_IDS];

	char m_EntityCollisionTable[NUM_MAX_ENTITY_GROUP_IDS][NUM_MAX_ENTITY_GROUP_IDS];

	/// save names of destroyed entities
	std::vector<std::string> m_vecDestroyedEntity;

	CStage* m_pStage;

	// static member variables to store default parammeter values for some member variables
	static float ms_DefaultPhysTimestep;

private:

	short MakeEntityNode_r( short sNodeIndex, CBSPTree* pSrcBSPTree,
		std::vector<CEntityNode>* pDestEntityTree );

	void LoadCopyEntityFromDesc_r( CCopyEntityDescFileData& desc, CCopyEntity *pParentEntity );

	void UpdatePhysics( float frametime );

	/// release entities that have been terminated and 'inuse' is false
	inline void ReleaseTerminatedEntities();

	//============ functions for entity mangement ============
	void Link( CCopyEntity* pEntity );
	inline void UpdateLink( CCopyEntity* pCpyEnt );

public:

	CEntitySet( CStage* pStage );

	~CEntitySet();

	// initialize light entity manager
//	void InitLightEntityManager();

	void SetEntityFactory( CEntityFactory *pEntityFactory );

    /// Move all the copy-entities in 'm_pEntityInUse'
	/// to the 'm_pEmptyEntity' list
	void Reset();

	void ReleaseAllEntities();

	inline CStage *GetStage() { return m_pStage; }

	//============ functions for collision checks ============
	void CheckPosition( STrace& tr );
	void CheckPosition( CTrace& tr );
	void GetVisibleEntities( CViewFrustumTest& vf_test );
	void GetOverlappingEntities( COverlapTestAABB& COverlapTestAABB );
	void ClipTrace( STrace& tr );


	inline bool IsCollisionEnabled( int group0, int group1 ) const;

	/// enable/disable collision between 2 entity groups
	/// - collisions are enabled between all groups by default
	void SetCollisionGroup( int group0, int group1, bool collision );

	/// enable/disable collision between an entity group and all the entity groups
	/// including itself.
	/// - Use this to completely turn off collision for non-collidable entities
	///   - Collisions for some entites are turned off by default through CBaseEntity::m_bNoClip 
	///     e.g.) billboards
	void SetCollisionGroup( int group, bool collision );

	inline void SetEntityGroupName( int group, const std::string& group_name );

	inline int GetEntityGroupFromName( const std::string& group_name );

	//============ functions for rendering ============
	void Render( CCamera& rCam );

	//============ functions for lighting control============
	inline void LinkLightEntity(CLightEntity *pLightEntity)	{ m_paEntityTree->LinkLightEntity_r(pLightEntity, m_paEntityTree); }

/*	inline int RegisterLight( CCopyEntity& rLightEntity, short sLightType );
	inline void DeleteLight( int light_index, short sLightType );
	inline CLightEntity *GetLightEntity( int light_index );
	inline void SetLightsForEntity( CCopyEntity *pEntity )	{ m_pLightEntityManager->SetLightsForEntity( pEntity ); }
	inline void EnableLightForEntity();
	inline void DisableLightForEntity();
	void UpdateLightForEntity(CCopyEntity *pEntity);
	inline void UpdateLightInfo( CCopyEntity *pEntity )		{ UpdateLightForEntity(pEntity); }//m_paEntityTree->CheckLight_r( pEntity, m_paEntityTree ); }
	CLightEntityManager *GetLightEntityManager() { return m_pLightEntityManager; }
*/

	bool MakeEntityTree( CBSPTree* pSrcBSPTree );

	void WriteEntityTreeToFile( const std::string& filename = "" );

	bool LoadCopyEntitiesFromDescFile( char* pcFilename );

    bool LoadBaseEntity( CBaseEntityHandle& base_entity_handle );

	/// create an entity in the stage
	/// - Returns the pointer to the created entity on success
	/// - Returns NULL on failure
	CCopyEntity *CreateEntity( CCopyEntityDesc& rCopyEntityDesc );

	CCopyEntity *CreateEntity( CBaseEntityHandle& rBaseEntityHandle,
		                       const Vector3& rvPosition,
		                       const Vector3& rvVelocity,
							   const Vector3& rvDirection = Vector3(0,0,0) );

	/// get entity with a specified individual name
	/// returns NULL if not found
	inline CCopyEntity *GetEntityByName( const char* name ) const;

	inline bool IsEntityAlive( const std::string& name ) const;
	inline bool EntityDestroyed( const std::string& name ) const;

	inline void HandleEntityTerminated( CCopyEntity* pEnttiy );

	/// update all the copy entities in the stage. This function must be called every frame
	void UpdateAllEntities( float dt );

	/// find base entity by name
	CBaseEntity* FindBaseEntity( const char* pcBaseEntityName );

	inline CCamera *GetCurrentCamera();	// return access to the current camera
	inline void UpdateCamera();
	inline void SetCameraEntity( CCopyEntity *pEntity ) { m_pCameraEntity = pEntity; }
	inline CCopyEntity *GetCameraEntity() { return m_pCameraEntity; }
	void GetBillboardRotationMatrix( Matrix33 &rmatBillboard ) const;

	inline CEntityRenderManager *GetRenderManager() { return m_pRenderManager; }

	static void SetDefaultPhysicsTimestep( float timestep ) { ms_DefaultPhysTimestep = timestep; }

	/// loading & saving data
//	void SaveCurrentCopyEntities(FILE *fp);
//	void LoadCopyEntitiesFromSavedData(FILE *fp);

	/// make friends with entity manager, since it needs to access
	/// base entity list in its LoadGraphicsResources() / ReleaseGraphicsResources()
	friend class CEntityRenderManager;

};


// ================================ inline implementations ================================ 


/*
// Note: This function doesn't unlink 'pCpyEnt' from the 'm_pEntityInUse' list
// It only link 'pCpyEnts' to the 'm_pEmptyEntity' list
inline void CEntitySet::ReleaseEntity( CCopyEntity* pEntity )
{
	m_pEntityFactory->ReleaseEntity( pEntity );
}
*/

inline void CEntitySet::UpdateLink( CCopyEntity* pEntity )
{
	pEntity->Unlink();
	Link( pEntity );

	// update world aabb
	pEntity->world_aabb.TransformCoord( pEntity->local_aabb, pEntity->Position() );
}


inline CCopyEntity *CEntitySet::GetEntityByName( const char* name ) const
{
	// cs->Enter()

	CCopyEntity *pEntity;
	for( pEntity = m_pEntityInUse.get();
		 pEntity != NULL;
		 pEntity = pEntity->m_pNextRawPtr )
	{
		if( !strcmp(pEntity->GetName().c_str(),name) && pEntity->inuse )
			return pEntity;
	}

	// not found in the list
	return NULL;

	// cs->Leave()
}


inline void CEntitySet::HandleEntityTerminated( CCopyEntity* pEntity )
{
	if( 0 < pEntity->GetName().length() )
		m_vecDestroyedEntity.push_back( pEntity->GetName() );

//	size_t i, num_callbacks = pEntity->Callback().size();
//	for( i=0; i<num_callbacks; i++ )
//		pEntity->Callback()[i]->OnEntityDestroyed()
}


inline bool CEntitySet::IsEntityAlive( const std::string& name ) const
{
	if( 0 < name.length() && GetEntityByName( name.c_str() ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


inline bool CEntitySet::EntityDestroyed( const std::string& name ) const
{
	size_t i, num_entities = m_vecDestroyedEntity.size();
	for( i=0; i<num_entities; i++ )
	{
		if( m_vecDestroyedEntity[i] == name )
			return true;
	}

	return false;
}


// returns access to the current camera
inline CCamera *CEntitySet::GetCurrentCamera()
{
	if( m_pCameraEntity )
		return m_pCameraEntity->pBaseEntity->GetCamera();
	else
		return NULL;
}


inline void CEntitySet::UpdateCamera()
{
	if( !m_pCameraEntity )
		return;

	m_pCameraEntity->pBaseEntity->UpdateCamera( m_pCameraEntity );
}

/**
 * \return [out] rotation matrix for billboard
 */
/* 10:30 PM 8/18/2007 - moved to EntitySet.cpp since the new version calls a method of CStage through m_pStage
inline void CEntitySet::GetBillboardRotationMatrix( Matrix33 &rmatBillboard ) const
{
	if( !m_pCameraEntity )
		return;

	CCopyEntity* pCameraEntity = m_pCameraEntity;

	CCamera *pCamera = pCameraEntity->pBaseEntity->GetCamera();
		if( pCamera )
			pCamera->GetOrientation( rmatBillboard );
		else
			pCameraEntity->GetOrientation( rmatBillboard );
			
}*/


/*	D3DXVECTOR3& rvDir   = pCameraEntity->GetDirection();	// the direction in which billboard polygons should face
	D3DXVECTOR3& rvRight = pCameraEntity->GetRight();
	D3DXVECTOR3& rvUp    = pCameraEntity->GetUp();

	rmatBillboard._11 = rvRight.x; rmatBillboard._12 = rvRight.y; rmatBillboard._13 = rvRight.z; rmatBillboard._14 = 0;
	rmatBillboard._21 =    rvUp.x; rmatBillboard._22 =    rvUp.y; rmatBillboard._23 =    rvUp.z; rmatBillboard._24 = 0;
	rmatBillboard._31 =   rvDir.x; rmatBillboard._32 =   rvDir.y; rmatBillboard._33 =   rvDir.z; rmatBillboard._34 = 0;
//	rmatBillboard._41 =   rvPos.x; rmatBillboard._42 =   rvPos.y; rmatBillboard._43 =   rvPos.z; rmatBillboard._44 = 1;
*/

/*
inline int CEntitySet::RegisterLight( CCopyEntity& rLightEntity, short sLightType )
{
	return m_pLightEntityManager->RegisterLight( rLightEntity, sLightType ); 
}


inline CLightEntity *CEntitySet::GetLightEntity( int light_index )
{
	return m_pLightEntityManager->GetLightEntity( light_index );
}


inline void CEntitySet::DeleteLight( int light_index, short sLightType )
{
	m_pLightEntityManager->DeleteLight( light_index, sLightType ); 
}

inline void CEntitySet::EnableLightForEntity()
{
	m_pLightEntityManager->EnableLightForEntity();
}


inline void CEntitySet::DisableLightForEntity()
{
	m_pLightEntityManager->DisableLightForEntity();
}
*/

inline bool CEntitySet::IsCollisionEnabled( int group0, int group1 ) const
{
	if( group0 < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group0
	 || group1 < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group1 )
		return false;

    return m_EntityCollisionTable[group0][group1] == 1 ? true : false;
}


inline void CEntitySet::SetEntityGroupName( int group, const std::string& group_name )
{
	if( group < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group )
		return;

	m_EntityGroupName[ group ] = group_name;
}


inline int CEntitySet::GetEntityGroupFromName( const std::string& group_name )
{
	if( group_name.length() == 0 )
		return ENTITY_GROUP_INVALID_ID; // invalid name

	int i;
	for( i=0; i<NUM_MAX_ENTITY_GROUP_IDS; i++ )
	{
		std::string ith_group_name = m_EntityGroupName[i];
		if( group_name == ith_group_name )
		{
			return i;
		}
	}

	return ENTITY_GROUP_INVALID_ID;
}



#endif  /*  __ENTITYSET_H__  */
