#include "TriangleMesh.hpp"
#include "JL_Shape_Box.hpp"
#include "JL_CollisionFunctor.hpp"
#include "3DMath/BSPTreeForBox.hpp"
using namespace std;


#define MAX_POINTS_BOX_TRIANGLE	8



static void CollisionDetect_Box_Triangle( CJL_Shape_Box& rBox,
										  int iTriangleIndex, CTriangleMesh& rTriMesh,
										  CJL_CollisionFunctor& rColFunctor );


static void AddContactPoint( Vector3& rvNormal, Scalar fPenetrationDepth,
												CJL_Shape_Box& rBox,
												CIndexedTriangle& rIndexedTriangle,
												CTriangle& rTriangle,
												CJL_CollisionFunctor& rColFunctor,
												Scalar fOldDepth );

void JL_CollisionDetect_Box_TriangleMesh( CJL_Shape_Box& rBox, CTriangleMesh& rTriMesh,
									      CJL_CollisionFunctor& rColFunctor )
{
	static vector<int> s_veciIndexList;

	s_veciIndexList.resize(0);
	rTriMesh.GetIntersectingTriangles( s_veciIndexList, rBox.GetWorldAABB() );

	int i, iNumTriangles = s_veciIndexList.size();
	for( i=0; i<iNumTriangles; i++ )
	{	// check collision with each triangle
		// store contact information when collision is found
		CollisionDetect_Box_Triangle( rBox, s_veciIndexList[i], rTriMesh, rColFunctor );
	}
}


// check intersection between box and a triangle using separation axes
static void CollisionDetect_Box_Triangle( CJL_Shape_Box& rBox,
										  int iTriangleIndex, CTriangleMesh& rTriMesh,
										  CJL_CollisionFunctor& rColFunctor )
{
	CIndexedTriangle& rTriangle	= rTriMesh.GetTriangle(iTriangleIndex);		// get indexed triangle
	CTriangle triangle( rTriMesh.GetVertex( rTriangle.GetIndex(0) ),
		                rTriMesh.GetVertex( rTriangle.GetIndex(1) ),
						rTriMesh.GetVertex( rTriangle.GetIndex(2) ),
						rTriangle.GetNormal() );

	Vector3 avEdge[3];
	avEdge[0] = triangle.GetEdge(0);
	avEdge[1] = triangle.GetEdge(1);
	avEdge[2] = triangle.GetEdge(2);

	// set up 13 axes needed to check overlaps between box and triangle
	Vector3 avAxis[13];
	avAxis[0] = rBox.GetWorldOrient().GetColumn(0);
	avAxis[1] = rBox.GetWorldOrient().GetColumn(1);
	avAxis[2] = rBox.GetWorldOrient().GetColumn(2);
	avAxis[3] = rTriangle.GetNormal();
	Vec3Cross( avAxis[4],  avAxis[0], avEdge[0] );
	Vec3Cross( avAxis[5],  avAxis[1], avEdge[0] );
	Vec3Cross( avAxis[6],  avAxis[2], avEdge[0] );
	Vec3Cross( avAxis[7],  avAxis[0], avEdge[1] );
	Vec3Cross( avAxis[8],  avAxis[1], avEdge[1] );
	Vec3Cross( avAxis[9],  avAxis[2], avEdge[1] );
	Vec3Cross( avAxis[10], avAxis[0], avEdge[2] );
	Vec3Cross( avAxis[11], avAxis[1], avEdge[2] );
	Vec3Cross( avAxis[12], avAxis[2], avEdge[2] );

	Scalar afDepth[13], fLength;
	Scalar fMinDepth = 100.0f;
	int i, iMinAxis = -1;
	float fCollTolerance = rColFunctor.m_fCollTolerance;
	for(i=0; i<13; i++)
	{
		if( Vec3Dot( avAxis[i], rTriangle.GetNormal() ) < 0 )
			continue;		// don't check collision when the box is approaching from behind the triangle 

		afDepth[i] = 101.0f;

		// When an edge direction of the triangle and an orientation vector of the box are exactly
		// the same, their cross product axis becomes 0. such an axis should be skipped
//		if( avAxis[i] == Vector3(0,0,0) )
		if( Vec3LengthSq(avAxis[i]) < 0.0000001f )
			continue;

		// separation axis test between 'rBox' and 'rTriangle'
		if( !rBox.SepAxisTest( afDepth[i], avAxis[i], triangle, fCollTolerance ) )
			return;	// no overlap

//		if( 3<i )
//		{	// need to normalize axis length and penetration depth
			fLength = Vec3Length( avAxis[i] );
//			if( fLength < 0.000001f )
			if( fLength < 0.005f )
				continue;

			avAxis[i] /= fLength;
			afDepth[i] /= fLength;
//		}

//		if( afDepth[i] < fMinDepth /* && 0.001f < afDepth[i]*/ )
//		if( afDepth[i] < fMinDepth - 0.01f ) // favors triangle normal slightly
		if( afDepth[i] < fMinDepth )
		{
			fMinDepth = afDepth[i];
			iMinAxis = i;
		}
	}

	const Vector3& vBoxOldPos = rBox.GetOldWorldPose().vPosition;
	const Vector3& vBoxNewPos = rBox.GetWorldPose().vPosition;

	if( iMinAxis < 0 )
		return;	// no collision detected

	if( 0.3f < fMinDepth || fMinDepth < 0.0f )
		int iUnexpected = 1;

	// re-check box v.s. floor collision for debug
	if( avAxis[iMinAxis] != Vector3(0,1,0) )
	{
		rBox.SepAxisTest( afDepth[3], avAxis[3], triangle, fCollTolerance );
		rBox.SepAxisTest( afDepth[iMinAxis], avAxis[iMinAxis], triangle, fCollTolerance );
	}


	// make sure the contact normal is heading toward box (pBody0)
	Vector3 vTriToBox = rBox.GetWorldPosition() - triangle.GetCenter();
/**	if( Vec3Dot( vTriToBox, avAxis[iMinAxis] ) < 0 )
		avAxis[iMinAxis] *= -1.0f;
**/
	if( Vec3Dot( triangle.GetNormal(), avAxis[iMinAxis] ) < 0.0 )
		avAxis[iMinAxis] *= -1.0f;

	Vector3 delta = vBoxNewPos - vBoxOldPos;
	Scalar old_depth = fMinDepth + Vec3Dot( delta, avAxis[iMinAxis] );

	// add contact points based on the minimum penetration depth
	AddContactPoint( avAxis[iMinAxis], fMinDepth, rBox, rTriangle, triangle, rColFunctor, old_depth );
}


