#ifndef __PrimitiveShapeMeshes_HPP__
#define __PrimitiveShapeMeshes_HPP__


#include "../fwd.hpp"
#include "Graphics/MeshModel/General3DMesh.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
using namespace MeshModel;


class AxisAndDirection
{
public:
	enum Name
	{
		POS_X,
		NEG_X,
		POS_Y,
		NEG_Y,
		POS_Z,
		NEG_Z,
	};
};

class CConeDesc
{
public:

	float radius;
	float cone_height;
	float body_height;
	AxisAndDirection::Name axis;
	int num_sides;
	int num_segments;

public:

	CConeDesc()
		:
	radius(1),
	cone_height(1),
	body_height(1),
	axis(AxisAndDirection::POS_Z),
	num_sides(16),
	num_segments(1)
	{}
};


extern void CreateConeMesh( const CConeDesc& desc, CGeneral3DMesh& mesh );


inline void CreateConeMeshArchive( const CConeDesc& desc, C3DMeshModelArchive& mesh_archive )
{
//	CGeneral3DMesh mesh;
//	CreateConeMesh( desc, mesh );
//	mesh.Create3DMeshModelArchive( mesh_archive );

	boost::shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh() );
	CreateConeMesh( desc, *(pMesh.get()) );
	C3DMeshModelBuilder mesh_builder;
	mesh_builder.BuildMeshModelArchive( pMesh );
	mesh_archive = mesh_builder.GetArchive();
}



#endif  /* __PrimitiveShapeMeshes_HPP__ */
