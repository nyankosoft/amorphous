#ifndef __ConvexVolume_HPP__
#define __ConvexVolume_HPP__


#include <vector>
#include "Plane.hpp"


namespace amorphous
{


class ConvexVolume
{
public:

	/// List of planes that composes the convex shape.
	/// The planes face outward.
	std::vector<SPlane> planes;

public:

	inline bool IsPointInside( const Vector3& rvPoint ) const
	{
		if( this->planes.size() < 4)
			return false;

		const size_t num_planes = this->planes.size();
		for( size_t i=0; i<num_planes; i++ )
		{
			if( 0 < this->planes[i].GetDistanceFromPoint( rvPoint ) )
				return false;
		}

		return true;
	}
};


} // amorphous



#endif /* __ConvexVolume_HPP__ */
