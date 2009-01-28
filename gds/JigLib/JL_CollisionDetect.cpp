
#include <algorithm>

#include "JL_CollisionDetect.h"

#include "JL_ShapeBase.h"
#include "JL_PhysicsActor.h"
#include "JL_PhysicsManager.h"
#include "JL_CollisionFunctor.h"

class CJL_Shape_Box;
class CTriangleMesh;

#include "3DMath/Vector2.h"
#include "3DMath/aabb2.h"

bool g_DoContactPointReduction = false;

void ReduceContactPointsX( CJL_CollisionFunctor& rDestCollFunctor, vector<CJL_ContactInfo>& vecSrcCollInfo );


void ReduceContactPointsX2( CJL_CollisionFunctor& rDestCollFunctor,
						    vector<CJL_CollisionInfo>& vecSrcCollInfo );


// don't make more than one triangle mesh actor
// The system supports one static triangle mesh per scene for the moment

static void JL_CollisionDetect_ActorActor( CJL_PhysicsActor& rActor0, CJL_PhysicsActor& rActor1,
								           CJL_CollisionFunctor& rCollisionFunctor )
{
	int i, j;
	int iNumShapes0 = rActor0.GetNumShapes();
	int iNumShapes1 = rActor1.GetNumShapes();
	short shape0, shape1;
	CJL_ShapeBase *pShape0, *pShape1;

	for( i=0; i<iNumShapes0; i++ )
	{
		for( j=0; j<iNumShapes1; j++ )
		{
			pShape0 = rActor0.GetShape(i);
			pShape1 = rActor1.GetShape(j);
			shape0 = pShape0->GetShapeType();
			shape1 = pShape1->GetShapeType();

			if( shape1 < shape0 )
			{	// swap shape0 & shape1
				swap( shape0, shape1 );
				swap( pShape0, pShape1 );
			}

			switch( shape0 )
			{
			case JL_SHAPE_BOX:
				CJL_Shape_Box *pBox0;
				pBox0 = (CJL_Shape_Box *)pShape0;

				switch( shape1 )
				{
				case JL_SHAPE_BOX:	// box - box
					CJL_Shape_Box *pBox1;
					pBox1 = (CJL_Shape_Box *)pShape1;
					JL_CollisionDetect_Box_Box( *pBox0, *pBox1, rCollisionFunctor );
					break;

				case JL_SHAPE_CAPSULE:	// box - capsule
					CJL_Shape_Capsule *pCapsule1;
					pCapsule1 = (CJL_Shape_Capsule *)pShape1;
					JL_CollisionDetect_Box_Capsule( *pBox0, *pCapsule1, rCollisionFunctor );
					break;

				case JL_SHAPE_TRIANGLEMESH:	// box - mesh	(static mesh only)
					CTriangleMesh *pTriMesh1;
					pTriMesh1 = (CTriangleMesh *)pShape1;
					if( pBox0->GetWorldAABB().vMin.y < -4.01f )
						int iStop = 1;
					JL_CollisionDetect_Box_TriangleMesh( *pBox0, *pTriMesh1, rCollisionFunctor );
					break;
				}
				break;

			case JL_SHAPE_CAPSULE:
				CJL_Shape_Capsule *pCapsule0;
				pCapsule0 = (CJL_Shape_Capsule *)pShape0;
				switch( shape1 )
				{
				case JL_SHAPE_CAPSULE:	// capsule - capsule
					CJL_Shape_Capsule *pCapsule1;
					pCapsule1 = (CJL_Shape_Capsule *)pShape1;
					JL_CollisionDetect_Capsule_Capsule( *pCapsule0, *pCapsule1, rCollisionFunctor );
					break;

				case JL_SHAPE_TRIANGLEMESH:	// capsule - mesh
					CTriangleMesh *pTriMesh1;
					pTriMesh1 = (CTriangleMesh *)pShape1;
					JL_CollisionDetect_Capsule_TriangleMesh( *pCapsule0, *pTriMesh1, rCollisionFunctor );
					break;
				}
				break;

			case JL_SHAPE_TRIANGLEMESH:
//				CTriangleMesh *pTriMesh0     = (CTriangleMesh *)pShape0;
				break;

			}

		}
	}

}


