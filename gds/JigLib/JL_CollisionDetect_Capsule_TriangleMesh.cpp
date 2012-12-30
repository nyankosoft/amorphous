#include "JL_CollisionDetect.hpp"

#include "TriangleMesh.hpp"
#include "JL_Shape_Capsule.hpp"
#include "JL_CollisionFunctor.hpp"
#include "Geometry/distance.hpp"

#include <vector>


namespace amorphous
{
using namespace std;
using namespace JigLib;


#define NUM_MAX_CAPSULE_TRIANGLE_COLLS	8


void JL_CollisionDetect_Capsule_TriangleMesh( CJL_Shape_Capsule& rCapsule, CTriangleMesh& rTriMesh,
											          CJL_CollisionFunctor& rColFunctor)
{
//	Scalar fCollTolerance = 0.00f;
//	Scalar fCollTolerance = 0.005f;
	Scalar fCollTolerance = rColFunctor.m_fCollTolerance;

	Scalar fCapsuleTolR = fCollTolerance + rCapsule.GetRadius();
	Scalar fCapsuleTolR2 = fCapsuleTolR * fCapsuleTolR;
	static vector<Vector3> vecvCollPts;
	vecvCollPts.resize(0);
	Vector3 vCollNormal = Vector3(0,0,0);
	Scalar fLargestDepth = 0.0f;

	// get candidate triangles for collisions
	static vector<int> s_veciIndexList;
	s_veciIndexList.resize(0);
	rTriMesh.GetIntersectingTriangles( s_veciIndexList, rCapsule.GetWorldAABB() );

	int i, iNumTriangles = s_veciIndexList.size();
	for( i=0; i<iNumTriangles; i++ )
	{	// check collision with each triangle

		if( NUM_MAX_CAPSULE_TRIANGLE_COLLS <= vecvCollPts.size() )
			break;

		CIndexedTriangle& rTriangle	= rTriMesh.GetTriangle( s_veciIndexList[i] );		// get indexed triangle
		CTriangle triangle( rTriMesh.GetVertex( rTriangle.GetIndex(0) ),
			                rTriMesh.GetVertex( rTriangle.GetIndex(1) ),
							rTriMesh.GetVertex( rTriangle.GetIndex(2) ),
							rTriangle.GetNormal() );

		SPlane plane = triangle.GetPlane();

		const Vector3 vCapsuleStart = rCapsule.GetSegmentStartPos();
		const Vector3 vCapsuleEnd   = rCapsule.GetSegmentEndPos();

		float fDistToStart = plane.GetDistanceFromPoint( vCapsuleStart );
		float fDistToEnd   = plane.GetDistanceFromPoint( vCapsuleEnd );

		if( fCapsuleTolR < fDistToStart && fCapsuleTolR < fDistToEnd )
			continue;

		if( fDistToStart < 0.0f && fDistToEnd < 0.0f )
			return;

		tSegment seg( vCapsuleStart, vCapsuleEnd - vCapsuleStart );

		Scalar tS, tT0, tT1;
		Scalar d2 = SegmentTriangleDistanceSq( &tS, &tT0, &tT1, seg, triangle );

		if( d2 < SCALAR_TINY )
		{
			fLargestDepth = rCapsule.GetRadius();
			vecvCollPts.push_back( seg.GetPoint(tS) );
			vCollNormal += rTriangle.GetNormal();
		}
		else if( d2 < fCapsuleTolR2 )
		{
			Scalar fDist = sqrt(d2);
			Scalar fDepth = rCapsule.GetRadius() - fDist;
			Vector3 vPos = triangle.GetPoint( tT0, tT1 );
			Vector3 vCollisionNormal = (seg.GetPoint(tS) - vPos);
			Vec3Normalize( vCollisionNormal, vCollisionNormal );	// TODO: need safer normalization

			// test for depth calc
			Scalar d0 = rCapsule.GetRadius() - triangle.GetPlane().GetDistanceFromPoint( vCapsuleStart );
			Scalar d1 = rCapsule.GetRadius() - triangle.GetPlane().GetDistanceFromPoint( vCapsuleEnd );
			fDepth = d0 > d1 ? d0 : d1;

			if( fLargestDepth < fDepth )
			{
				fLargestDepth = fDepth;
				vecvCollPts.push_back( vPos );
				vCollNormal += vCollisionNormal;
			}
		}
	}

	if( !vecvCollPts.empty() )
	{
		Vec3Normalize( vCollNormal, vCollNormal );	// TODO: need safer normalization

#ifndef USE_COMBINED_COLLISION_POINTS_INFO

///		rColFunctor.AddContacts( (CJL_ShapeBase *)&rCapsule,
		rColFunctor.AddTemporaryContacts( (CJL_ShapeBase *)&rCapsule,
			                     NULL,
								 vCollNormal,
								 &vecvCollPts[0],
								 vecvCollPts.size(),
								 fLargestDepth );

#else

		TCFixedVector< CJL_CollPointInfo, NUM_MAX_CAPSULE_TRIANGLE_COLLS > vecCollPtInfo;

		for( int j=0; j<vecvCollPts.size(); j++ )
		{
			vecCollPtInfo.push_back( CJL_CollPointInfo( vecvCollPts[j] - rCapsule.GetWorldPose().vPosition,
				                                        vecvCollPts[j] - Vector3(0,0,0),
														0 /* initial_penetration */ ) );
		}

		rColFunctor.AddTemporaryContacts( &rCapsule, NULL, vCollNormal, &vecCollPtInfo[0], vecCollPtInfo.size() );

#endif

	}
}


} // namespace amorphous
