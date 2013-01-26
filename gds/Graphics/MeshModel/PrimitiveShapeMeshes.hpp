#ifndef __PrimitiveShapeMeshes_HPP__
#define __PrimitiveShapeMeshes_HPP__


#include "../../base.hpp"
#include "../fwd.hpp"
#include "../../3DMath/Vector3.hpp"


namespace amorphous
{


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
	axis(AxisAndDirection::POS_Y),
	num_sides(16),
	num_segments(1)
	{}
};


class CBoxDesc
{
public:

	Vector3 vLengths;

public:

	CBoxDesc()
		:
	vLengths( Vector3(1,1,1) )
	{}
};


class CCylinderMeshStyleFlags
{
public:
	enum Flags
	{
		TOP_POLYGONS    = (1 << 0),
		BOTTOM_POLYGONS = (1 << 1),
		WELD_VERTICES   = (1 << 3),
	};
};


class CCylinderDesc
{
public:

	float radii[2]; ///< array of radii
	float height;
	AxisAndDirection::Name axis;

	/// How finely the cylinder is divided on its side.
	/// Greater value makes the cylinder closer to the circular shape.
	int num_sides;

	/// How many times the cylinder is divided along the direction of its axis.
	/// This value does not change the appearance of the mesh.
	int num_divisions;

	PrimitivePlacingStyle::Name style;

	U32 style_flags; ///< default = TOP_POLYGONS | BOTTOM_POLYGONS

public:

	CCylinderDesc()
		:
	height(1),
	axis(AxisAndDirection::POS_Y),
	num_sides(6),
	num_divisions(1),
	style(PrimitivePlacingStyle::PLACE_CENTER_AT_ORIGIN),
	style_flags( CCylinderMeshStyleFlags::TOP_POLYGONS | CCylinderMeshStyleFlags::BOTTOM_POLYGONS )
	{
		for( int i = 0; i<sizeof(radii)/sizeof(float); i++ )
			radii[i] = 0.5f;
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



extern void CreateCylinderMesh( const CCylinderDesc& desc, General3DMesh& mesh );
extern void CreateConeMesh( const CConeDesc& desc,         General3DMesh& mesh );
extern void CreateSphereMesh( const CSphereDesc& desc,     General3DMesh& mesh );
extern void CreateCapsuleMesh( const CCapsuleDesc& desc,   General3DMesh& mesh );

extern Result::Name CreateCylinderMeshArchive( const CCylinderDesc& desc, C3DMeshModelArchive& mesh_archive );
extern Result::Name CreateConeMeshArchive( const CConeDesc& desc,         C3DMeshModelArchive& mesh_archive );
extern Result::Name CreateSphereMeshArchive( const CSphereDesc& desc,     C3DMeshModelArchive& mesh_archive );
extern Result::Name CreateCapsuleMeshArchive( const CCapsuleDesc& desc,   C3DMeshModelArchive& mesh_archive );

extern Result::Name CreateArchiveFromGeneral3DMesh( boost::shared_ptr<General3DMesh>& pSrcMesh, C3DMeshModelArchive& dest_mesh_archive );


} // namespace amorphous



#endif  /* __PrimitiveShapeMeshes_HPP__ */
