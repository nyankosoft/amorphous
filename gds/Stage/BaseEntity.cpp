
#include "BaseEntity.h"
#include "CopyEntity.h"
#include "trace.h"
#include "EntitySet.h"
#include "Stage.h"
#include "bsptree.h"
#include "ScreenEffectManager.h"


#include "GameCommon/GameMathMisc.h"
#include "GameCommon/MeshBoneControllerBase.h"

#include "3DCommon/3DGameMath.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/D3DXMeshObject.h"
#include "3DCommon/D3DXSMeshObject.h"

#include "App/ApplicationBase.h"
#include "Task/GameTaskManager.h"

#include "Support/memory_helpers.h"
#include "Support/StringAux.h"
#include "Support/Profile.h"
#include "Support/Log/DefaultLog.h"
#include "Support/msgbox.h"

using namespace std;


//===============================================================================
// CBE_MeshObjectProperty
//===============================================================================

CBE_MeshObjectProperty::CBE_MeshObjectProperty()
:
m_PropertyFlags(0)
{
	m_ShaderTechnique.resize(1,1);
	m_ShaderTechnique(0,0).SetTechniqueName( "NoShader" );
	m_MeshObjectHandle.SetMeshType( CD3DXMeshObjectBase::TYPE_MESH );
}


CBE_MeshObjectProperty::CBE_MeshObjectProperty( const string& filename )
:
m_PropertyFlags(0)
{
	m_ShaderTechnique.resize(1,1);
	m_ShaderTechnique(0,0).SetTechniqueName( "NoShader" );
	m_MeshObjectHandle.SetMeshType( CD3DXMeshObjectBase::TYPE_MESH );
}


CBE_MeshObjectProperty::~CBE_MeshObjectProperty()
{
	// texture is released in the destructor of 'CTextureHandle'
//	m_SpecTex.Release();
}


void CBE_MeshObjectProperty::ValidateShaderTechniqueTable()
{
	CD3DXMeshObjectBase *pMeshObject = m_MeshObjectHandle.GetMeshObject();
	if( !pMeshObject )
		return;

	if( m_ShaderTechnique.size_y() == 0 )
	{
		m_ShaderTechnique.resize(1,1);
		m_ShaderTechnique(0,0).SetTechniqueName( "NoShader" );
	}
	else if( m_ShaderTechnique.size_y() == 1 && 2 <= pMeshObject->GetNumMaterials() )
	{
		// only one technique and 2 or more materials
		// - assumes the user wants to render all the materials with the same shader technique 
		m_PropertyFlags |= PF_USE_SINGLE_TECHNIQUE_FOR_ALL_MATERIALS;
	}
	else if( m_ShaderTechnique.size_y() < pMeshObject->GetNumMaterials() )
	{
		int num_orig_rows = m_ShaderTechnique.size_y();

		// (the number of techniques) < (the number of materials)
		// - copy the last technique and make sure there are as many techniques as materials

		// increase the columns to cover all the materials
		m_ShaderTechnique.increase_y( pMeshObject->GetNumMaterials() - m_ShaderTechnique.size_y() );

		// overwrite increased rows with the last technique for each LOD
		int lod, num_lods = m_ShaderTechnique.size_x(); // columns: for LODs
		int row, num_rows = m_ShaderTechnique.size_y(); // rows: for materials
		for( lod=0; lod<num_lods; lod++ )
		{
			for( row=num_orig_rows; row<num_rows; row++ )
			{
				m_ShaderTechnique( lod, row )
					= m_ShaderTechnique( lod, num_orig_rows - 1 );
			}
		}
	}
}


bool CBE_MeshObjectProperty::LoadMeshObject()
{
	m_MeshObjectHandle.Load();

	// validate shader technique table
	CD3DXMeshObjectBase *pMeshObject = m_MeshObjectHandle.GetMeshObject();
	if( pMeshObject )
	{
		ValidateShaderTechniqueTable();
	}

/*
	load mesh (standard, progressive or skeletal)
	support .x file
	...

	if( 0 < m_SpecTex.filename.length() )
	{
		m_SpecTex.Load();
	}
*/
	return true;
}


void CBE_MeshObjectProperty::Release()
{
	SafeDeleteVector( m_vecpMeshBoneController );
}


