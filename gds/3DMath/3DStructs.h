#ifndef __3DSTRUCTS_H__
#define __3DSTRUCTS_H__

#include "3DMath/Vector3.h" 


enum {BVTYPE_DOT, BVTYPE_SPHERE, BVTYPE_SMALLSPHERE, BVTYPE_AABB, BVTYPE_OBB, BVTYPE_CONVEX, BVTYPE_COMPLEX};

enum {BV_FRONT, BV_BACK, BV_CROSS};


#include "3DMath/Plane.h"

#include "3DMath/aabb3.h"	// axis aligned bounding box

#include "3DMath/Sphere.h"	// bounding sphere

/*
struct OBB3
{
	Vector3 va;
	Vector3 vb;
	Vector3 vc;
	float la;
	float lb;
	float lc;
};
*/


#endif	/*  __3DSTRUCTS_H__  */
