#ifndef __Capsule_HPP__
#define __Capsule_HPP__


class Capsule
{
public:

	Vector3 p0;
	Vector3 p1;
	float radius;

public:

	Capsule()
		:
	p0( Vector3(0,0,0) ),
	p1( Vector3(0,0,0) ),
	radius(0)
	{}

	Capsule( Vector3 _p0, Vector3 _p1, float _radius )
		:
	p0(_p0),
	p1(_p1),
	radius(_radius)
	{}

	~Capsule() {}
};



#endif /* __Capsule_HPP__ */
