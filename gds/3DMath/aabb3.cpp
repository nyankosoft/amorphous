#include "AABB3.hpp"
#include "Sphere.hpp"


Sphere AABB3::CreateBoundingSphere() const
{
	Sphere sphere;
	sphere.center = GetCenterPosition();
	Vector3 vHalfAcross = vMax - sphere.center;
	sphere.radius = Vec3Length( vHalfAcross );
	return sphere;
}
