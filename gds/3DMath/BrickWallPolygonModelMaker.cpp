#include "BrickWallPolygonModelMaker.hpp"


namespace amorphous
{

using namespace std;


template<typename T>
void Unweld(
	std::vector< tVector3<T> >& positions,
	std::vector< std::vector<unsigned int> >& polygons
	)
{
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
			}
			else
			{
				appeared[polygon_index] = 1;
			}
		}
	}
}


template<typename T>
void CreateBrick(
	const brick_wall_panel_desc<T>& desc,
	std::vector< tVector3<T> >& positions,
	std::vector< std::vector<unsigned int> >& polygons
	)
{
	unsigned int num_blocks_h = desc.num_horizontal_bricks;
	unsigned int num_blocks_v = desc.num_vertical_bricks;
	T interval  = desc.interval;
	T inset     = desc.inset;
	T extrusion = desc.extrusion;
	const T half_brick_width  = (1.0f / (float)num_blocks_h - interval) * 0.5f;
	const T half_brick_height = (1.0f / (float)num_blocks_v - interval) * 0.5f;

	positions.resize( 0 );
	positions.resize( 8 );

	// vertices[0,3] - brick face corners (TL,TR,BR,BL)
	positions[0] = tVector3<T>( -half_brick_width+inset,  half_brick_height-inset, -extrusion );
	positions[1] = tVector3<T>(  half_brick_width-inset,  half_brick_height-inset, -extrusion );
	positions[2] = tVector3<T>(  half_brick_width-inset, -half_brick_height+inset, -extrusion );
	positions[3] = tVector3<T>( -half_brick_width+inset, -half_brick_height+inset, -extrusion );

	// vertices[4,7] - bottom corners (TL,TR,BR,BL)
	positions[4] = tVector3<T>( -half_brick_width,        half_brick_height,       0 );
	positions[5] = tVector3<T>(  half_brick_width,        half_brick_height,       0 );
	positions[6] = tVector3<T>(  half_brick_width,       -half_brick_height,       0 );
	positions[7] = tVector3<T>( -half_brick_width,       -half_brick_height,       0 );

	polygons.resize( 0 );
	polygons.resize( 5 );
	for( size_t i=0; i<polygons.size(); i++ )
		polygons[i].resize( 4 );

	// brick face
	polygons[0][0] = 0;
	polygons[0][1] = 1;
	polygons[0][2] = 2;
	polygons[0][3] = 3;

	// top side
	polygons[1][0] = 4;
	polygons[1][1] = 5;
	polygons[1][2] = 1;
	polygons[1][3] = 0;

	// right side
	polygons[2][0] = 5;
	polygons[2][1] = 6;
	polygons[2][2] = 2;
	polygons[2][3] = 1;

	// bottom side
	polygons[3][0] = 6;
	polygons[3][1] = 7;
	polygons[3][2] = 3;
	polygons[3][3] = 2;

	// left side
	polygons[4][0] = 7;
	polygons[4][1] = 4;
	polygons[4][2] = 0;
	polygons[4][3] = 3;
}


template<typename T>
void CreateBrickWallPanel(
	const brick_wall_panel_desc<T>& desc,
	std::vector< tVector3<T> >& positions,
	std::vector< std::vector<unsigned int> >& polygons
	)
{
	// Create a brick and clone it.
	// The bricks are placed along the z-axis facing toward the negative half-space
	// in the left hand cooridinates.
	// rationale: A camera faces the positive half-space in the z-axis direction.

	vector< tVector3<T> > brick_positions;
	vector< vector<unsigned int> > brick_polygons;

	CreateBrick( desc, brick_positions, brick_polygons );

	unsigned int num_blocks_h = desc.num_horizontal_bricks;
	unsigned int num_blocks_v = desc.num_vertical_bricks;
	const T interval = desc.interval;
	const T brick_width  = 1.0f / (float)num_blocks_h - interval;
	const T brick_height = 1.0f / (float)num_blocks_v - interval;
	const T cell_width  = 1.0f / (float)num_blocks_h;
	const T cell_height = 1.0f / (float)num_blocks_v;

	const int num_brick_points   = (int)brick_positions.size();
	const int num_brick_polygons = (int)brick_polygons.size();

	for( unsigned int r=0; r<num_blocks_v; r++ )
	{
		T sy = cell_height * ((T)r + 0.5f) - 0.5f;

		const bool shift = ( r % 2 == desc.alternate_shift );
		T dx = shift ? -cell_width * (T)0.5 : (T)0.0;

		unsigned int num_columns = num_blocks_h;
		if( shift )
			num_columns += 1;

		for( unsigned int c=0; c<num_columns; c++ )
		{
			T sx = cell_width * ((T)c + 0.5f) - 0.5f;
			const tVector3<T> offset = tVector3<T>(sx+dx,sy,0.0f);

			int start_point_index = (int)positions.size();
			positions.insert( positions.end(), num_brick_points, tVector3<T>(0,0,0) );
			for( int i=0; i<num_brick_points; i++ )
				positions[start_point_index+i] = brick_positions[i] + offset;

			int start_polygon_index = (int)polygons.size();
			polygons.insert( polygons.end(), num_brick_polygons, vector<unsigned int>() );
			for( int i=0; i<num_brick_polygons; i++ )
			{
				polygons[start_polygon_index+i].resize( brick_polygons[i].size() );
				for( size_t j=0; j<brick_polygons[i].size(); j++ )
				{
					polygons[start_polygon_index+i][j] = start_point_index + brick_polygons[i][j];
				}
			}
		}
	}
}


// Explicit instantiations
template void CreateBrickWallPanel(
	const brick_wall_panel_desc<float>& desc,
	std::vector< tVector3<float> >& positions,
	std::vector< std::vector<unsigned int> >& polygons
	);

template void CreateBrickWallPanel(
	const brick_wall_panel_desc<double>& desc,
	std::vector< tVector3<double> >& positions,
	std::vector< std::vector<unsigned int> >& polygons
	);


} // namespace amorphous
