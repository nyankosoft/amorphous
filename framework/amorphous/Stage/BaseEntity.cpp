#include "BaseEntity.hpp"
#include "CopyEntity.hpp"
#include "CopyEntityDesc.hpp"
#include "AlphaEntity.hpp"
#include "trace.hpp"
#include "EntitySet.hpp"
#include "Stage.hpp"
#include "bsptree.hpp"

#include "amorphous/3DMath/3DGameMath.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/GameCommon/GameMathMisc.hpp"
#include "amorphous/GameCommon/MeshBoneControllerBase.hpp"
//#include "App/GameApplicationBase.hpp"
#include "amorphous/Task/GameTaskManager.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/Support/StringAux.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Utilities/TextFileScannerExtensions.hpp"

#include "amorphous/Physics/ActorDesc.hpp"
#include "amorphous/Physics/RaycastHit.hpp"
#include "amorphous/Physics.hpp"


namespace amorphous
{

using namespace std;
using std::map;
using std::shared_ptr;


//===============================================================================
// BaseEntity
//===============================================================================
BaseEntity::BaseEntity()
{
	this->m_bNoClip = false;
	this->m_bNoClipAgainstMap = false;

	m_bSweepRender = false;

	m_EntityFlag = 0;
	RaiseEntityFlag(BETYPE_VISIBLE);
//	m_EntityFlag = BETYPE_GLAREHINDER;

	// ENTITY_GROUP_MIN is set by default
	m_EntityGroup.SetID( ENTITY_GROUP_MIN_ID );

	m_BoundingVolumeType = BVTYPE_DOT;
	m_fRadius = 0;
	m_aabb.vMax = Vector3(0,0,0);
	m_aabb.vMin = Vector3(0,0,0);


	m_pBSPTree = NULL;

	m_fLife = 0;

	// lighting is disabled by dafault
	SetLighting( false );

}


BaseEntity::~BaseEntity()
{
	m_MeshProperty.Release();

	SafeDelete( m_pBSPTree );
}


void BaseEntity::SetStagePtr( CStageWeakPtr pStage )
{
	m_pStageWeakPtr = pStage;

	CStageSharedPtr stage_shared_ptr = pStage.lock();
	if( stage_shared_ptr.get() )
		m_pStage = stage_shared_ptr.get();
	else
		LOG_PRINT_ERROR( "Error: invalid stage ptr" );
}


bool BaseEntity::LoadBaseEntity( BaseEntityHandle& base_entity_handle )
{
	return m_pStage->LoadBaseEntity( base_entity_handle );
}


void BaseEntity::SetLighting( bool lighting )
{
	if( lighting )
		RaiseEntityFlag( BETYPE_LIGHTING );
	else
		ClearEntityFlag( BETYPE_LIGHTING );
}


int BaseEntity::GetEntityGroupID( EntityGroupHandle& entity_group_handle )
{
	int base_entity_group = entity_group_handle.GetID();
	if( base_entity_group == ENTITY_GROUP_ID_UNINITIALIZED )
	{
		// not initialized - try to get the id for the current entity group name
		if( 0 < entity_group_handle.GetGroupName().length() )
		{
			base_entity_group = m_pStage->GetEntitySet()->GetEntityGroupFromName( entity_group_handle.GetGroupName() );
			entity_group_handle.SetID( base_entity_group );
		}
		else
		{
			// No group name is specified by the user. Set the group 0
			base_entity_group = 0;
		}
	}

	return base_entity_group;
}


int BaseEntity::GetEntityGroupID()
{
	return GetEntityGroupID( m_EntityGroup );
}


// How to make a copy if T class has member variables of type shared_ptr
//template<class T>
//inline std::shared_ptr<T> create_copy( shared_ptr<T> p ) { return ; }


void BaseEntity::CreateAlphaEntities( CCopyEntity *pCopyEnt )
{
	if( pCopyEnt->GetEntityTypeID() == CCopyEntityTypeID::ALPHA_ENTITY )
		return;

	MeshHandle& mesh = pCopyEnt->m_MeshHandle;
	if( !mesh.IsLoaded() )
		return;

	// test with the plane model in the aircraft select menu
	BasicMesh *pMesh = mesh.GetMesh().get();//m_MeshProperty.m_MeshObjectHandle.GetMesh().get();
	if( !pMesh )
	{
//		pMesh = pCopyEnt->m_MeshHandle.GetMesh().get();
//		if( !pMesh )
			return;
	}

	// Got a valid mesh.
	// Create alpha entities for mesh materials that have transparency

	shared_ptr<MeshContainerRenderMethod> pContainerRenderMethod
		= pCopyEnt->m_pMeshRenderMethod;

	if( !pContainerRenderMethod )
		return;

	const float alpha_tolerance = 0.001f;

	const int num_materials = pMesh->GetNumMaterials();
	vector<int> subsets_with_transparency; // indices of subsets (material)
	for( int i=0; i<num_materials; i++ )
	{
		if( pMesh->Material(i).fMinVertexDiffuseAlpha < 1.0 - alpha_tolerance )
		{
			// need to create an alpha entity for this subset (material)
			subsets_with_transparency.push_back( i );
		}
	}

	if( subsets_with_transparency.empty() )
		return; // all the subsets are completely/almost opaque

	bool src_mesh_uses_subset_render_methods = 0 < pContainerRenderMethod->SubsetRenderMethodMaps().size();

//	if( !src_mesh_uses_subset_render_methods && pContainerRenderMethod->MeshRenderMethod().empty() )
//		return;

	// support only single LOD for now.

	// if the src mesh container renderer use a single render method to render all of its render subsets
	// break them into render methods for each subsets
	if( !src_mesh_uses_subset_render_methods )
	{
		// create the list of material names
		vector<string> subset_names;
		for( int i=0; i<num_materials; i++ )
			subset_names.push_back( pMesh->Material(i).Name );

		pContainerRenderMethod->BreakMeshRenderMethodsToSubsetRenderMethods( subset_names );
		pContainerRenderMethod->RenderMethodsAndSubsetIndices().resize( 0 );
	}

	const int num_alpha_subsets = (int)subsets_with_transparency.size();
	for( int i=0; i<num_alpha_subsets; i++ )
	{
		int mat_index = subsets_with_transparency[i];
		const string& mat_name = pMesh->Material(mat_index).Name;

		shared_ptr<MeshContainerRenderMethod> pContainerRenderMethodCopy
			= pContainerRenderMethod->CreateCopy();

		pContainerRenderMethodCopy->SubsetRenderMethodMaps().resize( 0 );

		BaseEntityHandle base_entity( "AlphaEntityBase" );
		CCopyEntityDesc desc;
		desc.TypeID            = CCopyEntityTypeID::ALPHA_ENTITY;
		desc.pBaseEntityHandle = &base_entity;
		desc.WorldPose         = pCopyEnt->GetWorldPose();
		desc.pParent           = pCopyEnt;
//		desc.NoCollision( true );

		vector< map<string,SubsetRenderMethod> >& mapDestSubsetRenderMethodMaps
			= pContainerRenderMethodCopy->SubsetRenderMethodMaps();

		mapDestSubsetRenderMethodMaps.resize(1); // single LOD

		map<string,SubsetRenderMethod>::iterator itr;
		itr = pContainerRenderMethod->SubsetRenderMethodMaps()[0].find( mat_name );
		if( itr != pContainerRenderMethod->SubsetRenderMethodMaps()[0].end() )
		{
			// copy the render method for the subset
			mapDestSubsetRenderMethodMaps[0][mat_name] = itr->second;

			// this subset is rendered by the alpha entity - remove the subset render method from the parent (pCopyEnt)
			pContainerRenderMethod->SubsetRenderMethodMaps()[0].erase( itr );
		}

		AlphaEntity *pEntity = dynamic_cast<AlphaEntity *>(m_pStage->CreateEntity( desc ));
		pEntity->m_MeshHandle = pCopyEnt->m_MeshHandle;
		pEntity->m_pMeshRenderMethod = pContainerRenderMethodCopy;

/*		if( pEntity )
		{
			pEntity->SetAlphaMaterialIndex( i );
			if( i < m_MeshProperty.m_ShaderTechnique.size_x()
			 && 0 < m_MeshProperty.m_ShaderTechnique.size_y() )
			 pEntity->SetShaderTechnique( m_MeshProperty.m_ShaderTechnique( 0, i ) );
		}*/

		// replace shader params loaders
		typedef std::pair<string,SubsetRenderMethod> str_and_rendermethod;
//		BOOST_FOREACH( str_and_rendermethod& p, mapDestSubsetRenderMethodMaps[0] )
		for( itr = mapDestSubsetRenderMethodMaps[0].begin();
			 itr != mapDestSubsetRenderMethodMaps[0].end();
			 itr++ )
		{
			// copy light info from parent entity?

/*			vector< shared_ptr<ShaderParamsLoader> >& vecpShaderParamsLoader
				= p.second.vecpShaderParamsLoader;

			shared_ptr<CEntityShaderLightParamsLoader> pLightParamsLoader
				= dynamic_pointer_cast<CEntityShaderLightParamsLoader,ShaderParamsLoader>(vecpShaderParamsLoader[i]);

			if( pLightParamsLoader )
				pLightParamsLoader->SetEntity( pCopyEnt->Self() );

			p.second->
*/
		}
	}
}


const physics::CActorDesc& BaseEntity::GetPhysicsActorDesc()
{
	static physics::CActorDesc empty_desc;
	return empty_desc;
}


// 'rTrace' is given in world coord
void BaseEntity::ClipTrace( STrace& rTrace, CCopyEntity* pMyself )
{
	PROFILE_FUNCTION();

	if( 0 < pMyself->m_vecpPhysicsActor.size()
	 && pMyself->m_vecpPhysicsActor[0] )
	{
		float trace_length = Vec3Length( rTrace.vGoal - rTrace.vStart );
		physics::CRay ray;
		ray.Origin    = rTrace.vStart;
		ray.Direction = ( rTrace.vGoal - rTrace.vStart ) / trace_length;
		physics::CRaycastHit hit;
		bool hit_detected = false;
		physics::CActor *pActor = pMyself->m_vecpPhysicsActor[0];

		// Overwrite the actor's pose with the entity's to synchronize
		// - Temporarily added to synchronize the poses of enemy aircrafts
		pActor->SetWorldPose( pMyself->GetWorldPose() );

		if( 0 < pActor->GetNumShapes() )
			hit_detected = pActor->GetShape(0)->Raycast( ray, trace_length, 0, hit, true );

		// bug: ray hit is not detected againt the box shape of enemy aircrafts.

		Matrix34 actor_world_pose = pMyself->m_vecpPhysicsActor[0]->GetWorldPose();

		if( hit_detected )
		{
			if( pMyself->GetName() == "player" )
				int break_here = 1;

			rTrace.vEnd = hit.WorldImpactPos;
			rTrace.fFraction = Vec3Length( hit.WorldImpactPos - rTrace.vStart ) / trace_length;
			rTrace.pTouchedEntity = pMyself;
		}

		return;
	}

	STrace local_trace = rTrace;
	Vector3 vS, vG;

	switch( m_BoundingVolumeType )
	{
	case BVTYPE_DOT:
		// No clipping of traces against a dot?
		break;

	case BVTYPE_AABB:
//	default:
		vS = rTrace.vStart - pMyself->GetWorldPosition();
		vG = rTrace.vGoal  - pMyself->GetWorldPosition();
		local_trace.vStart = vS;
		local_trace.vGoal  = vG;
		local_trace.fFraction = rTrace.fFraction;
		local_trace.vEnd = rTrace.vEnd - pMyself->GetWorldPosition();

		if( m_pBSPTree )
			m_pBSPTree->ClipTrace( local_trace );
		else
		{
			BSPTreeForAABB bsptree_aabb;
			bsptree_aabb.ClipTraceToAABB(local_trace, pMyself->local_aabb);
		}

		if( local_trace.fFraction < rTrace.fFraction )
		{	// update 'rTrace'
			rTrace.fFraction = local_trace.fFraction;
			rTrace.vEnd = local_trace.vEnd + pMyself->GetWorldPosition();
			rTrace.plane = local_trace.plane;
			rTrace.in_solid = local_trace.in_solid;
			rTrace.iMaterialIndex = -1;		// set invalid material index
		}
		break;

	case BVTYPE_OBB:
	case BVTYPE_CONVEX:
	case BVTYPE_COMPLEX:
		// transform 'rTrace' to local coord
		pMyself->GetWorldPose().InvTransform( vS, rTrace.vStart );
		pMyself->GetWorldPose().InvTransform( vG, rTrace.vGoal );
		local_trace.vStart = vS;
		local_trace.vGoal  = vG;
		local_trace.fFraction = rTrace.fFraction;
		pMyself->GetWorldPose().InvTransform( local_trace.vEnd, rTrace.vEnd );

		switch( local_trace.bvType )
		{
		case BVTYPE_DOT:
		case BVTYPE_SMALLSPHERE:
		case BVTYPE_OBB:
			if( m_pBSPTree )
				m_pBSPTree->ClipTrace( local_trace );
			else
			{
				// clip the trace by the bounding box of the entity 'pMyself'
				BSPTreeForAABB bspt;
				bspt.ClipTraceToAABB( local_trace, m_aabb );
			}

			if( local_trace.fFraction < rTrace.fFraction )
			{	// trace has been clipped - update 'rTrace'
				// transform 'local_trace' to world space and save to 'rTrace'
				rTrace.fFraction = local_trace.fFraction;
				pMyself->GetWorldPose().Transform( rTrace.vEnd, local_trace.vEnd );
				rTrace.plane = local_trace.plane;
				rTrace.plane.normal = pMyself->GetWorldPose().matOrient * local_trace.plane.normal;
				rTrace.plane.dist = Vec3Dot( rTrace.plane.normal, rTrace.vEnd );
				rTrace.in_solid = local_trace.in_solid;
				rTrace.iMaterialIndex = -1;		// set invalid material index
//				if( !IsSensible( rTrace.vEnd ) )
//					int error = 1;
			}
			break;
		}

		break;	/* case BVTYPE_OBB: */
	}
}


//free fall of spherical or aabb object
void BaseEntity::FreeFall(CCopyEntity* pCopyEnt)
{
	if( pCopyEnt->sState & CESTATE_ATREST )
		return;

	STrace tr;
	float fFrametime = m_pStage->GetFrameTime();
	Vector3 vVel = pCopyEnt->GetDirection() * pCopyEnt->fSpeed;
	Vector3 vGravityAccel = this->m_pStage->GetGravityAccel();

	vVel += vGravityAccel * fFrametime;	//Update velocity;
	
	Vector3 vStart = pCopyEnt->GetWorldPosition();
	tr.vStart = vStart;
	Vector3 vGoal = pCopyEnt->GetWorldPosition() + fFrametime * vVel;
	tr.vGoal = vGoal;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fRadius = this->m_fRadius;
	tr.pSourceEntity = pCopyEnt;

	this->m_pStage->ClipTrace( tr );

	if( tr.in_solid )
	{
		NudgePosition( pCopyEnt );
		pCopyEnt->sState |= CESTATE_ATREST;
		pCopyEnt->fSpeed = 0;
		pCopyEnt->SetDirection( Vector3(0,0,0) );
		return;
	}

	if( tr.fFraction < 1.0f )	//hit something
	{
		if( tr.plane.normal.y >= 0.7f )		//touch down
		{
			pCopyEnt->fSpeed = 0;
			pCopyEnt->SetDirection( Vector3(0,0,0) );
			pCopyEnt->sState |= CESTATE_ATREST;
			pCopyEnt->SetWorldPosition( tr.vEnd );
			return;
		}
		else	//hit steep surface  (vertical wall, etc.)
		{
			ClipVelocity(vVel, vVel, tr.plane.normal, 0.1f);
			tr.vEnd += tr.plane.normal * 0.002f;
		}
	}

	pCopyEnt->fSpeed = Vec3Length(vVel);
	pCopyEnt->SetDirection( vVel / pCopyEnt->fSpeed );
	pCopyEnt->SetWorldPosition( tr.vEnd );

}


#define MAX_CLIP_PLANES 5

char BaseEntity::SlideMove(CCopyEntity* pCopyEnt)
{
	char cBumpCount,cNumBumps;
	Vector3 vDir;
	float d;
	char numPlanes;
	Vector3 vNormals[MAX_CLIP_PLANES];
	Vector3 vPrimalVel, vOrigVel;
	char i,j;
	Vector3 vGoalPos;
	float fTimeLeft;
	char blocked;
	STrace tr;

	cNumBumps = 4;

	blocked = 0;
	vOrigVel = pCopyEnt->vVelocity;
	vPrimalVel = pCopyEnt->vVelocity;
	numPlanes = 0;

	fTimeLeft = m_pStage->GetFrameTime();

	if( fTimeLeft == 0.0f )
		return blocked;

	for(cBumpCount = 0; cBumpCount < cNumBumps; cBumpCount++)
	{
		vGoalPos = pCopyEnt->GetWorldPosition() + fTimeLeft * pCopyEnt->vVelocity;

		Vector3 vStart = pCopyEnt->GetWorldPosition();
		tr.vStart = vStart;
		tr.vGoal = vGoalPos;
		tr.aabb = this->m_aabb;
		tr.bvType = this->m_BoundingVolumeType;
		tr.fFraction = 1;
		tr.pSourceEntity = pCopyEnt;
		tr.pTouchedEntity = NULL;
		this->m_pStage->ClipTrace( tr );

		pCopyEnt->touch_plane = tr.plane;

		if(tr.in_solid)
		{	//trapped in solid
			pCopyEnt->vVelocity = Vector3(0,0,0);
			pCopyEnt->bInSolid = true;
			return 3;
		}

		if(0 < tr.fFraction)
		{//actually covered some distance
			pCopyEnt->SetWorldPosition( tr.vEnd );
			numPlanes = 0;		
		}

		//save entity for contact - modified on Nov. 14, 2004
		if( tr.pTouchedEntity || tr.fFraction < 1.0f )
		{	// hit an entity or the map
			pCopyEnt->AddTouchedEntity( tr.pTouchedEntity );
		}

		if(tr.fFraction == 1)  //moved the entire distance
			break;

		//(tr.fFraction < 1), i.e. the trace hit something
		//espacially, when ( tr.fFraction == 0 ), the entity is colliding to multiple planes at the same time

		fTimeLeft -= fTimeLeft * tr.fFraction;

		//save entity for contact
//		pCopyEnt->AddTouchedEntity( tr.pTouchedEntity );  // commented out on Nov. 14, 2004

		//test touch functions
/*		if( !tr.pTouchedEntity )
			pCopyEnt->Touch( NULL );
		else if( tr.pTouchedEntity->inuse )
		{
			pCopyEnt->Touch( tr.pTouchedEntity );

			tr.pTouchedEntity->Touch( pCopyEnt );
		}*/

		if(0.7f < tr.plane.normal.y)
		{
			blocked |= 1;	//slope or floor with small gradient (less than about 45 degrees)
		}
		if(!tr.plane.normal.y)
		{
			blocked |= 2;	//step (vertical wall)
		}

		//clipped to another plane
		if(numPlanes >= MAX_CLIP_PLANES)
		{	//this shouldn't really happen
			//MessageBox(NULL, "Simultaneous hits exceeded MAX_CLIP_PLANES", "Error", MB_OK|MB_ICONWARNING);
			pCopyEnt->vVelocity = Vector3(0,0,0);
			break;
		}
		vNormals[numPlanes++] = tr.plane.normal;

	//At this point
	// -'vNormals[]' & 'numPlanes' are set. they hold mutiple 
	// plane-normals if the player is touching more than two planes
	// at the same time.
	// -'tr.vEndPos' shows the currently colliding position.

	//
	//modify vOrigVel so it parallels all of the clip planes
	//
	//When (numPlanes == 1),the follwing for-loop will be broken
	//while (i = 0)
		for(i=0; i<numPlanes; i++)
		{
			ClipVelocity(pCopyEnt->vVelocity, vOrigVel, vNormals[i]);
			for(j=0; j<numPlanes; j++)
			{
				if(j != i)
				{
					if(Vec3Dot( pCopyEnt->vVelocity, vNormals[j]) < 0)
						break;  //not ok. Velocity is facing to the plane
				}
			}
			if(j == numPlanes)
				break;  //Velocity is not facing to any plane.
			//else {Velocity is facing to a plane, so clip Velocity with another plane if there is an unused plane.}
		}
		
		if(i != numPlanes ) //Velocity is not facing to any plane.
		{	//Velocity has been successfully clipped.
			//go along this plane.
		}
		else
		{// go along the crease
			if(numPlanes != 2) //Actually, numPlanes > 2
			{//unfortunately, bumping into more than 3 planes at the same time.
				pCopyEnt->vVelocity = Vector3(0,0,0);
				break;
			}
			Vec3Cross( vDir, vNormals[0], vNormals[1] );
			d = Vec3Dot( vDir, pCopyEnt->vVelocity );
			pCopyEnt->vVelocity = vDir * d;
		}

		if( Vec3Dot( pCopyEnt->vVelocity, vPrimalVel) < 0 )
		{
			pCopyEnt->vVelocity = Vector3(0,0,0);
			break;
		}

	}

	//In original QUAKE, there are codes about waterjump here

	return blocked;
}

void BaseEntity::GroundMove(CCopyEntity* pCopyEnt)
{
	Vector3 vOrigPos, vOrigVel, vDownDestPos, vDownVel, vGoal;
	float fDowndistSq, fUpdistSq;
	STrace tr;

	pCopyEnt->vVelocity.y = 0;
	if( pCopyEnt->vVelocity == Vector3(0,0,0) )
		return;

	//first try moving to the destination
	//Remember 'vVelocity.y' is 0, so there is no vertical motion
	vGoal = pCopyEnt->GetWorldPosition() + pCopyEnt->vVelocity * m_pStage->GetFrameTime();

	//first try moving to the next spot
	Vector3 vStart = pCopyEnt->GetWorldPosition();
	tr.vStart = vStart;
	tr.vGoal = vGoal;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fFraction = 1;
	tr.pSourceEntity = pCopyEnt;
	this->m_pStage->ClipTrace( tr );

	if(tr.fFraction == 1)
	{	//No collision occured
		pCopyEnt->SetWorldPosition( vGoal );
		return;
	}

	//try sliding forward both on ground & up ___ pixels
	//take the move that goes farthest
	vOrigPos = pCopyEnt->GetWorldPosition();
	vOrigVel = pCopyEnt->vVelocity;

	//slide move
	SlideMove(pCopyEnt);

	vDownDestPos = pCopyEnt->GetWorldPosition();
	vDownVel = pCopyEnt->vVelocity;

	pCopyEnt->SetWorldPosition( vOrigPos );
	pCopyEnt->vVelocity = vOrigVel;

	//move up a stair height
	vGoal = pCopyEnt->GetWorldPosition();
	vGoal.y += 0.3f;                        //Step Size

	vStart = pCopyEnt->GetWorldPosition();
	tr.vStart = vStart;
	tr.vGoal = vGoal;
	tr.fFraction = 1;
	tr.pSourceEntity = pCopyEnt;
	this->m_pStage->ClipTrace( tr );

	//Unable to step up (something is hitting player's head)
	if(tr.fFraction == 0)
		goto usedown;

	pCopyEnt->SetWorldPosition( tr.vEnd );

	SlideMove(pCopyEnt);  //Velocity.y == 0, so move horizontally

	//press down the stepheight
	vGoal = pCopyEnt->GetWorldPosition();
	vGoal.y -= 0.3f;                        //Step Size

	vStart = pCopyEnt->GetWorldPosition();
	tr.vStart = vStart;
	tr.vGoal = vGoal;
	tr.fFraction = 1;
	tr.pSourceEntity = pCopyEnt;
	this->m_pStage->ClipTrace( tr );

	if(tr.fFraction != 1) //collided to something while stepping down
	{
		if(tr.plane.normal.y < 0.7f)  //Stepped on a steep slope
			goto usedown;
		pCopyEnt->SetWorldPosition( tr.vEnd );
	}
	else
		pCopyEnt->SetWorldPosition( tr.vEnd );

	//decide which one went farther
	fDowndistSq = (vDownDestPos.x - vOrigPos.x) * (vDownDestPos.x - vOrigPos.x)
		+ (vDownDestPos.z - vOrigPos.z) * (vDownDestPos.z - vOrigPos.z);

	fUpdistSq = (pCopyEnt->GetWorldPosition().x - vOrigPos.x) * (pCopyEnt->GetWorldPosition().x - vOrigPos.x)
		+ (pCopyEnt->GetWorldPosition().z - vOrigPos.z) * (pCopyEnt->GetWorldPosition().z - vOrigPos.z);

	if(fDowndistSq > fUpdistSq)
	{
usedown:
	pCopyEnt->SetWorldPosition( vDownDestPos );
	pCopyEnt->vVelocity = vDownVel;
	}
	else //copy z value from the slide move
		pCopyEnt->vVelocity.y = vDownVel.y;

	//if at a dead stop, retry the move with nudges 
	//to get around the lips

	return;
}


void BaseEntity::ApplyFriction(CCopyEntity* pCopyEnt, float fFriction)
{
	float fSpeed;
	STrace tr;
	Vector3 vStart, vGoal;

	//In, original QUAKE
	//if(during the waterjump) return;

	fSpeed = Vec3Length(pCopyEnt->vVelocity);
	if(fSpeed < 0.03f)
	{
		pCopyEnt->vVelocity.x = 0;
		pCopyEnt->vVelocity.z = 0;
		return;
	}

	Vector3 vOrigVelDir = pCopyEnt->vVelocity / fSpeed;

	vStart = pCopyEnt->GetWorldPosition();
	//if the leading edge is over a dropoff, increase friction
/*	if(pCopyEnt->sState & CESTATE_ONGROUND)
	{
		vStart.x += pCopyEnt->vVelocity.x / fSpeed * pCopyEnt->fRadius;
		vStart.z += pCopyEnt->vVelocity.z / fSpeed * pCopyEnt->fRadius;
		vGoal = vStart;
		vGoal.y -= 0.9f;
	}
	tr.vStart = vStart;
	tr.vGoal =  vGoal;
	tr.aabb = pCopyEnt->local_aabb;
	tr.pSourceEntity = pCopyEnt;

	this->m_pStage->ClipTrace(tr);

	if(tr.fFraction == 1)
		fFriction *= 2;*/

	ApplyQFriction( fSpeed, m_pStage->GetFrameTime(), fFriction, 3.125f );

	// update velocity
	pCopyEnt->vVelocity = vOrigVelDir * fSpeed;

	return;
}


void BaseEntity::ApplyFriction(float& rfSpeed, float fFriction)
{
	ApplyQFriction( rfSpeed, m_pStage->GetFrameTime(), fFriction, 3.125f );
}


void BaseEntity::Accelerate(CCopyEntity* pCopyEnt,
							 Vector3& vWishdir, float& fWishspeed, float fAccel)
{
	float fAddspeed, fAccelspeed, fCurrentspeed;

	//if(player is dead) return;
	//if(waterjumping) return;

	fCurrentspeed = Vec3Dot(pCopyEnt->vVelocity, vWishdir);  //the length of Velocity in wish direction
	fAddspeed = fWishspeed - fCurrentspeed;
	if(fAddspeed < 0)
		return;
	fAccelspeed = fAccel * m_pStage->GetFrameTime() * fWishspeed;
	if(fAccelspeed > fAddspeed)   //fAddspeed is the maximum of the increase of Velocity
		fAccelspeed = fAddspeed;

	pCopyEnt->Velocity() += vWishdir * fAccelspeed;
	return;
}


void BaseEntity::AirAccelerate(CCopyEntity* pCopyEnt,
				   Vector3& vWishdir, float& fWishspeed, float fAccel)
{
	float fAddspeed, fAccelspeed, fCurrentspeed;

	//if(player is dead) return;
	//if(waterjumping) return;

	if(fWishspeed > 1)
		fWishspeed = 1;

	fCurrentspeed = Vec3Dot( pCopyEnt->vVelocity, vWishdir);  //the length of Velocity in wish direction
	fAddspeed = fWishspeed - fCurrentspeed;
	if(fAddspeed < 0)
		return;
	fAccelspeed = fAccel * m_pStage->GetFrameTime() * fWishspeed;
	if(fAccelspeed > fAddspeed)   //fAddspeed is the maximum of the increase of Velocity
		fAccelspeed = fAddspeed;

	pCopyEnt->vVelocity += vWishdir * fAccelspeed;
	return;
}


//only checks if the entity is on or off the ground
void BaseEntity::CategorizePosition(CCopyEntity* pCopyEnt)
{
	Vector3 vPoint;
	STrace tr;

	// FIX NEXT LINE
	if( 3 < pCopyEnt->vVelocity.y ) //going up with large velocity
	{
		pCopyEnt->sState &= ~CESTATE_ONGROUND;	//entity is off the ground
	}
	else
	{
		// If copy-entity's point one unit down is solid, it is on the ground.
		vPoint = pCopyEnt->GetWorldPosition();
		vPoint.y -= 0.02f;  //2cm
		Vector3 vStart = pCopyEnt->GetWorldPosition();
		tr.vStart = vStart;
		tr.vGoal = vPoint;
		tr.aabb = pCopyEnt->local_aabb;
		tr.pSourceEntity = pCopyEnt;

		this->m_pStage->ClipTrace(tr);

		if( tr.fFraction < 1.0f )
		{	//hit something
			if( tr.plane.normal.y < 0.7f ) //if staning on a steep slope
				pCopyEnt->sState &= ~CESTATE_ONGROUND;  //regard as off-ground
			else
			{	//on ground
				pCopyEnt->sState |= CESTATE_ONGROUND;

				if(!tr.in_solid)
					pCopyEnt->SetWorldPosition( tr.vEnd );
			}
		}
		else
			pCopyEnt->sState &= ~CESTATE_ONGROUND;  //regard as off-ground

		//Here, in original QUAKE,
		//if standing on an entity other than the world, modify the
		//value of 'numtouch' & 'touchindex[]'

	}

	return;
}


#define GRIDSIZE 0.0004f
#define INV_GRIDSIZE 2500

void BaseEntity::NudgePosition(CCopyEntity* pCopyEnt)
{
	Vector3 vBase;
	static char x, y, z;
	static int sign[3] = {0, 1, -1};
	STrace tr;

	tr.aabb = pCopyEnt->local_aabb;
	tr.bvType = pCopyEnt->bvType;
	tr.vEnd = pCopyEnt->GetWorldPosition();
	tr.pSourceEntity = pCopyEnt;
	Vector3& vPos = tr.vEnd;
	vPos.x = ((int)(vPos.x * 2500)) * 0.0004f;
	vPos.y = ((int)(vPos.y * 2500)) * 0.0004f;
	vPos.z = ((int)(vPos.z * 2500)) * 0.0004f;

	vBase = vPos;
	for(y=0; y<3; y++)
	{
/*		if(0.5 < m_fLower)
		{
			m_cPose = POS_CROUCHING;
			m_fLower -= GRIDSIZE;
			vPos.y -= GRIDSIZE;
		}*/
		for(z=0; z<3; z++)
		{
			for(x=0; x<3; x++)
			{
				vPos.x = vBase.x + (sign[x] * 0.0004f);
				vPos.y = vBase.y + (sign[y] * 0.0004f);
				vPos.z = vBase.z + (sign[z] * 0.0004f);

				tr.in_solid = false;
				m_pStage->CheckPosition( tr );
				if( !tr.in_solid )
					return;
			}
		}
	}
	vPos = vBase;
//	MessageBox(NULL, "NudgePosition: stuck", "Error", MB_OK|MB_ICONWARNING);
	return;
}


void BaseEntity::LoadFromFile( CTextFileScanner& scanner )
{
	string str, tag, strFilename;
	Vector3 vSide;
	bool rigid_body = false;
	bool envmap = false;
	string strCurrentLine;
	scanner.GetCurrentLine( strCurrentLine );

	string tex_filename;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		tag = scanner.GetTagString();

		if( scanner.TryScanLine( "NAME",          m_strName ) ) continue;
		else if( scanner.TryScanLine( "AABB_MAX", m_aabb.vMax ) ) continue;
		else if( scanner.TryScanLine( "AABB_MIN", m_aabb.vMin ) ) continue;

		else if( scanner.TryScanLine( "AABB", vSide ) )
		{
			// aabb represented by full lengths
			m_aabb.vMin = -vSide * 0.5f;
			m_aabb.vMax =  vSide * 0.5f;
			continue;
		}

		if( scanner.TryScanLine( "BV_TYPE", str ) )
		{
			if( str == "BVTYPE_DOT" )			m_BoundingVolumeType = BVTYPE_DOT;
			else if( str == "BVTYPE_AABB" )		m_BoundingVolumeType = BVTYPE_AABB;
			else if( str == "BVTYPE_OBB" )		m_BoundingVolumeType = BVTYPE_OBB;
			else if( str == "BVTYPE_CONVEX" )	m_BoundingVolumeType = BVTYPE_CONVEX;
			else if( str == "BVTYPE_COMPLEX" )	m_BoundingVolumeType = BVTYPE_COMPLEX;
			continue;
		}

		if( scanner.TryScanLine( "NOCLIP", str ) )
		{
			if( str == "TRUE" )
				m_bNoClip = true;
		}

		if( scanner.TryScanLine( "NO_COLLISION_WITH_STATICGEOMETRY", str ) )
		{
			if( str == "TRUE" )
				m_bNoClipAgainstMap = true;
		}

		if( scanner.TryScanLine( "BSPTREE", strFilename ) )
		{
			m_pBSPTree = new BSPTree;
			m_pBSPTree->LoadFromFile( strFilename.c_str() );

			// output the bsp-tree to a text file
			string tree_output_file = "debug/" + strFilename + ".txt";
			m_pBSPTree->WriteToFile( tree_output_file.c_str() );
			continue;
		}

		if( scanner.TryScanLine( "ENTITYGROUP", str ) )
		{
			m_EntityGroup.SetGroupName( str );
			continue;
		}

		if( scanner.TryScanLine( "LIFE", m_fLife ) )	continue;

		if( scanner.TryScanBool( "ENVMAP", "TRUE/FALSE", envmap ) )
		{
			if( envmap )
				RaiseEntityFlag( BETYPE_ENVMAPTARGET );
			else
				ClearEntityFlag( BETYPE_ENVMAPTARGET );
		}

		vector<string> separated_str;
		if( scanner.TryScanLine( "FLAGS", str ) )
		{
			SeparateStrings( separated_str, str.c_str(), "|" );
			size_t num_flags = separated_str.size();
			for( size_t i=0; i<num_flags; i++ )
			{
				string flag_str = separated_str[i];
				if( flag_str == "PLAYER" )                           m_EntityFlag |= BETYPE_PLAYER;
				else if( flag_str == "TERRESTRIAL" )                 m_EntityFlag |= BETYPE_TERRESTRIAL;
				else if( flag_str == "FLOATING" )                    m_EntityFlag |= BETYPE_FLOATING;
				else if( flag_str == "ENEMY" )                       m_EntityFlag |= BETYPE_ENEMY;
				else if( flag_str == "ITEM" )                        m_EntityFlag |= BETYPE_ITEM;
				else if( flag_str == "RIGIDBODY" )                   m_EntityFlag |= BETYPE_RIGIDBODY;
				else if( flag_str == "PHYSICS_SIM" )                 m_EntityFlag |= BETYPE_RIGIDBODY;
				else if( flag_str == "INDESTRUCTIBLE" )              m_EntityFlag |= BETYPE_INDESTRUCTIBLE;
				else if( flag_str == "LIGHTING" )                    m_EntityFlag |= BETYPE_LIGHTING;
				else if( flag_str == "NOCLIP" )                      m_EntityFlag |= BETYPE_NOCLIP;
				else if( flag_str == "NOCLIP_WITH_STATIC_GEOMETRY" ) m_EntityFlag |= BETYPE_NOCLIP_WITH_STATIC_GEOMETRY;
				else if( flag_str == "GLARESOURCE" )                 m_EntityFlag |= BETYPE_GLARESOURCE;
				else if( flag_str == "GLAREHINDER" )                 m_EntityFlag |= BETYPE_GLAREHINDER;
				else if( flag_str == "USE_ZSORT" )                   m_EntityFlag |= BETYPE_USE_ZSORT;
				else if( flag_str == "SUPPORT_TRANSPARENT_PARTS" )   m_EntityFlag |= BETYPE_SUPPORT_TRANSPARENT_PARTS;
//				else if( flag_str == "VISIBLE" )                     m_EntityFlag |= BETYPE_VISIBLE;
				else if( flag_str == "USE_PHYSSIM_RESULTS" )         m_EntityFlag |= BETYPE_USE_PHYSSIM_RESULTS;
				else if( flag_str == "ENVMAPTARGET" )                m_EntityFlag |= BETYPE_ENVMAPTARGET;
				else if( flag_str == "BETYPE_PLANAR_REFLECTOR" )     m_EntityFlag |= BETYPE_PLANAR_REFLECTOR;
				else if( flag_str == "SHADOW_CASTER" )               m_EntityFlag |= BETYPE_SHADOW_CASTER;
				else if( flag_str == "SHADOW_RECEIVER" )             m_EntityFlag |= BETYPE_SHADOW_RECEIVER;
				// Create entites to sort transparant alpha
			}
		}

//		if( scanner.GetTagString() == "TRANSLUCENT" )
//			m_EntityFlag |=  BETYPE_TRANSLUCENT;

		if( scanner.GetTagString() == "INDESTRUCTIBLE" )
			m_EntityFlag |=  BETYPE_INDESTRUCTIBLE;

//		if( scanner.GetTagString() == "RIGIDBODY" )
//			m_EntityFlag |=  BETYPE_RIGIDBODY;

		if( scanner.TryScanBool( "RIGIDBODY", "TRUE/FALSE", rigid_body ) )
		{
			if( rigid_body )
				RaiseEntityFlag( BETYPE_RIGIDBODY );
			else
				ClearEntityFlag( BETYPE_RIGIDBODY );
		}

		if( scanner.TryScanLine( "3DMODEL",  m_MeshProperty.m_MeshDesc.ResourcePath ) )
			continue;

		string model_to_generate;
		model_to_generate.clear();
		if( scanner.TryScanLine( "GENERATED_3DMODEL",  model_to_generate ) )
//		if( scanner.GetCurrentLine().find( "GENERATED_3DMODEL" ) ==  )
		{
			if( 0 < model_to_generate.length() )
				CreateMeshGenerator( scanner );
		}

		string alpha_mat_name;
		if( scanner.TryScanLine( "ALPHA_MATERIAL", alpha_mat_name ) )
		{
			m_MeshProperty.m_vecTransparentMaterialName.push_back( alpha_mat_name );
		}

		if( scanner.TryScanLine( "SPEC_TEX", tex_filename ) )
		{
			m_MeshProperty.m_vecExtraTexture.push_back( TextureHandle() );
			m_MeshProperty.m_vecExtraTextureFilepath.push_back( tex_filename );
			continue;
		}

		if( scanner.TryScanLine( "LIGHTING", str ) )
		{
			if( str == "TRUE" )
				SetLighting( true );
			else
				SetLighting( false );
			continue;
		}

		if( scanner.TryScanLine( "MESH_TYPE", str ) )
		{
			MeshType::Name type = MeshType::INVALID;
			if( str == "BASIC" )            type = MeshType::BASIC;
			else if( str == "PROGRESSIVE" ) type = MeshType::PROGRESSIVE;
			else if( str == "SKELETAL" )    type = MeshType::SKELETAL;
			m_MeshProperty.m_MeshDesc.MeshType = type;
		}

		/*
			// single material, no shader LOD
			SHADER_TECH  '(technique name string)'

			or 

			// multi-material, with shader LOD
			SHADER_TECH.MAT[0]  'tech_00'  'tech_01'  'tech_02'
			SHADER_TECH.MAT[1]  'tech_10'  'tech_11'  'tech_12'
			SHADER_TECH.MAT[2]  'tech_20'  'tech_21'  'tech_22'
		*/
		if( scanner.TryScanLine( "SHADER_TECH", str ) )
		{
			LOG_PRINT_VERBOSE( fmt_string("setting a shader technique: %s %s", m_strName.c_str(), str.c_str()) );

			array2d<ShaderTechniqueHandle>& rShaderTechTable = m_MeshProperty.m_ShaderTechnique;
			if( strCurrentLine[ strlen("SHADERTECH") ] != '.' )
			{
				// single material, no shader LOD
				rShaderTechTable.resize(1,1);
				rShaderTechTable(0,0).SetTechniqueName( str.c_str() );
			}
			else
			{
				const int mat = rShaderTechTable.size_y();
				char tag_str[32];
				char tech_str[3][64];
				memset( tech_str, 0, sizeof(tech_str) );

				// multi-material, with shader LOD
				if( rShaderTechTable.size_x() == 0 )
					rShaderTechTable.resize( 1, 3 ); /// 3 levels of LOD
				else
					rShaderTechTable.increase_y( 1 );

				sscanf( strCurrentLine.c_str(), "%s %s %s %s", tag_str, tech_str[0], tech_str[1], tech_str[2] );

				for( int tech=0; tech<3; tech++ )
					rShaderTechTable( mat, tech ).SetTechniqueName( tech_str[tech] );
			}
		}

		if( tag == "END" || tag == "END\n" )
			break;

		LoadSpecificPropertiesFromFile( scanner );
	}

	Sphere sphere = m_aabb.CreateBoundingSphere();
	m_fRadius = sphere.radius;

}


void BaseEntity::ReleaseGraphicsResources()
{
	m_MeshProperty.Release();
}


void BaseEntity::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	Init3DModel();
}


void BaseEntity::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_strName;

	ar & m_fRadius;
	ar & m_aabb;
	ar & m_BoundingVolumeType;

	ar & m_EntityFlag;

	ar & m_EntityGroup;

	ar & m_bNoClip;
	ar & m_bNoClipAgainstMap;

	ar & m_fLife;

	ar & m_bSweepRender;

	ar & m_MeshProperty;
}


} // namespace amorphous
