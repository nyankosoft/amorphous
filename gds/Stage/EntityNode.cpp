#include "EntityNode.h"
#include "EntitySet.h"
#include "EntityRenderManager.h"
#include "BaseEntity.h"
#include "bsptree.h"
#include "trace.h"
#include "ViewFrustumTest.h"
#include "Stage.h"
#include "LightEntity.h"

#include "Support/Profile.h"


int CEntityNode::ms_NumRenderedEntities = 0;


//====================================================================================
// CEntityNode::Method()                           - CCopyEntity : public CCopyEntity
//====================================================================================

CEntityNode::CEntityNode()
:
m_pEntitySet(NULL),
m_pStage(NULL)
{
	m_AABB.Nullify();
	m_Plane		= SPlane( Vector3(0,0,0), 0 );
	leaf		= false;
	sFrontChild = 0;
	sBackChild  = 0;
	sParent		= 0;
	m_sCellIndex= -1;
}

/*
inline bool CEntityNode::CheckCollisionGroup( int group_index, const vector<int>& vecTargetGroup )
{
	const size_t num_tgt_groups = vecTargetGroup.size();
	if( num_tgt_groups == 0 )
	{
		// no group is specified - overlaps with any entity will be registered
		return true;
	}
	else
	{
		size_t i;
		for( i=0; i<num_tgt_groups; i++ )
		{
//			const int& tgt_grp_index = overlap_test.vecTargetGroup[i];

//			if( m_pEntitySet->m_EntityCollisionGroup[group_index][tgt_grp_index] == 1 )
			if( m_pEntitySet->IsCollisionEnabled( group_index, vecTargetGroup[i] ) )
				break;
		}
		if( i < num_tgt_groups )
			return true;
		else
			return false;
	}
}*/


inline bool CEntityNode::CheckCollisionGroup( int group0, int group1 )
{
	return m_pEntitySet->IsCollisionEnabled( group0, group1 );
}


//=====================================================================
//   Rendering Copy Entities
//               Draw all the copy entities linked to this node
//=====================================================================
void CEntityNode::Render( CCamera& rCam )
{
//	if( 0 <= m_sCellIndex &&		// check if the current entity node is a complete leaf
//		!m_pStage->IsCurrentlyVisibleCell(m_sCellIndex) )
//		return;	// entities in the current entity node is not visible because the corresponding cell is not visible

	// Get the pointer to the first copy entity on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

		if( !(pEntity->EntityFlag & BETYPE_VISIBLE) )
			continue;

		if( pEntity->EntityFlag & BETYPE_USE_ZSORT )
		{	// 'pEntity' includes transparent polygons
			this->m_pEntitySet->GetRenderManager()->SendToZSortTable( pEntity );
		}
		else
		{
/*			if( pEntity->Lighting() )
			{
				if( pEntity->sState & CESTATE_LIGHT_INFORMATION_INVALID )
				{
					// need to update light information - find lights that reaches to this entity
					pEntity->ClearLights();
					m_pEntitySet->UpdateLightInfo( pEntity );
					pEntity->sState &= ~CESTATE_LIGHT_INFORMATION_INVALID;
				}

				// turn on lights that reach 'pCopyEnt'
				m_pEntitySet->EnableLightForEntity();
				m_pEntitySet->SetLightsForEntity( pEntity );
			}
			else
			{	// turn off lights
				m_pEntitySet->DisableLightForEntity();
			}*/

			// render the entity
			pEntity->Draw();
		}

		ms_NumRenderedEntities++;
	}
}


void CEntityNode::RenderShadowCasters( CCamera& rCam )
{
//	if( 0 <= m_sCellIndex &&		// check if the current entity node is a complete leaf
//		!m_pStage->IsCurrentlyVisibleCell(m_sCellIndex) )
//		return;	// entities in the current entity node is not visible because the corresponding cell is not visible

	// Get the pointer to the first copy entity on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

		if( (pEntity->EntityFlag & BETYPE_VISIBLE)
		 && (pEntity->EntityFlag & BETYPE_SHADOW_CASTER) )
		{
			// render the entity as a shadow caster
			pEntity->pBaseEntity->RenderAsShaderCaster( pEntity );
		}
	}
}


