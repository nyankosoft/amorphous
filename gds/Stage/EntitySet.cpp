#include "EntitySet.hpp"

#include "EntityNode.hpp"
#include "CopyEntityDesc.hpp"
#include "EntityFactory.hpp"

#include "BaseEntityManager.hpp"
#include "BaseEntity.hpp"
#include "EntityRenderManager.hpp"
#include "bsptree.hpp"
#include "trace.hpp"
#include "ViewFrustumTest.hpp"
#include "Stage.hpp"
#include "BE_PhysicsBaseEntity.hpp"
#include "CopyEntityDescFileArchive.hpp"

#include "Graphics/3DGameMath.hpp"
#include "Physics/Actor.hpp"
#include "Physics/Scene.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "Support/macro.h"
#include "Support/Profile.hpp"

using namespace std;
using namespace boost;
using namespace physics;


float CEntitySet::ms_DefaultPhysTimestep = 1.0f / 100;


//====================================================================================
// CEntitySet::Method()                                                  - CEntitySet
//====================================================================================

CEntitySet::CEntitySet( CStage* pStage )
:
m_pEntityFactory( new CEntityFactory() ),
m_EntityIDConter(1),
m_pStage(pStage),
m_paEntityTree(NULL)
{
//	m_pEntityInUse = NULL;

	m_pCameraEntity = NULL;

//	m_pLightEntityManager = NULL;

	m_PhysOverlapTime = 0;

	m_PhysTimestep = ms_DefaultPhysTimestep;

	m_pRenderManager = shared_ptr<CEntityRenderManager>( new CEntityRenderManager( this ) );

	// enable collision between all the groups
	memset( m_EntityCollisionTable, 1, sizeof(char) * NUM_MAX_ENTITY_GROUP_IDS * NUM_MAX_ENTITY_GROUP_IDS );

	// create light entity manager
//	InitLightEntityManager();

	// make default entity tree
/*	tree.m_paNode = new SNode_f [1];
	tree.m_paNode[0].aabb = AABB3( Vector3(-100,-100,-100), Vector3( 100, 100, 100) );
	tree.m_paNode[0].sBackChild = -1;
	tree.m_paNode[0].sFrontChild = -1;
	tree.m_paNode[0].sCellIndex = 0;
	tree.m_paNode[0].sPlaneIndex = -1;
	tree.m_NumNodes = 1;
	tree.m_NumPlanes = 0;*/
	SNode_f node;
	node.aabb = AABB3( Vector3(-1000000,-1000000,-1000000), Vector3( 1000000, 1000000, 1000000) );
	node.sBackChild = -1;
	node.sFrontChild = -1;
	node.sCellIndex = 0;
	node.sPlaneIndex = -1;
	CBSPTree tree( &node, 1, &SPlane( Vector3(0,0,0), 0 ), 1 );
	MakeEntityTree( &tree );

//	SafeDeleteArray( tree.m_paNode );

/*	m_paEntityTree = new CEntityNode [1];
	m_paEntityTree[0].m_AABB.vMin = Vector3(-100,-100,-100);
	m_paEntityTree[0].m_AABB.vMax = Vector3( 100, 100, 100);
	m_paEntityTree[0].leaf = true;
*/
}


CEntitySet::~CEntitySet()
{
	// move all the copy-entities from "m_pEntityInUse' to 'm_pEmptyEntity' list
	ReleaseAllEntities();

	SafeDeleteArray( m_paEntityTree );

//	ReleaseBaseEntities()

	// clear all the base entities
	SafeDeleteVector( m_vecpBaseEntity );

	m_vecpBaseEntity.resize( 0 );

//	SafeDelete( m_pLightEntityManager );
	SafeDelete( m_pEntityFactory );
	m_pRenderManager.reset();
}


inline void CEntitySet::ReleaseTerminatedEntities()
{
//	CCopyEntity *pEntity = m_pEntityInUse;
//	CCopyEntity *pPrevEntity = NULL;
//	CCopyEntity *pNextEntity;
	shared_ptr<CCopyEntity> pEntity = m_pEntityInUse;
	shared_ptr<CCopyEntity> pPrevEntity = shared_ptr<CCopyEntity>();
	shared_ptr<CCopyEntity> pNextEntity;
	while( pEntity )
	{
		if( !pEntity->inuse )
		{
			// 0, save the access to the next engaged entity in advance
			pNextEntity = pEntity->m_pNext;

			// 1, unlink 'pEntity' from 'm_pEntityInUse' list
			if( pPrevEntity )
//				pPrevEntity->m_pNext = pEntity->m_pNext;
				pPrevEntity->SetNext( pEntity->m_pNext );
			else	// 'pEntity' is the first copy-entity in list 'm_pEntityInUse
				m_pEntityInUse = pEntity->m_pNext;

			// 1.5 unlink from the active entity list
			pEntity->SetNextToNull();

			// 2, release the entity
			m_pEntityFactory->ReleaseEntity( pEntity->m_pSelf.lock() );

			// 3. skip this entity and check the next one
			pEntity = pNextEntity;
		}
		else
		{
			pPrevEntity = pEntity;
			pEntity = pEntity->m_pNext;
		}
	}
}

/*
void CEntitySet::InitLightEntityManager()
{
	SafeDelete( m_pLightEntityManager );
	m_pLightEntityManager = new CLightEntityManager;
	m_pLightEntityManager->Init( this );
}
*/

void CEntitySet::SetEntityFactory( CEntityFactory *pEntityFactory )
{
	if( !pEntityFactory )
		return;

	SafeDelete( m_pEntityFactory );
	m_pEntityFactory = pEntityFactory;
}


/// Release all the copy entities in 'm_pEntityInUse'
void CEntitySet::ReleaseAllEntities()
{
//	CCopyEntity *pEntity = this->m_pEntityInUse;
//	CCopyEntity *pNextEntity = NULL;
	shared_ptr<CCopyEntity> pEntity = this->m_pEntityInUse;
	shared_ptr<CCopyEntity> pNextEntity;

	// terminate all the remaining entities
	while( pEntity )
	{
		// save the pointer to the next entity since CStage::TerminateEntity()
		// takes the reference to an entity pointer and sets it to NULL
		pNextEntity = pEntity->m_pNext;

		if( IsValidEntity( pEntity.get() ) )
		{
			CCopyEntity *p = pEntity.get();
			m_pStage->TerminateEntity( p );
		}

		pEntity = pNextEntity;
	}

	ReleaseTerminatedEntities();
}


///		checks if 'tr.vEnd' is in a valid position
void CEntitySet::CheckPosition(STrace& tr)
{
	CEntityNode* pHeadNode = m_paEntityTree; /* pointer to the first entity-node */

	pHeadNode->CheckPosition_r( tr, pHeadNode );
}