//===============================================================================
// CBaseEntity
//===============================================================================
CBaseEntity::CBaseEntity()
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

	m_bLighting = false;	// lighting is disabled by dafault

}


CBaseEntity::~CBaseEntity()
{
	m_MeshProperty.Release();

	SafeDelete( m_pBSPTree );
}


void CBaseEntity::SetStagePtr( CStageWeakPtr pStage )
{
	m_pStageWeakPtr = pStage;

	CStageSharedPtr stage_shared_ptr = pStage.lock();
	if( stage_shared_ptr.get() )
		m_pStage = stage_shared_ptr.get();
	else
		g_Log.Print( WL_ERROR, "%s - error: invalid stage ptr", __FUNCTION__ );
}


bool CBaseEntity::LoadBaseEntity( CBaseEntityHandle& base_entity_handle )
{
	return m_pStage->LoadBaseEntity( base_entity_handle );
}


int CBaseEntity::GetEntityGroupID( CEntityGroupHandle& entity_group_handle )
{
	int base_entity_group = entity_group_handle.GetID();
	if( base_entity_group == ENTITY_GROUP_ID_UNINITIALIZED )
	{
		// not initialized - try to get the id for the current entity group name
		base_entity_group = m_pStage->GetEntitySet()->GetEntityGroupFromName( entity_group_handle.GetGroupName() );
		entity_group_handle.SetID( base_entity_group );
	}

	return base_entity_group;
}


int CBaseEntity::GetEntityGroupID()
{
	return GetEntityGroupID( m_EntityGroup );
}


void CBaseEntity::Init3DModel()
{
	m_MeshProperty.LoadMeshObject();
}


