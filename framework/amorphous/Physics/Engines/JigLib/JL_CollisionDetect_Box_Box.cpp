#include "JL_CollisionDetect.hpp"
#include "JL_Shape_Box.hpp"
#include "JL_PhysicsActor.hpp"
#include "JL_CollisionFunctor.hpp"
#include "Geometry/JL_Box.hpp"
#include "3DMath/BSPTreeForBox.hpp"


namespace amorphous
{

//#include "Stage/bsptree.hpp"
// static bool SepAxisTest( float& rfDepth, Vector3& rvAxis, CJL_Shape_Box& rBox0, CJL_Shape_Box& rBox1 ){}


void GetSupportPoint( Vector3& v, CJL_Shape_Box& box, Vector3& vAxis )
{
	const Scalar threshold = 0.001f;

	Matrix33& rBoxOrient = box.GetWorldPose().matOrient;
	Scalar as = Vec3Dot( vAxis, rBoxOrient.GetColumn(0) );
	Scalar au = Vec3Dot( vAxis, rBoxOrient.GetColumn(1) );
	Scalar ad = Vec3Dot( vAxis, rBoxOrient.GetColumn(2) );

	// get the center position of the box
	Vector3 vP = box.GetWorldPose().vPosition;

	Vector3 vBoxHalfLength = box.GetSideLength() * 0.5f;

	if( as < -threshold )
		vP += rBoxOrient.GetColumn(0) * vBoxHalfLength.x;
	else if( threshold < as )
		vP -= rBoxOrient.GetColumn(0) * vBoxHalfLength.x;

	if( au < -threshold )
		vP += rBoxOrient.GetColumn(1) * vBoxHalfLength.y;
	else if( threshold < au )
		vP -= rBoxOrient.GetColumn(1) * vBoxHalfLength.y;

	if( ad < -threshold )
		vP += rBoxOrient.GetColumn(2) * vBoxHalfLength.z;
	else if( threshold < ad )
		vP -= rBoxOrient.GetColumn(2) * vBoxHalfLength.z;
}


//#define MAX_CONTACTS_PER_BOX_PAIR	8
#define MAX_CONTACTS_PER_BOX_PAIR	12


static inline bool AddPoint( TCFixedVector<Vector3,MAX_CONTACTS_PER_BOX_PAIR> &pts,
							 Vector3& pt,
							 Scalar combination_dist_sq )
{
	for( unsigned i = pts.size(); i-- != 0; )
	{
		if( Vec3LengthSq( pts[i] - pt ) < combination_dist_sq )
		{
			pts[i] = 0.5f * ( pts[i] + pt );
			return false;
		}
	}
	if( pts.size() < MAX_CONTACTS_PER_BOX_PAIR )
		pts.push_back( pt );
	return true;
}


static unsigned GetAABox2EdgeIntersectionPoints( TCFixedVector<Vector3,MAX_CONTACTS_PER_BOX_PAIR> &pts,
										         const Vector3& vSides,
										         const CJL_Box& rBox,
										         const Vector3& vEdgePt0,
										         const Vector3& vEdgePt1,
										         const Matrix34& rOrigBoxPose,
										         Scalar combination_dist_sq )
{
	unsigned num = 0;
	for( unsigned iDir = 3; iDir-- != 0 ; )
	{
		const unsigned jDir = (iDir + 1) % 3;
		const unsigned kDir = (iDir + 2) % 3;

		// one plane goes through the origin, one is offset
//		const Scalar face_offset[] = {0.0f, vSides[iDir]};
		const Scalar face_offset[] = { -vSides[iDir] * 0.5f, vSides[iDir] * 0.5f };
		for( unsigned iFace = 2; iFace-- != 0 ; )
		{
			// distance of each point from to the plane
			const Scalar dist0 = vEdgePt0[iDir] - face_offset[iFace];
			const Scalar dist1 = vEdgePt1[iDir] - face_offset[iFace];
			Scalar frac = -1.0f;
			if( dist0 * dist1 < -SCALAR_TINY )
				frac= -dist0 / (dist1 - dist0);
//			else if(fabsf(dist0) < SCALAR_TINY )
			else if(fabsf(dist0) < SCALAR_TINY )
				frac = 0.0f;
			else if( fabsf(dist1) < SCALAR_TINY )
				frac = 1.0f;
			if( 0.0f <= frac )
			{
				assert( frac <= 1.0f );
				Vector3 vP = (1.0f - frac) * vEdgePt0 + frac * vEdgePt1;
				// check the point is within the face rectange
//				if( (-SCALAR_TINY < vP[jDir]) && (vP[jDir] < vSides[jDir] + SCALAR_TINY) &&
//					(-SCALAR_TINY < vP[kDir]) && (vP[kDir] < vSides[kDir] + SCALAR_TINY) )
				Scalar side_j = vSides[jDir] * 0.5f;
				Scalar side_k = vSides[kDir] * 0.5f;
				if( (-SCALAR_TINY - side_j < vP[jDir]) && (vP[jDir] < side_j + SCALAR_TINY) &&
					(-SCALAR_TINY - side_k < vP[kDir]) && (vP[kDir] < side_k + SCALAR_TINY) )
				{
					// got a point
					Vector3 vPos;
					rOrigBoxPose.Transform( vPos, vP );
					AddPoint(pts, vPos, combination_dist_sq);
//					pts.push_back(vPos);
					if( ++num == 2 )
						return num;
				}
			}
		}
	}
	return num;
}


static unsigned GetAABox2EdgesIntersectionPoints( TCFixedVector<Vector3,MAX_CONTACTS_PER_BOX_PAIR> &pts,
												  const Vector3& vSides,
												  const CJL_Box& rBox,
												  const Matrix34& rOrigBoxPose,
												  Scalar combination_dist_sq )
{
	unsigned num = 0;

	static Vector3 avBoxPts[8];
	rBox.GetCornerPoints( avBoxPts );

	const CJL_Box::Edge *edges = rBox.GetAllEdges();

	for( unsigned iEdge = 0; iEdge<12; ++iEdge )
	{
		const Vector3& vEdgePt0 = avBoxPts[ edges[iEdge].index0 ];
		const Vector3& vEdgePt1 = avBoxPts[ edges[iEdge].index1 ];

		num += GetAABox2EdgeIntersectionPoints( pts,
			                                    vSides,
												rBox,
												vEdgePt0, vEdgePt1,
												rOrigBoxPose,
												combination_dist_sq );
		// Don't think we can get more than 8...
		if( 8 <= num )
			return num;
	}

	return num;
}


static unsigned GetBoxBoxIntersectionPoints( TCFixedVector<Vector3,MAX_CONTACTS_PER_BOX_PAIR> &pts,
											 const CJL_Shape_Box& rBox0,
											 const CJL_Shape_Box& rBox1,
											 Scalar combination_dist )
{
	// transform box1 into box0 space
	for( unsigned iBox=0; iBox<2; ++iBox )
	{
		const CJL_Shape_Box& boxA = iBox ? rBox1 : rBox0;
		const CJL_Shape_Box& boxB = iBox ? rBox0 : rBox1;

		Matrix34 inv_pose_boxA;
/*		Matrix34 boxA_offset;
		Matrix34 boxB_offset;
		boxA_offset.Identity();
		boxA_offset.vPosition = boxA.GetSideLength() * 0.5f;
		boxB_offset.Identity();
		boxB_offset.vPosition = - boxB.GetSideLength() * 0.5f;*/
		boxA.GetWorldPose().GetInverseROT( inv_pose_boxA );
//		boxA_offset = boxA_offset * boxA.GetWorldPose();
//		boxA_offset.GetInverseROT( inv_pose_boxA );
		CJL_Box box( inv_pose_boxA * boxB.GetWorldPose(),
			         boxB.GetSideLength() );

		// if we get more than a certain number of points back from this cal,
		// and iBox == 0, could probably skip the other test...
		GetAABox2EdgesIntersectionPoints( pts, boxA.GetSideLength(), box,
			boxA.GetWorldPose(), combination_dist * combination_dist );
	}

	return pts.size();
}


static void AddCollisionPoints( TCFixedVector< CJL_CollPointInfo, MAX_CONTACTS_PER_BOX_PAIR >& vecCollPointInfo,
 							    Vector3& rvNormal,
							    Scalar fPenetrationDepth,
							    CJL_Shape_Box& rBox0,
							    CJL_Shape_Box& rBox1,
							    CJL_CollisionFunctor& rColFunctor )
{

	CJL_PhysicsActor *pActor0 = rBox0.GetPhysicsActor();
	CJL_PhysicsActor *pActor1 = rBox1.GetPhysicsActor();
	const Vector3& vOldPos0 = pActor0 ? rBox0.GetOldWorldPose().vPosition : Vector3(0,0,0);
	const Vector3& vOldPos1 = pActor1 ? rBox1.GetOldWorldPose().vPosition : Vector3(0,0,0);
	const Vector3& vNewPos0 = pActor0 ? rBox0.GetWorldPose().vPosition : Vector3(0,0,0);
	const Vector3& vNewPos1 = pActor1 ? rBox1.GetWorldPose().vPosition : Vector3(0,0,0);

	const Vector3 vBodyDelta = (vNewPos0 - vOldPos0) - (vNewPos1 - vOldPos1);
	const Scalar fBodyDeltaLen = Vec3Dot( vBodyDelta, rvNormal );
	Scalar fOldDepth = fPenetrationDepth + fBodyDeltaLen;

	// if oldDepth > 0 then reduce it so that the penetration code will
	// result in a torque???
	if( 0 < fOldDepth )
		fOldDepth *= 0.8f;

	TCFixedVector<Vector3,MAX_CONTACTS_PER_BOX_PAIR> vecPts;
	GetBoxBoxIntersectionPoints( vecPts, rBox0, rBox1, 0.01f );

	for( int i=0; i<vecPts.size(); i++ )
	{
		const Vector3& vNewActorPos0 = rBox0.GetPhysicsActor()->GetPosition();
		const Vector3& vNewActorPos1 = rBox1.GetPhysicsActor()->GetPosition();
		vecCollPointInfo.push_back( CJL_CollPointInfo( vecPts[i] - vNewActorPos0,
			                                           vecPts[i] - vNewActorPos1,
									                   fOldDepth ) );

		vecCollPointInfo.back().vContactPosition = vecPts[i];
	}
}


static void AddContactPoint( TCFixedVector< CJL_CollPointInfo, MAX_CONTACTS_PER_BOX_PAIR >& vecCollPointInfo,
 							 Vector3& rvNormal,
							 Scalar fPenetrationDepth,
							 CJL_Shape_Box& rBox0,
							 CJL_Shape_Box& rBox1,
							 CJL_CollisionFunctor& rColFunctor )
/*
static void AddContactPoint( Vector3& rvNormal, Scalar fPenetrationDepth,
												CJL_Shape_Box& rBox0,
												CJL_Shape_Box& rBox1,
												CJL_CollisionFunctor& rColFunctor )*/
{
	CJL_ContactInfo contact;

	contact.pBody0 = &rBox0;
	contact.pBody1 = &rBox1;

	// same normal and penetration depths are applied to contact points
	contact.vNormal = rvNormal;
	contact.fPenetrationDepth = fPenetrationDepth;

	///// check box edges against box
	float fFraction = 1.0f;
	Vector3 vEnd = Vector3(0,0,0);

	BSPTreeForBox bsp_tree;

	// transform edges to the local space of the box
	bool b0, b1;
	Vector3 vWorldStart, vWorldGoal, vLocalStart, vLocalGoal;
	Vector3 vHalfLength;
	Matrix34 actor_world_pose;
	Matrix34 box_local_pose;
	AABB3 aabb;
	int i, j;
	CJL_Shape_Box *pBox[2];
	pBox[0] = &rBox0;
	pBox[1] = &rBox1;

	TCFixedVector< Vector3, MAX_CONTACTS_PER_BOX_PAIR > vecPts;

	for( i=0; i<2; i++ )
	{
		// check the edges of box[(i+1)%2] against box[i] (swap boxes and do this for each other)

		vHalfLength = pBox[i]->GetSideLength() * 0.5f;
///		bsptree.SetAABB( vHalfLength );
		aabb.vMin = -vHalfLength;
		aabb.vMax =  vHalfLength;

		Vector3 vSideLength = pBox[i]->GetSideLength();
		bsp_tree.SetSize( vSideLength.x, vSideLength.y, vSideLength.z );


		for( j=0; j<12; j+=2 )
		{
			pBox[(i+1)%2]->GetWorldEdge( j, vWorldStart, vWorldGoal );

			// transform from world space to 'pBox[i]' space
			pBox[i]->GetWorldPose().InvTransform( vLocalStart, vWorldStart );
			pBox[i]->GetWorldPose().InvTransform( vLocalGoal,  vWorldGoal );

			b0 = aabb.IsPointInside( vLocalStart );
			b1 = aabb.IsPointInside( vLocalGoal );

			if( b0 && b1 )
				continue;	// the entire line segment is inside the box[i]

			if( !b0 )
			{
				fFraction = 1.0f;
				bsp_tree.ClipTrace( vEnd, fFraction, vLocalStart, vLocalGoal, 0 );
				if( fFraction < 1.0f )
				{
					// found collision
					if( fFraction < SCALAR_TINY )
					{
						fFraction = 0;
						vEnd = vLocalStart;
					}
					else if( 1 - SCALAR_TINY < fFraction )
					{
						fFraction = 1.0f;
						vEnd = vLocalGoal;
					}
					// transform contact point from the local space of pBox[i] to the world space
					pBox[i]->GetWorldPose().Transform( contact.vContactPoint, vEnd );
					
					// transform contact normal (orientation only)
//					contact.vNormal = pBox[i]->GetWorldOrient() * tr.plane.normal;
//					contact.fPenetrationDepth = fPenetrationDepth;

					if( vecPts.size() < MAX_CONTACTS_PER_BOX_PAIR )
						vecPts.push_back( contact.vContactPoint );
				}
			}
			if( !b1 )
			{
				fFraction = 1.0f;
				bsp_tree.ClipTrace( vEnd, fFraction, vLocalGoal, vLocalStart, 0 );
				if( fFraction < 1.0f )
				{
					// found collision
					if( fFraction < SCALAR_TINY )
					{
						fFraction = 0;
						vEnd = vLocalStart;
					}
					else if( 1 - SCALAR_TINY < fFraction )
					{
						fFraction = 1.0f;
						vEnd = vLocalGoal;
					}
					// transform contact point from the local space of pBox[i] to the world space
					pBox[i]->GetWorldPose().Transform( contact.vContactPoint, vEnd );
					
					if( vecPts.size() < MAX_CONTACTS_PER_BOX_PAIR )
						vecPts.push_back( contact.vContactPoint );
				}
			}
			
		}
	}

	CJL_PhysicsActor *pActor0 = rBox0.GetPhysicsActor();
	CJL_PhysicsActor *pActor1 = rBox1.GetPhysicsActor();
//	const Vector3& vOldPos0 = pActor0 ? pActor0->GetOldPosition() : Vector3(0,0,0);
//	const Vector3& vOldPos1 = pActor1 ? pActor1->GetOldPosition() : Vector3(0,0,0);
	const Vector3& vNewActorPos0 = pActor0 ? pActor0->GetPosition() : Vector3(0,0,0);
	const Vector3& vNewActorPos1 = pActor1 ? pActor1->GetPosition() : Vector3(0,0,0);

	const Vector3& vOldPos0 = pActor0 ? rBox0.GetOldWorldPose().vPosition : Vector3(0,0,0);
	const Vector3& vOldPos1 = pActor1 ? rBox1.GetOldWorldPose().vPosition : Vector3(0,0,0);
	const Vector3& vNewPos0 = pActor0 ? rBox0.GetWorldPose().vPosition : Vector3(0,0,0);
	const Vector3& vNewPos1 = pActor1 ? rBox1.GetWorldPose().vPosition : Vector3(0,0,0);

	const Vector3 vBodyDelta = (vNewPos0 - vOldPos0) - (vNewPos1 - vOldPos1);
	const Scalar fBodyDeltaLen = Vec3Dot( vBodyDelta, rvNormal );
	Scalar fOldDepth = fPenetrationDepth + fBodyDeltaLen;

	// if oldDepth > 0 then reduce it so that the penetration code will
	// result in a torque???
	if( 0 < fOldDepth )
		fOldDepth *= 0.8f;


#ifndef USE_COMBINED_COLLISION_POINTS_INFO


	contact.InitialPenetration = fOldDepth;
	contact.pBody0 = (CJL_ShapeBase *)&rBox0;
	contact.pBody1 = (CJL_ShapeBase *)&rBox1;

	int num_pnts = vecPts.size();
	for( i=0; i<num_pnts; i++ )
	{
		contact.vContactPoint = vecPts[i];
//		contact.vR0 = vecPts[i] - vNewPos0;
//		contact.vR1 = vecPts[i] - vNewPos1;
		contact.vR0 = vecPts[i] - vNewActorPos0;
		contact.vR1 = vecPts[i] - vNewActorPos1;

		rColFunctor.AddTemporaryContact( contact );
	}

#else

//	TCFixedVector< CJL_CollPointInfo, MAX_CONTACTS_PER_BOX_PAIR > vecCollPointInfo;
	int num_pnts = vecPts.size();
	for( i=0; i<num_pnts; i++ )
	{
		vecCollPointInfo.push_back( CJL_CollPointInfo( vecPts[i] - vNewActorPos0,
			                                           vecPts[i] - vNewActorPos1,
													   fOldDepth ) );

		// store contact position
		vecCollPointInfo.back().vContactPosition = vecPts[i];
	}

///	rColFunctor.AddTemporaryContacts( &rBox0,
///		                              &rBox1,
///									  rvNormal,
///									  &vecCollPointInfo[0],
///									  vecCollPointInfo.size() );


#endif

/*	rColFunctor.AddTemporaryContacts( (CJL_ShapeBase *)&rBox0,
		                              (CJL_ShapeBase *)&rBox1,
									  rvNormal,
									  &vecPts[0],
									  vecPts.size(),
									  fOldDepth );*/
}



void JL_CollisionDetect_Box_Box( CJL_Shape_Box& rBox0, CJL_Shape_Box& rBox1,
							     CJL_CollisionFunctor& rColFunctor )
{

	// set up 15 axes to check overlaps between boxes
	Vector3 avAxis[15];
	avAxis[0] = rBox0.GetWorldOrient().GetColumn(0);
	avAxis[1] = rBox0.GetWorldOrient().GetColumn(1);
	avAxis[2] = rBox0.GetWorldOrient().GetColumn(2);
	avAxis[3] = rBox1.GetWorldOrient().GetColumn(0);
	avAxis[4] = rBox1.GetWorldOrient().GetColumn(1);
	avAxis[5] = rBox1.GetWorldOrient().GetColumn(2);

	Vec3Cross( avAxis[6],  avAxis[0], avAxis[3] );
	Vec3Cross( avAxis[7],  avAxis[0], avAxis[4] );
	Vec3Cross( avAxis[8],  avAxis[0], avAxis[5] );
	Vec3Cross( avAxis[9],  avAxis[1], avAxis[3] );
	Vec3Cross( avAxis[10], avAxis[1], avAxis[4] );
	Vec3Cross( avAxis[11], avAxis[1], avAxis[5] );
	Vec3Cross( avAxis[12], avAxis[2], avAxis[3] );
	Vec3Cross( avAxis[13], avAxis[2], avAxis[4] );
	Vec3Cross( avAxis[14], avAxis[2], avAxis[5] );

	Scalar afDepth[15], fLength;
	Scalar fMinDepth = 100.0f;
	int i, iMinAxis = -1;
	float fCollTolerance = rColFunctor.m_fCollTolerance;
	for(i=0; i<15; i++)
	{
		afDepth[i] = 101.0f;

		if( Vec3LengthSq(avAxis[i]) < 0.0000001f )
			continue;	// invalid axis

		// separation axis test between 'rBox0' and 'rBox1'
		if( !rBox0.SepAxisTest( afDepth[i], avAxis[i], rBox1, fCollTolerance ) )
			return;	// no overlap

		if( 6<=i )
		{	// need to normalize axis length and penetration depth
			fLength = Vec3Length( avAxis[i] );
			if( fLength < 0.000001f )
				continue;

			avAxis[i] /= fLength;
			afDepth[i] /= fLength;
		}

		if( afDepth[i] < fMinDepth )
		{
			fMinDepth = afDepth[i];
			iMinAxis = i;
		}
	}

	if( iMinAxis < 0 )
		return;	// no collision detected

	if( 0.3f < fMinDepth || fMinDepth < 0.0f )
		int iUnexpected = 1;


	// make sure the contact normal is heading toward box0
	Vector3 vBox1ToBox0 = rBox0.GetWorldPosition() - rBox1.GetWorldPosition();
	if( Vec3Dot( vBox1ToBox0, avAxis[iMinAxis] ) < 0 )
		avAxis[iMinAxis] *= -1.0f;

	Vector3& N = avAxis[iMinAxis];

	TCFixedVector< CJL_CollPointInfo, MAX_CONTACTS_PER_BOX_PAIR > vecCollPointInfo;

	if( /*use_bsptree_collision_detect*/ false )
	{
		// add contact points based on the minimum penetration depth
//		AddContactPoint( avAxis[iMinAxis], fMinDepth, rBox0, rBox1, rColFunctor );
		AddContactPoint( vecCollPointInfo,
						 avAxis[iMinAxis], fMinDepth, rBox0, rBox1, rColFunctor );
	}
	else
		AddCollisionPoints( vecCollPointInfo, avAxis[iMinAxis], fMinDepth, rBox0, rBox1, rColFunctor );

	// also get the point from SAT so that we can obtain
	// the penetration depth for each intersection point
	Vector3 vSATPoint = Vector3(0,0,0);
	switch(iMinAxis)
	{
	case 0:
	case 1:
	case 2:
		{
			// box0 face, box1 corner collision
			// get the lowest point on the box1 along box1 normal
			Vector3 vP1;
			GetSupportPoint( vP1, rBox1, -N );
			vSATPoint = ( vP1 - (0.5f * fMinDepth) * N );
		}
		break;

	case 3:
	case 4:
	case 5:
		{
			// box0 corner, box1 face collision
			// get the lowest point on the box1 along box1 normal
			Vector3 vP0;
			GetSupportPoint( vP0, rBox0, N );
			vSATPoint = ( vP0 + (0.5f * fMinDepth) * N );
		}
		break;

	// we have an edge/edge collision
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		{
			// retrieve which edges collided
			int i = iMinAxis - 6;
			int ia = i / 3;
			int ib = i - ia * 3;
			assert(0 <= ia && ia < 3);
			assert(0 <= ib && ib < 3);
			// find two vP0, vP1 oint on both edges
			Vector3 vP0, vP1;
			GetSupportPoint( vP0, rBox0, N );
			GetSupportPoint( vP1, rBox1, -N );
			// find edge intersection
			// plane along N and F, and passing through PB
			Vector3 vPlaneNormal = Vec3Cross( N, rBox1.GetWorldOrient().GetColumn(ib) );
			Scalar plane_dist = Vec3Dot( vPlaneNormal, vP1 );
			// find the intersection T, where Pintersection = vP0 + t * box edge dir
			Scalar div = Vec3Dot( rBox0.GetWorldOrient().GetColumn(ia), vPlaneNormal );
			// plane and ray colinear, skip the intersection.
			if( fabsf(div) < SCALAR_TINY )
				return;	// false;

			Scalar t = (plane_dist - Vec3Dot(vP0, vPlaneNormal)) / div;
			// point on edge of box0
			vP0 += rBox0.GetWorldOrient().GetColumn(ia) * t;
			vSATPoint = ( vP0 + (0.5f * fMinDepth) * N );
		}
		break;

	}

	if( vecCollPointInfo.size() < MAX_CONTACTS_PER_BOX_PAIR )
	{
		const Vector3& vNewActorPos0 = rBox0.GetPhysicsActor()->GetPosition();
		const Vector3& vNewActorPos1 = rBox1.GetPhysicsActor()->GetPosition();
		Scalar old_depth = vecCollPointInfo.back().InitialPenetration;
		vecCollPointInfo.push_back( CJL_CollPointInfo( vSATPoint - vNewActorPos0,
													   vSATPoint - vNewActorPos1,
													   old_depth ) );

		vecCollPointInfo.back().vContactPosition = vSATPoint;
	}

	rColFunctor.AddTemporaryContacts( &rBox0,
		                              &rBox1,
									  N,
									  &vecCollPointInfo[0],
									  vecCollPointInfo.size() );
}


} // namespace amorphous
