#ifndef __wavefront_obj_input_HPP__
#define __wavefront_obj_input_HPP__


#include <vector>
#include <string>
#include "../Support/StringAux.hpp"


namespace amorphous
{


namespace wavefront_obj
{

//float stof( const std::string& src )
//{
//	return (float)atof(src.c_str());
//}


template<typename vec3_type, typename tex_uv_type, typename index_type>
bool read_from_file(
	const std::string& file_pathname,
	std::vector<vec3_type>& positions,
	std::vector<tex_uv_type>& texture_uvs,
	std::vector<vec3_type>& normals,
	std::vector< std::vector<index_type> >& polygon_indices
	)
{
	using std::stof;
	using std::stoi;

	FILE *fp = fopen( file_pathname.c_str(), "r" );
	if( !fp )
		return false;

	std::vector<std::string> elements;
	static const int line_buffer_size = 1024;
	char line_buffer[line_buffer_size];
	memset( line_buffer, sizeof(line_buffer), 0 );
	while( fgets( line_buffer, line_buffer_size-1, fp ) )
	{
		elements.resize( 0 );
		SeparateStrings( elements, line_buffer, " \t" );
		if( elements.empty() )
			continue;

		const std::string& tag = elements.front();
		if( tag == "v" && 4 <= elements.size() )
		{
			positions.push_back( vec3_type( stof(elements[1]),  stof(elements[2]),  stof(elements[3]) ) );
		}
		else if( tag == "vt" && 3 <= elements.size() )
		{
			texture_uvs.push_back( tex_uv_type( stof(elements[1]),  stof(elements[2]) ) );
		}
		else if( tag == "vn" && 4 <= elements.size() )
		{
			normals.push_back( vec3_type( stof(elements[1]),  stof(elements[2]),  stof(elements[3]) ) );
		}
		else if( tag == "f" && 3 <= elements.size() )
		{
			polygon_indices.push_back( std::vector<index_type>() );
			const int num_indices = (int)elements.size() - 1;
			for( int j=0; j<num_indices; j++ )
			{
				polygon_indices.back().push_back( stoi(elements[j+1]) );
			}
		}
	}

	return false;
}


} // wavefront_obj


} // amorphous



#endif /* __wavefront_obj_input_HPP__ */
