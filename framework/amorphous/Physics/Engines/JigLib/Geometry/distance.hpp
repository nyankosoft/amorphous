//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file distance.hpp 
///
/// Provides various functions to get the distances basic primitives
/// when the result cannot have a penetration depth. If you need overlap
/// information look in overlap.hpp which also covers swept intersection
/// tests.
/// The order of all these functions has the simplest primitive first
//                     
//==============================================================
#ifndef DISTANCE_HPP
#define DISTANCE_HPP

/*
#include "../geometry/include/line.hpp"
#include "../geometry/include/rectangle.hpp"
#include "../geometry/include/triangle.hpp"
#include "../geometry/include/sphere.hpp"
#include "../geometry/include/capsule.hpp"
#include "../geometry/include/plane.hpp"
#include "../geometry/include/heightmap.hpp"
#include "../geometry/include/box.hpp"
#include "../geometry/include/aabox.hpp"
#include "../utils/include/fixedvector.hpp"
*/

#include "3DMath/fwd.hpp"
#include "line.hpp"


namespace amorphous
{

class tBox;


#define Abs	fabsf


namespace JigLib
{
  /// Overlap tests - todo move these into overlap.hpp
  /// Indicates if a line intersects a triangle - if so the segment and triangle intersection
  /// parameters are optionally returned
  bool SegmentTriangleOverlap(Scalar* tS, Scalar* tT0, Scalar* tT1, const tSegment& seg, const Triangle& triangle);

  /// Point to point
  Scalar PointPointDistanceSq(const Vector3 & pt1, const Vector3 & pt2);
  Scalar PointPointDistance(const Vector3 & pt1, const Vector3 & pt2);

  /// Segment to point. Optionally returns the segment t-value that is closest
  Scalar PointSegmentDistanceSq(Scalar * t, const Vector3 & pt, const tSegment & seg);
  Scalar PointSegmentDistance(Scalar * t, const Vector3 & pt, const tSegment & seg);

  /// point to plane (+ve if above the plane, negative if below). 
//  Scalar PointPlaneDistance(const Vector3 & pt, const tPlane & plane);

  /// Point to rectangle. Optionally returns the t values on the rectangle
//  Scalar PointRectangleDistanceSq(Scalar * t0, Scalar * t1, const Vector3 & pt, const tRectangle & rect);

  /// Point to triangle. Optionally returns the t values on the triangle
  Scalar PointTriangleDistanceSq(Scalar * t0, Scalar * t1, const Vector3 & pt, const Triangle & triangle);

  /// Segment to triangle. Optionally returns the segment and triangle t values of the closest point
  Scalar SegmentTriangleDistanceSq(Scalar* segT, Scalar* triT0, Scalar* triT1, const tSegment& seg, const Triangle& triangle);

  /// Segment to segment. Optionally returns the two segment t-values that give the closest
  /// points
  Scalar SegmentSegmentDistanceSq(Scalar *t0, Scalar *t1, const tSegment & seg0, const tSegment & seg1);
//  Scalar SegmentSegmentDistance(Scalar *t0, Scalar *t1, const tSegment & seg0, const tSegment & seg1);
/*
  /// Segment to rectangle. Optionally returns the rectangle t values that give
  /// the closest point on the rectangle and the segment t value
  Scalar SegmentRectDistanceSq(Scalar * segT, Scalar * rectT0, Scalar * rectT1, const tSegment & seg, const tRectangle & rect);
  Scalar SegmentRectDistance(Scalar * segT, Scalar * rectT0, Scalar * rectT1, const tSegment & seg, const tRectangle & rect);
*/
  /// Segment to box. Optionally returns the box t values and the segment t galue that
  /// give the closest points.
  /// \TODO currently the box t values are based around the box centre, and aren't normalised
  Scalar SegmentBoxDistanceSq(Scalar * segT, Scalar * boxT0, Scalar * boxT1, Scalar * boxT2,
                               const tSegment & seg, const tBox & box);

//#include "../geometry/include/distance.inl"
}
} // namespace amorphous



#endif