void CEntitySet::CheckPosition(CTrace& tr)
{
	CEntityNode* pHeadNode = m_paEntityTree; /* pointer to the first entity-node */
	
	pHeadNode->CheckPosition_r( tr, pHeadNode );
}


void CEntitySet::GetVisibleEntities(CViewFrustumTest& vf_test)
{
	CEntityNode* pHeadNode = m_paEntityTree; /* pointer to the first entity-node */
	
	pHeadNode->GetVisibleEntities_r( vf_test, pHeadNode );
}

/**
 * clips a trace against entities
 * trace is represented by line-segment and AABB
 */
void CEntitySet::ClipTrace(STrace& tr)
{
	PROFILE_FUNCTION();

	CEntityNode* pHeadNode = m_paEntityTree; /* pointer to the first entity-node */
	
	pHeadNode->ClipTrace_r( tr, pHeadNode );
}


void CEntitySet::GetOverlappingEntities( COverlapTestAABB& overlap_test )
{
	m_paEntityTree->GetOverlappingEntities( overlap_test, m_paEntityTree );
}


void CEntitySet::Link( CCopyEntity* pEntity )
{
	CEntityNode* paEntNode = m_paEntityTree;
	short sEntNodeIndex = 0;
	float fRadius, d=0;
	Vector3 vExtents;

	if(pEntity->bvType != BVTYPE_DOT)
		fRadius = pEntity->fRadius;
	else
		fRadius = 0;

	while(1)
	{
		CEntityNode& rThisEntityNode = paEntNode[ sEntNodeIndex ];

		// if this is a leaf node, link the entity here.
		if( rThisEntityNode.leaf )
		{
			rThisEntityNode.Link(pEntity);
			return;
		}

		SPlane& rPlane = rThisEntityNode.m_Plane;

		d = rPlane.GetDistanceFromPoint( pEntity->GetWorldPosition() );

/*		if(pEntity->bvType != BVTYPE_DOT)
			fRadius = pEntity->local_aabb.GetRadiusForPlane( rPlane );
		else
			fRadius = 0;
*/			
		if(fRadius < d)
		{
			// recurse down to front
			sEntNodeIndex = rThisEntityNode.sFrontChild;
			continue;
		}
		else if(d < -fRadius)
		{
			// recurse down to back
			sEntNodeIndex = rThisEntityNode.sBackChild;
			continue;
		}
		else
		{
			// 'pEntity' is crossing the dividing plane.
			// - link the entity to this diverging-node
			rThisEntityNode.Link(pEntity);
			break;
		}
	}

	// Added for light entity
	// - Link the light entity to the light entity list of an entity node
	if( pEntity->GetEntityTypeID() != CCopyEntityTypeID::DEFAULT )
	{
		pEntity->LinkDerivedEntity();
	}
}


//==========================================================================
//
//						'Rendering' Functions
//
//==========================================================================

void CEntitySet::Render(CCamera& rCam)
{
	m_pRenderManager->Render( rCam );
}


//==========================================================================
//    Making an Entity Tree
//==========================================================================

bool CEntitySet::MakeEntityTree(CBSPTree* pSrcBSPTree)
{
//	Reset();

	vector<CEntityNode> entity_tree;

	// allocate some memory in advance
	// not doing this may cause error in Release build
	entity_tree.reserve( 10000 );

	// create the new entity tree on the temporary node buffer entity_tree
	// - Do not copy the tree to m_paEntityTree until all the entities are unlinked
	MakeEntityNode_r(0, pSrcBSPTree, &entity_tree);

	if(entity_tree.size() == 0)
		return false;	// entity tree costruction failed

	entity_tree[0].sParent = -1;	// the root node has no parent

	// unlink all the entities from the current entity tree
	if( m_pEntityInUse )
	{
		for( CCopyEntity* pEntity = m_pEntityInUse.get();
			 pEntity != NULL;
			 pEntity = pEntity->m_pNextRawPtr )
		{
			pEntity->Unlink();
		}
	}

	// Do this AFTER all the entities are unlinked from the entity nodes
	SafeDeleteArray( m_paEntityTree );

	// copy the new tree
	m_NumEntityNodes = (int)entity_tree.size();
	m_paEntityTree = new CEntityNode [ m_NumEntityNodes ];
	for(int i=0; i<m_NumEntityNodes; i++)
		m_paEntityTree[i] = entity_tree[i];

	entity_tree.clear();

	// set stage and entity set
	for(int i=0; i<m_NumEntityNodes; i++)
	{
		m_paEntityTree[i].m_pStage     = m_pStage;
		m_paEntityTree[i].m_pEntitySet = this;
	}


	// update entity tree of the render manager
	m_pRenderManager->UpdateEntityTree( m_paEntityTree, m_NumEntityNodes );

	WriteEntityTreeToFile( "debug/entity_tree - recreated the tree.txt" );

	// re-link all the entities to the new tree nodes
	if( m_pEntityInUse )
	{
		for( CCopyEntity* pEntity = m_pEntityInUse.get();
			 pEntity != NULL;
			 pEntity = pEntity->m_pNextRawPtr )
		{
			// added: 11:34 PM 5/25/2008
			// Do not re-link an entity if it has already been marked as 'not in use'
			// - Failure to do this leads to an invalid link in the entity tree node
			//   - Caused infinite loops in CEntityNode::CheckPosition_r()
			if( !IsValidEntity( pEntity ) )
				continue;

			Link( pEntity );
		}
	}

	WriteEntityTreeToFile( "debug/entity_tree - re-linked entities to the tree.txt" );

	// re-link all the light entities to the new tree nodes
//	m_pLightEntityManager->RelinkLightEntities();

	return true;
}


