#ifndef __CartridgeMaker_HPP__
#define __CartridgeMaker_HPP__


#include <vector>
#include "../base.hpp"
#include "../3DMath/Vector2.hpp"
#include "../3DMath/Vector3.hpp"
#include "../Graphics/fwd.hpp"
#include "../GameCommon/Caliber.hpp"


namespace amorphous
{


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


class BulletSliceControlPoint
{
public:
	Vector2 position;
	float tension;
	float continuity;
	float bias;

	BulletSliceControlPoint()
		:
	position(Vector2(0,0)),
	tension(0),
	continuity(0),
	bias(0)
	{}

	~BulletSliceControlPoint(){}
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
		NUM_MAX_BULLET_SLICES = 10,
	};

	float diameter;
	float length; ///< the length of the bullet including the portion inside the case.
	float exposed_length; ///< length of the portion exposed from the case

	bool create_model_only_for_exposed_part;

	unsigned int num_sides; ///< If not specified, i.e. left 0, CartridgeDesc::num_sides is used instead.

	BulletTipCurveDesc bullet_curve_points[NUM_MAX_BULLET_CURVE_PIONTS];

	BulletSliceControlPoint bullet_slice_control_points[NUM_MAX_BULLET_SLICES];

	unsigned int num_control_points;

	unsigned int num_segments;

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


	enum PrimerModel
	{
		PM_POLYGON_MESH,
		PM_NORMAL_MAP,
		PM_NONE,
		NUM_PRIMER_MODELS
	};

	enum DrillStyle
	{
		DS_SIMPLIFIED,
		DS_CLOSED,
		NUM_DRILL_STYLES
	};

	Caliber::Name caliber;

	CaseSlice case_slices[MAX_NUM_CASE_SLICES];

	int num_case_slices;

	int top_outer_slice_index;

	bool create_internal_polygons;

	uint num_sides;

	/// internal diameter of the case top (= (outer diameter) - (wall thickness at the top) * 2)
	/// When low polygon model is preferred, internals are created by simply beveling in
	/// the top surface norizontally to create wall and then vertically down
	/// to create simple internal models.
	float top_internal_diameter;

	float primer_diameter;

	PrimerModel primer_model;

	DrillStyle drill_style;

public:

	CaseDesc()
		:
	caliber(Caliber::OTHER),
	num_case_slices(0),
	top_outer_slice_index(0),
	create_internal_polygons(true),
	num_sides(16),
	top_internal_diameter(0.01f),
	primer_model(PM_NONE),
	drill_style(DS_SIMPLIFIED),
	primer_diameter(0.0053f) // default primer diameter: 5.3mm
	{}

	~CaseDesc(){}
	
	float GetTopHeight() const { return case_slices[top_outer_slice_index].height; };

	float GetMaxDiameter() const
	{
		float max_diameter = 0;
		const int num_actual_slices = (num_case_slices < MAX_NUM_CASE_SLICES) ? num_case_slices : MAX_NUM_CASE_SLICES;
		for( int i=0; i<num_case_slices; i++ )
		{
			if( max_diameter < case_slices[i].diameter )
				max_diameter = case_slices[i].diameter;
		}

		return max_diameter;
	}
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
                     bool weld_top_rim_vertices,
                     bool create_bottom_polygons,
                     bool weld_bottom_rim_vertices,
                     std::vector<Vector3>& vecDestPos,
                     std::vector<Vector3>& vecDestNormal,
                     std::vector<TEXCOORD2>& vecDestTexUV,
					 std::vector< std::vector<int> >& vecDestPoly );

	Result::Name AddPrimerAndPrimerWell( const CaseDesc& src_desc, unsigned int num_sides, std::vector<Vector3>& points, std::vector<Vector3>& normals, std::vector<TEXCOORD2>& tex_uvs, std::vector< std::vector<int> >& polygons );

	Result::Name MakeCaseInternals( const CaseDesc& src_desc, unsigned int num_sides, std::vector<Vector3>& points, std::vector<Vector3>& normals, std::vector<TEXCOORD2>& tex_uvs, std::vector< std::vector<int> >& polygons );


public:

	CartridgeMaker(){}
	~CartridgeMaker(){}

//	Result::Name MakeBullet( const BulletDesc bullet_desc, General3DMesh& dest_mesh );

//	Result::Name MakeCase( const CaseDesc src_desc, unsigned int num_sides, General3DMesh& dest_mesh );

//	Result::Name Make( const CartridgeDesc src_desc, General3DMesh& dest_mesh );

	Result::Name MakeBullet(
		const BulletDesc& bullet_desc,
		unsigned int num_sides,
		float case_top_height,
		std::vector<Vector3>& points,
		std::vector<Vector3>& normals,
		std::vector<TEXCOORD2>& tex_uvs,
		std::vector< std::vector<int> >& polygons
		);

	Result::Name MakeCase(
		const CaseDesc& src_desc,
		unsigned int num_sides,
		std::vector<Vector3>& points,
		std::vector<Vector3>& normals,
		std::vector<TEXCOORD2>& tex_uvs,
		std::vector< std::vector<int> >& polygons
		);

	Result::Name Make(
		const CartridgeDesc& src_desc,
		unsigned int num_sides,
		std::vector<Vector3>& points,
		std::vector<Vector3>& normals,
		std::vector<TEXCOORD2>& tex_uvs,
		std::vector< std::vector<int> >& polygons
		);
};


} // firearm


} // amorphous



#endif /* __CartridgeMaker_HPP__ */
