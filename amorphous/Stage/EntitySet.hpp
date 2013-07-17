#ifndef __ENTITYSET_H__
#define __ENTITYSET_H__


#include <boost/shared_ptr.hpp>

#include "fwd.hpp"
#include "CopyEntity.hpp"
#include "EntityNode.hpp"
#include "EntityGroupHandle.hpp"
#include "BaseEntityHandle.hpp"
#include "OverlapTestAABB.hpp"

#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Physics/fwd.hpp"


namespace amorphous
{


/**
 * manages base entities and copy entities
 *
 */
class EntityManager
{
private:

	enum params { NUM_MAX_ENTITY_GROUP_IDS = ENTITY_GROUP_MAX_ID + 1 };

	U32 m_EntityIDConter;

	boost::shared_ptr<CCopyEntity> m_pEntityInUse; ///< list of copy-entities currently in use

	EntityFactory *m_pEntityFactory;

	/// list of base entities
	std::vector<BaseEntity *> m_vecpBaseEntity;

	int m_NumEntityNodes;
	EntityNode* m_paEntityTree;	///< pointer to the first node of a binary tree that holds copy-entities

	CCopyEntity *m_pCameraEntity;

//	LightEntityManager *m_pLightEntityManager;

	boost::shared_ptr<EntityRenderManager> m_pRenderManager;

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

	short MakeEntityNode_r( short sNodeIndex, BSPTree* pSrcBSPTree,
		std::vector<EntityNode>* pDestEntityTree );

	void LoadCopyEntityFromDesc_r( CCopyEntityDescFileData& desc, CCopyEntity *pParentEntity );

	inline BaseEntity *GetBaseEntity( BaseEntityHandle& rBaseEntityHandle );

	void SetBasicEntityAttributes( CCopyEntity *pEntity, BaseEntity& rBaseEntity );

	void InitEntity(
		boost::shared_ptr<CCopyEntity> pNewCopyEntPtr,
		CCopyEntity *pParent,
		BaseEntity *pBaseEntity,
		physics::CActorDesc* pPhysActorDesc
		);

	void UpdatePhysics( float frametime );

	inline void UpdateEntityAfterMoving( CCopyEntity *pEntity );

	/// release entities that have been terminated and 'inuse' is false
	inline void ReleaseTerminatedEntities();

	//============ functions for entity mangement ============
	void Link( CCopyEntity* pEntity );
	inline void UpdateLink( CCopyEntity* pCpyEnt );

public:

	EntityManager( CStage* pStage );

	~EntityManager();

	// initialize light entity manager
//	void InitLightEntityManager();

	void SetEntityFactory( EntityFactory *pEntityFactory );

    /// Move all the copy-entities in 'm_pEntityInUse'
	/// to the 'm_pEmptyEntity' list
	void Reset();

	void ReleaseAllEntities();

	inline CStage *GetStage() { return m_pStage; }

	//============ functions for collision checks ============
	void CheckPosition( STrace& tr );
	void CheckPosition( CTrace& tr );
	void GetVisibleEntities( ViewFrustumTest& vf_test );
	void GetOverlappingEntities( OverlapTestAABB& OverlapTestAABB );
	void ClipTrace( STrace& tr );


	inline bool IsCollisionEnabled( int group0, int group1 ) const;

	/// enable/disable collision between 2 entity groups
	/// - collisions are enabled between all groups by default
	void SetCollisionGroup( int group0, int group1, bool collision );

	/// enable/disable collision between an entity group and all the entity groups
	/// including itself.
	/// - Use this to completely turn off collision for non-collidable entities
	///   - Collisions for some entites are turned off by default through BaseEntity::m_bNoClip 
	///     e.g.) billboards
	void SetCollisionGroup( int group, bool collision );

	inline void SetEntityGroupName( int group, const std::string& group_name );

	inline int GetEntityGroupFromName( const std::string& group_name );

	//============ functions for rendering ============
	void Render( Camera& rCam );

	//============ functions for lighting control============
	inline void LinkLightEntity(LightEntity *pLightEntity)	{ m_paEntityTree->LinkLightEntity_r(pLightEntity, m_paEntityTree); }

/*	inline int RegisterLight( CCopyEntity& rLightEntity, short sLightType );
	inline void DeleteLight( int light_index, short sLightType );
	inline LightEntity *GetLightEntity( int light_index );
	inline void SetLightsForEntity( CCopyEntity *pEntity )	{ m_pLightEntityManager->SetLightsForEntity( pEntity ); }
	inline void EnableLightForEntity();
	inline void DisableLightForEntity();
	void UpdateLightForEntity(CCopyEntity *pEntity);
	LightEntityManager *GetLightEntityManager() { return m_pLightEntityManager; }
*/
	void UpdateLights( CCopyEntity *pEntity ) { m_paEntityTree->CheckLight_r( pEntity, m_paEntityTree ); }

	bool MakeEntityTree( BSPTree* pSrcBSPTree );

	void WriteEntityTreeToFile( const std::string& filename = "" );

	bool LoadCopyEntitiesFromDescFile( char* pcFilename );

    bool LoadBaseEntity( BaseEntityHandle& base_entity_handle );

	/// create an entity in the stage
	/// - Returns the pointer to the created entity on success
	/// - Returns NULL on failure
	CCopyEntity *CreateEntity( CCopyEntityDesc& rCopyEntityDesc );

