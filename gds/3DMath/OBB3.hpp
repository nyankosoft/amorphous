#ifndef __3DMath_OBB3_HPP__
#define __3DMath_OBB3_HPP__


#include "Matrix34.hpp"


namespace amorphous
{


class OBB3
{
public:

	OBB3()
		:
	center( Matrix34Identity() ),
	radii( Vector3(1,1,1) )
	{}

	OBB3( Matrix34 _center, Vector3 _radii )
		:
	center(_center),
	radii(_radii)
	{}

	~OBB3() {}

	Matrix34 center; ///< center pose
	Vector3 radii;   ///< radii of the box
};

} // namespace amorphous



#endif /* __3DMath_OBB3_HPP__ */
