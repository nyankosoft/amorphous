#ifndef __3DSTRUCTS_H__
#define __3DSTRUCTS_H__


#include "gds/3DMath/Plane.hpp"
#include "gds/3DMath/Sphere.hpp" // bounding sphere
#include "gds/3DMath/AABB3.hpp"  // axis aligned bounding box


enum {BVTYPE_DOT, BVTYPE_SPHERE, BVTYPE_SMALLSPHERE, BVTYPE_AABB, BVTYPE_OBB, BVTYPE_CONVEX, BVTYPE_COMPLEX};

enum {BV_FRONT, BV_BACK, BV_CROSS};



#endif	/*  __3DSTRUCTS_H__  */
