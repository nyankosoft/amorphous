#include "BrickWallMeshGenerator.hpp"
#include "../../3DMath/PolygonModelUtilities.hpp"
#include "../../Support/MTRand.hpp"

namespace amorphous
{

using std::vector;
//using boost::shared_ptr;

/*
void CreateBrick( std::vector<Vector3>& positions, std::vector< std::vector<int> >& polygons )
{
	unsigned int num_blocks_h = 8;
	unsigned int num_blocks_v = 16;
	float interval = 0.01f;
	float extrusion = 0.05f;
	const unsigned int half_brick_width  = (1.0f / (float)num_blocks_h - interval) * 0.5f;
	const unsigned int half_brick_height = (1.0f / (float)num_blocks_v - interval) * 0.5f;

	// vertices[0,3] - brick face corners (TL,TR,BR,BL)
	positions[0] = Vector3( -half_brick_width+inset,  half_brick_height-inset, extrusion );
	positions[1] = Vector3(  half_brick_width-inset,  half_brick_height-inset, extrusion );
	positions[2] = Vector3(  half_brick_width-inset, -half_brick_height+inset, extrusion );
	positions[3] = Vector3( -half_brick_width+inset, -half_brick_height+inset, extrusion );

	// vertices[4,7] - bottom corners (TL,TR,BR,BL)
	positions[0] = Vector3( -half_brick_width,        half_brick_height,       0 );
	positions[1] = Vector3(  half_brick_width,        half_brick_height,       0 );
	positions[2] = Vector3(  half_brick_width,       -half_brick_height,       0 );
	positions[3] = Vector3( -half_brick_width,       -half_brick_height,       0 );

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
*/

Result::Name BrickWallMeshGenerator::Generate()
{
	// Create a brick and clone
	// The bricks are placed along the z-axis facing toward the negative half-space
	// in the left hand cooridinates.
	// rationale: A camera faces the positive half-space in the z-axis direction.

//	vector<Vector3> brick_positions;
//	vector< vector<int> > brick_polygons;
//
//	positions.resize( 8 );
//
//	CreateBrick( brick_positions, brick_polygons );

	vector<Vector3> positions;// = m_MeshArchive.GetVertexSet().vecPosition;
	vector<Vector3> normals;//   = m_MeshArchive.GetVertexSet().vecNormal;
	vector< vector<unsigned int> > polygons;

	const brick_wall_panel_desc<float>& desc = m_BrickPanelDesc.polygon_model_desc;
	CreateBrickWallPanel( desc, positions, polygons );

	unsigned int num_vertices_before_unwelding = (unsigned int)positions.size();

	unsigned int num_vertices_per_brick = 8;
	unsigned int num_bricks = num_vertices_before_unwelding / num_vertices_per_brick;

	vector<SFloatRGBAColor> diffuse_colors;
	diffuse_colors.resize( positions.size(), SFloatRGBAColor::White() );

	const range<SFloatRGBAColor>& color = m_BrickPanelDesc.per_brick_color_variations;
	for( unsigned int i=0; i<num_bricks; i++ )
	{
		float f = RangedRand( 0.0f, 1.0f );
		SFloatRGBAColor c = SFloatRGBAColor(
			color.min.red   + (color.max.red   - color.min.red)   * f,
			color.min.green + (color.max.green - color.min.green) * f,
			color.min.blue  + (color.max.blue  - color.min.blue)  * f,
			color.min.alpha + (color.max.alpha - color.min.alpha) * f
			);

		for( unsigned int j=0; j<num_vertices_per_brick; j++ )
		{
			diffuse_colors[i*num_vertices_per_brick+j] = c;
		}
	}

	vector< std::pair<unsigned int,unsigned int> > unweld_info;
	Unweld( positions, polygons, unweld_info );

	CopyUnweld( unweld_info, num_vertices_before_unwelding, positions.size(), diffuse_colors );

	CalculateVertexNormals( positions, polygons, normals );

	unsigned int num_vertices = (unsigned int)positions.size();
/*
	const range<SFloatRGBAColor>& pv_color = m_BrickPanelDesc.per_vertex_color_variations;
	for( unsigned int i=0; i<num_vertices; i++ )
	{
//		diffuse_colors[i] = SFloatRGBAColor(
//			RangedRand( pv_color.min.red,   pv_color.max.red ),
//			RangedRand( pv_color.min.green, pv_color.max.green ),
//			RangedRand( pv_color.min.blue,  pv_color.max.blue ),
//			RangedRand( pv_color.min.alpha, pv_color.max.alpha )
//			);

		float f = RangedRand( 0.0f, 1.0f );
		diffuse_colors[i] = SFloatRGBAColor(
			pv_color.min.red   + (pv_color.max.red,  - pv_color.min.red)   * f,
			pv_color.min.green + (pv_color.max.green - pv_color.min.green) * f,
			pv_color.min.blue  + (pv_color.max.blue  - pv_color.min.blue)  * f,
			pv_color.min.alpha + (pv_color.max.alpha - pv_color.min.alpha) * f
			);
	}
*/
//	const range<float>& grayscale = m_BrickPanelDesc.grayscale;
//	for( unsigned int i=0; i<num_vertices; i++ )
//	{
//		float f = RangedRand( grayscale.min, grayscale.max );
//		diffuse_colors[i] = SFloatRGBAColor( f, f, f, 1.0f );
//	}

	vector<TEXCOORD2> tex_coords;
	tex_coords.resize( num_vertices, TEXCOORD2(0,0) );

	Result::Name res = CreateSingleSubsetMeshArchive(
		positions,
		normals,
		diffuse_colors,
		tex_coords,
		polygons,
		m_MeshArchive
		);

	return res;
}


//CMeshObjectHandle CreateBrickWallMesh( const BrickPanelDesc& desc )
//{
//	return CMeshObjectHandle();
//}


} // namespace amorphous