void JL_CollisionDetect_All( TCPreAllocDynamicLinkList<CJL_PhysicsActor>& rActorList,
							 CJL_CollisionFunctor& rCollisionFunctor,
							 CJL_PhysicsManager *pPhysicsManager )
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor0, itrActor1;

	// create a temporary copy of collision functor for contact point reduction
#ifndef USE_COMBINED_COLLISION_POINTS_INFO
	static vector<CJL_ContactInfo> vecTempCollInfo;
#else
	static vector<CJL_CollisionInfo> vecTempCollInfo;
#endif

	CJL_CollisionFunctor temp_coll_functor;
	temp_coll_functor.SetCollisionBuffer( &vecTempCollInfo );


	for( itrActor0 = rActorList.Begin();
	     itrActor0 != rActorList.End();
		 itrActor0++ )
	{

		for( itrActor1 = rActorList.Begin();
		     itrActor1 != itrActor0;
			 itrActor1++ )
		{
			if( !itrActor0->GetWorldAABB().IsIntersectingWith( itrActor1->GetWorldAABB() ) )
				continue;	// no collision between these two actors

			if( itrActor0->GetActorFlag() & JL_ACTOR_STATIC &&
				itrActor1->GetActorFlag() & JL_ACTOR_STATIC )
				continue;
			
			if( itrActor0->GetActivityState() == CJL_PhysicsActor::FROZEN &&
				itrActor1->GetActivityState() == CJL_PhysicsActor::FROZEN )
				continue;
			
			if( itrActor0->GetActivityState() == CJL_PhysicsActor::FROZEN &&
				itrActor1->GetActorFlag() == JL_ACTOR_STATIC ||
				itrActor1->GetActivityState() == CJL_PhysicsActor::FROZEN &&
				itrActor0->GetActorFlag() == JL_ACTOR_STATIC )
				continue;

//			if( piCollisionGruop[ itrActor0->GetCollisionGroup() ][ itrActor1->GetCollisionGroup() ] == 0 )
			if( !pPhysicsManager->IsCollisionGroupEnabled( itrActor0->GetCollisionGroup(),
				                                           itrActor1->GetCollisionGroup() ) )
				continue;

//			JL_CollisionDetect_ActorActor( *itrActor0, *itrActor1, rCollisionFunctor );

			vecTempCollInfo.resize( 0 );
			JL_CollisionDetect_ActorActor( *itrActor0, *itrActor1, temp_coll_functor );


#ifndef USE_COMBINED_COLLISION_POINTS_INFO
			if( g_DoContactPointReduction )
			{
				if( 0 < vecTempCollInfo.size() )
					ReduceContactPointsX( rCollisionFunctor, vecTempCollInfo );
			}
			else
			{
				for( size_t i=0; i<vecTempCollInfo.size(); i++ )
					rCollisionFunctor.AddContact( vecTempCollInfo[i] );
			}
#else
			if( g_DoContactPointReduction  )
				ReduceContactPointsX2( rCollisionFunctor, vecTempCollInfo );
			else
			{
				for( size_t i=0; i<vecTempCollInfo.size(); i++ )
					rCollisionFunctor.AddContact( vecTempCollInfo[i] );
			}
#endif
		}
	}

}


void JL_CollisionDetect_FrozenActor( CJL_PhysicsActor *pActor,
									 TCPreAllocDynamicLinkList<CJL_PhysicsActor>& rActorList,
									 CJL_CollisionFunctor& rCollisionFunctor )
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

#ifndef USE_COMBINED_COLLISION_POINTS_INFO
	static vector<CJL_ContactInfo> vecTempCollInfo;
#else
	static vector<CJL_CollisionInfo> vecTempCollInfo;
#endif

	CJL_CollisionFunctor temp_coll_functor;
	temp_coll_functor.SetCollisionBuffer( &vecTempCollInfo );

	for( itrActor = rActorList.Begin();
	     itrActor != rActorList.End();
		 itrActor++ )
		 {
//			 if( itrActor->GetActorFlag() & JL_ACTOR_STATIC )
//				 	continue;

			 if( itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE )
				 continue;

			 if( !pActor->GetWorldAABB().IsIntersectingWith( itrActor->GetWorldAABB() ) )
				 continue;	// no collision between these two actors

			 if( pActor == &(*itrActor) )
				 continue;

//			 JL_CollisionDetect_ActorActor( *pActor, *itrActor, rCollisionFunctor );

			vecTempCollInfo.resize( 0 );
			JL_CollisionDetect_ActorActor( *pActor, *itrActor, temp_coll_functor );

#ifndef USE_COMBINED_COLLISION_POINTS_INFO

			if( 0 < vecTempCollInfo.size() )
				ReduceContactPointsX( rCollisionFunctor, vecTempCollInfo );

#else
			if( g_DoContactPointReduction )
				 ReduceContactPointsX2( rCollisionFunctor, vecTempCollInfo );
			else
			{
				for( size_t i=0; i<vecTempCollInfo.size(); i++ )
					rCollisionFunctor.AddContact( vecTempCollInfo[i] );
			}
#endif
		 }


}