/**
 Create Entity-Tree from BSP-Tree of the map.
 There are 4 cases we have to deal with 
 (1) frontchild - NO  / backchild - NO	... Create a leaf node
 (2) frontchild - YES / backchild - YES	... Create a diverging node and recurse down for both sides
 (3) frontchild - YES / backchild - NO	... Just recurse down to frontchild
 (4) frontchild - NO  / backchild - YES ... This is an exception. In this case,
										    1) create a diverging node.
										    2) create a leaf node as a frontchild.
*/
short CEntitySet::MakeEntityNode_r(short sNodeIndex, CBSPTree* pSrcBSPTree,
					   vector<CEntityNode>* pDestEntityTree)
{
//	SNode_f& rThisNode = pSrcBSPTree->m_paNode[ sNodeIndex ];
	const SNode_f& rThisNode = pSrcBSPTree->GetNode( sNodeIndex );
	short sFrontChild = rThisNode.sFrontChild;
	short sBackChild = rThisNode.sBackChild;

//	g_Log.Print( "NODE: %d / FC: %d / BC: %d", sNodeIndex, sFrontChild, sBackChild );

	if( 0 <= sFrontChild && sBackChild < 0 )	//case (1)
		return MakeEntityNode_r(sFrontChild, pSrcBSPTree, pDestEntityTree);  //recurse down to front

	CEntityNode newnode;
	short sThisEntNodeIndex = (short)pDestEntityTree->size();
	pDestEntityTree->push_back( newnode );
	CEntityNode& rNewNode = pDestEntityTree->at( sThisEntNodeIndex );

//	rNewNode.m_pLightEntity = NULL;

	if( sFrontChild < 0 && 0 <= sBackChild )  //This shouldn't really happen
	{
		//current node 'rThisNode' has no front child but it should be treated as a diverging node
		rNewNode.leaf = false;
		rNewNode.sFrontChild = (short)pDestEntityTree->size();

		//We have to add one leaf node as a front child to this diverging node
		pDestEntityTree->push_back( newnode );
		CEntityNode& rFrontLeafNode = pDestEntityTree->back();
		rFrontLeafNode.sFrontChild = -1;
		rFrontLeafNode.sBackChild = -1;
		rFrontLeafNode.sParent = sThisEntNodeIndex;  //Index to 'rNewNode' in pDestEntityTree
		rFrontLeafNode.leaf = true;
		rFrontLeafNode.m_AABB = rThisNode.aabb;

		//Recurse down to the back side
		pDestEntityTree->at( sThisEntNodeIndex ).sBackChild
			= MakeEntityNode_r(sBackChild, pSrcBSPTree, pDestEntityTree);
		
		CEntityNode& rNewNode = pDestEntityTree->at( sThisEntNodeIndex );
		rNewNode.m_Plane = pSrcBSPTree->GetPlane( rThisNode );
		rNewNode.m_AABB.Merge2AABBs( rThisNode.aabb,
			pDestEntityTree->at( rNewNode.sBackChild ).m_AABB );
		return sThisEntNodeIndex;  //Return the index to this diverging-node
	}

	else if( sFrontChild < 0 && sBackChild < 0 )  // complete leaf-node
	{	
		rNewNode.m_AABB = rThisNode.aabb;
		rNewNode.leaf = true;
		rNewNode.sFrontChild = -1;
		rNewNode.sBackChild = -1;
		rNewNode.m_sCellIndex = rThisNode.sCellIndex;
		if( rThisNode.sCellIndex < 0 )
			LOG_PRINT_ERROR( fmt_string( "invalid cell index found in a bsp-tree node (node index: %d)", sNodeIndex ) );
		return  sThisEntNodeIndex;	//returns the index of this node
	}

	else  //diverging-node
	{	
		rNewNode.m_AABB = rThisNode.aabb;
		rNewNode.leaf = false;

		//Recurse down to the both sides to make 2 sub-trees (front & back)
		pDestEntityTree->at( sThisEntNodeIndex ).sFrontChild
			= MakeEntityNode_r(sFrontChild, pSrcBSPTree, pDestEntityTree);
		pDestEntityTree->at( sThisEntNodeIndex ).sBackChild
			= MakeEntityNode_r(sBackChild, pSrcBSPTree, pDestEntityTree);

		//The internal memory allocation of 'pDestEntityTree' may have changed
		//during the above recursions.
		//In other words, the variable 'rNewNode' may be no longer valid
		//due to memory re-allocation, so we have to update the reference variable
		CEntityNode& rNewNode = pDestEntityTree->at( sThisEntNodeIndex );
		rNewNode.m_Plane = pSrcBSPTree->GetPlane( rThisNode );  //Set 'binary partition plane'
		pDestEntityTree->at( rNewNode.sFrontChild ).sParent = sThisEntNodeIndex;
		pDestEntityTree->at( rNewNode.sBackChild ).sParent = sThisEntNodeIndex;
		return sThisEntNodeIndex;
	}

}


inline CBaseEntity *CEntitySet::GetBaseEntity( CBaseEntityHandle& rBaseEntityHandle )
{
	CBaseEntity *pBaseEntity = NULL;
	if( rBaseEntityHandle.GetBaseEntityPointer() )
	{
		// already loaded
		pBaseEntity = rBaseEntityHandle.GetBaseEntityPointer();
	}
	else if( rBaseEntityHandle.GetState() == CBaseEntityHandle::STATE_INVALID )
	{
		// invalid base entity handle
		// - already tried to load the base entity before but it was not found in the database
		return NULL;
	}
	else // i.e. rBaseEntityHandle.GetState() == CBaseEntityHandle::STATE_UNINITIALIZED
	{
		// get pointer to the base entity
		// if the base entity has not been loaded yet, load it
		if( LoadBaseEntity( rBaseEntityHandle ) )
		{
			pBaseEntity = rBaseEntityHandle.GetBaseEntityPointer();
		}
		else
		{
			LOG_PRINT( " - unable to create a copy entity (base entity: " + string(rBaseEntityHandle.GetBaseEntityName()) + ")" );
			return NULL;
		}
	}

	return pBaseEntity;
}


void CEntitySet::SetBasicEntityAttributes( CCopyEntity *pEntity, CBaseEntity& rBaseEntity )
{
	pEntity->bvType        = rBaseEntity.m_BoundingVolumeType;
	pEntity->bNoClip       = rBaseEntity.m_bNoClip;
	pEntity->fRadius       = rBaseEntity.m_fRadius;
	pEntity->local_aabb    = rBaseEntity.m_aabb;
	pEntity->fLife         = rBaseEntity.m_fLife;
	pEntity->SetEntityFlags( rBaseEntity.m_EntityFlag );
}


