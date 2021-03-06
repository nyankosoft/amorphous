#ifndef  __NxPhysShapeImpl_H__
#define  __NxPhysShapeImpl_H__


#include "fwd.hpp"
#include "amorphous/Physics/ShapeImpl.hpp"
#include "NxPhysics.h"


namespace amorphous
{


namespace physics
{


class CNxBoxShapeImpl : public CBoxShapeImpl
{
	NxBoxShape *m_pBox;

public:

	CNxBoxShapeImpl() {}

	virtual ~CNxBoxShapeImpl() {}

	virtual void SetRadii( Vector3 vRadii ) {}//{ = vRadii; }
};


class CNxSphereShapeImpl : public CSphereShapeImpl
{
	NxSphereShape *m_pSphere;

public:

	CNxSphereShapeImpl() {}

	virtual ~CNxSphereShapeImpl() {}

	virtual void SetRadius( Scalar radius ) {}//{ m_Desc.radius = radius; }
};


class CNxCapsuleShapeImpl : public CCapsuleShapeImpl
{
	NxCapsuleShape *m_pCapsule;

public:

	CNxCapsuleShapeImpl() {}

	virtual ~CNxCapsuleShapeImpl() {}

	virtual void SetRadius( Scalar radius ) {}//{ m_Desc.radius = radius; }
	virtual void SetLength( Scalar length ) {}//{ m_Desc.height = length; };
};


class CNxTriangleMeshShapeImpl : public CCylinderShapeImpl
{
	NxCylinderShape *m_pCapsule;

public:

	CNxTriangleMeshShapeImpl() {}

	virtual ~CNxTriangleMeshShapeImpl() {}

	virtual void SetRadius( Scalar radius ) {}//{ m_Desc.radius = radius; }
	virtual void SetLength( Scalar length ) {}//{ m_Desc.height = length; };
};


} // namespace physics

} // namespace amorphous



#endif /* __NxPhysShapeImpl_H__ */
