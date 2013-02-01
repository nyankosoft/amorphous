#ifndef  __ItemEntity_H__
#define  __ItemEntity_H__


#include "gds/Stage/fwd.hpp"
#include "gds/Stage/CopyEntity.hpp"
#include "gds/Stage/CopyEntityDesc.hpp"
#include "gds/Stage/EntityMotionPathRequest.hpp"
#include "gds/Stage/BaseEntityHandle.hpp"
#include "gds/Stage/GraphicsResourcesUpdateCallback.hpp"
#include "fwd.hpp"


namespace amorphous
{


class CItemModuleEntityTypeID
{
public:
	enum TypeID
	{
		ITEM_ENTITY = CCopyEntityTypeID::SYSTEM_ENTITY_ID_OFFSET
	};
};


/**
 - 
 - Mount item on the entity
 - Place item instances in the stage

 - Call CGameItem::Update()

*/
class CItemEntity : public CCopyEntity
{
	boost::shared_ptr<CGameItem> m_pItem;

	U32 m_ItemEntityFlags;

	/// Variables used by the item which has a skeletal mesh

	/// Set to m_pMeshBonesUpdateCallback
	boost::shared_ptr<BlendTransformsLoader> m_pBlendTransformsLoader;

	/// Set to CCopyEntity::m_pGraphicsUpdate
	boost::shared_ptr<CMeshBonesUpdateCallback> m_pMeshBonesUpdateCallback;

	boost::shared_ptr< CGraphicsResourcesUpdateDelegate<CItemEntity> > m_pGraphicsUpdateDelegate;
	
private:

//	void InitMeshRenderMethod();

public:

	CItemEntity();

	/// A game item
	/// - must be set via this ctor.
	/// - cannot be changed later.
	CItemEntity( boost::shared_ptr<CGameItem> pItem );

	virtual ~CItemEntity();

	virtual void Init( CCopyEntityDesc& desc );

//	void Init( CItemEntityDesc& desc );

	virtual void Update( float dt );

//	virtual void UpdatePhysics( float dt );

	const boost::shared_ptr<CGameItem> GetItem() const { return m_pItem; }

	boost::shared_ptr<CGameItem> Item() { return m_pItem; }

	virtual void Draw();

	virtual void DrawAs( CRenderContext& render_context );

	virtual void OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity& other_entity, physics::CShape& other_shape, U32 trigger_flags );

	virtual void OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity );

	virtual void HandleMessage( SGameMessage& msg );

	virtual void TerminateDerived();

	void SetItemEntityFlags( U32 flags ) { m_ItemEntityFlags = flags; }

	void InitMesh();

	// Needs to be called when a skeletal mesh of the item is loaded.
	void UpdateGraphicsUpdateCallbacks();

	void RenderAs( CRenderContext& rc );

	boost::shared_ptr<CMeshBonesUpdateCallback> MeshBonesUpdateCallback() { return m_pMeshBonesUpdateCallback; }

	enum ItemEntityFlags
	{
		SF_USE_ENTITY_ATTRIBUTES_FOR_RENDERING = (1 << 0),
//		SF_ANOTHER_FLAG                        = (1 << 1),
//		SF_YET_ANOTHER_FLAG                    = (1 << 2),
	};
};


class CItemStageUtility
{
	boost::shared_ptr<CStage> m_pStage;

	CBaseEntityHandle m_BaseEntityHandle;

public:

	CItemStageUtility( boost::shared_ptr<CStage> pStage )
		:
	m_pStage(pStage)
	{
		m_BaseEntityHandle.SetBaseEntityName( "__ForItemEntity__" );
	}

	CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
												 CBaseEntityHandle& attributes_base_entity_handle,
		                                         physics::CActorDesc& actor_desc,
												 bool create_physics_actor = false );

	CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
												 CBaseEntityHandle& attributes_base_entity_handle,
		                                         const Matrix34& pose,
		                                         const Vector3& vLinearVelocity  = Vector3(0,0,0),
												 const Vector3& vAngularVelocity = Vector3(0,0,0) );

	inline CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
		                                                CBaseEntityHandle& attributes_base_entity_handle,
														const Vector3& vPosition );

	/// \param heading heading angle [deg]
	inline CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
		                                                CBaseEntityHandle& attributes_base_entity_handle,
		                                                const Vector3& vPosition,
														float heading );

	/// Uses the default base entity for item entity
	inline CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
														const Vector3& vPosition );

	/// Uses the default base entity for item entity
	/// \param heading heading angle [deg]
	inline CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
		                                                const Vector3& vPosition,
														float heading );

	/// The created entity is registered to the physics scene of the stage
	inline CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
		                                         physics::CActorDesc& actor_desc );
};


inline CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
																	   CBaseEntityHandle& attributes_base_entity_handle,
													                   const Vector3& vPosition )
{
	return CreateItemEntity( pItem, attributes_base_entity_handle, Matrix34( vPosition, Matrix33Identity() ) );
}


inline CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
																	   CBaseEntityHandle& attributes_base_entity_handle,
													                   const Vector3& vPosition,
													                   float heading )
{
	return CreateItemEntity( pItem, attributes_base_entity_handle, Matrix34( vPosition, Matrix33RotationY(deg_to_rad(heading)) ) );
}


inline CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
													                   const Vector3& vPosition )
{
	return CreateItemEntity( pItem, m_BaseEntityHandle, Matrix34( vPosition, Matrix33Identity() ) );
}


inline CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
													                   const Vector3& vPosition,
													                   float heading )
{
	return CreateItemEntity( pItem, m_BaseEntityHandle, Matrix34( vPosition, Matrix33RotationY(deg_to_rad(heading)) ) );
}

inline CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
																	   physics::CActorDesc& actor_desc )
{
	return CreateItemEntity( pItem, m_BaseEntityHandle, actor_desc, true );
}

} // namespace amorphous



#endif		/*  __ItemEntity_H__  */
