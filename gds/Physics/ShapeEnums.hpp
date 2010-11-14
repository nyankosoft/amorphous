#ifndef  __PhysShapeEnums_H__
#define  __PhysShapeEnums_H__


namespace physics
{


class PhysShape
{
public:

	enum Shape
	{
		Box,
		Sphere,
		Capsule,
//		Cylinder,
		Convex,
		TriangleMesh,
		NumShapes
	};
};


} // namespace physics


#endif /* __PhysShapeEnums_H__ */