void CEntityNode::RenderShadowReceivers( CCamera& rCam )
{
//	if( 0 <= m_sCellIndex &&		// check if the current entity node is a complete leaf
//		!m_pStage->IsCurrentlyVisibleCell(m_sCellIndex) )
//		return;	// entities in the current entity node is not visible because the corresponding cell is not visible

	// Get the pointer to the first copy entity on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

		if( (pEntity->EntityFlag & BETYPE_VISIBLE)
		 && (pEntity->EntityFlag & BETYPE_SHADOW_RECEIVER) )
		{
			// render the entity as a shadow receiver
			pEntity->pBaseEntity->RenderAsShaderReceiver( pEntity );
		}
	}
}


void CEntityNode::RenderAllButEnvMapTraget( CCamera& rCam, U32 target_entity_id  )
{
//	if( 0 <= m_sCellIndex &&		// check if the current entity node is a complete leaf
//		!m_pStage->IsCurrentlyVisibleCell(m_sCellIndex) )
//		return;	// entities in the current entity node is not visible because the corresponding cell is not visible

	// Get the pointer to the first copy entity on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

		if( pEntity->GetID() == target_entity_id
		 && pEntity->EntityFlag  & BETYPE_ENVMAPTARGET )
		{
			// envmap target
			// - should no be rendered to the cube texture of its own envmap
			continue;
		}

		pEntity->pBaseEntity->Draw( pEntity );
	}
}


void CEntityNode::CheckPosition_r(STrace& tr, CEntityNode* paEntTree)
{
	float d, fRadius;

	// Set the first copy entity on this entity node and cycle through all the copy entities
	// on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

		if( pEntity->bNoClip )
			continue;

		if( pEntity->bvType == BVTYPE_DOT && tr.bvType == BVTYPE_DOT )
			continue;	// Points never interact

		if( tr.pSourceEntity == pEntity )
			continue;	// Do not clip against myself

		if( !tr.aabb_swept.IsIntersectingWith( pEntity->world_aabb ) )
			continue;

		// check collision group table
//		if( !CheckCollisionGroup( pEntity->GroupIndex, tr.vecTargetGroup ) )
		if( !CheckCollisionGroup( tr.GroupIndex, pEntity->GroupIndex ) )
			continue;

/**		if( pEntity->pBaseEntity->m_pBSPTree )
		{	// 'pEntity' has a bsp-tree that represents its volume. we need to run the overlap test using that bsp-tree
			if( pEntity->pBaseEntity->m_pBSPTree->CheckPosition(tr) != CONTENTS_SOLID )
				continue;	// tr is out of the 
		}
**/
		tr.in_solid = true;
		tr.pTouchedEntity = pEntity;
		return;
	}


	// collision handling in this 'EntityNode' has finished
	// now, we recurse down 'EntityTree', if this 'EntityNode' is not a leaf node

	if( this->leaf ) return;

	d = m_Plane.GetDistanceFromPoint( tr.aabb_swept.GetCenterPosition() );
	fRadius = tr.aabb_swept.GetRadiusForPlane( m_Plane );

	if(fRadius < d)
		paEntTree[ this->sFrontChild ].CheckPosition_r( tr, paEntTree );
	else if(d < -fRadius)
		paEntTree[ this->sBackChild ].CheckPosition_r( tr, paEntTree );
	else
	{
		paEntTree[ this->sFrontChild ].CheckPosition_r( tr, paEntTree );
		paEntTree[ this->sBackChild ].CheckPosition_r( tr, paEntTree );
	}

	return;
}

