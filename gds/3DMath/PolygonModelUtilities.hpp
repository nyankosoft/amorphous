#ifndef __PolygonModelUtilities_HPP__
#define __PolygonModelUtilities_HPP__


#include <vector>
#include "aabb3.hpp"


namespace amorphous
{


template<typename T>
inline void CalculatePolygonNormals(
	const std::vector< tVector3<T> >& positions,
	const std::vector< std::vector<unsigned int> >& polygons,
	std::vector< tVector3<T> >& polygon_normals
	)
{
	const size_t num_polygons = polygons.size();

	polygon_normals.resize( num_polygons, tVector3<T>(0,0,0) );

	for( size_t i=0; i<num_polygons; i++ )
	{
		for( size_t j=0; j<polygons[i].size(); j++ )
		{
			tVector3<T> edge0 = positions[ polygons[i][j  ] ] - positions[ polygons[i][j+1] ];
			tVector3<T> edge1 = positions[ polygons[i][j+2] ] - positions[ polygons[i][j+1] ];

			tVector3<T> normal = Vec3Cross( edge1, edge0 );

			if( (T)0.0000001 < normal.GetLength() )
			{
				polygon_normals[i] = Vec3GetNormalized( normal );
				break;
			}
		}
	}
}


/// \brief precondition: no polygon has duplicate vertex indices (may crash if there are polygons with duplicate vertices. Not confirmed).
template<typename T>
inline void CalculateVertexNormals(
	const std::vector< tVector3<T> >& positions,
	const std::vector< std::vector<unsigned int> >& polygons,
	std::vector< tVector3<T> >& vertex_normals
	)
{
	using std::vector;

	if( positions.empty() )
		return;

	const size_t num_vertices = positions.size();

	vector< tVector3<T> > polygon_normals;
	polygon_normals.resize( polygons.size(), tVector3<T>(0,0,0) );

	CalculatePolygonNormals( positions, polygons, polygon_normals );

	// indices of polygons that share the i-th vertex
	vector< vector<int> > polygon_indices;
	polygon_indices.resize( num_vertices );

	for( size_t i=0; i<polygons.size(); i++ )
	{
		for( size_t j=0; j<polygons[i].size(); j++ )
		{
			polygon_indices[ polygons[i][j] ].push_back( (int)i );
		}
	}

	vertex_normals.resize( 0 );
	vertex_normals.resize( num_vertices );

	for( size_t i=0; i<num_vertices; i++ )
	{
		tVector3<T> sum_normal = Vector3(0,0,0);
		for( size_t j=0; j<polygon_indices[i].size(); j++ )
		{
			sum_normal += polygon_normals[ polygon_indices[i][j] ];
		}

		vertex_normals[i] = Vec3GetNormalized( sum_normal );
	}
}


template<typename T>
inline void Unweld(
	std::vector< tVector3<T> >& positions,
	std::vector< std::vector<unsigned int> >& polygons,
	std::vector< std::pair<unsigned int,unsigned int> >& maps
	)
{
	typedef std::pair<unsigned int,unsigned int> uint_pair;

	std::vector<int> appeared;
	appeared.resize( positions.size(), 0 );

	const size_t num_polygons = polygons.size();
	for( size_t i=0; i<num_polygons; i++ )
	{
		const size_t num_indices = polygons[i].size();
		for( size_t j=0; j<num_indices; j++ )
		{
			int polygon_index = polygons[i][j];
			if( appeared[polygon_index] )
			{
				// Duplicate the vertex because it is shared by at least two polygons.
				polygons[i][j] = (int)positions.size();
				Vector3 position = positions[polygon_index];
				positions.push_back( position );

				maps.push_back( uint_pair( polygon_index, polygons[i][j] ) );
			}
			else
			{
				appeared[polygon_index] = 1;
			}
		}
	}
}


template<typename T>
inline void Unweld(
	std::vector< tVector3<T> >& positions,
	std::vector< std::vector<unsigned int> >& polygons
	)
{
	std::vector< std::pair<unsigned int,unsigned int> > maps;
	Unweld( positions, polygons, maps );
}


template<typename D>
inline bool CopyUnweld(
	const std::vector< std::pair<unsigned int,unsigned int> >& maps,
	unsigned int num_vertices_before_unwelding,
	unsigned int num_vertices_after_unwelding,
	std::vector<D>& dest
	)
{
	if( (unsigned int)dest.size() != num_vertices_before_unwelding )
		return false;

	if( num_vertices_after_unwelding < num_vertices_before_unwelding )
		return false;

	unsigned int num_added_vertices = num_vertices_after_unwelding - num_vertices_before_unwelding;
	dest.insert( dest.end(), num_added_vertices, D() );
	for( size_t i=0; i<maps.size(); i++ )
	{
		dest[maps[i].second] = dest[maps[i].first];
	}

	return true;
}


template<typename T, typename Tex2D>
inline void MakeTextureCoordinatesAlongAxis( 
	std::vector< tVector3<T> >& positions,
	unsigned int plane_axis,
	unsigned int u_axis,
	unsigned int v_axis,
	std::vector< Tex2D >& tex_coords
	)
{
	if( positions.empty() )
		return;

	AABB3 aa_box;
	aa_box.Nullify();

	const size_t num_vertices = positions.size();

	for( size_t i=0; i<num_vertices; i++ )
	{
		aa_box.AddPoint( positions[i] );
	}

	tex_coords.resize( 0 );
	tex_coords.resize( num_vertices );

	T u_extent = aa_box.vMax[u_axis] - aa_box.vMin[u_axis];
	T v_extent = aa_box.vMax[v_axis] - aa_box.vMin[v_axis];

	for( size_t i=0; i<num_vertices; i++ )
	{
		tex_coords[i].u = (positions[i][u_axis] - aa_box.vMin[u_axis]) / u_extent;
		tex_coords[i].v = (positions[i][v_axis] - aa_box.vMin[v_axis]) / v_extent;
	}
}


} // namespace amorphous



#endif /* __PolygonModelUtilities_HPP__ */
