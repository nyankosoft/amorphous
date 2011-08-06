#include "JL_CollisionDetect.hpp"
#include "JL_Shape_Capsule.hpp"
#include "JL_CollisionFunctor.hpp"

#include "JigLib/Geometry/line.hpp"
#include "JigLib/Geometry/distance.hpp"

using namespace std;

//using namespace JigLib;



void JL_CollisionDetect_Capsule_Capsule( CJL_Shape_Capsule& rCapsule0, CJL_Shape_Capsule& rCapsule1,
									     CJL_CollisionFunctor& rColFunctor )
{
//	Scalar fColTolerance = 0.005f;

	Scalar fColTolerance = rColFunctor.m_fCollTolerance;

//	const tSegment seg0( rCapsule0.GetWorldPosition(), rCapsule0.GetLength() * rCapsule0.GetWorldOrient().get_col(2) );
//	const tSegment seg1( rCapsule1.GetWorldPosition(), rCapsule1.GetLength() * rCapsule1.GetWorldOrient().get_col(2) );
	const tSegment seg0 = rCapsule0.GetSegment();
	const tSegment seg1 = rCapsule1.GetSegment();
	Scalar t0, t1;
	Scalar fDistSq = SegmentSegmentDistanceSq( &t0, &t1, seg0, seg1 );
	Scalar fRadSum = rCapsule0.GetRadius() + rCapsule1.GetRadius();

	if( (fRadSum + fColTolerance) * (fRadSum + fColTolerance) <= fDistSq )
		return;

	Vector3 vPos0 = seg0.GetPoint(t0);
	Vector3 vPos1 = seg1.GetPoint(t1);

	Vector3 vDelta = vPos0 - vPos1;

	Scalar fDist = sqrtf(fDistSq);
	Scalar fDepth = fRadSum - fDist;
	if( SCALAR_TINY < fDist )
	{
		vDelta /= fDist;
	}
	else
	{
		// TODO - make this not random... !
		Vec3Normalize( vDelta, vDelta );
	}
	Vector3 vWorldPos = vPos1 + ( rCapsule1.GetRadius() - 0.5f * fDepth ) * vDelta;


#ifndef USE_COMBINED_COLLISION_POINTS_INFO

	CJL_ContactInfo collision;
	collision.pBody0 = (CJL_ShapeBase *)&rCapsule0;
	collision.pBody1 = (CJL_ShapeBase *)&rCapsule1;
	collision.fPenetrationDepth = fDepth;
	collision.vNormal = vDelta;
	collision.vContactPoint = vWorldPos;
///	rColFunctor.AddContact( collision );
	rColFunctor.AddTemporaryContact( collision );

#else

	CJL_CollPointInfo coll_pt_info( vWorldPos - rCapsule0.GetWorldPose().vPosition,
		                            vWorldPos - rCapsule1.GetWorldPose().vPosition,
									0 /* initial_penetration */);

	rColFunctor.AddTemporaryContacts( &rCapsule0, &rCapsule1, vDelta, &coll_pt_info, 1 );



#endif
}
