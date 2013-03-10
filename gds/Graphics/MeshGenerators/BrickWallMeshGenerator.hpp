#ifndef __BrickWallMeshGenerator_HPP__
#define __BrickWallMeshGenerator_HPP__


#include "MeshGenerator.hpp"
#include "../FloatRGBAColor.hpp"
#include "../../3DMath/BrickWallPolygonModelMaker.hpp"


namespace amorphous
{


class BrickPanelDesc
{
public:
	brick_wall_panel_desc<float> polygon_model_desc;

	SFloatRGBAColor base_color;
	range<SFloatRGBAColor> per_brick_color_variations;
	range<SFloatRGBAColor> per_vertex_color_variations;
	range<float> grayscale;

	BrickPanelDesc()
		:
	base_color( SFloatRGBAColor::White() ),
	per_brick_color_variations( range<SFloatRGBAColor>( SFloatRGBAColor::White(), SFloatRGBAColor::White() ) ),
	per_vertex_color_variations( range<SFloatRGBAColor>( SFloatRGBAColor::White(), SFloatRGBAColor::White() ) ),
	grayscale(1.0f,1.0f)
	{}
};


class BrickWallMeshGenerator : public MeshGenerator
{
	BrickPanelDesc m_BrickPanelDesc;

public:

	BrickWallMeshGenerator( const BrickPanelDesc& desc = BrickPanelDesc() ) : m_BrickPanelDesc(desc) {}

	~BrickWallMeshGenerator() {}

	Result::Name Generate();
};


} // namespace amorphous



#endif /* __BrickWallMeshGenerator_HPP__ */
