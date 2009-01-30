#ifndef  __JL_SHAPE_SPHERE_H__
#define  __JL_SHAPE_SPHERE_H__

#include "JL_ShapeBase.hpp"

#include "Geometry/line.hpp"


//namespace JigLib
//{

using namespace JigLib;

class CJL_Shape_Sphere : public CJL_ShapeBase
{
	Scalar m_fRadius;

public:
	CJL_Shape_Sphere();
	~CJL_Shape_Sphere() {}

	void InitSpecific( CJL_ShapeDesc& rShapeDesc );

	/// calculate the world coordinats of aabb
	/// must be called every frame before the collision detection
	void UpdateWorldProperties();


	void SetSize( Scalar fRaduis );

//	bool SepAxisTest( Scalar& depth, Vector3& rvAxis, CTriangle& rTriangle );	// returns true if intersecting on the given axis
//	bool SepAxisTest( Scalar& depth, Vector3& rvAxis, CJL_Shape_Sphere& rBox );	// returns true if intersecting on the given axis

	// axis must be in world space
//	inline void GetSpan( Scalar& rfMin, Scalar& rfMax, const Vector3& rvAxis )	{}

	inline Scalar GetRadius() { return m_fRadius; }

};


//}


#endif		/*  __JL_SHAPE_SPHERE_H__  */