void CEntityNode::CheckPosition_r(CTrace& tr, CEntityNode* paEntTree)
{
	float d, fRadius;

	Vector3 vDist; 

	// Set the first copy entity on this entity node and cycle through all the copy entities
	// on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

		if( pEntity->bNoClip && (tr.GetTraceType() & TRACETYPE_IGNORE_NOCLIP_ENTITIES) )
			continue;

		if( pEntity->bvType == BVTYPE_DOT && tr.GetBVType() == BVTYPE_DOT )
			continue;	// Points never interact

//		if( tr.pSourceEntity == pEntity )
//			continue;	// Do not clip against myself

		if( !tr.GetSweptAABB().IsIntersectingWith( pEntity->world_aabb ) )
			continue;

		// check collision group table
//		if( !CheckCollisionGroup( pEntity->GroupIndex, tr.vecTargetGroup ) )
//			continue;

		switch( tr.GetBVType() )
		{
		case BVTYPE_AABB:
			tr.AddTouchEntity( pEntity );
			break;

		case BVTYPE_SPHERE:
			vDist = pEntity->Position() - tr.GetSphereCenter();
			if( Vec3LengthSq( vDist ) < tr.GetRadius() * tr.GetRadius() )
				tr.AddTouchEntity( pEntity );
			break;
		}
	}


	// collision handling in this 'EntityNode' has finished
	// now, we recurse down 'EntityTree', if this 'EntityNode' is not a leaf node

	if( this->leaf ) return;

	d = m_Plane.GetDistanceFromPoint( tr.GetSweptAABB().GetCenterPosition() );
	fRadius = tr.GetSweptAABB().GetRadiusForPlane( m_Plane );

	if(fRadius < d)
		paEntTree[ this->sFrontChild ].CheckPosition_r( tr, paEntTree );
	else if(d < -fRadius)
		paEntTree[ this->sBackChild ].CheckPosition_r( tr, paEntTree );
	else
	{
		paEntTree[ this->sFrontChild ].CheckPosition_r( tr, paEntTree );
		paEntTree[ this->sBackChild ].CheckPosition_r( tr, paEntTree );
	}

	return;
}


void CEntityNode::GetVisibleEntities_r(CViewFrustumTest& vf_test, CEntityNode* paEntTree)
{

	// Set the first copy entity on this entity node and cycle through all the copy entities
	// on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

		if( !vf_test.GetCamera()->ViewFrustumIntersectsWith( pEntity->world_aabb ) )
			continue;

		if( (vf_test.m_Flag & VFT_IGNORE_NOCLIP_ENTITIES) && pEntity->bNoClip )
			continue;

		// the whole entity or part of the entity is in the viewfrustum
		vf_test.AddEntity( pEntity );
	}

	// collision handling in this 'EntityNode' has finished
	// now, we recurse down 'EntityTree', if this 'EntityNode' is not a leaf node

	if( this->leaf )
		return;

	if( vf_test.GetCamera()->ViewFrustumIntersectsWith(paEntTree[this->sFrontChild].m_AABB) )
		paEntTree[this->sFrontChild].GetVisibleEntities_r(vf_test, paEntTree);

	if( vf_test.GetCamera()->ViewFrustumIntersectsWith(paEntTree[this->sBackChild].m_AABB) )
		paEntTree[this->sBackChild].GetVisibleEntities_r(vf_test, paEntTree);

	return;
}