#ifndef USE_COMBINED_COLLISION_POINTS_INFO


void ReduceContactPointsX( CJL_CollisionFunctor& rDestCollFunctor, vector<CJL_ContactInfo>& vecSrcCollInfo )
{
	// hold indices to collision infos
	static int coll_cluster[16][32];
	static int num_clustered_colls[16];	// stores the number of contacts at each cluster
	static Vector2 avProjPt[32];
	memset( num_clustered_colls, 0, sizeof(int) * 16 );

	int num_clusters = 0;
	int num_colls = vecSrcCollInfo.size();
	int i, j;

	// classify contacts
	for( i=0; i<num_colls; i++ )
	{
		if( num_clusters == 16 )
		{
			rDestCollFunctor.AddContact( vecSrcCollInfo[i] );	// too many clusters
			continue;
		}

		for( j=0; j<num_clusters; j++ )
		{
			if( 0.99f < Vec3Dot( vecSrcCollInfo[i].vNormal, vecSrcCollInfo[coll_cluster[j][0]].vNormal ) )
			{
				if( num_clustered_colls[j] < 32 )
				{
					// store the contact index in the existing cluster
                    coll_cluster[j][num_clustered_colls[j]++] = i;
				}
				else
					rDestCollFunctor.AddContact( vecSrcCollInfo[i] );	// too many contacts for a cluster

				break;
			}
		}
		if( j == num_clusters )
			coll_cluster[ num_clusters++ ][ num_clustered_colls[j]++ ] = i;	// store the index in a new cluster
	}

	for( j=0; j<num_clusters; j++ )
	{
		// calc 2D plane perpendicular to the normal of collisions in this cluster
		Vector3 vNormal = vecSrcCollInfo[ coll_cluster[j][0] ].vNormal;
		Vector3 vAxis0, vAxis1;
		if( 0.999f < fabsf(vNormal.y) )
			vAxis0 = Vector3(1,0,0);
		else
			vAxis0 = Vector3(0,1,0);
		vAxis1 = Vec3Cross( vAxis0, vNormal );
		vAxis1 = Vec3Normalize( vAxis1 );
		vAxis0 = Vec3Cross( vNormal, vAxis1 );
		vAxis0 = Vec3Normalize( vAxis0 );

		AABB2 aabb;
		aabb.Nullify();
		Scalar max_depth = -99999;
		int candidate_index[5];
		// index[0]: holds contact with the max penetration depth
		// index[1] - [4]; hold contacts on the edge of the 2D AABB on the projected plane
		memset( candidate_index, 0, sizeof(int) * 5 );
		for( i=0; i<num_clustered_colls[j]; i++ )
		{
			const Vector3& pos = vecSrcCollInfo[ coll_cluster[j][i] ].vContactPoint;

			// project contact ponints to the 2D plane
			avProjPt[i] = Vector2( Vec3Dot(vAxis0, pos), Vec3Dot(vAxis1, pos) );
			aabb.AddPoint( avProjPt[i] );

			// update a contact with the maximum penetration depth
			if( max_depth < vecSrcCollInfo[ coll_cluster[j][i] ].fPenetrationDepth )
			{
				candidate_index[0] = coll_cluster[j][i];
			}
		}

		// find contacts with extremal values
		for( i=0; i<num_clustered_colls[j]; i++ )
		{
			if( avProjPt[i].x == aabb.vMin.x )
				candidate_index[1] = coll_cluster[j][i];
			else if( avProjPt[i].x == aabb.vMax.x )
				candidate_index[2] = coll_cluster[j][i];
			if( avProjPt[i].y == aabb.vMin.y )
				candidate_index[3] = coll_cluster[j][i];
			else if( avProjPt[i].y == aabb.vMax.y )
				candidate_index[4] = coll_cluster[j][i];
		}

		int k;
		for( i=0; i<5; i++ )
		{
			for( k=0; k<i; k++ )
			{
				// check if already sent to the dest contact buffer
				if( candidate_index[i] == candidate_index[k] )
					break;
			}
			if( i == k )
			{
				// hasn't been sent to the dest buffer
				// register as a new contact
				rDestCollFunctor.AddContact( vecSrcCollInfo[ candidate_index[i] ] );
			}
		}
	}
}


