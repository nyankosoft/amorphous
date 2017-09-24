#include "ItemEntity.hpp"

#include "amorphous/Graphics/Shader/BlendTransformsLoader.hpp"
#include "amorphous/Stage/MeshBonesUpdateCallback.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/CopyEntityDesc.hpp"
#include "amorphous/Stage/GameMessage.hpp"
#include "amorphous/Stage/BaseEntity_Draw.hpp"
//#include "amorphous/Stage/RenderContext.hpp"
#include "amorphous/Support/Log/StateLog.hpp"
#include "amorphous/Item/GameItem.hpp"
#include "amorphous/Physics/ActorDesc.hpp"


namespace amorphous
{

using namespace boost;



/**

- Placing an item in the stage


0) Create an instance of CStage with the StageLoader

	StageLoader stg_loader;
	shared_ptr<CStage> pStage = stg_loader.LoadStage( "path/to/scriptfile.bin" );

1) Create an instance of CItemStageUtility

	CItemStageUtility item_stg_util( pStage );

2) Create an item entity with CItemStageUtility::CItemStageUtility()

	item_stg_util.CItemStageUtility( pItem, Vector3(0,0,0) );


*/

// Make these member functions of GameItem if they are frequently called.

MeshHandle GetPrimaryMeshHandle( GameItem& item )
{
	if( 0 < item.GetMeshContainerRootNode().GetNumMeshContainers()
	 && item.GetMeshContainerRootNode().GetMeshContainer(0) )
	{
		return item.GetMeshContainerRootNode().GetMeshContainer(0)->m_MeshObjectHandle;
	}
	else
		return MeshHandle();
}

std::shared_ptr<BasicMesh> GetPrimaryMesh( GameItem& item )
{
	std::shared_ptr<BasicMesh> pMesh = GetPrimaryMeshHandle(item).GetMesh();
	return pMesh;
}

std::shared_ptr<SkeletalMesh> GetPrimarySkeletalMesh( GameItem& item )
{
	std::shared_ptr<SkeletalMesh> pSkeletalMesh
		= boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>( GetPrimaryMesh(item) );

	return pSkeletalMesh;
}


ItemEntity::ItemEntity()
:
m_ItemEntityFlags(0)
{
//	m_Attrib |= DESTROY_IF_PARENT_IS_GONE;
}


ItemEntity::ItemEntity( std::shared_ptr<GameItem> pItem )
:
CCopyEntity(CItemModuleEntityTypeID::ITEM_ENTITY),
m_pItem(pItem),
m_ItemEntityFlags(0)
{
	m_pGraphicsUpdate.reset( new CGraphicsResourcesUpdateDelegate<GameItem>( m_pItem.get() ) );
}


ItemEntity::~ItemEntity()
{
}


void ItemEntity::UpdateGraphicsUpdateCallbacks()
{
//	if( m_MeshHandle.GetMesh()
//	 && m_MeshHandle.GetMesh()->GetMeshType() == MeshType::SKELETAL )

	MeshHandle mesh = GetPrimaryMeshHandle( *m_pItem );
	shared_ptr<SkeletalMesh> pSkeletalMesh
		= dynamic_pointer_cast<SkeletalMesh,BasicMesh>( mesh.GetMesh() );

	if( pSkeletalMesh )
	{
		if( !m_pBlendTransformsLoader )
			m_pBlendTransformsLoader.reset( new BlendTransformsLoader );

		m_pMeshBonesUpdateCallback.reset( new CMeshBonesUpdateCallback );
		m_pMeshBonesUpdateCallback->SetBlendTransformsLoader( m_pBlendTransformsLoader );
		m_pMeshBonesUpdateCallback->SetSkeletalMesh( mesh );
		m_pMeshBonesUpdateCallback->MeshBoneLocalTransforms().resize( pSkeletalMesh->GetNumBones(), IdentityTransform() );
//		m_pGraphicsUpdate = m_pMeshBonesUpdateCallback;
	}
}


void ItemEntity::RenderAs( CRenderContext& rc )
{
//	if( !m_pItem )
//		return;
//
//	bool render_skeletal_mesh = m_pBlendTransformsLoader ? true : false;
//	CRenderContext::RenderObjectType model_type
//		= render_skeletal_mesh ? CRenderContext::ROT_SKELETAL_3D_MODEL : CRenderContext::ROT_3D_MODEL;
//
//	rc.SetShaderTechnique( model_type );
//
//	ShaderManager& shader_mgr = rc.GetShaderManager( model_type );
//
//	shader_mgr.SetWorldTransform( this->GetWorldPose() );
//
//	if( render_skeletal_mesh )
//		shader_mgr.SetVertexBlendTransforms( m_pBlendTransformsLoader->BlendTransforms() );
//
//	shared_ptr<BasicMesh> pMesh = GetPrimaryMesh(*m_pItem);
//	if( pMesh )
//		pMesh->Render( shader_mgr );
}



/*
void ItemEntity::InitMeshRenderMethod()
{
	::InitMeshRenderMethod( *this, m_pBlendTransformsLoader );
}*/


void ItemEntity::InitMesh()
{
	if( m_ItemEntityFlags & ItemEntity::SF_USE_ENTITY_ATTRIBUTES_FOR_RENDERING )
	{
		SharedMeshContainer& mesh_property = this->pBaseEntity->MeshProperty();
		if( 0 < mesh_property.m_ShaderTechnique.size_x() )
		{
			// Set shader and shader params loaders to this->m_pMeshRenderMethod
			// - Shader of BaseEntity::m_MeshProperty is set to this->m_pMeshRenderMethod.
			// - Current shader of this->m_pMeshRenderMethod will be overwritten. 
			CreateMeshRenderMethod( EntityHandle<>( this->Self() ),
				mesh_property.m_ShaderHandle,
				mesh_property.m_ShaderTechnique(0,0)
				);
		}
		else
		{
			// Set shader params loaders
			// let's assume that shaders are already set to the render method, this->m_pMeshRenderMethod,
			// or user want to set them later
			InitMeshRenderMethod( *this );
		}
	}
	else
	{
		// Set up shader params loaders without using base entity attributes
		InitMeshRenderMethod( *this );
	}

	UpdateGraphicsUpdateCallbacks();

	if( GetPrimarySkeletalMesh(*m_pItem)
	 && m_pBlendTransformsLoader
	 && m_pMeshRenderMethod )
	{
		m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( m_pBlendTransformsLoader );
	}
}



void ItemEntity::Init( CCopyEntityDesc& desc )
{
	RaiseEntityFlags( pBaseEntity->GetEntityFlag() );
	this->fRadius = pBaseEntity->GetRadius();
}

/*
void ItemEntity::Init( ItemEntityDesc& desc )
{
}*/


void ItemEntity::Update( float dt )
{
	m_pItem->Update( dt );
}

/*
void ItemEntity::UpdatePhysics( float dt )
{
}
*/

void ItemEntity::Draw()
{
	if( m_ItemEntityFlags & ItemEntity::SF_USE_ENTITY_ATTRIBUTES_FOR_RENDERING )
	{
		bool single_mesh = true;
		if( single_mesh
		 && 0 < m_pItem->GetMeshContainerRootNode().GetNumMeshContainers() )
		{
			MeshObjectContainer& mesh_container
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


void ItemEntity::DrawAs( CRenderContext& render_context )
{
//	render_context.pRenderMethod->AddSubsetRenderMethod( m_pBlendTransformsLoader );
}


void ItemEntity::OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity& other_entity, physics::CShape& other_shape, U32 trigger_flags )
{
	m_pItem->OnPhysicsTrigger( my_shape, other_entity, other_shape, trigger_flags );
}


void ItemEntity::OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity )
{
	m_pItem->OnPhysicsContact( pair, other_entity );
}


void ItemEntity::HandleMessage( GameMessage& msg )
{
/*	switch( msg.iEffect )
	{
	default:
		break;
	}*/

//	m_pItem->HandleMessage( msg ) ???;
}


void ItemEntity::TerminateDerived()
{
//	shared_ptr<CCopyEntity> pSelf = this->Self().lock();
//	m_pPool->release( pSelf ); // pSelf is CCopyEntity type pointer!!!
}


/// Place an item in the stage
/// \return handle to the item entity that contains the item given as the argument
EntityHandle<ItemEntity> CItemStageUtility::CreateItemEntity( shared_ptr<GameItem> pItem,
															    BaseEntityHandle& attributes_base_entity_handle,
																physics::CActorDesc& actor_desc,
																bool create_physics_actor )
//													            const Matrix34& pose,
//                                                              const Vector3& vLinearVelocity,
//                                                              const Vector3& vAngularVelocity )

{
	shared_ptr<ItemEntity> pEntity = shared_ptr<ItemEntity>( new ItemEntity( pItem ) );

	EntityHandle<ItemEntity> entity_handle;

	BaseEntityHandle *pAttribBaseEntityHandle;
	if( 0 < strlen(attributes_base_entity_handle.GetBaseEntityName()) )
		pAttribBaseEntityHandle = &attributes_base_entity_handle;
	else
		pAttribBaseEntityHandle = &m_BaseEntityHandle;

	pEntity->SetWorldPose( actor_desc.WorldPose );// pose );
	pEntity->SetVelocity( actor_desc.BodyDesc.LinearVelocity );// vLinearVelocity );
	pEntity->SetAngularVelocity( actor_desc.BodyDesc.AngularVelocity );// vAngularVelocity );

	physics::CActorDesc *pActorDesc = create_physics_actor ?  &actor_desc : NULL;
	entity_handle = m_pStage->CreateEntity<ItemEntity>( pEntity, *pAttribBaseEntityHandle, pActorDesc );

	pItem->SetItemEntity( entity_handle );

	return entity_handle;
/*
	if( res == OK )
	{
		return entity_handle;
	}
	else
		return shared_ptr<ItemEntity>();
*/
}


EntityHandle<ItemEntity> CItemStageUtility::CreateItemEntity( shared_ptr<GameItem> pItem,
															    BaseEntityHandle& attributes_base_entity_handle,
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


} // namespace amorphous
