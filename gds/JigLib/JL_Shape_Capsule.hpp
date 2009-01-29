#ifndef  __JL_SHAPE_CAPSULE_H__
#define  __JL_SHAPE_CAPSULE_H__

#include "JL_ShapeBase.h"

#include "Geometry/line.hpp"


//namespace JigLib
//{

using namespace JigLib;

class CJL_Shape_Capsule : public CJL_ShapeBase
{
	Scalar m_fRadius;
	Scalar m_fLength;

public:
	CJL_Shape_Capsule();
	~CJL_Shape_Capsule() {}

	void InitSpecific( CJL_ShapeDesc& rShapeDesc );

	// calculate the world coordinats of aabb
	// must be called every frame before the collision detection
	void UpdateWorldProperties();


	void SetSize( Scalar fRaduis, Scalar fLength );
//	bool SepAxisTest( Scalar& depth, Vector3& rvAxis, CTriangle& rTriangle );	// returns true if intersecting on the given axis
//	bool SepAxisTest( Scalar& depth, Vector3& rvAxis, CJL_Shape_Capsule& rBox );	// returns true if intersecting on the given axis

	// axis must be in world space
//	inline void GetSpan( Scalar& rfMin, Scalar& rfMax, const Vector3& rvAxis )	{}

	inline Scalar GetLength() { return m_fLength; }
	inline Scalar GetRadius() { return m_fRadius; }

	inline Vector3 GetSegmentStartPos() { return GetWorldPosition() - m_fLength * 0.5f * GetWorldOrient().GetColumn(2); }
	inline Vector3 GetSegmentEndPos()   { return GetWorldPosition() + m_fLength * 0.5f * GetWorldOrient().GetColumn(2); }

	inline tSegment GetSegment();

};


inline tSegment CJL_Shape_Capsule::GetSegment()
{
	Vector3 vDir = GetWorldOrient().GetColumn(2);
	tSegment seg( GetWorldPosition() - vDir * m_fLength * 0.5f, vDir * m_fLength );
	return seg;
}



//}


#endif		/*  __JL_SHAPE_CAPSULE_H__  */