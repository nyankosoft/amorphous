#include "MassSpringModelMaker.hpp"
#include "amorphous/3DMath/GridPolygonModelMaker.hpp"
//#include "amorphous/Graphics/MeshGenerators/GridMeshGenerator.hpp"
#include <set>

using namespace std;


namespace amorphous
{


Result::Name UpdateControlPoints(
	const vector<Vector3>& points,
	const vector< pair<int,int> >& edges,
	float spring_const,
	vector<CMS_PointProperty>& control_points
	)
{
	const size_t num_edges = edges.size();
	for( size_t i=0; i<num_edges; i++ )
	{
		const int index0 = edges[i].first;
		const int index1 = edges[i].second;
		CMS_PointProperty& cp0 = control_points[index0];
		CMS_PointProperty& cp1 = control_points[index1];

		cp0.aiPointIndex[ cp0.iNumSprings++ ] = index1;
		cp1.aiPointIndex[ cp1.iNumSprings++ ] = index0;

		const float edge_length = Vec3Length( points[index0] - points[index1] );
		cp0.afSpringLength[ cp0.iNumSprings ] = edge_length;
		cp1.afSpringLength[ cp1.iNumSprings ] = edge_length;

		cp0.afSpring[ cp0.iNumSprings ] = spring_const;
		cp1.afSpring[ cp1.iNumSprings ] = spring_const;

		cp0.iNumSprings++;
		cp1.iNumSprings++;
	}

	return Result::SUCCESS;
}


Result::Name MassSpringModelMaker::SetFixedPoints( const std::vector< std::vector<unsigned int> >& fixed_points )
{
	vector<CMS_PointProperty>& control_points = m_Archive.m_vecControlPoint;

	const size_t num_fixed_point_groups= fixed_points.size();
	for( size_t i=0; i<num_fixed_point_groups; i++ )
	{
		const size_t num_fixed_points = fixed_points[i].size();
		for( size_t j=0; j<num_fixed_points; j++ )
		{
			control_points[ fixed_points[i][j] ].iFixedPointGroup = (int)i;
		}
	}

	return Result::SUCCESS;
}


void SetFixedPointsOfRectangularModel(
	U32 flags,
	uint num_divisionis0,
	uint num_divisionis1,
	std::vector<CMS_PointProperty>& control_points
	)
{
	int group = 0;

	if( flags & FixedPointFlags::FPF_TOP_EDGE )
	{
		for( uint i=0; i<num_divisionis0 + 1; i++ )
			control_points[i].iFixedPointGroup = group;
	}

	if( flags & FixedPointFlags::FPF_RIGHT_EDGE )
	{
		for( uint i=0; i<num_divisionis1 + 1; i++ )
			control_points[(num_divisionis0+1)*i + num_divisionis0].iFixedPointGroup = group;
	}

	if( flags & FixedPointFlags::FPF_BOTTOM_EDGE )
	{
		for( uint i=0; i<num_divisionis0 + 1; i++ )
			control_points[(num_divisionis0+1)*num_divisionis1 + i].iFixedPointGroup = group;
	}

	if( flags & FixedPointFlags::FPF_LEFT_EDGE )
	{
		for( uint i=0; i<num_divisionis1 + 1; i++ )
			control_points[(num_divisionis0+1)*i].iFixedPointGroup = group;
	}

	if( flags & FixedPointFlags::FPF_TOP_LEFT_CORNER )
		control_points[0].iFixedPointGroup = group;

	if( flags & FixedPointFlags::FPF_TOP_RIGHT_CORNER )
		control_points[num_divisionis0].iFixedPointGroup = group;

	if( flags & FixedPointFlags::FPF_BOTTOM_RIGHT_CORNER )
		control_points[(num_divisionis0+1)*num_divisionis1+num_divisionis1].iFixedPointGroup = group;

	if( flags & FixedPointFlags::FPF_BOTTOM_LEFT_CORNER )
		control_points[(num_divisionis0+1)*num_divisionis1].iFixedPointGroup = group;
}


Result::Name MassSpringModelMaker::MakeRectangularModel(
	float width,
	float height,
	unsigned int num_horizontal_divisions,
	unsigned int num_vertical_divisions,
	float spring_const,
//	const vector< vector<unsigned int> >& fixed_points
	U32 fixed_point_flags
	)
{
	unsigned int axis = 0;
	int sign = 0;
	std::vector<Vector3> points;

	// Points
	CreateGridPoints( width, height,num_horizontal_divisions,num_vertical_divisions,axis,sign,points );

	// Edges
	int point_index = 0;
	vector< pair<int,int> > edges;
	const size_t num_points = points.size();
	edges.reserve( num_points * 8 / 2 );
	for( uint i=0; i<=num_horizontal_divisions; i++ )
	{
		for( uint j=0; j<=num_vertical_divisions; j++ )
		{
			if( i<num_horizontal_divisions )
				edges.push_back( pair<int,int>( point_index,   point_index+1 ) ); // from the current point to the next point

			if( j<num_vertical_divisions )
				edges.push_back( pair<int,int>( point_index,   point_index+(num_horizontal_divisions+1) ) ); // from the current point to the point below

			if( i<num_horizontal_divisions && j<num_vertical_divisions )
			{
				edges.push_back( pair<int,int>( point_index,   point_index+(num_horizontal_divisions+1)+1 ) ); // from the current point to the right-bottom from the current
				edges.push_back( pair<int,int>( point_index+1, point_index+(num_horizontal_divisions+1)+1 ) ); // from the next point to the point below
			}

			point_index += 1;
		}
	}

	vector<CMS_PointProperty>& control_points = m_Archive.m_vecControlPoint;
	control_points.resize( num_points );
	for( size_t i=0; i<num_points; i++ )
	{
		control_points[i].vBasePosition = points[i];
		control_points[i].aiNormalPointIndex[0] = (int)i;
//		control_points[i].aiNormalPointIndex[1] = (int)(num_points + i);
	}

	UpdateControlPoints( points, edges, spring_const, control_points );

	// Fixed points
//	SetFixedPoints( fixed_points );
	SetFixedPointsOfRectangularModel(
		fixed_point_flags,
		num_horizontal_divisions,
		num_vertical_divisions,
		control_points
		);

	return Result::UNKNOWN_ERROR;
}


class connected_points_info
{
public:
	enum Params { NUM_MAX_INDICES = 8 };

