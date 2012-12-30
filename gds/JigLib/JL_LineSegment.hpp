#ifndef __JL_LINESEGMENT_H__
#define __JL_LINESEGMENT_H__


#include "3DMath/Vector3.hpp"
#include "3DMath/Plane.hpp"


namespace amorphous
{


class CJL_LineSegment
{
public:

	/// in ///
	Vector3 vStart;
	Vector3 vGoal;

	/// out ///

	/// contact position
	Vector3 vEnd;

	float fFraction;

	int iMaterialIndex;

	SPlane plane;


	inline CJL_LineSegment();

	inline void SetDefault();

};



//----------------------- inline implementations -----------------------

inline CJL_LineSegment::CJL_LineSegment()
//: vStart(Vector3(0,0,0)), vGoal(Vector3(0,0,0)), vEnd(Vector3(0,0,0)), fFraction(0.0f), iMaterialIndex(-1)
{}


inline void CJL_LineSegment::SetDefault()
{
	vStart	= Vector3(0,0,0);
	vGoal	= Vector3(0,0,0);
	vEnd	= Vector3(0,0,0);

	fFraction	= 1.0f;

	plane.normal	= Vector3(0,0,0);
	plane.dist		= 0.0f;
	plane.type		= 3;

	iMaterialIndex	= -1;
}

} // namespace amorphous



#endif  /*  __JL_LINESEGMENT_H__  */
