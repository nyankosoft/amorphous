#ifndef  __PhysRay_H__
#define  __PhysRay_H__


#include "3DMath/Matrix34.hpp"
#include "fwd.hpp"


namespace physics
{


class CRay
{

public:

	Vector3 Origin;
	Vector3 Direction;


public:

	CRay() : Origin(Vector3(0,0,0)), Direction(Vector3(0,0,1)) {}
	CRay( const Vector3& vOrig, const Vector3& vDir ) : Origin(vOrig), Direction(vDir) {}

	~CRay() {}
};


class CLineSegment
{

public:

	Vector3 Start;
	Vector3 End;

public:

	CLineSegment() : Start(Vector3(0,0,0)), End(Vector3(0,0,0)) {}
	
	~CLineSegment() {}
};


} // namespace physics


#endif		/*  __PhysRay_H__  */
