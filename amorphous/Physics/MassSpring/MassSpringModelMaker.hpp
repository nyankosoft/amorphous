#ifndef __MassSpringModelMaker_HPP__
#define __MassSpringModelMaker_HPP__


#include <utility>
#include "MassSpringSim.hpp"


namespace amorphous
{


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

	Result::Name MakeRectangularModel(
		float width  = 1.0f,
		float height = 1.0f,
		unsigned int num_horizontal_divisions = 10,
		unsigned int num_vertical_divisions   = 10,
		float spring_const = 1.0f,
		const std::vector< std::vector<unsigned int> >& fixed_points = std::vector< std::vector<unsigned int> >()
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