void CEntitySet::InitEntity( boost::shared_ptr<CCopyEntity> pNewCopyEntPtr,
							 CCopyEntity *pParent,
							 CBaseEntity *pBaseEntity,
							 CActorDesc* pPhysActorDesc )
{
	CCopyEntity* pNewCopyEnt = pNewCopyEntPtr.get();

	// Mark the entity as in use
	pNewCopyEnt->inuse = true;

	pNewCopyEnt->m_pSelf = pNewCopyEntPtr;

	pNewCopyEnt->pBaseEntity = pBaseEntity;
	CBaseEntity& rBaseEntity = (*pBaseEntity);

	pNewCopyEnt->m_pStage = m_pStage;

	// set id and increment the counter
	pNewCopyEnt->m_ID = m_EntityIDConter++;

	// z-sort is disabled by default initialization
	// Entities that have translucent polygons have to turn on their copy entities'
	// 'BETYPE_USE_ZSORT' in InitCopyEntity()
	if( pNewCopyEnt->m_TypeID == CCopyEntityTypeID::ALPHA_ENTITY )
	{
		// For alpha entity, always use the  z-sorting
		pNewCopyEnt->RaiseEntityFlags( BETYPE_USE_ZSORT );
	}
	else
	{
		// Otherwise, disable z-sorting by default
		pNewCopyEnt->ClearEntityFlags( BETYPE_USE_ZSORT );
	}

	// set the glare type
	if( rBaseEntity.m_EntityFlag & BETYPE_GLARESOURCE )
	{
		pNewCopyEnt->RaiseEntityFlags( BETYPE_GLARESOURCE );
	}
	else if( rBaseEntity.m_EntityFlag & BETYPE_GLAREHINDER )
	{
		pNewCopyEnt->RaiseEntityFlags( BETYPE_GLAREHINDER );
	}

	// update world aabb
	pNewCopyEnt->world_aabb.TransformCoord( pNewCopyEnt->local_aabb, pNewCopyEnt->GetWorldPosition() );


	// link the new copy-entity to the top of 'm_pEntityInUse'
	if( m_pEntityInUse )
		pNewCopyEnt->SetNext( m_pEntityInUse );
	else
		pNewCopyEnt->SetNextToNull(); // first entity in the link list

	m_pEntityInUse = pNewCopyEntPtr;


	// set the created time of the entity
	pNewCopyEnt->m_CreatedTime = m_pStage->GetElapsedTime();

	// set parent entity
	pNewCopyEnt->m_pParent = pParent;
	if( pNewCopyEnt->m_pParent )
	{
		// 'pNewCopyEnt' is being created as a child of another copy entity
		pNewCopyEnt->m_pParent->AddChild( pNewCopyEnt->m_pSelf );	// establish link from the parent to this entity
	}

//	LOG_PRINT( "linking a copy entity of " + rBaseEntity.GetNameString() + " to the tree" );

	// link the new copy-entity to the entity-tree
	Link( pNewCopyEnt );

	// update light information
	if( pNewCopyEnt->Lighting() )
	{
		pNewCopyEnt->ClearLights();
//		UpdateLightInfo( pNewCopyEnt );

		pNewCopyEnt->sState |= CESTATE_LIGHT_INFORMATION_INVALID;
	}

	// create object for physics simulation
	if( pNewCopyEnt->GetEntityFlags() & BETYPE_RIGIDBODY )
	{
		CActor *pPhysActor = NULL;
		if( pPhysActorDesc )
		{
			pPhysActorDesc->WorldPose = pNewCopyEnt->GetWorldPose();// * pNewCopyEnt->GetActorLocalPose();
			pPhysActorDesc->BodyDesc.LinearVelocity = pNewCopyEnt->Velocity();

			// each entity has its own actor desc
			pPhysActor = m_pStage->GetPhysicsScene()->CreateActor( *pPhysActorDesc );
		}
		else if( pBaseEntity->GetPhysicsActorDesc().IsValid() )
		{
			// actor desc is defined by entity attributes
			CActorDesc actor_desc = pBaseEntity->GetPhysicsActorDesc();
			actor_desc.WorldPose = pNewCopyEnt->GetWorldPose();
			actor_desc.BodyDesc.LinearVelocity = pNewCopyEnt->Velocity();
//			pNewCopyEnt->pPhysicsActor = m_pStage->GetPhysicsScene()->CreateActor( actor_desc );
			pPhysActor = m_pStage->GetPhysicsScene()->CreateActor( actor_desc );
		}

		if( pPhysActor )
		{
			pNewCopyEnt->m_vecpPhysicsActor.resize( 1 );
			pNewCopyEnt->m_vecpPhysicsActor[0] = pPhysActor;
			pPhysActor->m_pFrameworkData = pNewCopyEnt;
		}
	}

	// When all the basic properties are copied, InitCopyEntity() is called to 
	// do additional initialization specific to each base entity.
	rBaseEntity.InitCopyEntity( pNewCopyEnt );
}


CCopyEntity *CEntitySet::CreateEntity( CBaseEntityHandle& rBaseEntityHandle,
									   const Vector3& rvPosition,
							           const Vector3& rvVelocity,
									   const Vector3& rvDirection)
{
	CCopyEntityDesc new_entity;

	new_entity.pBaseEntityHandle = &rBaseEntityHandle;

	new_entity.SetWorldPosition( rvPosition );
	new_entity.SetWorldOrient( CreateOrientFromFwdDir(rvDirection) );
	new_entity.vVelocity  = rvVelocity;
	new_entity.fSpeed = Vec3Length( rvVelocity );

	return CreateEntity( new_entity );
}


