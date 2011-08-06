#include "JL_Shape_Box.hpp"
#include "JL_Shape_Capsule.hpp"
#include "JL_CollisionFunctor.hpp"
#include "Geometry/Box.hpp"
#include "Geometry/distance.hpp"


void JL_CollisionDetect_Box_Capsule( CJL_Shape_Box& rBox, CJL_Shape_Capsule& rCapsule,
									 CJL_CollisionFunctor& rColFunctor )
{
//	Scalar fCollTolerance = 0.005f;

	Scalar fCollTolerance = rColFunctor.m_fCollTolerance;

	tBox box( rBox.GetWorldPosition(), rBox.GetSideLength(), rBox.GetWorldOrient() );

	Scalar fRadius = rCapsule.GetRadius();
	tSegment seg = rCapsule.GetSegment();

	Scalar segT;
	Scalar boxT0, boxT1, boxT2;

	Scalar fDistSq = SegmentBoxDistanceSq( &segT, &boxT0, &boxT1, &boxT2, seg, box );

	if( (fRadius + fCollTolerance) * (fRadius + fCollTolerance) <= fDistSq )
		return;

	Vector3 vSegPos = seg.GetPoint(segT);
	Vector3 vBoxPos = box.GetCentre()
		            + boxT0 * box.GetOrient()(0)
					+ boxT1 * box.GetOrient()(1)
					+ boxT2 * box.GetOrient()(2);

	Scalar fDist = sqrtf( fDistSq );
	Scalar fDepth = fRadius - fDist;

	Vector3 vDir;
	if( fDist > SCALAR_TINY )
	{
		// TODO: safer normalization
//		vDir = - Vec3Normalize( (vSegPos - vBoxPos) );
		Vec3Normalize( vDir, - (vSegPos - vBoxPos) );
	}
	else if( Vec3Length( vSegPos - box.GetCentre() ) > SCALAR_TINY )
	{
		// TODO: safer normalization
//		vDir = - Vec3Normalize( vSegPos - box.GetCentre() );
		Vec3Normalize( vDir, - (vSegPos - box.GetCentre()) );
	}
	else
	{
		// TODO: make this random
		vDir = - rCapsule.GetWorldOrient()(0);
	}


#ifndef USE_COMBINED_COLLISION_POINTS_INFO

///	rColFunctor.AddContacts( (CJL_ShapeBase *)(&rBox), (CJL_ShapeBase *)(&rCapsule), vDir, &vBoxPos, 1, fDepth );
	rColFunctor.AddTemporaryContacts( (CJL_ShapeBase *)(&rBox), (CJL_ShapeBase *)(&rCapsule), vDir, &vBoxPos, 1, fDepth );

#else

	CJL_CollPointInfo coll_pt_info( vBoxPos - rBox.GetWorldPose().vPosition,
		                            vBoxPos - rCapsule.GetWorldPose().vPosition,
									0 /* initial_penetration */);

	rColFunctor.AddTemporaryContacts( &rBox, &rCapsule, vDir, &coll_pt_info, 1 );



#endif

}