#else


void ReduceContactPointsX2( CJL_CollisionFunctor& rDestCollFunctor,
						    vector<CJL_CollisionInfo>& vecSrcCollInfo )
{
	// hold indices to collision infos
	static Vector2 avProjPt[32];

	int num_colls = vecSrcCollInfo.size();
	int i, j;

	// classify contacts
	for( i=0; i<num_colls; i++ )
	{
		CJL_CollisionInfo& src_collision = vecSrcCollInfo[i];
		int num_coll_points = vecSrcCollInfo[i].m_vecPointInfo.size();

		if( 32 < num_coll_points )
		{
			assert( !"ReduceContactPointsX2() - too many coll points" );
			num_coll_points = 32;
		}

		// calc 2D plane perpendicular to the normal of collisions in this cluster
		Vector3 vNormal = src_collision.m_vDirToBody0;
		Vector3 vAxis0, vAxis1;
		if( 0.999f < fabsf(vNormal.y) )
			vAxis0 = Vector3(1,0,0);
		else
			vAxis0 = Vector3(0,1,0);
		vAxis1 = Vec3Cross( vAxis0, vNormal );
//		vAxis1 = Vec3Normalize( vAxis1 );
		Vec3Normalize( vAxis1, vAxis1 );
		vAxis0 = Vec3Cross( vNormal, vAxis1 );
//		vAxis0 = Vec3Normalize( vAxis0 );
		Vec3Normalize( vAxis0, vAxis0 );

		AABB2 aabb;
		aabb.Nullify();
//		Scalar max_depth = -99999;
		int candidate_index[5];
		// index[0]: holds contact with the max penetration depth
		// index[1] - [4]; hold contacts on the edge of the 2D AABB on the projected plane
		memset( candidate_index, 0, sizeof(int) * 5 );

		for( j=0; j<num_coll_points; j++ )
		{
			CJL_CollPointInfo& pt_info = vecSrcCollInfo[i].m_vecPointInfo[j];
			const Vector3& pos = pt_info.vContactPosition;

			// project contact ponints to the 2D plane
			avProjPt[j] = Vector2( Vec3Dot(vAxis0, pos), Vec3Dot(vAxis1, pos) );
			aabb.AddPoint( avProjPt[j] );

			// update a contact with the maximum penetration depth
/*				if( max_depth < vecSrcCollInfo[ coll_cluster[j][i] ].fPenetrationDepth )
			{
				candidate_index[0] = coll_cluster[j][i];
			}*/
		}

		// find contacts with extremal values
		for( j=0; j<num_coll_points; j++ )
		{
			if( avProjPt[j].x == aabb.vMin.x )
				candidate_index[0] = j;
			else if( avProjPt[j].x == aabb.vMax.x )
				candidate_index[1] = j;
			if( avProjPt[j].y == aabb.vMin.y )
				candidate_index[2] = j;
			else if( avProjPt[j].y == aabb.vMax.y )
				candidate_index[3] = j;
		}

		TCFixedVector<CJL_CollPointInfo, 32> vecDestCollPtInfo;

		int k;
		for( j=0; j<4; j++ )
		{
			for( k=0; k<j; k++ )
			{
				// check if already sent to the dest contact buffer
				if( candidate_index[j] == candidate_index[k] )
					break;
			}
			if( j == k )
			{
				// hasn't been sent to the dest buffer
				// register as a new contact
				vecDestCollPtInfo.push_back( src_collision.m_vecPointInfo[ candidate_index[j] ] );
			}
		}
		rDestCollFunctor.AddContacts( src_collision.pBody0,
			                          src_collision.pBody1,
				                      src_collision.m_vDirToBody0,
				                      &vecDestCollPtInfo[0],
				                      vecDestCollPtInfo.size() );

	}
}

#endif  /*  USE_COMBINED_COLLISION_POINTS_INFO  */