CCopyEntity *CEntitySet::CreateEntity( CCopyEntityDesc& rCopyEntityDesc )
{
	if( !rCopyEntityDesc.pBaseEntityHandle )
		return NULL;

	CBaseEntityHandle& rBaseEntityHandle = *(rCopyEntityDesc.pBaseEntityHandle);

//	PrintLog( "creating a copy entity of " + string(rBaseEntityHandle.GetBaseEntityName()) );

	CBaseEntity *pBaseEntity = GetBaseEntity( rBaseEntityHandle );
	if( !pBaseEntity )
		return NULL;

	CBaseEntity& rBaseEntity = *(pBaseEntity);

//	LOG_PRINT( "checking the initial position of " + rBaseEntity.GetNameString() );

	// determine the entity group id
	// priority (higher to lower):
	// (id set to copy entity desc) -> (id set to base entity)
	int entity_group_id = ENTITY_GROUP_INVALID_ID;
	if( rCopyEntityDesc.sGroupID != ENTITY_GROUP_INVALID_ID )
	{
		entity_group_id = rCopyEntityDesc.sGroupID;
	}
	else
	{
		// try the group of base entity
		entity_group_id = rBaseEntity.GetEntityGroupID();
	}

	if( !rBaseEntity.m_bNoClip )
	{	// check for overlaps with other entities
		// to see if the new entity is in a valid position
//		if( rCopyEnt.bvType == BVTYPE_AABB || rCopyEnt.bvType == BVTYPE_DOT )
//		{
			STrace tr;
			tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
			tr.vEnd       = rCopyEntityDesc.WorldPose.vPosition;
			tr.bvType     = rBaseEntity.m_BoundingVolumeType;
			tr.aabb       = rBaseEntity.m_aabb;
			tr.GroupIndex = entity_group_id;
			if( rBaseEntity.m_bNoClipAgainstMap )
				tr.sTraceType |= TRACETYPE_IGNORE_MAP;
			m_pStage->CheckPosition( tr );
			if(tr.in_solid)
			{
				LOG_PRINT( " - cannot create a copy entity due to overlaps: " + string(rBaseEntityHandle.GetBaseEntityName()) );
				return NULL;	// specified position is invalid - cannot create entity
			}
//		}
	}

//	g_Log.Print( "the copy entity of " + rBaseEntity.GetNameString() + " is in a valid position" );

	// create an entity
	shared_ptr<CCopyEntity> pNewEntitySharedPtr = m_pEntityFactory->CreateEntity( rCopyEntityDesc.TypeID );
	if( !pNewEntitySharedPtr )
	{
		/// too many entities or no entity is defined for rCopyEntityDesc.TypeID
		LOG_PRINT_ERROR( " - cannot create a copy entity of '" + string(rBaseEntityHandle.GetBaseEntityName()) + "'" );
		return NULL;
	}

	CCopyEntity *pNewCopyEnt = pNewEntitySharedPtr.get();

	pNewCopyEnt->m_TypeID    = rCopyEntityDesc.TypeID;

	// copy parameter values from base entity (entity attributes set)
	SetBasicEntityAttributes( pNewCopyEnt, rBaseEntity );

	// copy parameter values from entity desc

	pNewCopyEnt->SetName( rCopyEntityDesc.strName );

	pNewCopyEnt->SetWorldPose( rCopyEntityDesc.WorldPose );

	pNewCopyEnt->Velocity()  = rCopyEntityDesc.vVelocity;
	pNewCopyEnt->fSpeed      = rCopyEntityDesc.fSpeed;

	pNewCopyEnt->m_MeshHandle = rCopyEntityDesc.MeshObjectHandle;

	pNewCopyEnt->f1 = rCopyEntityDesc.f1;
	pNewCopyEnt->f2 = rCopyEntityDesc.f2;
	pNewCopyEnt->f3 = rCopyEntityDesc.f3;
	pNewCopyEnt->f4 = rCopyEntityDesc.f4;
	pNewCopyEnt->f5 = 0.0f;
	pNewCopyEnt->s1 = rCopyEntityDesc.s1;
	pNewCopyEnt->v1 = rCopyEntityDesc.v1;
	pNewCopyEnt->v2 = rCopyEntityDesc.v2;
	pNewCopyEnt->iExtraDataIndex = rCopyEntityDesc.iExtraDataIndex;
	pNewCopyEnt->pUserData = rCopyEntityDesc.pUserData;

	pNewCopyEnt->sState   = 0;
	pNewCopyEnt->bInSolid = false;

	pNewCopyEnt->GroupIndex = entity_group_id;

//	pNewCopyEnt->GroupIndex = rCopyEntityDesc.sGroupID;

	pNewCopyEnt->touch_plane.dist   = 0;
	pNewCopyEnt->touch_plane.normal = Vector3(0,0,0);

	if( rBaseEntity.m_bLighting )
		pNewCopyEnt->RaiseEntityFlags( BETYPE_LIGHTING );

//	pNewCopyEnt->bLighting = rBaseEntity.m_bLighting;

	InitEntity( pNewEntitySharedPtr, rCopyEntityDesc.pParent, pBaseEntity, rCopyEntityDesc.pPhysActorDesc );

	pNewCopyEnt->Init( rCopyEntityDesc );

	LOG_PRINT_VERBOSE( " - created a copy entity of " + rBaseEntity.GetNameString() );

	return pNewCopyEnt;
}


void CEntitySet::LoadCopyEntityFromDesc_r( CCopyEntityDescFileData& desc, CCopyEntity *pParentEntity )
{
	CBaseEntityHandle base_entity_handle;

	base_entity_handle.SetBaseEntityName( desc.strBaseEntityName.c_str() );

	// load the base entity for this copy entity
	LoadBaseEntity( base_entity_handle );

	// create the copy entity
	desc.CopyEntityDesc.pBaseEntityHandle = &base_entity_handle;

	desc.CopyEntityDesc.pParent = pParentEntity;

//	CreateEntity( desc.CopyEntityDesc );
	CCopyEntity *pEntity = m_pStage->CreateEntity( desc.CopyEntityDesc );

	if( !pEntity )
        LOG_PRINT_ERROR( "Failed to create a copy entity: " + string(base_entity_handle.GetBaseEntityName()) );
	else
        LOG_PRINT_ERROR( "Created a copy entity: "          + string(base_entity_handle.GetBaseEntityName()) );

	size_t i, num_children = desc.vecChild.size();
	for( i=0; i<num_children; i++ )
	{
		LoadCopyEntityFromDesc_r( desc.vecChild[i], pEntity );
	}
}


bool CEntitySet::LoadCopyEntitiesFromDescFile( char* pcFilename )
{
	CCopyEntityDescFileArchive entity_desc_archive;

	CBinaryArchive_Input input_archive( pcFilename );

	if( !(input_archive >> entity_desc_archive) )
	{
		LOG_PRINT_ERROR( "wrong .ent file" + string(pcFilename) );
		return false;
	}

	size_t i, num_entities = entity_desc_archive.GetNumEntityDescs();

	for( i=0; i<num_entities; i++ )
	{
		CCopyEntityDescFileData& desc = entity_desc_archive.GetCopyEntityDesc( (int)i );

		LoadCopyEntityFromDesc_r( desc, NULL );
	}

	return true;
}


CBaseEntity* CEntitySet::FindBaseEntity( const char* pcBaseEntityName )
{
	size_t i, num_base_entities = m_vecpBaseEntity.size();
	for(i=0; i<num_base_entities; i++)
	{
		if( strcmp(pcBaseEntityName, m_vecpBaseEntity[i]->GetName()) == 0 )
		{
			return m_vecpBaseEntity[i];
		}
	}
	return NULL;
}


bool CEntitySet::LoadBaseEntity( CBaseEntityHandle& base_entity_handle )
{
	const string base_entity_name = base_entity_handle.GetBaseEntityName();

	if( base_entity_name.length() == 0 )
	{
		base_entity_handle.SetState( CBaseEntityHandle::STATE_INVALID );
		return false; // base entity name has to be specified.
	}

	// find the base entity from the current list
	CBaseEntity *pBaseEntity = FindBaseEntity( base_entity_name.c_str() );

	if( pBaseEntity )
	{
		// already on the list - set the pointer to the handle and return
		base_entity_handle.SetBaseEntityPointer( pBaseEntity );
		base_entity_handle.SetState( CBaseEntityHandle::STATE_VALID );
		return true;
	}

	// not loaded yet - load the base entity and set the base entity pointer to the handle
	pBaseEntity = BaseEntityManager().LoadBaseEntity( base_entity_name );

	if( !pBaseEntity )
	{
		// the requested base entity was not found in the database
		// - mark the handle as invalid and return
		base_entity_handle.SetState( CBaseEntityHandle::STATE_INVALID );
		LOG_PRINT_ERROR( "the requested base entity '" + base_entity_name + "' was not found in the database" );
		return false;
	}

	// the base entity was successfully loaded from the database

	// set stage ptr
	pBaseEntity->SetStagePtr( m_pStage->GetWeakPtr() );

	m_vecpBaseEntity.push_back( pBaseEntity );

	// initialize - load textures, 3D models, other base entities, or so
	pBaseEntity->Init();

	// add to the sweep render table if all the copy entities should be rendered at once
	if( pBaseEntity->m_bSweepRender )
		m_pRenderManager->AddSweepRenderEntity( pBaseEntity );

	LOG_PRINT( " - name (confirm): " + pBaseEntity->GetNameString() );

	base_entity_handle.SetBaseEntityPointer( pBaseEntity );
	base_entity_handle.SetState( CBaseEntityHandle::STATE_VALID );

	return true;
}


