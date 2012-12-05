#include "HelixPolygonModelMaker.hpp"


static void CreateHelix_AddCordFacePolygon(
	unsigned int vertex_offset,
	unsigned int num_cord_sides,
	bool invert_indices,
	std::vector< std::vector<unsigned int> >& polygons
)
{
	polygons.push_back( std::vector<unsigned int>() );
	polygons.back().resize( num_cord_sides );

	if( invert_indices )
	{
		for( unsigned int i=0; i<num_cord_sides; i++ )
			polygons.back()[i] = (unsigned int)vertex_offset + (num_cord_sides - i - 1);
	}
	else
	{
		for( unsigned int i=0; i<num_cord_sides; i++ )
			polygons.back()[i] = (unsigned int)vertex_offset + i;
	}
}


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
	)
{
	if( num_helix_sides < 3
	 || num_cord_sides < 3
	 || num_coils == 0 )
	{
		return;
	}

	unsigned int vertex_index_offset = (unsigned int)positions.size();

	const float single_helix_length = helix_length / (float)num_coils;
	const float tilt_angle = atan( single_helix_length / (2.0f * (float)PI * helix_radius) );
	const Matrix33 face_tilt( Matrix33RotationZ( tilt_angle ) );

	std::vector<Vector3> face_vertices;
	face_vertices.resize( num_cord_sides );
	for( unsigned int i=0; i<num_cord_sides; i++ )
	{
		float angle = (float)i / (float)num_cord_sides * 2.0f * (float)PI;
		face_vertices[i] = Vector3( cos(angle), sin(angle), 0.0f ) * cord_radius;
		face_vertices[i].x += (helix_radius - cord_radius);

		face_vertices[i] = face_tilt * face_vertices[i];
	}

	std::vector<Vector3> single_helix_face_vertices;
	single_helix_face_vertices.resize( num_cord_sides * num_helix_sides );

	// Create vertices of the single coil
	float step_height = single_helix_length / (float)num_helix_sides;
	for( unsigned int i=0; i<num_helix_sides; i++ )
	{
		float heading_angle = (float)i / (float)num_helix_sides * 2.0f * (float)PI;
		Matrix33 face_heading( Matrix33RotationY( heading_angle ) );
		float height_offset = step_height * (float)i;

		for( unsigned int j=0; j<num_cord_sides; j++ )
		{
			Vector3& pos = single_helix_face_vertices[num_cord_sides*i+j];
			pos = face_heading * face_vertices[j];
			pos.y += height_offset;
		}
	}

	// Duplicate the vertices of the single coil to make the vertices of specified coil
	const int point_index_offset = (int)positions.size();
	for( unsigned int i=0; i<num_coils; i++ )
	{
		int helix_start_offset = (int)positions.size();
		float helix_start_height = single_helix_length * (float)i;

		positions.insert(
			positions.end(),
			single_helix_face_vertices.begin(),
			single_helix_face_vertices.end()
			); 

		for( unsigned int j=helix_start_offset; j<positions.size(); j++ )
			positions[j].y += helix_start_height;
	}

	// add face for the top of the coil
	unsigned int last_face_vertices_offset = (unsigned int)positions.size();
	positions.insert( positions.end(), face_vertices.begin(), face_vertices.end() );
	for( unsigned int i=last_face_vertices_offset; i<positions.size(); i++ )
		positions[i].y += helix_length;

	polygons.reserve( polygons.size() + num_helix_sides * num_cord_sides * num_coils );

	unsigned int num_helix_sections = num_helix_sides * num_coils;
	for( unsigned int i=0; i<num_helix_sections; i++ )
	{
		unsigned int face_vertices_offset = vertex_index_offset + num_cord_sides * i;
		for( unsigned int j=0; j<num_cord_sides; j++ )
		{
			polygons.push_back( std::vector<unsigned int>() );

			polygons.back().push_back( face_vertices_offset + (j+1)%num_cord_sides );
			polygons.back().push_back( face_vertices_offset + j );
			polygons.back().push_back( face_vertices_offset + j                    + num_cord_sides );
			polygons.back().push_back( face_vertices_offset + (j+1)%num_cord_sides + num_cord_sides );
		}
	}

	if( create_bottom_face )
	{
		if( top_face_style == PolygonModelStyle::EDGE_VERTICES_WELDED )
		{
			CreateHelix_AddCordFacePolygon( 0, num_cord_sides, false, polygons );
		}
		else // i.e. PolygonModelStyle::EDGE_VERTICES_UNWELDED
		{
			// Add vertices and polygons
			size_t bottom_face_vertices_offset = positions.size();
			positions.insert( positions.end(), face_vertices.begin(), face_vertices.end() );
			CreateHelix_AddCordFacePolygon( bottom_face_vertices_offset, num_cord_sides, false, polygons );
		}
	}

	if( create_top_face )
	{
		if( top_face_style == PolygonModelStyle::EDGE_VERTICES_WELDED )
		{
			// Just add polygons
			size_t top_face_vertices_offset = positions.size() - num_cord_sides;
			CreateHelix_AddCordFacePolygon( top_face_vertices_offset, num_cord_sides, true, polygons );
		}
		else // i.e. PolygonModelStyle::EDGE_VERTICES_UNWELDED
		{
			// Add vertices and polygons
			size_t top_face_vertices_offset = positions.size();
			positions.insert( positions.end(), face_vertices.begin(), face_vertices.end() );
			for( size_t i=top_face_vertices_offset; i<positions.size(); i++ )
				positions[i].y += helix_length;

			CreateHelix_AddCordFacePolygon( (unsigned int)top_face_vertices_offset, num_cord_sides, true, polygons );
		}
	}
}

