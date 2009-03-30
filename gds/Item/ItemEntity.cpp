#include "ItemEntity.hpp"

#include "Stage/Stage.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/MTRand.hpp"
#include "Item/GameItem.hpp"

using namespace std;
using namespace boost;



/**

- Placing an item in the stage


0) Create an instance of CStage with the CStageLoader

	CStageLoader stg_loader;
	shared_ptr<CStage> pStage = stg_loader.LoadStage( "path/to/scriptfile.bin" );

1) Create an instance of CItemStageUtility

	CItemStageUtility item_stg_util( pStage );

2) Create an item entity with CItemStageUtility::CItemStageUtility()

	item_stg_util.CItemStageUtility( pItem, Vector3(0,0,0) );


*/

CItemEntity::CItemEntity()
{
//	m_Attrib |= DESTROY_IF_PARENT_IS_GONE;
}


CItemEntity::CItemEntity( boost::shared_ptr<CGameItem> pItem )
:
CCopyEntity(CItemModuleEntityTypeID::ITEM_ENTITY),
m_pItem(pItem)
{
}


CItemEntity::~CItemEntity()
{
}


/*
void CItemEntity::Init( CCopyEntityDesc& desc )
{
}

void CItemEntity::Init( CItemEntityDesc& desc )
{
}*/


void CItemEntity::Update( float dt )
{
	m_pItem->Update( dt );
}

/*
void CItemEntity::UpdatePhysics( float dt )
{
}
*/

void CItemEntity::Draw()
{
/*	CCopyEntity *pParent = m_pParent;
	if( !pParent )
		return;

	pParent->pBaseEntity->DrawMeshMaterial(
		pParent->GetWorldPose(),
		m_AlphaMaterialIndex,
		m_ShaderTechnique
		);
*/
	bool single_mesh = true;
	if( single_mesh
	 && 0 < m_pItem->GetMeshContainerRootNode().GetNumMeshContainers() )
	{
		CMeshObjectContainer& mesh_container
			= *(m_pItem->GetMeshContainerRootNode().GetMeshContainer(0).get());

		this->MeshObjectHandle = mesh_container.m_MeshObjectHandle;

//		pBaseEntity->Draw3DModel( this );
//		pBaseEntity->Draw3DModel( this, mesh_container.m_ShaderTechnique );
		pBaseEntity->Draw3DModel( this, pBaseEntity->MeshProperty().m_ShaderTechnique );
	}
}


void CItemEntity::HandleMessage( SGameMessage& msg )
{
/*	switch( msg.iEffect )
	{
	default:
		break;
	}*/

//	m_pItem->HandleMessage( msg ) ???;
}


void CItemEntity::TerminateDerived()
{
//	shared_ptr<CCopyEntity> pSelf = this->Self().lock();
//	m_pPool->release( pSelf ); // pSelf is CCopyEntity type pointer!!!
}


/// Place an item in the stage
/// \return handle to the item entity that contains the item given as the argument
CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( shared_ptr<CGameItem> pItem,
															    CBaseEntityHandle& attributes_base_entity_handle,
													            const Matrix34& pose,
                                                                const Vector3& vLinearVelocity,
                                                                const Vector3& vAngularVelocity )

{
	shared_ptr<CItemEntity> pEntity = shared_ptr<CItemEntity>( new CItemEntity( pItem ) );
//	CEntityHandle<CItemEntity> entity_handle( pEntity );
	CEntityHandle<CItemEntity> entity_handle;

	CBaseEntityHandle *pAttribBaseEntityHandle;
	if( 0 < strlen(attributes_base_entity_handle.GetBaseEntityName()) )
		pAttribBaseEntityHandle = &attributes_base_entity_handle;
	else
		pAttribBaseEntityHandle = &m_BaseEntityHandle;

	entity_handle = m_pStage->CreateEntity<CItemEntity>( pEntity, *pAttribBaseEntityHandle );

	return entity_handle;
/*
	if( res == OK )
	{
		return entity_handle;
	}
	else
		return shared_ptr<CItemEntity>();
*/
}
