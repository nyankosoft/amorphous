#ifndef __MassSpringModelMaker_HPP__
#define __MassSpringModelMaker_HPP__


#include <utility>
#include "MassSpringSim.hpp"


namespace amorphous
{


class FixedPointFlags
{
public:

	enum EnumName
	{
		FPF_TOP_EDGE             = (1 <<  0),
		FPF_RIGHT_EDGE           = (1 <<  1),
		FPF_BOTTOM_EDGE          = (1 <<  2),
		FPF_LEFT_EDGE            = (1 <<  3),

		FPF_TOP_LEFT_CORNER      = (1 <<  4),
		FPF_TOP_RIGHT_CORNER     = (1 <<  5),
		FPF_BOTTOM_RIGHT_CORNER  = (1 <<  6),
		FPF_BOTTOM_LEFT_CORNER   = (1 <<  7),

		FPF_TOP_EDGE_CENTER      = (1 <<  8),
		FPF_RIGHT_EDGE_CENTER    = (1 <<  9),
		FPF_BOTTOM_EDGE_CENTER   = (1 << 10),
		FPF_LEFT_EDGE_CENTER     = (1 << 11),

		FPF_TOP_EDGE_3_POINTS    = FPF_TOP_LEFT_CORNER     | FPF_TOP_EDGE_CENTER    | FPF_TOP_RIGHT_CORNER,
		FPF_RIGHT_EDGE_3_POINTS  = FPF_TOP_RIGHT_CORNER    | FPF_RIGHT_EDGE_CENTER  | FPF_BOTTOM_RIGHT_CORNER,
		FPF_BOTTOM_EDGE_3_POINTS = FPF_BOTTOM_RIGHT_CORNER | FPF_BOTTOM_EDGE_CENTER | FPF_BOTTOM_LEFT_CORNER,
		FPF_LEFT_EDGE_3_POINTS   = FPF_BOTTOM_LEFT_CORNER  | FPF_LEFT_EDGE_CENTER   | FPF_TOP_LEFT_CORNER,

		FPF_TOP_EDGE_4_POINTS    = (1 << 12),
		FPF_RIGHT_EDGE_4_POINTS  = (1 << 13),
		FPF_BOTTOM_EDGE_4_POINTS = (1 << 14),
		FPF_LEFT_EDGE_4_POINTS   = (1 << 15),

		FPF_TOP_EDGE_5_POINTS    = (1 << 16),
		FPF_RIGHT_EDGE_5_POINTS  = (1 << 17),
		FPF_BOTTOM_EDGE_5_POINTS = (1 << 18),
		FPF_LEFT_EDGE_5_POINTS   = (1 << 19),

		FPF_TOP_EDGE_6_POINTS    = (1 << 20),
		FPF_RIGHT_EDGE_6_POINTS  = (1 << 21),
		FPF_BOTTOM_EDGE_6_POINTS = (1 << 22),
		FPF_LEFT_EDGE_6_POINTS   = (1 << 23),
	};
};


class MassSpringModelMaker
{
	/// Holds the output model
	CMS_MassSpringArchive m_Archive;

private:

	Result::Name UpdateControlPoints(
		const std::vector<Vector3>& points,
		const std::vector< std::pair<int,int> >& edges,
		float spring_const,
		std::vector<CMS_PointProperty>& control_points
		);

	Result::Name SetFixedPoints( const std::vector< std::vector<unsigned int> >& fixed_points );

public:

	MassSpringModelMaker(){}

	~MassSpringModelMaker(){}

	/// \brief Creates a rectangular mass spring model.
	/// The creaed model has full cross diagonal links.
	Result::Name MakeRectangularModel(
		float width  = 1.0f,
		float height = 1.0f,
		unsigned int num_horizontal_divisions = 10,
		unsigned int num_vertical_divisions   = 10,
		float spring_const = 1.0f,
//		const std::vector< std::vector<unsigned int> >& fixed_points = std::vector< std::vector<unsigned int> >()
		U32 fixed_point_flags = FixedPointFlags::FPF_TOP_EDGE
		);

	Result::Name MakeFromMesh(
		const std::vector<Vector3>& points,
		std::vector<unsigned int>& triangles,
		float spring_const = 1.0f,
		const std::vector< std::vector<unsigned int> >& fixed_points = std::vector< std::vector<unsigned int> >()
		);
};


} // amorphous


#endif /* __MassSpringModelMaker_HPP__ */