static void AddPoint( TCFixedVector<Vector3, MAX_POINTS_BOX_TRIANGLE>& rvecvContactPoint,
					  Vector3 v,
					  Scalar fMaxCombinationDistSq )
{
	int i;
	for( i=0; i<rvecvContactPoint.size(); i++ )
	{
		if( Vec3LengthSq( v - rvecvContactPoint[i] ) < fMaxCombinationDistSq )
		{
			rvecvContactPoint[i] = ( v + rvecvContactPoint[i] ) * 0.5f;
			return;
		}
	}
	rvecvContactPoint.push_back( v );
}



static void AddContactPoint( Vector3& rvNormal, Scalar fPenetrationDepth,
												CJL_Shape_Box& rBox,
												CIndexedTriangle& rIndexedTriangle,
												CTriangle& rTriangle,
												CJL_CollisionFunctor& rColFunctor,
												Scalar fOldDepth )
{

	Scalar fMaxCombinationDistSq = 0.05f;
	TCFixedVector<Vector3, MAX_POINTS_BOX_TRIANGLE> vecvContactPoint;

	///// 1. check box edges against triangle
	Vector3 vWorldOnPlanePos, vLocalPointPos, vEdge, vCross[3], vWorldStart, vWorldGoal;
	Scalar d0 = 0, d1 = 0;
	Scalar fDist = Vec3Dot( rTriangle.GetNormal(), rTriangle.GetVertex(0) );
	Scalar min_depth2 = 999999;

	int i, iNumEdges = 12;
	for( i=0; i<iNumEdges; i++ )
	{
		rBox.GetWorldEdge(i, vWorldStart, vWorldGoal);

		d0 = Vec3Dot( rTriangle.GetNormal(), vWorldStart ) - fDist;
		d1 = Vec3Dot( rTriangle.GetNormal(), vWorldGoal  ) - fDist;
		if( (0 < d0 && 0 < d1) || (d0 < 0 && d1 < 0) )
			continue;	// no intersection

		if( fabsf(d0-d1) < 0.0001f )
//		if( fabsf(d0-d1) < 0.005f )
			continue;	// the box edge is almost parallel to the plane of the triangle

		// update minimum penetration depth
/*		Scalar depth;
		if( d0 < 0 )
			depth = -d0;
		else
			depth = -d1;

		if( depth < min_depth2 )
			min_depth2 = depth;
*/
		// calculate the point of intersection on the plane
		vWorldOnPlanePos = vWorldStart + (vWorldGoal - vWorldStart) * d0 / (d0 - d1);

		// Now, we know that the line segment is crossing the plane of the polygon.
		// check if it is intersecting with the polygon

		vLocalPointPos = vWorldOnPlanePos - rTriangle.GetVertex(0);
		Vec3Cross( vCross[0], rTriangle.GetEdge(0), vLocalPointPos );

		vLocalPointPos = vWorldOnPlanePos - rTriangle.GetVertex(1);
		Vec3Cross( vCross[1], rTriangle.GetEdge(1), vLocalPointPos );

		if( Vec3Dot(vCross[0], vCross[1]) <= 0 )
			continue;	// not intersecting with this triangle

		vLocalPointPos = vWorldOnPlanePos - rTriangle.GetVertex(2);
		Vec3Cross( vCross[2], rTriangle.GetEdge(2), vLocalPointPos );

		if( Vec3Dot(vCross[1], vCross[2]) <= 0 )
			continue;	// not intersecting with this triangle

		if( Vec3Dot(vCross[2], vCross[0]) <= 0 )
			continue;	// not intersecting with this triangle

		if( !IsSensible( vWorldOnPlanePos ) )
			int error = 1;

		// found intersection - add contact point
		AddPoint( vecvContactPoint, vWorldOnPlanePos, fMaxCombinationDistSq );
	}

	///// 2. check triangle edges against box
	STrace tr;
	tr.bvType = BVTYPE_DOT;
	CBSPTreeForAABB bsptree;
	Vector3 vHalfLength = rBox.GetSideLength() * 0.5f;
	bsptree.SetAABB( vHalfLength );
	AABB3 box_aabb;
	box_aabb.vMin = -vHalfLength;
	box_aabb.vMax =  vHalfLength;

	CBSPTreeForBox bsp_tree;
	bsp_tree.SetSize( rBox.GetSideLength().x, rBox.GetSideLength().y, rBox.GetSideLength().z );

	// transform edges to the local space of the box, where the origin is the center of the box
	// and the orientation is aligned with the three faces of the box
	Vector3 *pavLocalEdge = vCross;
	rBox.GetWorldPose().InvTransform( pavLocalEdge[0], rTriangle.GetVertex(0) );
	rBox.GetWorldPose().InvTransform( pavLocalEdge[1], rTriangle.GetVertex(1) );
	rBox.GetWorldPose().InvTransform( pavLocalEdge[2], rTriangle.GetVertex(2) );

//	char acIndex[12] = { 0,1, 1,2, 2,0,  1,0, 2,1, 0,2 };
	char acIndex[12] = { 0,1, 1,0,  1,2, 2,1,  2,0, 0,2 };
	for( i=0; i<12; i+=2 )
	{
		if( !rIndexedTriangle.EdgeCollision(i/4) )
			continue;

		// check if the start of the line segment is in the volume of the box
		if( box_aabb.IsPointInside( pavLocalEdge[acIndex[i]] ) )
			continue;	// the vertex is included in the box

//		tr.vStart = pavLocalEdge[ acIndex[i] ];
//		tr.vGoal  = pavLocalEdge[ acIndex[i+1] ];
		tr.fFraction = 1.0f;
//		tr.in_solid = false;
///		bsptree.ClipLineTrace( tr );

		bsp_tree.ClipTrace( tr.vEnd,
			                tr.fFraction,
							pavLocalEdge[acIndex[i]],
							pavLocalEdge[acIndex[i+1]],
							0 );

		if( !IsSensible( tr.vEnd ) )
			int error = 1;

		if( tr.fFraction < 1.0f )
		{	// found collision
			// transform contact point from box space to world space
			rBox.GetWorldPose().Transform( vWorldGoal, tr.vEnd );
			AddPoint( vecvContactPoint, vWorldGoal, fMaxCombinationDistSq );
		}
	}

	///// 3.  store contact info

	const Vector3& vBoxNewPos = rBox.GetWorldPose().vPosition;
	const Vector3& vBoxActorNewPos = rBox.GetPhysicsActor()->GetPosition();
//	const Vector3 vTriNewPos = rTriangle.GetCenter();

#ifndef USE_COMBINED_COLLISION_POINTS_INFO

	CJL_ContactInfo contact;

	contact.pBody0 = &rBox;
	contact.pBody1 = NULL;	// static geometry only

	//same normal and penetration depths are applied to contact points
	contact.vNormal = rvNormal;
	contact.fPenetrationDepth = fPenetrationDepth;
	contact.InitialPenetration = fOldDepth;


///	if( 0 < d0 && 0 < d1 )
///		contact.fPenetrationDepth = d0 <= 0 ? -d0 : -d1;
//	contact.vNormal = rBox.GetWorldOrient() * tr.plane.normal;

	// if the contact normal is the normal of the triangle
	// use the penetration depth calculated by the box edge penetration into triangle surface
//	if( rvNormal == rTriangle.GetNormal() )
//		contact.fPenetrationDepth = min_depth2;

	for( i=0; i<vecvContactPoint.size(); i++ )
	{
		contact.vContactPoint = vecvContactPoint[i];
		contact.vR0 = vecvContactPoint[i] - vBoxActorNewPos;
		contact.vR1 = vecvContactPoint[i] - Vector3(0,0,0);
///		rColFunctor.AddContact( contact );
		rColFunctor.AddTemporaryContact( contact );
	}

#else

	TCFixedVector<CJL_CollPointInfo, MAX_POINTS_BOX_TRIANGLE> vecCollPointInfo;

	for( i=0; i<vecvContactPoint.size(); i++ )
	{
		vecCollPointInfo.push_back( CJL_CollPointInfo( vecvContactPoint[i] - vBoxActorNewPos,
			                                           vecvContactPoint[i] - Vector3(0,0,0),
													   fOldDepth ) );

		// store contact position
		vecCollPointInfo.back().vContactPosition = vecvContactPoint[i];
	}


	rColFunctor.AddTemporaryContacts( &rBox,
		                              NULL,
									  rvNormal,
									  &vecCollPointInfo[0],
									  vecCollPointInfo.size() );

#endif

}
