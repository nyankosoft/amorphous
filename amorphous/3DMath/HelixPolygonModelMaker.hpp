#ifndef __HelixPolygonModelMaker_HPP__
#define __HelixPolygonModelMaker_HPP__


#include <vector>
#include "PolygonModelStyle.hpp"
#include "Vector3.hpp"


namespace amorphous
{


template<typename T>
void CreateUniformCylindricalHelix(
	T helix_length,
	T helix_radius,
	unsigned int num_coils,
	T cord_radius,
	unsigned int num_helix_sides,
	unsigned int num_cord_sides,
	bool create_top_face,
	PolygonModelStyle::Name top_face_style,
	bool create_bottom_face,
	PolygonModelStyle::Name bottom_face_style,
	std::vector< tVector3<T> >& positions,
	std::vector< tVector3<T> >& normals,
	std::vector< std::vector<unsigned int> >& polygons
	);


} // namespace amorphous



#endif /* __HelixPolygonModelMaker_HPP__ */
