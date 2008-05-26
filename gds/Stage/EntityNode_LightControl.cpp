#include "EntityNode.h"
#include "EntitySet.h"

#include "Stage.h"
#include "BaseEntity.h"
#include "bsptree.h"
#include "trace.h"
#include "LightEntity.h"
//#include "ViewFrustumTest.h"
//#include "PerformanceCheck.h"

#include "Support/msgbox.h"
#include "Support/Vec3_StringAux.h"


void CEntityNode::LinkLightEntity_r(CLightEntity *pLightEntity, CEntityNode* paEntTree)
{
	float r = pLightEntity->GetRadius();

	float d = m_Plane.GetDistanceFromPoint( pLightEntity->GetPosition() );

	if( r < d )
		paEntTree[sFrontChild].LinkLightEntity_r( pLightEntity, paEntTree );
	else if( d < -r )
		paEntTree[sBackChild].LinkLightEntity_r( pLightEntity, paEntTree );
	else
	{	// link the light entity to this node
//		MsgBoxFmt( "linking a light(index:%d) to entity tree - r: %f, d: %f",
//			pLightEntity->GetIndex(), r, d );

		pLightEntity->SetPrev( &m_LightEntityHead );
		pLightEntity->SetNext( m_LightEntityHead.GetNext() );
		if( m_LightEntityHead.GetNext() )
			m_LightEntityHead.GetNext()->SetPrev( pLightEntity );
		m_LightEntityHead.SetNext( pLightEntity );

/*		pLightEntity->SetPrev( NULL );
		if( m_pLightEntity )
		{	// some light entities have been already linked
			pLightEntity->SetNext( m_pLightEntity );
			m_pLightEntity->SetPrev( pLightEntity );
		}
		else
		{	// the first link
			pLightEntity->SetNext( NULL );
		}
		m_pLightEntity = pLightEntity;*/
	}
}


void CEntityNode::CheckLight_r(CCopyEntity *pEntity, CEntityNode* paEntTree)
{
	if( m_LightEntityHead.GetNext() )
	{	// check trace from light to entity
		STrace tr;
		tr.bvType = BVTYPE_DOT;
		tr.pvGoal = &pEntity->Position();
//		tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
		tr.sTraceType = TRACETYPE_IGNORE_ALL_ENTITIES;
//		tr.pSourceEntity = pEntity;

		Vector3 vLightCenterPos, vLightToEntity;

		CLightEntity* pLightEntity;
		for( pLightEntity = m_LightEntityHead.GetNext();
			 pLightEntity != NULL;
			 pLightEntity = pLightEntity->GetNext() )
		{	// find lights that reach 'pEntity'
			tr.pTouchedEntity = NULL;
			tr.fFraction = 1.0f;
			vLightCenterPos = pLightEntity->GetPosition();
			tr.pvStart = &vLightCenterPos;

			m_pEntitySet->GetStage()->ClipTrace( tr );

//			if( tr.pTouchedEntity != pEntity )
			if( tr.fFraction < 1.0f )
				continue;	// static geometry obstacle between light and entity

			float fMaxRangeSq = pLightEntity->GetRadius() + pEntity->local_aabb.vMax.x;
			fMaxRangeSq = fMaxRangeSq * fMaxRangeSq;
			vLightToEntity = pEntity->Position() - vLightCenterPos;
			if( fMaxRangeSq < D3DXVec3LengthSq(&vLightToEntity) )
				continue;	// out of the light range

			// light is reaching the entity - register its index
//			pEntity->AddLightIndex( pLightEntity->GetIndex() );

			int light_type = pLightEntity->GetLightEntityType();
			CLightEntityManager *pLightManager = m_pEntitySet->GetLightEntityManager();
			if( light_type == D3DLIGHT_POINT )
			{
				// point light - simply add to the end of the index array
				pEntity->AddLightIndex( pLightEntity->GetIndex() );
			}
			else if( light_type == D3DLIGHT_DIRECTIONAL )
			{
				// directional light - must come before point lights
			//	pEntity->InsertLightIndex( 0, pLightEntity->GetIndex() );
			}

		}
	}

	if( this->leaf )
		return;

	float r = pEntity->local_aabb.vMax.x;

	float d = m_Plane.GetDistanceFromPoint( pEntity->Position() );

	if( r < d )
		paEntTree[sFrontChild].CheckLight_r( pEntity, paEntTree );
	else if( d < -r )
		paEntTree[sBackChild].CheckLight_r( pEntity, paEntTree );
	else
	{	// need check both nodes
		paEntTree[sFrontChild].CheckLight_r( pEntity, paEntTree );
		paEntTree[sBackChild].CheckLight_r( pEntity, paEntTree );
	}

}
