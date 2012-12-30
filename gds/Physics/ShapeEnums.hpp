#ifndef  __PhysShapeEnums_H__
#define  __PhysShapeEnums_H__


namespace amorphous
{


namespace physics
{


class PhysShape
{
public:

	enum Shape
	{
		Box,
		Sphere,
		Cylinder,
		Capsule,
		Convex,
		TriangleMesh,
		Plane,
		NumShapes
	};
};


} // namespace physics


} // namespace amorphous


#endif /* __PhysShapeEnums_H__ */