void CEntityNode::ClipTrace_r(STrace& tr, CEntityNode* paEntTree)
{
//	ProfileBegin( "CEntityNode::ClipTrace_r()" );

	float d, fRadius;
	Vector3 vEnd;

	//Set the first copy entity on this entity node and cycle through all the copy entities
	//on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

//		ProfileBegin( "CEntityNode::ClipTrace_r() - linked entities loop" );

		if( pEntity->bvType == BVTYPE_DOT && tr.bvType == BVTYPE_DOT )
			continue;	//Points never interact

		if( pEntity->bNoClip && (tr.sTraceType & TRACETYPE_IGNORE_NOCLIP_ENTITIES) )
			continue;

		if( tr.pSourceEntity == pEntity )
			continue;	//do not clip against myself

//		ProfileBegin( "aabb test in CEntNode::ClipTr_r()" );

		if( !tr.aabb_swept.IsIntersectingWith( pEntity->world_aabb ) )
			continue;

		// check collision group table
//		if( !CheckCollisionGroup( pEntity->GroupIndex, tr.vecTargetGroup ) )
		if( !CheckCollisionGroup( tr.GroupIndex, pEntity->GroupIndex ) )
			continue;

		// maybe intersecting. need the precise check.
		// the trace is clipped by 'pEntity', if collision occurs

		STrace copy_trace = tr;
		Vector3 vS = *tr.pvStart;
		Vector3 vG = *tr.pvGoal;
		copy_trace.pvStart = &vS;
		copy_trace.pvGoal = &vG;
		copy_trace.fFraction = tr.fFraction;
		copy_trace.vEnd = tr.vEnd;

//		ProfileBegin( "pEnt->ClipTr() in CEntNode::ClipTr_r()" );

		pEntity->ClipTrace( copy_trace );	//clip trace against 'pEntity'

		if( copy_trace.fFraction < tr.fFraction )
		{
			if( !pEntity->bNoClip )
			{
				// The trace hit 'pEntity' and needs to be updated
				tr.pTouchedEntity = pEntity;
//				tr.vEnd = local_trace.vEnd + pEntity->Position();
				tr.vEnd		= copy_trace.vEnd;
				tr.fFraction= copy_trace.fFraction;
				tr.plane	= copy_trace.plane;
				tr.in_solid	= copy_trace.in_solid;
			}
			else
			{	// run into no-clip entity. trace must not be clipped
/*				int iNumTouchesWithNoClipEntities = this->ms_iNumTouchesWithNoClipEntities;
				this->ms_afTouchedNoClipEntity_Fraction[ iNumTouchesWithNoClipEntities ] = local_trace.fFraction;
				this->ms_aTouchedNoClipEntity[ iNumTouchesWithNoClipEntities ] = local_trace.pTouchedEntity;*/
				tr.pTouchedEntity = pEntity;
			}
		}
		
//		ProfileEnd( "pEnt->ClipTr() in CEntNode::ClipTr_r()" );

//		ProfileEnd( "aabb test in CEntNode::ClipTr_r()" );

//		ProfileEnd( "CEntityNode::ClipTrace_r() - linked entities loop" );
	}


	//collision handling in this 'EntityNode' has finished
	//now, we recurse down 'EntityTree', if this 'EntityNode' is not a leaf node

//	ProfileEnd( "CEntityNode::ClipTrace_r()" );

	if( this->leaf ) return;

	d = m_Plane.GetDistanceFromPoint( tr.aabb_swept.GetCenterPosition() );
	fRadius = tr.aabb_swept.GetRadiusForPlane( m_Plane );

	if(fRadius < d)
		paEntTree[ this->sFrontChild ].ClipTrace_r( tr, paEntTree );
	else if(d < -fRadius)
		paEntTree[ this->sBackChild ].ClipTrace_r( tr, paEntTree );
	else
	{
		paEntTree[ this->sFrontChild ].ClipTrace_r( tr, paEntTree );
		paEntTree[ this->sBackChild ].ClipTrace_r( tr, paEntTree );
	}

	return;
}


