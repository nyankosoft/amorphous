#ifndef  __ShapeDetector_HPP__
#define  __ShapeDetector_HPP__


#include "../3DMath/fwd.hpp"
#include "../3DMath/Transform.hpp"
#include "../Graphics/fwd.hpp"


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
};


inline IArchive& operator & ( IArchive& ar, Transform& pose )
{
	ar & pose.qRotation & pose.vTranslation;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, Capsule& cap )
{
	ar & cap.pose & cap.radius & cap.length;
	return ar;
}

class CShapeDetector
{
public:
	bool IsAABox( const CGeneral3DMesh& src_mesh, AABB3& aabb );

	bool IsBox( const CGeneral3DMesh& connected_mesh, CBoxDesc& desc, Matrix34& pose );

	bool IsConvex( const CGeneral3DMesh& connected_mesh )

	bool IsSphere( const CGeneral3DMesh& src_mesh, Sphere& sphere );

	bool IsCapsule( const CGeneral3DMesh& src_mesh, Capsule& capsule );

//	bool IsCylinder( const CGeneral3DMesh& connected_mesh, CCylinderDesc& cylinder );
};



#endif /* __ShapeDetector_HPP__ */