	int point_indices[NUM_MAX_INDICES];

	connected_points_info()
	{
		for( int i=0; i<NUM_MAX_INDICES; i++ )
			point_indices[i] = -1;
	}
};


Result::Name MassSpringModelMaker::MakeFromMesh(
	const std::vector<Vector3>& points,
	std::vector<unsigned int>& triangles,
	float spring_const,
	const vector< vector<unsigned int> >& fixed_points
	)
{
	const size_t num_points = points.size();

	CMS_MassSpringArchive& dest = m_Archive;

	dest.m_fSpringConst = spring_const;

	vector<connected_points_info> connection_info;
	connection_info.resize( num_points );

	if( triangles.size() % 3 != 0 )
	{
		return Result::INVALID_ARGS;
	}

	set< pair<int,int> > src_edges;
//	edges.reserve( num_points * 8 / 2 );

	const size_t num_triangles = triangles.size() / 3;
	
	for( uint i=0; i<num_triangles; i++ )
	{
		src_edges.insert( pair<int,int>( (int)triangles[i*3+0], (int)triangles[i*3+1] ) );
		src_edges.insert( pair<int,int>( (int)triangles[i*3+1], (int)triangles[i*3+2] ) );
		src_edges.insert( pair<int,int>( (int)triangles[i*3+2], (int)triangles[i*3+0] ) );
	}

	// Convert to a vector
	vector< pair<int,int> > edges;
	edges.assign( src_edges.begin(), src_edges.end() );

	vector<CMS_PointProperty>& control_points = m_Archive.m_vecControlPoint;

	control_points.resize( num_points );
	for( size_t i=0; i<num_points; i++ )
	{
		control_points[i].vBasePosition = points[i];
//		control_points[i].aiPointIndex
	}

	UpdateControlPoints( points, edges, spring_const, control_points );

	// Fixed points
	SetFixedPoints( fixed_points );

	return Result::UNKNOWN_ERROR;
}


} // amorphous
