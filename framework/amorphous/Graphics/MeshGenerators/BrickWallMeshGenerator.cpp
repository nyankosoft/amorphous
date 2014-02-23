#include "BrickWallMeshGenerator.hpp"
#include "../TextureGenerators/PerlinNoiseTextureGenerator.hpp"
//#include "../TextureGenerators/GridTextureGenerator.hpp"
#include "../TextureGenerators/TextureFilter.hpp"
#include "../../3DMath/PolygonModelUtilities.hpp"
#include "../../Support/MTRand.hpp"

namespace amorphous
{

using std::vector;
//using boost::shared_ptr;


typedef BrickPanelDesc bpd;
typedef SFloatRGBAColor rgba;
//typedef PerlinNoiseParams pnp;
typedef range<rgba> cr; // color range
typedef range<float> fr; // float range

PerlinNoiseParams pnp( int octaves, float freq, float amp )
{
	return PerlinNoiseParams( octaves, freq, amp, 0.0f, 1.0f, 0, true );
}

brick_wall_panel_desc<float> bd( // basic desc
	unsigned int num_bricks_h,
	unsigned int num_bricks_v,
	int alternate_shift,
	float inset,
	float extrusion,
	float interval,
	range<float> size_variations,
	range<float> position_variations
	)
{
	brick_wall_panel_desc<float> desc;
	desc.num_horizontal_bricks = num_bricks_h;
	desc.num_vertical_bricks   = num_bricks_v;
	desc.alternate_shift       = alternate_shift;
	desc.inset                 = inset;
	desc.extrusion             = extrusion;
	desc.interval              = interval;
	desc.size_variations       = size_variations;
	desc.position_variations   = position_variations;
	return desc;
}

static BrickPanelDesc sg_presets[] = 
{
	//   name                           bricks(h,v) alt inset  ext     interval                                          base layer color           brick color                       brick color variations                                                      vertex color variations             grayscale      Perlin             contrast brightness
	bpd( bpd::PS_DARK_GREEN_TILE_16X16,   bd(16, 16,-1, 0.002f, 0.002f, 0.005f, fr(-0.002f,0.002f), fr(-0.002f,0.002f) ), rgba(0.3f,0.3f,0.3f,1.0f), rgba(0.60f, 0.65f, 0.56f, 1.0f ), cr( rgba(-0.20f,-0.20f,-0.20f, 1.0f ), rgba( 0.20f, 0.20f, 0.20f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.50f,   0.60f ),
	bpd( bpd::PS_DARK_GREEN_BRICK_10x20,  bd(10, 20, 1, 0.005f, 0.005f, 0.005f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba(0.60f, 0.65f, 0.56f, 1.0f ), cr( rgba(-0.20f,-0.20f,-0.20f, 1.0f ), rgba( 0.20f, 0.20f, 0.20f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 12, 15, 10 ), 0.50f,   0.60f ),
	bpd( bpd::PS_DARK_GREEN_BRICK_16x32,  bd(16, 32, 1, 0.002f, 0.002f, 0.003f, fr(-0.001f,0.001f), fr( 0.001f,0.001f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba(0.60f, 0.65f, 0.56f, 1.0f ), cr( rgba(-0.20f,-0.20f,-0.20f, 1.0f ), rgba( 0.20f, 0.20f, 0.20f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 12, 15, 10 ), 0.50f,   0.60f ),
	bpd( bpd::PS_GRAY_BRICK_8x16,         bd( 8, 16, 1, 0.007f, 0.007f, 0.005f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba::White(),                    cr( rgba::White(), rgba::White() ),                                         cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.35f,   0.65f ),
	bpd( bpd::PS_GRAY_BRICK_8x24,         bd( 8, 24, 1, 0.005f, 0.005f, 0.005f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba::White(),                    cr( rgba::White(), rgba::White() ),                                         cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.35f,   0.65f ),
	bpd( bpd::PS_LIGHT_BROWN_BRICK_8x16,  bd( 8, 16, 1, 0.006f, 0.006f, 0.006f, fr(-0.001f,0.001f), fr(-0.002f,0.002f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba(0.87f, 0.81f, 0.72f, 1.0f ), cr( rgba(-0.35f,-0.35f,-0.35f, 1.0f ), rgba( 0.30f, 0.30f, 0.30f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.45f,   0.68f ),
	bpd( bpd::PS_LIGHT_BROWN_BRICK_16x32, bd(16, 32, 1, 0.003f, 0.003f, 0.004f, fr(-0.001f,0.001f), fr(-0.001f,0.001f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba(0.87f, 0.81f, 0.72f, 1.0f ), cr( rgba(-0.35f,-0.35f,-0.35f, 1.0f ), rgba( 0.30f, 0.30f, 0.30f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.45f,   0.68f ),
	bpd( bpd::PS_RED_BRICK_8X16,          bd( 8, 16, 1, 0.007f, 0.007f, 0.005f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba(0.85f, 0.48f, 0.41f, 1.0f ), cr( rgba(-0.25f,-0.25f,-0.25f, 1.0f ), rgba( 0.25f, 0.25f, 0.25f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.35f,   0.65f ),
	bpd( bpd::PS_BLUE_BRICK_6X12,         bd( 6, 12, 1, 0.008f, 0.008f, 0.005f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.2f,0.2f,0.2f,1.0f), rgba(0.50f, 0.58f, 0.82f, 1.0f ), cr( rgba(-0.10f,-0.10f,-0.10f, 1.0f ), rgba( 0.15f, 0.15f, 0.15f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.35f,   0.65f ),
	bpd( bpd::PS_BEIGE_BRICK_8X16,        bd( 8, 16, 1, 0.006f, 0.006f, 0.005f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.3f,0.3f,0.3f,1.0f), rgba(1.00f, 0.98f, 0.81f, 1.0f ), cr( rgba(-0.10f,-0.10f,-0.10f, 1.0f ), rgba( 0.10f, 0.10f, 0.10f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.35f,   0.65f ),
	bpd( bpd::PS_GRAY_BRICK_TILE_16X16,   bd(16, 16, 1, 0.004f, 0.004f, 0.004f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.3f,0.3f,0.3f,1.0f), rgba(0.90f, 0.90f, 0.90f, 1.0f ), cr( rgba(-0.10f,-0.10f,-0.10f, 1.0f ), rgba( 0.10f, 0.10f, 0.10f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.35f,   0.65f ),
	bpd( bpd::PS_GRAY_TILE_12X12,         bd(12, 12,-1, 0.002f, 0.002f, 0.005f, fr(-0.001f,0.001f), fr(-0.002f,0.002f) ), rgba(0.3f,0.3f,0.3f,1.0f), rgba(0.65f, 0.65f, 0.65f, 1.0f ), cr( rgba(-0.12f,-0.12f,-0.12f, 1.0f ), rgba( 0.12f, 0.12f, 0.12f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.50f,   0.50f ),
	bpd( bpd::PS_LIGHT_GRAY_TILE_12X12,   bd(12, 12,-1, 0.003f, 0.003f, 0.004f, fr( 0.000f,0.000f), fr( 0.000f,0.000f) ), rgba(0.3f,0.3f,0.3f,1.0f), rgba(0.92f, 0.92f, 0.93f, 1.0f ), cr( rgba(-0.10f,-0.10f,-0.10f, 1.0f ), rgba( 0.10f, 0.10f, 0.10f, 1.0f ) ), cr( rgba::White(), rgba::White() ), fr(1.0f,1.0f), pnp( 10, 10, 10 ), 0.35f,   0.65f ),
};

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


BrickWallMeshGenerator::BrickWallMeshGenerator( BrickPanelDesc::Preset preset )
{
	for( int i=0; i<BrickPanelDesc::NUM_PRESETS; i++ )
	{
		if( preset == i )
			m_BrickPanelDesc = sg_presets[i];
	}
}


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
	diffuse_colors.resize( positions.size(), m_BrickPanelDesc.brick_color );

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
			diffuse_colors[i*num_vertices_per_brick+j] += c;
			diffuse_colors[i*num_vertices_per_brick+j].Clamp();
		}
	}

	if( 4 < diffuse_colors.size() && m_BrickPanelDesc.polygon_model_desc.make_base_layer )
	{
		// The last for diffuse colors are for the base layer rectangle
		for( size_t i=diffuse_colors.size()-4; i<diffuse_colors.size(); i++ )
		{
			diffuse_colors[i] = m_BrickPanelDesc.base_layer_color;
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

	MakeTextureCoordinatesAlongAxis( positions, 2, 0, 1, tex_coords );

	Result::Name res = CreateSingleSubsetMeshArchive(
		positions,
		normals,
		diffuse_colors,
		tex_coords,
		polygons,
		m_MeshArchive
		);

	vector<CMMA_Material>& materials = m_MeshArchive.GetMaterial();
	if( 0 < materials.size() )
	{
		if( 0 < materials[0].vecTexture.size() )
		{
			materials[0].vecTexture[0].Width  = 1024;
			materials[0].vecTexture[0].Height = 1024;
			materials[0].vecTexture[0].Format = TextureFormat::A8R8G8B8;
			materials[0].vecTexture[0].pLoader.reset( new PerlinNoiseTextureGenerator() );
//			materials[0].vecTexture[0].pLoader.reset( new GridTextureGenerator() );
			boost::shared_ptr<ContrastBrightnessFilter> pFilter( new ContrastBrightnessFilter( 0.36f, 0.68f ) );
			materials[0].vecTexture[0].pLoader->AddFilter( pFilter );
		}
	}

	return res;
}


//CMeshObjectHandle CreateBrickWallMesh( const BrickPanelDesc& desc )
//{
//	return CMeshObjectHandle();
//}


} // namespace amorphous
