//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file tLine.hpp 
//                     
//==============================================================
#ifndef JIGLINE_HPP
#define JIGLINE_HPP


//#include "../maths/include/precision.hpp"
//#include "../maths/include/vector3.hpp"

#include "3DMath/Vector3.hpp"


namespace amorphous
{


namespace JigLib
{
  /// A line goes through pos, and extends infinitely far in both
  /// directions along dir.
  class tLine
  {
  public:
    tLine() {}
    tLine(const Vector3 & origin, const Vector3 & dir) : mOrigin(origin), mDir(dir) {}
    
    Vector3 GetOrigin(Scalar t) const {return mOrigin + t * mDir;}
    
    Vector3 mOrigin;
    Vector3 mDir;
  };
  
  /// A Ray is just a line that extends in the +ve direction
  class tRay
  {
  public:
    tRay() {}
    tRay(const Vector3 & origin, const Vector3 & dir) : mOrigin(origin), mDir(dir) {}
    
    Vector3 GetOrigin(Scalar t) const {return mOrigin + t * mDir;}
    
    Vector3 mOrigin;
    Vector3 mDir;
  };
  
  /// A Segment is a line that starts at origin and goes only as far as
  /// (origin + delta).
  class tSegment
  {
  public:
    tSegment() {}
    tSegment(const Vector3 & origin, const Vector3 & delta) : mOrigin(origin), mDelta(delta) {}
    
    const Vector3& GetOrigin() const {return mOrigin;}
    const Vector3& GetDelta() const {return mDelta;}

    Vector3 GetPoint(Scalar t) const {return mOrigin + t * mDelta;}
    Vector3 GetEnd() const {return mOrigin + mDelta;}
    
    Vector3 mOrigin;
    Vector3 mDelta;
  };
  
}
} // namespace amorphous



#endif
