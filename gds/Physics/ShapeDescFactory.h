#ifndef __PhysShapeDescFactory_H__
#define __PhysShapeDescFactory_H__


#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;

#include "ShapeDesc.h"
#include "BoxShapeDesc.h"
//#include "ShpereShapeDesc.h"
#include "CapsuleShapeDesc.h"
#include "TriangleMeshShapeDesc.h"


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
//	case PhysShape::Sphere:       return new CSphereShapeDesc();
	case PhysShape::Capsule:      return new CCapsuleShapeDesc();
	case PhysShape::TriangleMesh: return new CTriangleMeshShapeDesc();
//	case :	return new C*ShapeDesc();
	default: return NULL;
	}
}


} // namespace physics


#endif /* __PhysShapeDescFactory_H__ */
