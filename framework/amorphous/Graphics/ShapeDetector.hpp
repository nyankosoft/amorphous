#ifndef  __ShapeDetector_HPP__
#define  __ShapeDetector_HPP__


#include <vector>
#include "../3DMath/fwd.hpp"
#include "../3DMath/AABB3.hpp"
#include "../3DMath/Transform.hpp"
#include "../3DMath/Capsule.hpp"
#include "../Graphics/fwd.hpp"
#include "../Graphics/MeshModel/PrimitiveShapeMeshes.hpp"


namespace amorphous
{


class MeshShape
{
public:
	enum Name
	{
		INVALID,
		AXIS_ALIGNED_BOX,  ///< Axis-aligned box. Can be represented with AABB3 class
		ORIENTED_BOX,      ///< Non-axis-aligned bounding box. Can be represented with OBB3 class
		SPHERE,
		CAPSULE,
		CONVEX,
		CONCAVE,
		NUM_MESH_SHAPES
	};
};


class ShapeDetectionResults
{
public:
	MeshShape::Name shape;

	AABB3 aabb; ///< stores the detected AABB

	Matrix34 pose;

	Vector3 radii; ///< stores the radii of the detected OBB

	Capsule capsule; ///< Stores the detected capsule

	/// Stores the detected convex shape
	std::vector<Vector3> points;
	std::vector<unsigned int> triangle_indices;

	ShapeDetectionResults()
		:
	shape(MeshShape::INVALID),
	radii(Vector3(0,0,0))
	{}
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

class ShapeDetector
{
public:
	bool IsAABox( const General3DMesh& src_mesh, AABB3& aabb );

	bool IsBox( const General3DMesh& src_mesh, BoxDesc& desc, Matrix34& pose );

	bool IsConvex( const General3DMesh& src_mesh );

	bool IsSphere( const General3DMesh& src_mesh, Sphere& sphere );

	bool IsCapsule( const General3DMesh& src_mesh, Capsule& capsule );

//	bool IsCylinder( const General3DMesh& src_mesh, CylinderDesc& cylinder );

	/// How to represent the detection results?
	bool DetectShape( const General3DMesh& src_mesh, ShapeDetectionResults& results );
};


} // namespace amorphous



#endif /* __ShapeDetector_HPP__ */
