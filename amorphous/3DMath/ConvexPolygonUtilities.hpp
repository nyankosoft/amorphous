#ifndef __ConvexPolygonUtilities_HPP__
#define __ConvexPolygonUtilities_HPP__

#include <vector>
#include <set>
#include "Plane.hpp"


namespace amorphous
{


class sort_by_angle
{
public:
	bool operator()( const std::pair<int,float>& lhs, const std::pair<int,float>& rhs ) const
	{
		return (lhs.second < rhs.second);
	}
};


/// \param points [in]
/// \param sorted_point_indices [out]
inline void CalculateConvexPolygonIndices( const std::vector<Vector3>& points, std::vector<int>& sorted_point_indices )
{
	using std::set;
	using std::pair;
	using std::vector;

	if( points.size() < 3 )
		return;

	Vector3 center( Vector3(0,0,0) );
	const size_t num_points = points.size();
	for( size_t i=0; i<num_points; i++ )
		center += points[i];

	center = center / (float)points.size();

	Vector3 start = points[0];

	Vector3 polygon_normal( Vector3(0,0,0) );
	for( size_t i=0; i<num_points-2; i++ )
	{
		polygon_normal = Vec3Cross( points[i+2] - points[i+1], points[i] - points[i+1] );
		float len = polygon_normal.GetLength();
		if( 0.001f < len )
		{
			polygon_normal /= len;
			break;
		}
	}

	if( polygon_normal == Vector3(0,0,0) )
		return;

	Plane perpendicular_plane;
	perpendicular_plane.normal = Vec3GetNormalized( Vec3Cross( polygon_normal, start - center ) );
	perpendicular_plane.dist   = Vec3Dot( perpendicular_plane.normal, center );

//	if( perpendicular_plane.normal )

	set< pair<int,float>, sort_by_angle > angle_and_index_pairs;

	angle_and_index_pairs.insert( pair<int,float>( 0, 0.0f ) );

	for( size_t i=1; i<num_points; i++ )
	{
		float angle = Vec3GetAngleBetween( start - center, points[i] - center );

		float d = perpendicular_plane.GetDistanceFromPoint( points[i] );

		if( d < 0 )
			angle = 2.0f * (float)PI - angle;

		angle_and_index_pairs.insert( pair<int,float>( (int)i, angle ) );
	}

	sorted_point_indices.resize( 0 );
	sorted_point_indices.reserve( angle_and_index_pairs.size() );

	set< pair<int,float>, sort_by_angle >::iterator itr;
	for( itr = angle_and_index_pairs.begin();
	itr != angle_and_index_pairs.end();
	itr++ )
	{
		sorted_point_indices.push_back( itr->first );
	}
}


//void TriangulateConvexPolygon( const std::vector<Vector3>& points, std::vector<int>& triangle_indices )

inline void TriangulateConvexPolygon(
	const std::vector<Vector3>& points,
	const std::vector<int> point_indices,
	std::vector<int>& triangle_indices
	)
{
	if( points.size() < 3 )
		return;

	std::vector<int> indices;
	indices = point_indices;

	triangle_indices.reserve( point_indices.size() );
	
	for( int num_orig_indices = indices.size();
		0 < num_orig_indices;
		num_orig_indices--)
	{
		if( indices.size() < 3 )
			break;

		for( size_t i=0; i<indices.size()-2; i++ )
		{
			Vector3 cross = Vec3Cross(
				points[indices[i]]   - points[indices[i+1]],
				points[indices[i+2]] - points[indices[i+1]]
			);

			if( cross.GetLengthSq() < 0.000001f )
				continue;

			triangle_indices.push_back( indices[i] );
			triangle_indices.push_back( indices[i+1] );
			triangle_indices.push_back( indices[i+2] );

			indices.erase( indices.begin() + i + 1 );

			break;
		}
	}
}


} // namespace amorphous



#endif /* __ConvexPolygonUtilities_HPP__ */
