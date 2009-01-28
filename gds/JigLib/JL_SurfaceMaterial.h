
#ifndef  __JL_SURFACEMATERIAL_H__
#define  __JL_SURFACEMATERIAL_H__

#include "3DMath/precision.h"

class CJL_PhysicsManager;

class CJL_SurfaceMaterial
{
public:
	Scalar fStaticFriction;
	Scalar fDynamicFriction;
	Scalar fElasticity;		// coefficient of restitution - Gets multiplied with the other body in collisions. default = 0


	CJL_SurfaceMaterial() { SetDefault(); }
	~CJL_SurfaceMaterial() {}

	void SetDefault()
	{
		fStaticFriction = 1.0f;
		fDynamicFriction = 1.0f;
		fElasticity = 0.0f;
	}

	friend class CJL_PhysicsManager;
};



#endif		/*  __JL_SURFACEMATERIAL_H__  */
