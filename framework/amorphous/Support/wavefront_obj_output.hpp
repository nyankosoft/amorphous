#ifndef __wavefront_obj_output_HPP__
#define __wavefront_obj_output_HPP__


#include <vector>
#include <string>


namespace amorphous
{
//#include "Vector2.hpp"
//#include "Vector3.hpp"


namespace wavefront_obj
{


template<typename vec3_type>
inline void write_vec3_elements_to_file( FILE *fp, const char *tag, const std::vector<vec3_type>& elements )
{
	const size_t num_elements = elements.size();
	for( size_t i=0; i<num_elements; i++ )
	{
		const vec3_type& element = elements[i];
		fprintf( fp, "%s %f %f %f\n", tag, element.x, element.y, element.z );
	}
}


template<typename vec4_type>
inline void write_vec4_elements_to_file( FILE *fp, const char *tag, const std::vector<vec4_type>& elements )
{
	const size_t num_elements = elements.size();
	for( size_t i=0; i<num_elements; i++ )
	{
		const vec3_type& element = elements[i];
		fprintf( fp, "%s %f %f %f %f\n", tag, element.x, element.y, element.z, element.w );
	}
}


template<typename tex_uv_type>
inline void write_uv_elements_to_file( FILE *fp, const char *tag, const std::vector<tex_uv_type>& elements )
{
	const size_t num_elements = elements.size();
	for( size_t i=0; i<num_elements; i++ )
	{
		const tex_uv_type& element = elements[i];
		fprintf( fp, "%s %f %f\n", tag, element.u, element.v );
	}
}


template<typename vec3_type, typename tex_uv_type, typename index_type>
inline bool write_to_obj_file(
	const std::string& file_pathname,
	const std::vector<vec3_type>& positions,
	const std::vector<tex_uv_type>& texture_uvs,
	const std::vector<vec3_type>& normals,
	const std::vector<index_type>& triangle_indices
	)
{
	FILE *fp = fopen( file_pathname.c_str(), "w" );
	if( !fp )
		return false;

	write_vec3_elements_to_file( fp, "v",  positions );
	write_uv_elements_to_file(   fp, "vt", texture_uvs );
	write_vec3_elements_to_file( fp, "vn", normals );

	const size_t num_triangles = triangle_indices.size() / 3;
	for( size_t i=0; i<num_triangles; i++ )
	{
		int start_index = i*3;
		const int i0 = triangle_indices[start_index  ] + 1;
		const int i1 = triangle_indices[start_index+1] + 1;
		const int i2 = triangle_indices[start_index+2] + 1;

//		fprintf( fp, "f %d %d %d\n", i0, i1, i2 );
//		fprintf( fp, "f %d/%d %d/%d %d/%d\n", i0, i0, i1, i1, i2, i2 );
		fprintf( fp, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", i0, i0, i0, i1, i1, i1, i2, i2, i2 );
	}

	int ret = fclose(fp);

	return true;
}


// Write non-triangulated polygons
template<typename vec3_type, typename tex_uv_type, typename index_type>
inline bool write_to_obj_file(
	const std::string& file_pathname,
	const std::vector<vec3_type>& positions,
	const std::vector<tex_uv_type>& texture_uvs,
	const std::vector<vec3_type>& normals,
	const std::vector< std::vector<index_type> >& polygon_indices
	)
{
	FILE *fp = fopen( file_pathname.c_str(), "w" );
	if( !fp )
		return false;

	write_vec3_elements_to_file( fp, "v",  positions );
	write_uv_elements_to_file(   fp, "vt", texture_uvs );
	write_vec3_elements_to_file( fp, "vn", normals );

	const size_t num_polygons = polygon_indices.size();
	for( size_t i=0; i<num_polygons; i++ )
	{
		fprintf( fp, "f" );
		const std::vector<index_type>& polygon = polygon_indices[i];
		for( size_t j=0; j<polygon.size(); j++ )
		{
			index_type index = polygon[j] + 1;
			fprintf( fp, " %d/%d/%d", index, index, index );
		}

		fprintf( fp, "\n" );
	}

	int ret = fclose(fp);

	return false;
}


} // wavefront_obj


} // amorphous



#endif /* __wavefront_obj_output_HPP__ */