void CEntityNode::GetOverlappingEntities( COverlapTestAABB& overlap_test, CEntityNode* paEntTree )
{
	float d, fRadius;
	Vector3 vEnd;

	// set the first copy entity on this entity node
	// and cycle through all the copy entities on this entity node
	CLinkNode<CCopyEntity> *pLinkNode;
	CCopyEntity* pEntity;
	for( pLinkNode = m_EntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pEntity = pLinkNode->pOwner;

//		if( pEntity->bNoClip && (tr.sTraceType & TRACETYPE_IGNORE_NOCLIP_ENTITIES) )
//			continue;
//		if( tr.pSourceEntity == pEntity )	continue;	//do not clip against myself

		if( !overlap_test.aabb.IsIntersectingWith( pEntity->world_aabb ) )
			continue;

		// found an overlap of 2 AABBs

		// check groups
//		if( CheckCollisionGroup( pEntity->GroupIndex, overlap_test.vecTargetGroup ) )
		if( CheckCollisionGroup( overlap_test.GroupIndex, pEntity->GroupIndex ) )
			overlap_test.pvecBuffer->push_back( pEntity );

	}


	// collision handling in this 'EntityNode' has finished
	// now, we recurse down 'EntityTree', if this 'EntityNode' is not a leaf node

	if( this->leaf ) return;

	d = m_Plane.GetDistanceFromPoint( overlap_test.aabb.GetCenterPosition() );
	fRadius = overlap_test.aabb.GetRadiusForPlane( m_Plane );

	if(fRadius < d)
		paEntTree[ this->sFrontChild ].GetOverlappingEntities( overlap_test, paEntTree );
	else if(d < -fRadius)
		paEntTree[ this->sBackChild ].GetOverlappingEntities( overlap_test, paEntTree );
	else
	{
		paEntTree[ this->sFrontChild ].GetOverlappingEntities( overlap_test, paEntTree );
		paEntTree[ this->sBackChild ].GetOverlappingEntities( overlap_test, paEntTree );
	}

	return;
}


void CEntityNode::LinkLightEntity_r( CLightEntity *pLightEntity, CEntityNode* paEntTree )
{
	float r = pLightEntity->GetRadius();

	float d = m_Plane.GetDistanceFromPoint( pLightEntity->GetWorldPose().vPosition );

	if( r < d )
		paEntTree[sFrontChild].LinkLightEntity_r( pLightEntity, paEntTree );
	else if( d < -r )
		paEntTree[sBackChild].LinkLightEntity_r( pLightEntity, paEntTree );
	else
	{
		// link the light entity to this node

//		MsgBoxFmt( "linking a light(index:%d) to entity tree - r: %f, d: %f",
//			pLightEntity->GetIndex(), r, d );

		m_LightEntityLinkHead.InsertNext( &(pLightEntity->m_LightEntityLink) );
	}
}


void CEntityNode::CheckLight_r( CCopyEntity *pEntity, CEntityNode* paEntTree )
{
	float fMaxRange, fMaxRangeSq;
	Vector3 vLightToEntity;

	CLinkNode<CLightEntity> *pLinkNode;
	CLightEntity* pLightEntity;
	for( pLinkNode = m_LightEntityLinkHead.pNext;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext )
	{
		pLightEntity = pLinkNode->pOwner;

		if( pLightEntity->GetLightType() == CLight::POINT
		 || pLightEntity->GetLightType() == CLight::HEMISPHERIC_POINT
		 || pLightEntity->GetLightType() == CLight::TRI_POINT )
		{
			fMaxRange = pLightEntity->GetRadius() + pEntity->GetRadius();
			fMaxRangeSq = fMaxRange * fMaxRange;
			vLightToEntity = pEntity->Position() - pLightEntity->Position();
			if( fMaxRangeSq < Vec3LengthSq(vLightToEntity) )
				continue;	// out of the light range
		}

//		bool light_reaches_entity
//			= pLightEntity->ReachesEntity( pEntity );

//		if( light_reaches_entity )
//			pEntity->AddLight( pLightEntity->... ) /// how can I get the weak_ptr of pLightEntity???

		pLightEntity->AddLightIfReachesEntity( pEntity );
	}

	if( this->leaf )
		return;

	float r = pEntity->GetRadius();

	float d = m_Plane.GetDistanceFromPoint( pEntity->Position() );

	if( r < d )
		paEntTree[sFrontChild].CheckLight_r( pEntity, paEntTree );
	else if( d < -r )
		paEntTree[sBackChild].CheckLight_r( pEntity, paEntTree );
	else
	{
		// need to check both nodes
		paEntTree[sFrontChild].CheckLight_r( pEntity, paEntTree );
		paEntTree[sBackChild].CheckLight_r( pEntity, paEntTree );
	}

}
