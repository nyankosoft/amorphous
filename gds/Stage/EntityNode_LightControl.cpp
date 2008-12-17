#include "EntityNode.h"

#include "Support/Vec3_StringAux.h"
#include "EntitySet.h"
#include "Stage.h"
#include "BaseEntity.h"
#include "LightEntity.h"
//#include "bsptree.h"
//#include "trace.h"
//#include "ViewFrustumTest.h"


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

