#ifndef  __PhysShapeImpl_H__
#define  __PhysShapeImpl_H__


#include "3DMath/Matrix34.hpp"

#include "fwd.hpp"


namespace amorphous
{


namespace physics
{


class CShapeImpl
{
public:

	CShapeImpl() {}

	virtual ~CShapeImpl() {}

	virtual bool Raycast( CRay& world_ray, Scalar max_ray_dist, bool first_hit ) = 0;

	void SetDefault()
	{
	}
};


class CBoxShapeImpl : public CShapeImpl
{
public:

	CBoxShapeImpl() {}
	virtual ~CBoxShapeImpl() {}
};


class CSphereShapeImpl : public CShapeImpl
{
public:

	CSphereShapeImpl() {}
	virtual ~CSphereShapeImpl() {}
};


class CCapsuleShapeImpl : public CShapeImpl
{
public:

	CCapsuleShapeImpl() {}
	virtual ~CCapsuleShapeImpl() {}
};


class CCylinderShapeImpl : public CShapeImpl
{
public:

	CCylinderShapeImpl() {}
	virtual ~CCylinderShapeImpl() {}
};


class CTriangleMeshShapeImpl : public CShapeImpl
{
public:

	CTriangleMeshShapeImpl() {}
	virtual ~CTriangleMeshShapeImpl() {}
};


} // namespace physics

} // namespace amorphous



#endif		/*  __PhysShapeImpl_H__  */
