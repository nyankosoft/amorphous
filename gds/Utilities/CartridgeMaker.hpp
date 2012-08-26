#ifndef __CartridgeMaker_HPP__
#define __CartridgeMaker_HPP__


#include <vector>
#include "../base.hpp"
#include "../3DMath/Vector3.hpp"
#include "../Graphics/fwd.hpp"


namespace firearm
{


class CartridgeSegment;


class CaseSlice
{
public:
//	float top_diameter;
	float diameter;
	float height;
	float curvature_radius;

	CaseSlice()
		:
//	top_diameter(0),
	diameter(0),
	height(0),
	curvature_radius(0)
	{}

};


class BulletTipCurveDesc
{
public:
	float diameter;
	float height;
};

class BulletDesc
{
public:

	enum Params
	{
		NUM_MAX_BULLET_CURVE_PIONTS = 10,
	};

	float diameter;
	float length; ///< the length of the bullet including the portion inside the case.
	float exposed_length; ///< length of the portion exposed from the case

	unsigned int num_sides; ///< If not specified, i.e. left 0, CartridgeDesc::num_sides is used instead.

	BulletTipCurveDesc bullet_curve_points[NUM_MAX_BULLET_CURVE_PIONTS];

//	float hollow_piont_cavity_diameter; ///< diameter of the hollow point cavity. If this is 0, the bullet will have no hollow point cavity, i.e. the bullet is created as a FMJ
//	float hollow_piont_cavity_depth   ; ///< depth of the hollow point cavity;

	BulletDesc()
		:
	diameter(0),
	length(0),
	exposed_length(0),
	num_sides(0)
	{}
};


class CaseDesc
{
public:

	enum Params
	{
		MAX_NUM_CASE_SLICES = 16,
	};

public:

	CaseDesc(){}
	~CaseDesc(){}

	CaseSlice case_slices[MAX_NUM_CASE_SLICES];
};



class CartridgeDesc
{
public:

	BulletDesc bullet_desc;

	CaseDesc case_desc;

	uint num_sides;

//	float neck_length;
//	float body_length;
//	float extractor_groove_length;
//	float base_length;

public:

	CartridgeDesc()
		:
	num_sides(16)
	{}
};


class CartridgeMaker
{
//	void AddSegment( float base_y, const CartridgeSegment& segment, bool use_prev_vertices );

	void AddSegments(
                     const std::vector< std::pair<float,float> >& diameter_and_height,
                     int num_sides,
                     bool create_top_polygons,
                     //PrimitiveModelStyle::Name top_style,
                     bool create_bottom_polygons,
                     //PrimitiveModelStyle::Name bottom_style,
                     std::vector<Vector3>& vecDestPos,
                     std::vector<Vector3>& vecDestNormal,
					 std::vector< std::vector<int> >& vecDestPoly );

public:

	CartridgeMaker(){}
	~CartridgeMaker(){}

	Result::Name MakeBullet( const BulletDesc bullet_desc, CGeneral3DMesh& dest_mesh );

	Result::Name MakeCase( const CaseDesc src_desc, unsigned int num_sides, CGeneral3DMesh& dest_mesh );

	Result::Name Make( const CartridgeDesc src_desc, CGeneral3DMesh& dest_mesh );

	Result::Name MakeBullet(
		const BulletDesc& bullet_desc,
		unsigned int num_sides,
		std::vector<Vector3>& points,
		std::vector<Vector3>& normals,
		std::vector< std::vector<int> >& polygons
		);

	Result::Name MakeCase(
		const CaseDesc& src_desc,
		unsigned int num_sides,
		std::vector<Vector3>& points,
		std::vector<Vector3>& normals,
		std::vector< std::vector<int> >& polygons
		);

	Result::Name Make(
		const CartridgeDesc& src_desc,
		unsigned int num_sides,
		std::vector<Vector3>& points,
		std::vector<Vector3>& normals,
		std::vector< std::vector<int> >& polygons
		);
};


} // firearm



#endif /* __CartridgeMaker_HPP__ */
