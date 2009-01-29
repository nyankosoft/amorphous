#ifndef  __BOUNDINGVOLUME_SPHERE_H__
#define  __BOUNDINGVOLUME_SPHERE_H__

#include "Vector3.h"


class Sphere
{
public:
	Vector3 vCenter;
	float radius;

public:

	Sphere() : vCenter(Vector3(0,0,0)), radius(1.0f) {}

	Sphere( const Vector3& _center, const float _radius ) : vCenter(_center), radius(_radius) {}

	bool IntersectsWith( const Sphere& sphere )
	{
		const float r_sum = radius + sphere.radius;
		return Vec3LengthSq( sphere.vCenter - vCenter ) < r_sum * r_sum;
	}
};


#endif /* __BOUNDINGVOLUME_SPHERE_H__ */