void CEntitySet::UpdatePhysics( float frametime )
{
	PROFILE_FUNCTION();

//	frametime *= 0.5f;

	Scalar total_time = frametime + m_PhysOverlapTime;

	if (total_time > 0.1f)
		total_time = 0.1f;

	// split the timestep into fixed size chunks

	int num_loops = (int) (total_time / m_PhysTimestep);
	Scalar timestep = m_PhysTimestep;

	if ( false /*m_allow_smaller_timesteps*/ )
	{
		if (num_loops == 0)
			num_loops = 1;
		timestep = total_time / num_loops;
	}

	m_PhysOverlapTime = total_time - num_loops * timestep;

//	TCPreAllocDynamicLinkList<CJL_PhysicsActor>& rActorList = m_pStage->m_pPhysicsManager->GetActorList();

//	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

	int i;
	for (i=0 ; i<num_loops ; ++i)
	{
//		m_physics_time += timestep;

		// apply gravity
/*		for( itrActor = rActorList.Begin();
			itrActor != rActorList.End();
			itrActor++ )
		{
			if( !(itrActor->GetActorFlag() & JL_ACTOR_STATIC) )
                itrActor->AddWorldForce( itrActor->GetMass() * Vector3(0,-9.8f,0) );
		}
*/

		ProfileBegin( "Entity Update (Physics)" );

		// update physics properties that are specific to each entity
		// DO NOT CONFUSE THIS WITH CCopyEntity::UpdatePhysics()
		CCopyEntity *pEntity;
		for( pEntity = m_pEntityInUse.get();
			 pEntity != NULL;
			 pEntity = pEntity->m_pNextRawPtr )
		{
			if( pEntity->GetEntityFlags() & BETYPE_COPY_PARENT_POSE )
				pEntity->CopyParentPose();

//			if( pEntity->inuse && pEntity->pPhysicsActor )
			if( pEntity->inuse && 0 < pEntity->m_vecpPhysicsActor.size() )
                pEntity->pBaseEntity->UpdatePhysics( pEntity, timestep );

			UpdateEntityAfterMoving( pEntity );

		}

		ProfileEnd( "Entity Update (Physics)" );


		{
			PROFILE_SCOPE( "Physics Simulation" );

			// handle the motions and collisions of rigid body entities
//			m_pStage->m_pPhysicsManager->Integrate( timestep );
			m_pStage->GetPhysicsScene()->Simulate( timestep );

			while( !m_pStage->GetPhysicsScene()->FetchResults( physics::SimulationStatus::RigidBodyFinished ) )
			{}
		}

		// clear forces on actors
/*		for( itrActor = rActorList.Begin();
			itrActor != rActorList.End();
			itrActor++ )
		{
			itrActor->ClearForces();
		}
*/
	}
}


void CEntitySet::SetCollisionGroup( int group0, int group1, bool collision )
{
	if( group0 < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group0
	 || group1 < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group1 )
		return;

	m_EntityCollisionTable[group0][group1] = collision ? 1 : 0;
	m_EntityCollisionTable[group1][group0] = collision ? 1 : 0;

//	this->m_pStage->GetPhysicsManager()->SetCollisionGroupState( group0, group1, collision );
}


void CEntitySet::SetCollisionGroup( int group, bool collision )
{
	if( group < 0 || NUM_MAX_ENTITY_GROUP_IDS <= group )
		return;

	for( int i=0; i<NUM_MAX_ENTITY_GROUP_IDS; i++ )
	{
		m_EntityCollisionTable[group][i] = collision ? 1 : 0;

//		this->m_pStage->GetPhysicsManager()->SetCollisionGroupState( group0, group1, collision );
	}
}


void CEntitySet::UpdateEntityAfterMoving( CCopyEntity *pEntity )
{
  {
	PROFILE_FUNCTION();

	if( pEntity->PrevPosition() != pEntity->GetWorldPosition() )
	{
		//'pEntity' has moved during this frame
		pEntity->sState |= CESTATE_MOVED_DURING_LAST_FRAME;
		pEntity->sState |= CESTATE_LIGHT_INFORMATION_INVALID;

		// update the link in the entity tree
		pEntity->Unlink();
		Link( pEntity );

		// update world aabb
		pEntity->world_aabb.TransformCoord( pEntity->local_aabb, pEntity->GetWorldPosition() );
	}
	else
		pEntity->sState &= ~CESTATE_MOVED_DURING_LAST_FRAME;

  }
	if( true /*pEntity->GetEntityFlags() & AUTOMATICALLY_UPDATE_CHILDREN*/ )
	{
		const int num_children = pEntity->GetNumChildren();
		for( int i=0; i<num_children; i++ )
		{
			if( IsValidEntity( pEntity->GetChild(i) ) )
				UpdateEntityAfterMoving( pEntity->GetChild(i) );
		}
	}
}


/**
 Update all the entities cerrently existing in the stage.
 This function must be called once per frame.
 - Basic steps
   - 1. Call CBaseEntity::UpdateBaseEntity( dt ) for each base entity
   - 2. Save positions of copy entities
   - 3. Run physics simulator
   - 4. Remove terminated entities from the active list
   - 5. Call CCopyEntity::Act() for each copy entity except for child entities
   - 6. Update link to the entity tree node if an entity has changed its position in Act()
   TODO: Do 5 & 6 in a single loop to update link for each entity right after is Act().
         Current code does this in separate loops.
 */
