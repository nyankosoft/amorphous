#ifndef  __3DMath_Sphere_HPP__
#define  __3DMath_Sphere_HPP__

#include "Vector3.hpp"


namespace amorphous
{


template<typename T>
class tSphere
{
public:
	tVector3<T> center;
	T radius;

public:

	tSphere<T>() : center(tVector3<T>(0,0,0)), radius(1.0f) {}

	tSphere<T>( const tVector3<T>& _center, const float _radius ) : center(_center), radius(_radius) {}

	bool IntersectsWith( const tSphere<T>& sphere )
	{
		const float r_sum = radius + sphere.radius;
		return Vec3LengthSq( sphere.center - center ) < r_sum * r_sum;
	}
};


typedef tSphere<float> Sphere;
typedef tSphere<double> dSphere;


} // namespace amorphous



#endif /* __3DMath_Sphere_HPP__ */
