
#ifndef  __JK_COLLISIONFUNCTOR_H__
#define  __JK_COLLISIONFUNCTOR_H__


#include "JL_ContactInfo.h"
#include "JL_ShapeBase.h"
#include "JL_PhysicsActor.h"

#include <vector>
using namespace std;


#ifndef USE_COMBINED_COLLISION_POINTS_INFO  /////////////////////////////////////////////////////////


class CJL_CollisionFunctor
{
	vector<CJL_ContactInfo> *m_pvecContact;

public:
	inline CJL_CollisionFunctor() { m_pvecContact = NULL; m_fCollTolerance = 0.001f; }

	Scalar m_fCollTolerance;

	inline void SetCollisionBuffer( vector<CJL_ContactInfo> *pvecCollisionBuffer )
	{
		m_pvecContact = pvecCollisionBuffer;
	}

	inline void AddContact( const CJL_ContactInfo& contact )
	{
		contact.pBody0->GetPhysicsActor()->GetCollisionIndexBuffer().push_back( m_pvecContact->size() );
		if( contact.pBody1 )
			contact.pBody1->GetPhysicsActor()->GetCollisionIndexBuffer().push_back( m_pvecContact->size() );

		m_pvecContact->push_back( contact );
	}

	/// holds temporary contacts
	/// temporary contacts are moved to the final contact list later
	/// contact indices are not set to the actor
	inline void AddTemporaryContact( const CJL_ContactInfo& contact )
	{
		m_pvecContact->push_back( contact );
	}

	inline void AddContacts( CJL_ShapeBase *pBody0,
		                     CJL_ShapeBase *pBody1,
		                     Vector3 vNormal,
							 Vector3 *pvPos,
							 int iNumContacts,
							 Scalar fDepth );

	inline void AddTemporaryContacts( CJL_ShapeBase *pBody0,
		                     CJL_ShapeBase *pBody1,
		                     Vector3 vNormal,
							 Vector3 *pvPos,
							 int iNumContacts,
							 Scalar fDepth );

};


inline void CJL_CollisionFunctor::AddContacts( CJL_ShapeBase *pBody0,
											   CJL_ShapeBase *pBody1,
											   Vector3 vNormal,
											   Vector3 *pvPos,
											   int iNumContacts,
											   Scalar fDepth )
{
	CJL_ContactInfo collision;
	for( int i=0; i<iNumContacts; i++ )
	{
		collision.pBody0 = pBody0;
		collision.pBody1 = pBody1;
		collision.vNormal = vNormal;
		collision.vContactPoint = pvPos[i];
		collision.fPenetrationDepth = fDepth;
		AddContact( collision );
	}
}


inline void CJL_CollisionFunctor::AddTemporaryContacts( CJL_ShapeBase *pBody0,
											   CJL_ShapeBase *pBody1,
											   Vector3 vNormal,
											   Vector3 *pvPos,
											   int iNumContacts,
											   Scalar fDepth )
{
	CJL_ContactInfo collision;
	for( int i=0; i<iNumContacts; i++ )
	{
		collision.pBody0 = pBody0;
		collision.pBody1 = pBody1;
		collision.vNormal = vNormal;
		collision.vContactPoint = pvPos[i];
		collision.fPenetrationDepth = fDepth;
		AddTemporaryContact( collision );
	}
}



#else  /////////////////////////////////////////////////////////////////////////////////////////////



class CJL_CollisionFunctor
{
	vector<CJL_CollisionInfo> *m_pvecCollInfo;

public:
	inline CJL_CollisionFunctor() { m_pvecCollInfo = NULL; m_fCollTolerance = 0.001f; }

	Scalar m_fCollTolerance;

	inline void SetCollisionBuffer( vector<CJL_CollisionInfo> *pvecCollisionBuffer )
	{
		m_pvecCollInfo = pvecCollisionBuffer;
	}

	inline void AddContact( const CJL_CollisionInfo& coll_info )
	{
		if( coll_info.pBody0 )
			coll_info.pBody0->GetPhysicsActor()->GetCollisionIndexBuffer().push_back( m_pvecCollInfo->size() );
		if( coll_info.pBody1 )
			coll_info.pBody1->GetPhysicsActor()->GetCollisionIndexBuffer().push_back( m_pvecCollInfo->size() );

		m_pvecCollInfo->push_back( coll_info );
	}
/*
	/// holds temporary contacts
	/// temporary contacts are moved to the final contact list later
	/// contact indices are not set to the actor
	inline void AddTemporaryContact( const CJL_CollisionInfo& contact )
	{
		m_pvecCollInfo->push_back( contact );
	}*/

	inline void AddContacts( CJL_ShapeBase *pBody0,
		                     CJL_ShapeBase *pBody1,
		                     Vector3 vNormal,
							 CJL_CollPointInfo *pCollPtInfo,
							 int num_points );

	inline void AddTemporaryContacts( CJL_ShapeBase *pBody0,
		                     CJL_ShapeBase *pBody1,
		                     Vector3 vNormal,
							 CJL_CollPointInfo *pCollPtInfo,
							 int num_points );

};


inline void CJL_CollisionFunctor::AddContacts( CJL_ShapeBase *pBody0,
											   CJL_ShapeBase *pBody1,
											   Vector3 vNormal,
											   CJL_CollPointInfo *pCollPtInfo,
											   int num_points )
{
	if( pBody0 )
		pBody0->GetPhysicsActor()->GetCollisionIndexBuffer().push_back( m_pvecCollInfo->size() );
	if( pBody1 )
		pBody1->GetPhysicsActor()->GetCollisionIndexBuffer().push_back( m_pvecCollInfo->size() );

	m_pvecCollInfo->push_back( CJL_CollisionInfo( pBody0, pBody1, vNormal, pCollPtInfo, num_points ) );
}


inline void CJL_CollisionFunctor::AddTemporaryContacts( CJL_ShapeBase *pBody0,
											   CJL_ShapeBase *pBody1,
											   Vector3 vNormal,
											   CJL_CollPointInfo *pCollPtInfo,
											   int num_points )
{
	m_pvecCollInfo->push_back( CJL_CollisionInfo( pBody0, pBody1, vNormal, pCollPtInfo, num_points ) );
}


#endif  /////////////////////////////////////////////////////////////////////////////////////////////



#endif  /*  __JK_COLLISIONFUNCTOR_H__  */