void CEntitySet::UpdateAllEntities( float dt )
{
	CCopyEntity *pEntity = NULL;
	CCopyEntity *pPrevEntity = NULL;
	CCopyEntity *pTouchedEnt = NULL;

	ONCE( g_Log.Print( "CEntitySet::UpdateAllEntities() - updating base entities" ) );

	size_t i, num_base_entities = m_vecpBaseEntity.size();
	for( i=0; i<num_base_entities; i++ )
	{
		m_vecpBaseEntity[i]->UpdateBaseEntity( dt );
	}

	// save the current entity positions
	for( pEntity = m_pEntityInUse.get();
	     pEntity != NULL;
	     pEntity = pEntity->m_pNextRawPtr )
	{
		pEntity->PrevPosition() = pEntity->GetWorldPosition();
	}

	// run physics simulator
	// entity position may be modified in this call
	UpdatePhysics( dt );

	ONCE( g_Log.Print( "CEntitySet::UpdateAllEntities() - updated physics" ) );

	// remove terminated entities from the active entity list
	ReleaseTerminatedEntities();

	ONCE( g_Log.Print( "CEntitySet::UpdateAllEntities() - removed terminated entities from the active entity list" ) );

	// update active entities
	for( pEntity = this->m_pEntityInUse.get(), pPrevEntity = NULL;
		 pEntity != NULL;
		 pPrevEntity = pEntity, pEntity = pEntity->m_pNextRawPtr )
	{
		// before updating pEntity, check if it has been terminated.
		if( !pEntity->inuse )
			continue;

		// set the results of physics simulation to
		// pose, velocity and angular velocity of the entity
//		if( pEntity->pPhysicsActor && pEntity->GetEntityFlags() & BETYPE_USE_PHYSSIM_RESULTS )
		if( 0 < pEntity->m_vecpPhysicsActor.size() && pEntity->GetEntityFlags() & BETYPE_USE_PHYSSIM_RESULTS )
			pEntity->UpdatePhysics();

		if( pEntity->sState & CESTATE_ATREST )
			continue;

		if( !pEntity->m_pParent || !pEntity->m_pParent->inuse )
		{
			// 'pEntity' has no parent or its parent is already terminated
			pEntity->pBaseEntity->Act( pEntity );
		}

		if( !pEntity->inuse )
			continue; // terminated in its own update routine

		UpdateEntityAfterMoving( pEntity );

		// deal with entities touched during this frame
		for(int i=0; i<pEntity->vecpTouchedEntity.size(); i++)
		{
			pTouchedEnt = pEntity->vecpTouchedEntity[i];
			pEntity->pBaseEntity->Touch( pEntity, pTouchedEnt );

			if( pTouchedEnt )
				pTouchedEnt->pBaseEntity->Touch( pTouchedEnt, pEntity );
		}

		// clear touched entities for the next frame
		pEntity->vecpTouchedEntity.clear();
	}

	ONCE( g_Log.Print( "CEntitySet::UpdateAllEntities() - updated active entities" ) );

	// unlink and link the entity in the entity tree if it changed its position
/*
	for( pEntity = m_pEntityInUse.get();
		 pEntity != NULL;
		 pPrevEntity = pEntity, pEntity = pEntity->m_pNextRawPtr )
	{
		if( !pEntity->inuse )
			continue;

		UpdateEntityAfterMoving();
	}
*/
}


void CEntitySet::UpdateGraphics()
{
	if( m_pEntityInUse )
	{
		for( CCopyEntity* pEntity = m_pEntityInUse.get();
			 pEntity != NULL;
			 pEntity = pEntity->m_pNextRawPtr )
		{
			pEntity->UpdateGraphics();
		}
	}
}


void CEntitySet::GetBillboardRotationMatrix( Matrix33 &rmatBillboard ) const
{
	CCamera* pCamera = m_pStage->GetCurrentCamera();

	if( pCamera )
		pCamera->GetOrientation( rmatBillboard );
}


typedef std::pair<U32,const char *> flag_string_pair;

flag_string_pair g_EntityFlagStringPair[] =
{
	flag_string_pair( BETYPE_PLAYER,                      "PLAYER" ),
	flag_string_pair( BETYPE_TERRESTRIAL,                 "TERRESTRIAL" ),
	flag_string_pair( BETYPE_FLOATING,                    "FLOATING" ),
	flag_string_pair( BETYPE_ENEMY,                       "ENEMY" ),
	flag_string_pair( BETYPE_ITEM,                        "ITEM" ),
//	flag_string_pair( BETYPE_RIGIDBODY,                   "RIGIDBODY" ),
	flag_string_pair( BETYPE_RIGIDBODY,                   "PHYSICS_SIM" ),
	flag_string_pair( BETYPE_INDESTRUCTIBLE,              "INDESTRUCTIBLE" ),
	flag_string_pair( BETYPE_LIGHTING,                    "LIGHTING" ),
	flag_string_pair( BETYPE_NOCLIP,                      "NOCLIP" ),
	flag_string_pair( BETYPE_NOCLIP_WITH_STATIC_GEOMETRY, "NOCLIP_WITH_STATIC_GEOMETRY" ),
	flag_string_pair( BETYPE_GLARESOURCE,                 "GLARESOURCE" ),
	flag_string_pair( BETYPE_GLAREHINDER,                 "GLAREHINDER" ),
	flag_string_pair( BETYPE_USE_ZSORT,                   "USE_ZSORT" ),
	flag_string_pair( BETYPE_SUPPORT_TRANSPARENT_PARTS,   "SUPPORT_TRANSPARENT_PARTS" ),
	flag_string_pair( BETYPE_VISIBLE,                     "VISIBLE" ),
	flag_string_pair( BETYPE_USE_PHYSSIM_RESULTS,         "USE_PHYSSIM_RESULTS" ),
	flag_string_pair( BETYPE_ENVMAPTARGET,                "ENVMAPTARGET" ),
	flag_string_pair( BETYPE_PLANAR_REFLECTOR,            "PLANAR_REFLECTOR" ),
	flag_string_pair( BETYPE_SHADOW_CASTER,               "SHADOW_CASTER" ),
	flag_string_pair( BETYPE_SHADOW_RECEIVER,             "SHADOW_RECEIVER" )
};


