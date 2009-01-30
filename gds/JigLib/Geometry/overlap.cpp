//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file distance.cpp 
//                     
//==============================================================
//#include "overlap.hpp"
#include "distance.hpp"
#include "3DMath/Triangle.hpp"
#include <limits>
using namespace JigLib;

//====================================================================
// SegmentTriangleOverlap
// See Real-Time Rendering p581
//====================================================================
bool JigLib::SegmentTriangleOverlap( Scalar* tS, Scalar* tT0, Scalar* tT1, 
									 const tSegment& seg, const CTriangle& triangle)
{
	/// the parameters - if hit then they get copied into the args
	Scalar u, v, t;

//	const Vector3& e1 = triangle.GetEdge0();
//	const Vector3& e2 = triangle.GetEdge1();
	const Vector3 e1 = triangle.GetEdge0();
	const Vector3 e2 = triangle.GetEdge1();
	Vector3 p = Vec3Cross(seg.GetDelta(), e2);
	Scalar a = Vec3Dot(e1, p);
	if (a > -SCALAR_TINY && a < SCALAR_TINY)
		return false;
	Scalar f = 1.0f / a;
	Vector3 s = seg.GetOrigin() - triangle.GetOrigin();
	u = f * Vec3Dot(s, p);
	if (u < 0.0f || u > 1.0f)
		return false;
	Vector3 q = Vec3Cross(s, e1);
	v = f * Vec3Dot(seg.GetDelta(), q);
	if (v < 0.0f || (u + v) > 1.0f)
		return false;
	t = f * Vec3Dot(e2, q);
	if (t < 0.0f || t > 1.0f)
		return false;
	if (tS) *tS = t;
	if (tT0) *tT0 = u;
	if (tT1) *tT1 = v;
	return true;
}
/*
//====================================================================
// SweptSphereTriangleOverlap
// See Real Time Rendering p624
//====================================================================
bool JigLib::SweptSphereTriangleOverlap(Vector3& pt, Vector3& N, Scalar& depth,
																				const tSphere& oldSphere, const tSphere& newSphere, 
																				const CTriangle& triangle,
																				Scalar* oldCentreDistToPlane, Scalar* newCentreDistToPlane, 
																				tEdgesToTest edgesToTest,
																				tCornersToTest cornersToTest)
{
	unsigned i;
	const tPlane trianglePlane(triangle.GetPlane());

	// Check against plane
	if (!SweptSpherePlaneOverlap(pt, depth, oldSphere, newSphere, trianglePlane, oldCentreDistToPlane, newCentreDistToPlane))
		return false;

	Vector3 trianglePts[3] = {
		triangle.GetPoint(0), 
		triangle.GetPoint(1), 
		triangle.GetPoint(2) };
	Vector3 triangleEdges[3] = {
		trianglePts[1] - trianglePts[0], 
		trianglePts[2] - trianglePts[1], 
		trianglePts[0] - trianglePts[2] };

	// If the point is inside the triangle, this is a hit
	bool allInside = true;
	for (i = 0 ; i < 3 ; ++i)
	{
		Vector3 outDir = Vec3Cross(triangleEdges[i], trianglePlane.GetN());
		if (Dot(pt - trianglePts[i], outDir) > 0.0f)
		{
			allInside = false;
			break;
		}
	}

	// Quick result?
	if (allInside)
	{
		N = trianglePlane.GetN();
		return true;
	}

	// Now check against the edges
	Scalar bestT = SCALAR_HUGE;
	const Vector3 Ks = newSphere.GetPos() - oldSphere.GetPos();
	const Scalar kss = Vec3Dot(Ks, Ks);
	const Scalar radius = newSphere.GetRadius();
	const Scalar radiusSq = Sq(radius);
	for (i = 0 ; i < 3 ; ++i)
	{
		unsigned mask = 1 << i;
		if (!(mask & edgesToTest))
			continue;
		const Vector3& Ke = triangleEdges[i];
		const Vector3  Kg = trianglePts[i] - oldSphere.GetPos();

		const Scalar kee = Vec3Dot(Ke, Ke);
		if (Abs(kee) < SCALAR_TINY)
			continue;
		const Scalar kes = Vec3Dot(Ke, Ks);
		const Scalar kgs = Vec3Dot(Kg, Ks);
		const Scalar keg = Vec3Dot(Ke, Kg);
		const Scalar kgg = Vec3Dot(Kg, Kg);

		// a * t^2 + b * t + c = 0
		const Scalar a = kee * kss - Sq(kes);
		if (Abs(a) < SCALAR_TINY)
			continue;
		const Scalar b = 2.0f * (keg * kes - kee * kgs);
		const Scalar c = kee * (kgg - radiusSq) - Sq(keg);
	
		Scalar blah = Sq(b) - 4.0f * a * c;
		if (blah < 0.0f)
			continue;

		// solve for t - take minimum
		const Scalar t = (-b - Sqrt(blah)) / (2.0f * a);

		if (t < 0.0f || t > 1.0f)
			continue;

		if (t > bestT)
			continue;

		// now check where it hit on the edge
		Vector3 Ct = oldSphere.GetPos() + t * Ks;
		Scalar d = Vec3Dot((Ct - trianglePts[i]), triangleEdges[i]) / kee;

		if (d < 0.0f || d > 1.0f)
			continue;

		// wahay - got hit. Already checked that t < bestT
		bestT = t;

		pt = trianglePts[i] + d * triangleEdges[i];
		N = (Ct - pt).GetNormalisedSafe();
		// depth is already calculated
	}
	if (bestT <= 1.0f)
		return true;

	// check the corners
	bestT = SCALAR_HUGE;
	for (i = 0 ; i < 3 ; ++i)
	{
		unsigned mask = 1 << i;
		if (!(mask & cornersToTest))
			continue;
		const Vector3  Kg = trianglePts[i] - oldSphere.GetPos();
		const Scalar kgs = Vec3Dot(Kg, Ks);
		const Scalar kgg = Vec3Dot(Kg, Kg);
		Scalar a = kss;
		if (Abs(a) < SCALAR_TINY)
			continue;
		Scalar b = -2.0f * kgs;
		Scalar c = kgg - radiusSq;

		Scalar blah = Sq(b) - 4.0f * a * c;
		if (blah < 0.0f)
			continue;

		// solve for t - take minimum
		const Scalar t = (-b - Sqrt(blah)) / (2.0f * a);

		if (t < 0.0f || t > 1.0f)
			continue;

		if (t > bestT)
			continue;

		bestT = t;

		pt = trianglePts[i];
		Vector3 Ct = oldSphere.GetPos() + t * Ks;
		N = (Ct - pt).GetNormalisedSafe();
	}
	if (bestT <= 1.0f)
		return true;

	return false;
}

*/