// 'rTrace' is given in world coord
void CBaseEntity::ClipTrace( STrace& rTrace, CCopyEntity* pMyself )
{
	ProfileBegin( "CBaseEntity::ClipTrace()" );

	STrace local_trace = rTrace;
	Vector3 vS, vG;
//	D3DXMATRIX matRot_inv;

	switch( m_BoundingVolumeType )
	{
	case BVTYPE_AABB:
		vS = *rTrace.pvStart - pMyself->Position();
		vG = *rTrace.pvGoal  - pMyself->Position();
		local_trace.pvStart = &vS;
		local_trace.pvGoal = &vG;
		local_trace.fFraction = rTrace.fFraction;
		local_trace.vEnd = rTrace.vEnd - pMyself->Position();

		if( m_pBSPTree )
			m_pBSPTree->ClipTrace( local_trace );
		else
		{
			CBSPTreeForAABB bsptree_aabb;
			bsptree_aabb.ClipTraceToAABB(local_trace, pMyself->local_aabb);
		}

		if( local_trace.fFraction < rTrace.fFraction )
		{	// update 'rTrace'
			rTrace.fFraction = local_trace.fFraction;
			rTrace.vEnd = local_trace.vEnd + pMyself->Position();
			rTrace.plane = local_trace.plane;
			rTrace.in_solid = local_trace.in_solid;
			rTrace.iMaterialIndex = -1;		// set invalid material index
		}
		break;

	case BVTYPE_OBB:
	case BVTYPE_CONVEX:
	case BVTYPE_COMPLEX:
		// transform 'rTrace' to local coord
		pMyself->GetWorldPose().InvTransform( vS, *rTrace.pvStart );
		pMyself->GetWorldPose().InvTransform( vG, *rTrace.pvGoal );
		local_trace.pvStart = &vS;
		local_trace.pvGoal = &vG;
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
				CBSPTreeForAABB bspt;
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

	ProfileEnd( "CBaseEntity::ClipTrace()" );
}


//free fall of spherical or aabb object
void CBaseEntity::FreeFall(CCopyEntity* pCopyEnt)
{
	if( pCopyEnt->sState & CESTATE_ATREST )
		return;

	STrace tr;
	float fFrametime = m_pStage->GetFrameTime();
	Vector3 vVel = pCopyEnt->GetDirection() * pCopyEnt->fSpeed;
	Vector3 vGravityAccel = this->m_pStage->GetGravityAccel();

	vVel += vGravityAccel * fFrametime;	//Update velocity;
	
	tr.pvStart = &pCopyEnt->Position();
	Vector3 vGoal = pCopyEnt->Position() + fFrametime * vVel;
	tr.pvGoal = &vGoal;
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
			pCopyEnt->Position() = tr.vEnd;
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
	pCopyEnt->Position() = tr.vEnd;

}


#define MAX_CLIP_PLANES 5

char CBaseEntity::SlideMove(CCopyEntity* pCopyEnt)
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
		vGoalPos = pCopyEnt->Position() + fTimeLeft * pCopyEnt->vVelocity;

		tr.pvStart = &pCopyEnt->Position();
		tr.pvGoal = &vGoalPos;
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
			pCopyEnt->Position() = tr.vEnd;
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

void CBaseEntity::GroundMove(CCopyEntity* pCopyEnt)
{
	D3DXVECTOR3 vOrigPos, vOrigVel, vDownDestPos, vDownVel, vGoal;
	float fDowndistSq, fUpdistSq;
	STrace tr;

	pCopyEnt->vVelocity.y = 0;
	if( pCopyEnt->vVelocity == D3DXVECTOR3(0,0,0) )
		return;

	//first try moving to the destination
	//Remember 'vVelocity.y' is 0, so there is no vertical motion
	vGoal = pCopyEnt->Position() + pCopyEnt->vVelocity * m_pStage->GetFrameTime();

	//first try moving to the next spot
	tr.pvStart = &pCopyEnt->Position();
	tr.pvGoal = &vGoal;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fFraction = 1;
	tr.pSourceEntity = pCopyEnt;
	this->m_pStage->ClipTrace( tr );

	if(tr.fFraction == 1)
	{	//No collision occured
		pCopyEnt->Position() = vGoal;
		return;
	}

	//try sliding forward both on ground & up ___ pixels
	//take the move that goes farthest
	vOrigPos = pCopyEnt->Position();
	vOrigVel = pCopyEnt->vVelocity;

	//slide move
	SlideMove(pCopyEnt);

	vDownDestPos = pCopyEnt->Position();
	vDownVel = pCopyEnt->vVelocity;

	pCopyEnt->Position() = vOrigPos;
	pCopyEnt->vVelocity = vOrigVel;

	//move up a stair height
	vGoal = pCopyEnt->Position();
	vGoal.y += 0.3f;                        //Step Size

	tr.pvStart = &pCopyEnt->Position();
	tr.pvGoal = &vGoal;
	tr.fFraction = 1;
	tr.pSourceEntity = pCopyEnt;
	this->m_pStage->ClipTrace( tr );

	//Unable to step up (something is hitting player's head)
	if(tr.fFraction == 0)
		goto usedown;

	pCopyEnt->Position() = tr.vEnd;

	SlideMove(pCopyEnt);  //Velocity.y == 0, so move horizontally

	//press down the stepheight
	vGoal = pCopyEnt->Position();
	vGoal.y -= 0.3f;                        //Step Size

	tr.pvStart = &pCopyEnt->Position();
	tr.pvGoal = &vGoal;
	tr.fFraction = 1;
	tr.pSourceEntity = pCopyEnt;
	this->m_pStage->ClipTrace( tr );

	if(tr.fFraction != 1) //collided to something while stepping down
	{
		if(tr.plane.normal.y < 0.7f)  //Stepped on a steep slope
			goto usedown;
		pCopyEnt->Position() = tr.vEnd;
	}
	else
		pCopyEnt->Position() = tr.vEnd;

	//decide which one went farther
	fDowndistSq = (vDownDestPos.x - vOrigPos.x) * (vDownDestPos.x - vOrigPos.x)
		+ (vDownDestPos.z - vOrigPos.z) * (vDownDestPos.z - vOrigPos.z);

	fUpdistSq = (pCopyEnt->Position().x - vOrigPos.x) * (pCopyEnt->Position().x - vOrigPos.x)
		+ (pCopyEnt->Position().z - vOrigPos.z) * (pCopyEnt->Position().z - vOrigPos.z);

	if(fDowndistSq > fUpdistSq)
	{
usedown:
	pCopyEnt->Position() = vDownDestPos;
	pCopyEnt->vVelocity = vDownVel;
	}
	else //copy z value from the slide move
		pCopyEnt->vVelocity.y = vDownVel.y;

	//if at a dead stop, retry the move with nudges 
	//to get around the lips

	return;
}


void CBaseEntity::ApplyFriction(CCopyEntity* pCopyEnt, float fFriction)
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

	vStart = pCopyEnt->Position();
	//if the leading edge is over a dropoff, increase friction
/*	if(pCopyEnt->sState & CESTATE_ONGROUND)
	{
		vStart.x += pCopyEnt->vVelocity.x / fSpeed * pCopyEnt->fRadius;
		vStart.z += pCopyEnt->vVelocity.z / fSpeed * pCopyEnt->fRadius;
		vGoal = vStart;
		vGoal.y -= 0.9f;
	}
	tr.pvStart = &vStart;
	tr.pvGoal =  &vGoal;
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


void CBaseEntity::ApplyFriction(float& rfSpeed, float fFriction)
{
	ApplyQFriction( rfSpeed, m_pStage->GetFrameTime(), fFriction, 3.125f );
}


void CBaseEntity::Accelerate(CCopyEntity* pCopyEnt,
							 Vector3& vWishdir, float& fWishspeed, float fAccel)
{
	float fAddspeed, fAccelspeed, fCurrentspeed;

	//if(player is dead) return;
	//if(waterjumping) return;

	fCurrentspeed = Vec3Dot(&pCopyEnt->vVelocity, &vWishdir);  //the length of Velocity in wish direction
	fAddspeed = fWishspeed - fCurrentspeed;
	if(fAddspeed < 0)
		return;
	fAccelspeed = fAccel * m_pStage->GetFrameTime() * fWishspeed;
	if(fAccelspeed > fAddspeed)   //fAddspeed is the maximum of the increase of Velocity
		fAccelspeed = fAddspeed;

	pCopyEnt->Velocity() += vWishdir * fAccelspeed;
	return;
}


void CBaseEntity::AirAccelerate(CCopyEntity* pCopyEnt,
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
void CBaseEntity::CategorizePosition(CCopyEntity* pCopyEnt)
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
		vPoint = pCopyEnt->Position();
		vPoint.y -= 0.02f;  //2cm
		tr.pvStart = &pCopyEnt->Position();
		tr.pvGoal = &vPoint;
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
					pCopyEnt->Position() = tr.vEnd;
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

void CBaseEntity::NudgePosition(CCopyEntity* pCopyEnt)
{
	Vector3 vBase;
	static char x, y, z;
	static int sign[3] = {0, 1, -1};
	STrace tr;

	tr.aabb = pCopyEnt->local_aabb;
	tr.bvType = pCopyEnt->bvType;
	tr.vEnd = pCopyEnt->Position();
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

static void LogTaskError()
{
	if( !g_pAppBase )
		g_Log.Print( "%s 'g_pAppBase' is NULL", __FILE__ );
	else if( !g_pAppBase->GetTaskManager() )
		g_Log.Print( "%s g_pAppBase->GetTaskManager() == NULL", __FILE__ );
	else if( !g_pAppBase->GetTaskManager()->GetCurrentTask() )
		g_Log.Print( "%s g_pAppBase->GetTaskManager()->GetCurrentTask() == NULL", __FILE__ );
	else
		g_Log.Print( "%s unknown error with app, task manager or task", __FILE__ );
}

/*
void CBaseEntity::RequestTaskTransition( const std::string& next_task_title,
	                                     float delay_in_sec,
	                                     float fade_out_time_in_sec,
								         float fade_in_time_in_sec )
{
	if( g_pAppBase
	 && g_pAppBase->GetTaskManager()
	 && g_pAppBase->GetTaskManager()->GetCurrentTask() )
	{
		g_pAppBase->GetTaskManager()->GetCurrentTask()->RequestTaskTransition(
			next_task_title,
			delay_in_sec,
			fade_out_time_in_sec,
			fade_in_time_in_sec );
	}
	else
		LogTaskError();
}


void CBaseEntity::RequestTaskTransitionMS( const std::string& next_task_title,
								           int delay_in_ms,
	                                       int fade_out_time_in_ms,
								           int fade_in_time_in_ms )
{
	if( g_pAppBase
	 && g_pAppBase->GetTaskManager()
	 && g_pAppBase->GetTaskManager()->GetCurrentTask() )
	{
		g_pAppBase->GetTaskManager()->GetCurrentTask()->RequestTaskTransitionMS(
			next_task_title,
			fade_out_time_in_ms,
			fade_in_time_in_ms,
			delay_in_ms );
	}
	else
		LogTaskError();
}
*/

void CBaseEntity::LoadFromFile( CTextFileScanner& scanner )
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
			m_pBSPTree = new CBSPTree;
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
				else if( flag_str == "INDESTRUCTIBLE" )              m_EntityFlag |= BETYPE_INDESTRUCTIBLE;
				else if( flag_str == "LIGHTING" )                    m_EntityFlag |= BETYPE_LIGHTING;
				else if( flag_str == "NOCLIP" )                      m_EntityFlag |= BETYPE_NOCLIP;
				else if( flag_str == "NOCLIP_WITH_STATIC_GEOMETRY" ) m_EntityFlag |= BETYPE_NOCLIP_WITH_STATIC_GEOMETRY;
				else if( flag_str == "GLARESOURCE" )                 m_EntityFlag |= BETYPE_GLARESOURCE;
				else if( flag_str == "GLAREHINDER" )                 m_EntityFlag |= BETYPE_GLAREHINDER;
				else if( flag_str == "USE_ZSORT" )                   m_EntityFlag |= BETYPE_USE_ZSORT;
//				else if( flag_str == "VISIBLE" )                     m_EntityFlag |= BETYPE_VISIBLE;
				else if( flag_str == "USE_PHYSSIM_RESULTS" )         m_EntityFlag |= BETYPE_USE_PHYSSIM_RESULTS;
				else if( flag_str == "ENVMAPTARGET" )                m_EntityFlag |= BETYPE_ENVMAPTARGET;
				else if( flag_str == "SHADOW_CASTER" )               m_EntityFlag |= BETYPE_SHADOW_CASTER;
				else if( flag_str == "SHADOW_RECEIVER" )             m_EntityFlag |= BETYPE_SHADOW_RECEIVER;
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

		if( scanner.TryScanLine( "3DMODEL",  m_MeshProperty.m_MeshObjectHandle.filename ) )
			continue;

		if( scanner.TryScanLine( "SPEC_TEX", tex_filename ) )
		{
			m_MeshProperty.m_vecExtraTexture.push_back( CTextureHandle() );
			m_MeshProperty.m_vecExtraTexture.back().filename = tex_filename;
			continue;
		}

		if( scanner.TryScanLine( "LIGHTING", str ) )
		{
			if( str == "TRUE" )
				m_bLighting = true;
			else
				m_bLighting = false;
			continue;
		}

		if( scanner.TryScanLine( "MESH_TYPE", str ) )
		{
			int type = CD3DXMeshObjectBase::TYPE_MESH;
			if( str == "SIMPLE" )			type = CD3DXMeshObjectBase::TYPE_MESH;
			if( str == "PROGRESSIVE" )		type = CD3DXMeshObjectBase::TYPE_PMESH;
			if( str == "HIERARCHICAL" )		type = CD3DXMeshObjectBase::TYPE_SMESH;
//			if( str == "DYNAMICRESOURCE" )	type = ;
			m_MeshProperty.m_MeshObjectHandle.SetMeshType( type );
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
			g_Log.Print( "setting a shader technique: %s %s", m_strName.c_str(), str.c_str() );

			C2DArray<CShaderTechniqueHandle>& rShaderTechTable = m_MeshProperty.m_ShaderTechnique;
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


void CBaseEntity::ReleaseGraphicsResources()
{
	m_MeshProperty.Release();
}


void CBaseEntity::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	Init3DModel();
}


void CBaseEntity::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_strName;

	ar & m_fRadius & m_aabb;
	ar & m_BoundingVolumeType;

	ar & m_EntityFlag;

	ar & m_EntityGroup;

	ar & m_bNoClip;
	ar & m_bNoClipAgainstMap;

	ar & m_bLighting;

	ar & m_bSweepRender;

	ar & m_fLife;

	ar & m_MeshProperty;
}