	CCopyEntity *CreateEntity( BaseEntityHandle& rBaseEntityHandle,
		                       const Vector3& rvPosition,
		                       const Vector3& rvVelocity,
							   const Vector3& rvDirection = Vector3(0,0,0) );

	template<class T>
	inline EntityHandle<T> CreateEntity( boost::shared_ptr<T> pEntity, BaseEntityHandle& rBaseEntityHandle, physics::CActorDesc *pPhysActorDesc = NULL );

	/// get entity with a specified individual name
	/// returns NULL if not found
	inline CCopyEntity *GetEntityByName( const char* name ) const;

	inline bool IsEntityAlive( const std::string& name ) const;
	inline bool EntityDestroyed( const std::string& name ) const;

	inline void HandleEntityTerminated( CCopyEntity* pEnttiy );

	/// update all the copy entities in the stage. This function must be called every frame
	void UpdateAllEntities( float dt );

	/// find base entity by name
	BaseEntity* FindBaseEntity( const char* pcBaseEntityName );

	inline Camera *GetCurrentCamera();	// return access to the current camera
	inline void UpdateCamera();
	inline void SetCameraEntity( CCopyEntity *pEntity ) { m_pCameraEntity = pEntity; }
	inline CCopyEntity *GetCameraEntity() { return m_pCameraEntity; }
	void UpdateGraphics();
	void GetBillboardRotationMatrix( Matrix33 &rmatBillboard ) const;

	inline boost::shared_ptr<EntityRenderManager> GetRenderManager() { return m_pRenderManager; }

	static void SetDefaultPhysicsTimestep( float timestep ) { ms_DefaultPhysTimestep = timestep; }

	/// loading & saving data
//	void SaveCurrentCopyEntities(FILE *fp);
//	void LoadCopyEntitiesFromSavedData(FILE *fp);

	/// make friends with entity manager, since it needs to access
	/// base entity list in its LoadGraphicsResources() / ReleaseGraphicsResources()
	friend class EntityRenderManager;

};


// ================================ inline implementations ================================ 

inline void EntityManager::UpdateLink( CCopyEntity* pEntity )
{
	pEntity->Unlink();
	Link( pEntity );

	// update world aabb
	pEntity->world_aabb.TransformCoord( pEntity->local_aabb, pEntity->GetWorldPosition() );
}


/// T must be a derived class of CCopyEntity
template<class T>
inline EntityHandle<T> EntityManager::CreateEntity( boost::shared_ptr<T> pEntity,
												  BaseEntityHandle& rBaseEntityHandle,
												  physics::CActorDesc *pPhysActorDesc )
{
	EntityHandle<T> entity_handle( pEntity );

	BaseEntity *pBaseEntity = GetBaseEntity( rBaseEntityHandle );

	// Do null checks
	if( !pEntity || !pBaseEntity )
		return EntityHandle<T>();

	// Copy attribute values from 'pBaseEntity'
	SetBasicEntityAttributes( pEntity.get(), *pBaseEntity );

	InitEntity( pEntity, NULL, pBaseEntity, pPhysActorDesc );


	return entity_handle;
}


inline CCopyEntity *EntityManager::GetEntityByName( const char* name ) const
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


inline void EntityManager::HandleEntityTerminated( CCopyEntity* pEntity )
{
	if( 0 < pEntity->GetName().length() )
		m_vecDestroyedEntity.push_back( pEntity->GetName() );

//	size_t i, num_callbacks = pEntity->Callback().size();
//	for( i=0; i<num_callbacks; i++ )
//		pEntity->Callback()[i]->OnEntityDestroyed()
}


inline bool EntityManager::IsEntityAlive( const std::string& name ) const
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


inline bool EntityManager::EntityDestroyed( const std::string& name ) const
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
inline Camera *EntityManager::GetCurrentCamera()
{
	if( m_pCameraEntity )
		return m_pCameraEntity->pBaseEntity->GetCamera();
	else
		return NULL;
}


inline void EntityManager::UpdateCamera()
{
	if( !m_pCameraEntity )
		return;

	m_pCameraEntity->pBaseEntity->UpdateCamera( m_pCameraEntity );
}


/*
inline int EntityManager::RegisterLight( CCopyEntity& rLightEntity, short sLightType )
{
	return m_pLightEntityManager->RegisterLight( rLightEntity, sLightType ); 
}


inline LightEntity *EntityManager::GetLightEntity( int light_index )
{
	return m_pLightEntityManager->GetLightEntity( light_index );
}


inline void EntityManager::DeleteLight( int light_index, short sLightType )
{
	m_pLightEntityManager->DeleteLight( light_index, sLightType ); 
}

inline void EntityManager::EnableLightForEntity()
{
	m_pLightEntityManager->EnableLightForEntity();
}


inline void EntityManager::DisableLightForEntity()
{
	m_pLightEntityManager->DisableLightForEntity();
}
*/

inline bool EntityManager::IsCollisionEnabled( int group0, int group1 ) const
{
	if( group0 < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group0
	 || group1 < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group1 )
		return false;

    return m_EntityCollisionTable[group0][group1] == 1 ? true : false;
}


inline void EntityManager::SetEntityGroupName( int group, const std::string& group_name )
{
	if( group < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group )
		return;

	m_EntityGroupName[ group ] = group_name;
}


inline int EntityManager::GetEntityGroupFromName( const std::string& group_name )
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


} // namespace amorphous



#endif  /*  __ENTITYSET_H__  */
