#ifndef __Capsule_HPP__
#define __Capsule_HPP__


namespace amorphous
{


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

	bool IsValid() const
	{
		if( 0.000001f <= Vec3LengthSq( p1 - p0 )
		 && 0.000001f <= radius )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};


} // namespace amorphous


#endif /* __Capsule_HPP__ */
