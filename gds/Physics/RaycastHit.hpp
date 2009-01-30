#ifndef  __Physics_PhysRaycastHit_H__
#define  __Physics_PhysRaycastHit_H__


#include "fwd.hpp"


namespace physics
{


/**


*/
class CRaycastHit
{
public:

	Vector3  WorldImpactPos;
	Vector3  WorldNormal;

	CShape *pShape;

	U32  FaceID;
//	TriangleID  InternalFaceID;
	Scalar fDistance;
//	Scalar u;
//	Scalar v;
	int  MaterialID;
	U32  Flags;

public:

	CRaycastHit()
		:
	WorldImpactPos(Vector3(0,0,0)),
	WorldNormal(Vector3(0,0,0)),
	pShape(NULL),
	FaceID(0),
//	InternalFaceID(0),
	fDistance(0),
	MaterialID(0),
	Flags(0)
	{}

	virtual ~CRaycastHit() { ; }
};


} // namespace physics


#endif		/*  __Physics_PhysRaycastHit_H__  */
