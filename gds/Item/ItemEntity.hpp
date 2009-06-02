#ifndef  __ItemEntity_H__
#define  __ItemEntity_H__


#include "Stage/fwd.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/EntityMotionPathRequest.hpp"
#include "Stage/BaseEntityHandle.hpp"


class CGameItem;


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

public:

	CItemEntity();

	CItemEntity( boost::shared_ptr<CGameItem> pItem );

	virtual ~CItemEntity();

	virtual void Init( CCopyEntityDesc& desc );

//	void Init( CItemEntityDesc& desc );

	virtual void Update( float dt );

//	virtual void UpdatePhysics( float dt );

	const boost::shared_ptr<CGameItem> GetItem() const { return m_pItem; }

	boost::shared_ptr<CGameItem> Item() { return m_pItem; }

	virtual void Draw();

	virtual void HandleMessage( SGameMessage& msg );

	virtual void TerminateDerived();

	void SetItemEntityFlags( U32 flags ) { m_ItemEntityFlags = flags; }

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
		                                         const Matrix34& pose,
		                                         const Vector3& vLinearVelocity  = Vector3(0,0,0),
												 const Vector3& vAngularVelocity = Vector3(0,0,0) );

	inline CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
		                                                CBaseEntityHandle& attributes_base_entity_handle,
														const Vector3& vPosition );

	inline CEntityHandle<CItemEntity> CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
		                                                CBaseEntityHandle& attributes_base_entity_handle,
		                                                const Vector3& vPosition,
														float heading );
};


inline CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
																	   CBaseEntityHandle& attributes_base_entity_handle,
													                   const Vector3& vPosition )
{
	return CreateItemEntity( pItem, attributes_base_entity_handle, Matrix34( vPosition, Matrix33Identity() ) );
}


/// \param heading heading angle [deg]
inline CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( boost::shared_ptr<CGameItem> pItem,
																	   CBaseEntityHandle& attributes_base_entity_handle,
													                   const Vector3& vPosition,
													                   float heading )
{
	return CreateItemEntity( pItem, attributes_base_entity_handle, Matrix34( vPosition, Matrix33RotationY(deg_to_rad(heading)) ) );
}


#endif		/*  __ItemEntity_H__  */
