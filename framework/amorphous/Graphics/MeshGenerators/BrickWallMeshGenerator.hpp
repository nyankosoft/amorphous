#ifndef __BrickWallMeshGenerator_HPP__
#define __BrickWallMeshGenerator_HPP__


#include "MeshGenerator.hpp"
#include "../FloatRGBAColor.hpp"
#include "../../3DMath/BrickWallPolygonModelMaker.hpp"
#include "../../Utilities/PerlinAux.hpp"


namespace amorphous
{


class BrickPanelDesc
{
public:

	enum Preset
	{
		PS_CUSTOM,
		PS_DARK_GREEN_BRICK_10x20,
		PS_DARK_GREEN_BRICK_16x32,
		PS_GRAY_BRICK_8x16,
		PS_GRAY_BRICK_8x24,
		PS_LIGHT_BROWN_BRICK_8x16,
		PS_LIGHT_BROWN_BRICK_16x32,
		PS_RED_BRICK_8X16,
		PS_BLUE_BRICK_6X12,
		PS_BEIGE_BRICK_8X16,
		PS_GRAY_BRICK_TILE_16X16,
		PS_GRAY_TILE_12X12,
		PS_LIGHT_GRAY_TILE_12X12,
		PS_DARK_GREEN_TILE_16X16,
		NUM_PRESETS
	};

//	const char *name; ///< used with the presets

	brick_wall_panel_desc<float> polygon_model_desc;

	SFloatRGBAColor base_layer_color;
	SFloatRGBAColor brick_color;
	range<SFloatRGBAColor> per_brick_color_variations;
	range<SFloatRGBAColor> per_vertex_color_variations;
	range<float> grayscale;

	PerlinNoiseParams perlin_noise;
	float pn_constrast;  ///< contrast factor for the perlin noise texture
	float pn_brightness; ///< brightness shift for the perlin noise texture

	Preset preset_id;

public:

	BrickPanelDesc(
		Preset preset_id_ = BrickPanelDesc::PS_CUSTOM,
		brick_wall_panel_desc<float> desc = brick_wall_panel_desc<float>(),
		SFloatRGBAColor base_layer_color_ = SFloatRGBAColor(0.2f,0.2f,0.2f,1.0f),
		SFloatRGBAColor brick_color_ = SFloatRGBAColor::White(),
		range<SFloatRGBAColor> per_brick_color_variations_  = range<SFloatRGBAColor>( SFloatRGBAColor::White(), SFloatRGBAColor::White() ),
		range<SFloatRGBAColor> per_vertex_color_variations_ = range<SFloatRGBAColor>( SFloatRGBAColor::White(), SFloatRGBAColor::White() ),
		range<float> grayscale_ = range<float>(1.0f,1.0f),
		PerlinNoiseParams perlin_noise_ = PerlinNoiseParams(),
		float pn_constrast_ = 1.0f,
		float pn_brightness_ = 0.0f
		)
		:
	preset_id(preset_id_),
	polygon_model_desc( desc ),
	base_layer_color( base_layer_color_ ),
	brick_color( brick_color_ ),
	per_brick_color_variations(  per_brick_color_variations_  ),
	per_vertex_color_variations( per_vertex_color_variations_ ),
	grayscale( grayscale_ ),
	perlin_noise( perlin_noise_ ),
	pn_constrast( pn_constrast_ ),
	pn_brightness( pn_brightness_ )
	{}
};


class BrickWallMeshGenerator : public MeshGenerator
{
	BrickPanelDesc m_BrickPanelDesc;

public:

	BrickWallMeshGenerator( const BrickPanelDesc& desc = BrickPanelDesc() ) : m_BrickPanelDesc(desc) {}

	BrickWallMeshGenerator( BrickPanelDesc::Preset preset );

	~BrickWallMeshGenerator() {}

	Result::Name Generate();
};


} // namespace amorphous



#endif /* __BrickWallMeshGenerator_HPP__ */
