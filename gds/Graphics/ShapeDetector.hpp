#ifndef  __ShapeDetector_HPP__
#define  __ShapeDetector_HPP__


#include "../3DMath/fwd.hpp"
#include "../3DMath/Sphere.hpp"
#include "../3DMath/Transform.hpp"
#include "../Graphics/fwd.hpp"
#include "../Graphics/MeshModel/PrimitiveShapeMeshes.hpp"


class MeshShape
{
public:
	enum Name
	{
		AXIS_ALIGNED_BOX, // Axis-aligned box. Can be represented with AABB3 class
		ORIENTED_BOX,  // Non-axis-aligned bounding box. Can be represented with OBB3 class
		SPHERE,
		CAPSULE,
		CONVEX,
		CONCAVE,
		NUM_MESH_SHAPES
	};
};


/*
class Capsule
{
public:
	Transform pose;
	float radius;
	float length;

	Capsule()
		:
	pose(Matrix34Identity()),
	radius(1.0f),
	length(3.0f)
	{}
};*/

/*
inline IArchive& operator & ( IArchive& ar, Capsule& cap )
{
	ar & cap.pose & cap.radius & cap.length;
	return ar;
}
*/

class CShapeDetector
{
public:
	bool IsAABox( const CGeneral3DMesh& src_mesh, AABB3& aabb );

	bool IsBox( const CGeneral3DMesh& connected_mesh, CBoxDesc& desc, Matrix34& pose );

	bool IsConvex( const CGeneral3DMesh& connected_mesh );

	bool IsSphere( const CGeneral3DMesh& src_mesh, Sphere& sphere );

	bool IsCapsule( const CGeneral3DMesh& src_mesh, Capsule& capsule );

//	bool IsCylinder( const CGeneral3DMesh& connected_mesh, CCylinderDesc& cylinder );

	/// How to represent the detection results?
	bool DetectShape( const CGeneral3DMesh& src_mesh );
};



#endif /* __ShapeDetector_HPP__ */
