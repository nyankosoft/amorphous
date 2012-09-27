#ifndef __HelixPolygonModelMaker_HPP__
#define __HelixPolygonModelMaker_HPP__


#include <vector>
#include "Matrix33.hpp"
#include "PolygonModelStyle.hpp"


void CreateUniformCylindricalHelix(
	float helix_length,
	float helix_radius,
	unsigned int num_coils,
	float cord_radius,
	unsigned int num_helix_sides,
	unsigned int num_cord_sides,
	bool create_top_face,
	PolygonModelStyle::Name top_face_style,
	bool create_bottom_face,
	PolygonModelStyle::Name bottom_face_style,
	std::vector<Vector3>& positions,
	std::vector<Vector3>& normals,
	std::vector< std::vector<unsigned int> >& polygons
	);



#endif /* __HelixPolygonModelMaker_HPP__ */
