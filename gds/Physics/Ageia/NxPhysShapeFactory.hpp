#ifndef __NxPhysShapeFactory_H__
#define __NxPhysShapeFactory_H__

// wrapper headers
#include "fwd.hpp"
#include "NxPhysShape.hpp"

// ageia header
#include "NxPhysics.h"


namespace physics
{


class CNxPhysShapeFactory// : public CShapeDescImplFactory
{
public:

	inline CShape *CreateShape( NxShape *pNxShape );

	virtual CBoxShape *CreateBoxShape( NxBoxShape* pNxBoxShape ) { return new CNxPhysBoxShape( pNxBoxShape ); }

	virtual CSphereShape *CreateSphereShape( NxSphereShape* pNxSphereShape ) { return new CNxPhysSphereShape( pNxSphereShape ); }

	virtual CCapsuleShape *CreateCapsuleShape( NxCapsuleShape* pNxCapsuleShape ) { return new CNxPhysCapsuleShape( pNxCapsuleShape ); }

	virtual CTriangleMeshShape *CreateTriangleMeshShape( NxTriangleMeshShape* pNxTriMeshShape ) { return new CNxPhysTriangleMeshShape( pNxTriMeshShape ); }
};


inline CShape *CNxPhysShapeFactory::CreateShape( NxShape *pNxShape )
{
	switch( pNxShape->getType() )
	{
	case NX_SHAPE_BOX:     return CreateBoxShape( pNxShape->isBox() );
	case NX_SHAPE_SPHERE:  return CreateSphereShape( pNxShape->isSphere() );
	case NX_SHAPE_CAPSULE: return CreateCapsuleShape( pNxShape->isCapsule() );
	case NX_SHAPE_MESH:    return CreateTriangleMeshShape( pNxShape->isTriangleMesh() );
	default: return NULL;
	}
}


} // namespace physics


#endif /* __NxPhysShapeFactory_H__ */
