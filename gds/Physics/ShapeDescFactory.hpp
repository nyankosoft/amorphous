#ifndef __PhysShapeDescFactory_H__
#define __PhysShapeDescFactory_H__


#include "BoxShapeDesc.hpp"
#include "SphereShapeDesc.hpp"
#include "CylinderShapeDesc.hpp"
#include "CapsuleShapeDesc.hpp"
#include "ConvexShapeDesc.hpp"
#include "TriangleMeshShapeDesc.hpp"
#include "PlaneShapeDesc.hpp"

#include "../Support/Serialization/ArchiveObjectFactory.hpp"


namespace physics
{


//===========================================================================
// CShapeDescFactory
//===========================================================================

class CShapeDescFactory : public IArchiveObjectFactory
{
public:

	CShapeDescFactory() {}
	virtual ~CShapeDescFactory() {}

	inline virtual IArchiveObjectBase *CreateObject( const unsigned int id ) { return CreateShapeDesc( id ); }

	inline CShapeDesc *CreateShapeDesc( const unsigned int id );

/*
	virtual CBoxShapeDesc *CreateBoxShapeDesc();
	virtual CSphereShapeDesc *CreateSphereShapeDesc();
	virtual CCapsuleShapeDesc *CreateCapsuleShapeDesc();
	virtual CTriangleMeshShapeDesc *CreateTriangleMeshShapeDesc();
*/
};


inline CShapeDesc *CShapeDescFactory::CreateShapeDesc( const unsigned int id )
{
	switch(id)
	{
	case PhysShape::Box:          return new CBoxShapeDesc();
	case PhysShape::Sphere:       return new CSphereShapeDesc();
	case PhysShape::Cylinder:     return new CCylinderShapeDesc();
	case PhysShape::Capsule:      return new CCapsuleShapeDesc();
	case PhysShape::TriangleMesh: return new CTriangleMeshShapeDesc();
	case PhysShape::Plane:        return new CPlaneShapeDesc();
//	case :	return new C*ShapeDesc();
	default: return NULL;
	}
}


} // namespace physics


#endif /* __PhysShapeDescFactory_H__ */
