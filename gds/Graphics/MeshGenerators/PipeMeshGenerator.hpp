#ifndef __PipeMeshGenerator_HPP__
#define __PipeMeshGenerator_HPP__


#include "MeshGenerators.hpp"


namespace amorphous
{


class PipeMeshGenerator : public MeshGenerator
{
	CCylinderDesc m_Desc;

	float m_fWallThickness;

	Result::Name CreateVertices( General3DMesh& mesh );

	Result::Name CreatePolygons( General3DMesh& mesh );

public:

	PipeMeshGenerator( float length = 1.00f, float outer_radius = 0.25f, float inner_radius = 0.20f, unsigned int num_sides = 32, unsigned int num_divisions = 1 )
	{
		m_Desc.height        = length;
		m_Desc.radii[0]      = m_Desc.radii[1] = outer_radius;
		m_Desc.num_sides     = num_sides;
		m_Desc.num_divisions = num_divisions;
		m_fWallThickness     = outer_radius - inner_radius;
	}

	PipeMeshGenerator( const CCylinderDesc& desc, float wall_thickness )
		:
	m_Desc(desc),
	m_fWallThickness(wall_thickness)
	{}

	Result::Name Generate();
};


//MeshHandle CreatePipeMesh( float outer_radius, float inner_radius, float height, unsigned int num_sides );


} // namespace amorphous



#endif /* __PipeMeshGenerator_HPP__ */
