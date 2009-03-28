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
//	m_pItem->Draw();
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
													            const Matrix34& pose,
                                                                const Vector3& vLinearVelocity,
                                                                const Vector3& vAngularVelocity )

{
	shared_ptr<CItemEntity> pEntity = shared_ptr<CItemEntity>( new CItemEntity( pItem ) );
//	CEntityHandle<CItemEntity> entity_handle( pEntity );
	CEntityHandle<CItemEntity> entity_handle;

	entity_handle = m_pStage->CreateEntity<CItemEntity>( pEntity, m_BaseEntityHandle );

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
