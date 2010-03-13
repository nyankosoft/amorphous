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
		POS_Y,
		POS_Z,
		NEG_X,
		NEG_Y,
		NEG_Z,
	};

	static Vector3 GetAxis( AxisAndDirection::Name axis_dir )
	{
		Vector3 vAxis = Vector3(0,0,0);
		vAxis[ axis_dir % 3 ] = 1.0f * ( (axis_dir < 3) ? 1 : -1 );
		return vAxis;
	}
};


class PrimitivePlacingStyle
{
public:
	enum Name
	{
		PLACE_CENTER_AT_ORIGIN,
		PLACE_ON_PLANE,
		NUM_STYLES,
	};
};


class MeshPolygonDirection
{
public:
	enum Type
	{
		INWARD,
		OUTWARD,
		NUM_TYPES
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


class CBoxDesc
{
	Vector3 vLengths;

public:

	CBoxDesc()
		:
	vLengths( Vector3(1,1,1) )
	{}
};


class CCylinderDesc
{
public:

	float radii[2]; ///< array of radii
	float height;
	AxisAndDirection::Name axis;

	int num_sides;

	PrimitivePlacingStyle::Name style;

public:

	CCylinderDesc()
		:
	height(1),
	axis(AxisAndDirection::POS_Y),
	num_sides(6),
	style(PrimitivePlacingStyle::PLACE_CENTER_AT_ORIGIN)
	{
		for( int i = 0; i<sizeof(float)/sizeof(radii); i++ )
			radii[i] = 1.0f;
	}

	bool IsValid() const
	{
		if( 0.001f < radii[0]
		 && 0.001f < radii[1]
		 && 0.001f < height
		 && 2 < num_sides )
		{
			return true;
		}
		else
			return false;
	}
};


class CSphereDesc
{
public:
	MeshPolygonDirection::Type poly_dir;
	float radii[3]; ///< array of radii
	int num_sides;
	int num_segments;
	int axis;

public:

	CSphereDesc()
		:
	poly_dir(MeshPolygonDirection::OUTWARD),
	num_sides(12),
	num_segments(8),
	axis(2)
	{
		for( int i = 0; i<3; i++ )
			radii[i] = 0.5f;
	}

	bool IsValid() const
	{
		if( 0.001f < radii[0]
		 && 0.001f < radii[1]
		 && 0.001f < radii[2]
		 && 2 < num_sides
		 && 1 < num_segments )
		{
			return true;
		}
		else
			return false;
	}
};



class CCapsuleDesc
{
public:
	float radius;
	float length;
	int num_sides;
	int num_segments; ///< segments of a hemisphere

public:

	CCapsuleDesc()
		:
	radius(0.5f),
	length(2.0f),
	num_sides(12),
	num_segments(6)
	{
	}

	bool IsValid() const
	{
		if( 0.001f < radius
		 && 0.001f < length - radius * 2.0f
		 && 2 < num_sides
		 && 1 < num_segments )
		{
			return true;
		}
		else
			return false;
	}
};



extern void CreateConeMesh( const CConeDesc& desc,       CGeneral3DMesh& mesh );
extern void CreateSphereMesh( const CSphereDesc& desc,   CGeneral3DMesh& mesh );
extern void CreateCapsuleMesh( const CCapsuleDesc& desc, CGeneral3DMesh& mesh );


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
