#include "ItemEntity.hpp"

#include "Graphics/Shader/BlendTransformsLoader.hpp"
#include "Stage/MeshBonesUpdateCallback.hpp"
#include "Stage/Stage.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Stage/BaseEntity_Draw.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/MTRand.hpp"
#include "Item/GameItem.hpp"
#include "Physics/ActorDesc.hpp"

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
:
m_ItemEntityFlags(0)
{
//	m_Attrib |= DESTROY_IF_PARENT_IS_GONE;
}


CItemEntity::CItemEntity( boost::shared_ptr<CGameItem> pItem )
:
CCopyEntity(CItemModuleEntityTypeID::ITEM_ENTITY),
m_pItem(pItem),
m_ItemEntityFlags(0)
{
}


CItemEntity::~CItemEntity()
{
}


void CItemEntity::InitMeshRenderMethod()
{
//	CGraphicsResourcesUpdateDelegate;
//	boost::shared_ptr<CGraphicsResourcesUpdateDelegate> pGraphicsUpdate( new CGraphicsResourcesUpdateDelegate(this) );

//	m_pGraphicsUpdate = pGraphicsUpdate;
	m_pGraphicsUpdate.reset( new CGraphicsResourcesUpdateDelegate<CGameItem>( m_pItem.get() ) );

	if( m_MeshHandle.GetMesh()
	 && m_MeshHandle.GetMesh()->GetMeshType() == CMeshType::SKELETAL )
	{
		m_pBlendTransformsLoader.reset( new CBlendTransformsLoader );

		m_pMeshBonesUpdateCallback.reset( new CMeshBonesUpdateCallback );
		m_pMeshBonesUpdateCallback->SetBlendTransformsLoader( m_pBlendTransformsLoader );
		m_pMeshBonesUpdateCallback->SetSkeletalMesh( m_MeshHandle );
//		m_pGraphicsUpdate = m_pMeshBonesUpdateCallback;
	}

	::InitMeshRenderMethod( *this, m_pBlendTransformsLoader );
}


void CItemEntity::InitMesh()
{
	if( m_ItemEntityFlags & CItemEntity::SF_USE_ENTITY_ATTRIBUTES_FOR_RENDERING )
	{
		CBE_MeshObjectProperty& mesh_property = this->pBaseEntity->MeshProperty();
		if( 0 < mesh_property.m_ShaderTechnique.size_x() )
		{
			// Set shader and shader params loaders to this->m_pMeshRenderMethod
			// - Shader of CBaseEntity::m_MeshProperty is set to this->m_pMeshRenderMethod.
			// - Current shader of this->m_pMeshRenderMethod will be overwritten. 
			CreateMeshRenderMethod( CEntityHandle<>( this->Self() ),
				mesh_property.m_ShaderHandle,
				mesh_property.m_ShaderTechnique(0,0)
				);
		}
		else
		{
			// Set shader params loaders
			// let's assume that shaders are already set to the render method, this->m_pMeshRenderMethod,
			// or user want to set them later
			InitMeshRenderMethod();
		}
	}
	else
	{
		// Set up shader params loaders without using base entity attributes
		InitMeshRenderMethod();
	}
}



void CItemEntity::Init( CCopyEntityDesc& desc )
{
	RaiseEntityFlags( pBaseEntity->GetEntityFlag() );
	this->fRadius = pBaseEntity->GetRadius();
}

/*
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
	if( m_ItemEntityFlags & CItemEntity::SF_USE_ENTITY_ATTRIBUTES_FOR_RENDERING )
	{
		bool single_mesh = true;
		if( single_mesh
		 && 0 < m_pItem->GetMeshContainerRootNode().GetNumMeshContainers() )
		{
			CMeshObjectContainer& mesh_container
				= *(m_pItem->GetMeshContainerRootNode().GetMeshContainer(0).get());

			this->m_MeshHandle = mesh_container.m_MeshObjectHandle;

			CCopyEntity::Draw();
//			pBaseEntity->Draw3DModel( this );
//			pBaseEntity->Draw3DModel( this, mesh_container.m_ShaderTechnique );
//			pBaseEntity->Draw3DModel( this, pBaseEntity->MeshProperty().m_ShaderTechnique );
		}
	}
	else
	{
		// Update lights info, etc.
		UpdateEntityForRendering( *this );

		m_pItem->Render();
	}
}


void CItemEntity::DrawAs( CRenderContext& render_context )
{
//	render_context.pRenderMethod->AddSubsetRenderMethod( m_pBlendTransformsLoader );
}


void CItemEntity::OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity& other_entity, physics::CShape& other_shape, U32 trigger_flags )
{
	m_pItem->OnPhysicsTrigger( my_shape, other_entity, other_shape, trigger_flags );
}


void CItemEntity::OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity )
{
	m_pItem->OnPhysicsContact( pair, other_entity );
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
																physics::CActorDesc& actor_desc,
																bool create_physics_actor )
//													            const Matrix34& pose,
//                                                              const Vector3& vLinearVelocity,
//                                                              const Vector3& vAngularVelocity )

{
	shared_ptr<CItemEntity> pEntity = shared_ptr<CItemEntity>( new CItemEntity( pItem ) );

	CEntityHandle<CItemEntity> entity_handle;

	CBaseEntityHandle *pAttribBaseEntityHandle;
	if( 0 < strlen(attributes_base_entity_handle.GetBaseEntityName()) )
		pAttribBaseEntityHandle = &attributes_base_entity_handle;
	else
		pAttribBaseEntityHandle = &m_BaseEntityHandle;

	pEntity->SetWorldPose( actor_desc.WorldPose );// pose );
	pEntity->SetVelocity( actor_desc.BodyDesc.LinearVelocity );// vLinearVelocity );
	pEntity->SetAngularVelocity( actor_desc.BodyDesc.AngularVelocity );// vAngularVelocity );

	physics::CActorDesc *pActorDesc = create_physics_actor ?  &actor_desc : NULL;
	entity_handle = m_pStage->CreateEntity<CItemEntity>( pEntity, *pAttribBaseEntityHandle, pActorDesc );

	pItem->SetItemEntity( entity_handle );

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


CEntityHandle<CItemEntity> CItemStageUtility::CreateItemEntity( shared_ptr<CGameItem> pItem,
															    CBaseEntityHandle& attributes_base_entity_handle,
													            const Matrix34& pose,
                                                                const Vector3& vLinearVelocity,
                                                                const Vector3& vAngularVelocity )
{
	physics::CActorDesc actor_desc;
	actor_desc.WorldPose = pose;
	actor_desc.BodyDesc.LinearVelocity  = vLinearVelocity;
	actor_desc.BodyDesc.AngularVelocity = vAngularVelocity;

	return CreateItemEntity(
		pItem,
		attributes_base_entity_handle,
		actor_desc,
		false );
}
