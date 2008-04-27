
#include "aabb3.h"
#include "Sphere.h"


Sphere AABB3::CreateBoundingSphere() const
{
	Sphere sphere;
	sphere.vCenter = GetCenterPosition();
	Vector3 vHalfAcross = vMax - sphere.vCenter;
	sphere.radius = Vec3Length( vHalfAcross );
	return sphere;
}
