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

//	inline CShapeImpl *CreateShapeImpl( NxShape *pNxShape );

	virtual CShapeImpl *CreateBoxShapeImpl( NxBoxShape* pNxBoxShape ) { return new CNxPhysBoxShape( pNxBoxShape ); }

	virtual CShapeImpl *CreateSphereShapeImpl( NxSphereShape* pNxSphereShape ) { return new CNxPhysSphereShape( pNxSphereShape ); }

	virtual CShapeImpl *CreateCapsuleShapeImpl( NxCapsuleShape* pNxCapsuleShape ) { return new CNxPhysCapsuleShape( pNxCapsuleShape ); }

	virtual CShapeImpl *CreateTriangleMeshShapeImpl( NxTriangleMeshShape* pNxTriMeshShape ) { return new CNxPhysTriangleMeshShape( pNxTriMeshShape ); }

	virtual CShapeImpl *CreateConvexShapeImpl( NxConvexShape* pNxConvexShape ) { return new CNxPhysConvexShape( pNxConvexShape ); }
};


inline CShape *CNxPhysShapeFactory::CreateShape( NxShape *pNxShape )
{
	switch( pNxShape->getType() )
	{
	case NX_SHAPE_BOX:     return new CBoxShape( CreateBoxShapeImpl( pNxShape->isBox() ) );
	case NX_SHAPE_SPHERE:  return new CSphereShape( CreateSphereShapeImpl( pNxShape->isSphere() ) );
	case NX_SHAPE_CAPSULE: return new CCapsuleShape( CreateCapsuleShapeImpl( pNxShape->isCapsule() ) );
	case NX_SHAPE_MESH:    return new CTriangleMeshShape( CreateTriangleMeshShapeImpl( pNxShape->isTriangleMesh() ) );
	case NX_SHAPE_CONVEX:  return new CConvexShape( CreateConvexShapeImpl( pNxShape->isConvexMesh() ) );
	default: return NULL;
	}
}


} // namespace physics


#endif /* __NxPhysShapeFactory_H__ */