void CEntitySet::WriteEntityTreeToFile( const string& filename )
{
	if( this->m_NumEntityNodes == 0 )
		return;		// EntityTree is not constructed

	FILE* fp;
	if( filename == "" )
		fp = fopen("entity_tree.txt", "w");		// use default filename
	else
		fp = fopen(filename.c_str(), "w");

	if( !fp )
		return;

	int i;
	string strPlaneNormal, strPos, strDir;

	fprintf(fp, "time: %f s\n", m_pStage->GetElapsedTime() );
	fprintf(fp, "total entity nodes: %d\n\n", m_NumEntityNodes );
	for(i=0; i<m_NumEntityNodes; i++)
	{
		//Write an entity node
		CEntityNode& rEntityNode = m_paEntityTree[i];
		fprintf(fp, "Node[%d]", i);
		if (rEntityNode.leaf)
			fprintf(fp, " (leaf) ====================================================\n");
		else
			fprintf(fp, " ===========================================================\n");

		fprintf(fp, "  P[%d]   F[%d] B[%d]\n",
			rEntityNode.sParent, rEntityNode.sFrontChild, rEntityNode.sBackChild);

		strPlaneNormal = to_string(rEntityNode.m_Plane.normal );
		fprintf(fp, "  plane: n%s, dist = %.2f\n", strPlaneNormal.c_str(), rEntityNode.m_Plane.dist);

		fprintf(fp, "  aabb: %s\n\n", to_string(rEntityNode.m_AABB).c_str() );

		// Write the copy entities linked to this entity node

		CCopyEntity* pEntity = NULL;
		CLinkNode<CCopyEntity> *pNode;
		for(
			pNode = rEntityNode.m_EntityLinkHead.pNext;
			pNode != NULL;
			pNode = pNode->pNext )
		{
			pEntity = pNode->pOwner;

			fprintf( fp, "--------------------------------------------------------\n" );

			fprintf( fp, "name:         %s\n", pEntity->GetName().c_str() );
			fprintf( fp, "id:           %d\n", pEntity->GetID() );
			fprintf( fp, "type id:      %d\n", pEntity->GetEntityTypeID() );
			fprintf( fp, "base name:    %s\n", pEntity->pBaseEntity->GetName() );
			fprintf( fp, "created time: %f s\n", pEntity->GetCreatedTime() );
			fprintf( fp, "group:        %d\n", pEntity->GroupIndex );

			strPos = to_string(pEntity->GetWorldPosition() );
			strDir = to_string(pEntity->GetDirection() );
			fprintf( fp, "  pos%s, dir%s, spd: %.2f\n", strPos.c_str(), strDir.c_str(), pEntity->fSpeed );

			fprintf( fp, "  local_aabb: %s\n", to_string(pEntity->local_aabb).c_str() );

			fprintf( fp, "  world_aabb: %s\n", to_string(pEntity->world_aabb).c_str() );

			fprintf(fp, "  state: %d\n", pEntity->sState);

			fprintf(fp, "flags: ");

			const U32 flags = pEntity->GetEntityFlags();
			bool first_flag = true;
			for( int j=0; j<numof(g_EntityFlagStringPair); j++ )
			{
				if( flags & g_EntityFlagStringPair[j].first )
				{
					fprintf( fp, "%s%s", first_flag ? "" : " | ", g_EntityFlagStringPair[j].second );
					first_flag = false;
				}
			}
			fprintf( fp, "\n" );

			if( pEntity->bInSolid == true ) fprintf(fp, "  (in solid)\n");
			else fprintf(fp, "  (not in solid)\n");

			fprintf(fp, "\n");
		}
	}

	fclose(fp);
}


/*
static float s_DirLightCheckDist = 100.0f;

static short s_asEntityNodeStack[256];

void CEntitySet::UpdateLightForEntity(CCopyEntity *pEntity)
{
	PROFILE_FUNCTION();

//	if( 1 ) // disable lights for entities
//		return;


	int iNumStackedNodes = 0;
//	static short s_asEntityNodeStack[256];

	CEntityNode *pEntityNode = m_paEntityTree;
	CLightEntity* pLightEntity;

	STrace tr;
	tr.bvType = BVTYPE_DOT;
	tr.pvGoal = &pEntity->GetWorldPosition();
//	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr.sTraceType = TRACETYPE_IGNORE_ALL_ENTITIES;
//	tr.pSourceEntity = pEntity;

	Vector3 vLightCenterPos, vLightToEntity, vLightRefPos;
	float r, d;

	CLinkNode<CLightEntity> *pLinkNode = NULL;
	while(1)
	{
		for( pLinkNode = pEntityNode->m_LightEntityLinkHead.pNext;
			 pLinkNode != NULL;
			 pLinkNode = pLinkNode->pNext )
//		for( pLightEntity = pEntityNode->m_LightEntityLinkHead.pNext;
//			 pLightEntity != NULL;
//			 pLightEntity = pLightEntity->GetNext() )
		{
			pLightEntity = pLinkNode->pOwner;

			// find lights that reach 'pEntity'
			bool lit = pLightEntity->ReachesEntity( pEntity );

			if( lit )
			{
				// register light to the entity
				pEntity->AddLight( CEntityHandle<CLightEntity>( pLightEntity->LightEntitySelf() ) );
			}

//			tr.pTouchedEntity = NULL;
//			tr.fFraction = 1.0f;
//
//			if( pLightEntity->GetLightType() == D3DLIGHT_POINT )
//			{
//				vLightCenterPos = pLightEntity->GetPosition();
//				tr.pvStart = &vLightCenterPos;
//			}
//			else if( pLightEntity->GetLightType() == D3DLIGHT_DIRECTIONAL )
//			{
//				vLightRefPos = pEntity->GetWorldPosition() - pLightEntity->GetHemisphericDirLight().vDirection * s_DirLightCheckDist;
//				tr.pvStart = &vLightRefPos;
//			}
//			else
//			{
//				MsgBox( "CEntitySet::UpdateLightForEntity() - invalid light entity" );
//				continue;
//			}
//
//			if( !IsSensible(vLightCenterPos) )
//				int iError = 1;
//
//			GetStage()->ClipTrace( tr );
//
////			if( tr.pTouchedEntity != pEntity )
//			if( tr.fFraction < 1.0f )
//				continue;	// static geometry obstacle between light and entity
//
//			if( pLightEntity->GetLightType() == D3DLIGHT_POINT )
//			{
//				fMaxRangeSq = pLightEntity->GetRadius() + pEntity->local_aabb.vMax.x;
//				fMaxRangeSq = fMaxRangeSq * fMaxRangeSq;
//				vLightToEntity = pEntity->GetWorldPosition() - vLightCenterPos;
//				if( fMaxRangeSq < Vec3LengthSq(vLightToEntity) )
//					continue;	// out of the light range
//			}
//
//			// register light to the entity
//			pEntity->AddLightIndex( pLightEntity->GetIndex() );

		}

		if( pEntityNode->leaf )
		{
			if( 0 < iNumStackedNodes )
			{	// see if there is any unchecked entity node left in stack
				iNumStackedNodes--;
				pEntityNode = &m_paEntityTree[ s_asEntityNodeStack[iNumStackedNodes] ];
				continue;
			}
			else
				break;
		}

		r = pEntity->local_aabb.vMax.x;

		d = pEntityNode->m_Plane.GetDistanceFromPoint( pEntity->GetWorldPosition() );

		if( r < d )
		{
			pEntityNode = &m_paEntityTree[pEntityNode->sFrontChild];
			continue;
		}	
		else if( d < -r )
		{
			pEntityNode = &m_paEntityTree[pEntityNode->sBackChild];
			continue;
		}
		else
		{	// need check both nodes

			// 1. store the index to the back child into stack - this will be handled later
			s_asEntityNodeStack[iNumStackedNodes++] = pEntityNode->sBackChild;

			// 2. recurse down to the front child 
			pEntityNode = &m_paEntityTree[pEntityNode->sFrontChild];
		}
	}
}
*/
