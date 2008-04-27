#ifndef  __BOUNDINGVOLUME_SPHERE_H__
#define  __BOUNDINGVOLUME_SPHERE_H__

#include "Vector3.h"


class Sphere
{
public:
	Vector3 vCenter;
	float radius;

	Sphere() : vCenter(Vector3(0,0,0)), radius(1.0f) {}
	Sphere( const Vector3& _center, const float _radius ) : vCenter(_center), radius(_radius) {}
};


#endif /* __BOUNDINGVOLUME_SPHERE_H__ */
