#ifndef __NxPhysShapeDescFactory_H__
#define __NxPhysShapeDescFactory_H__


// ageia header
#include "NxPhysics.h"

// wrapper headers
#include "../fwd.h"
#include "../ShapeDesc.h"
#include "fwd.h"


namespace physics
{


class CNxPhysShapeDescFactory
{
public:

	CNxPhysShapeDescFactory() {}

	~CNxPhysShapeDescFactory() {}

	NxShapeDesc *CreateNxShapeDesc( CShapeDesc &src_desc );
};


} // namespace physics


#endif /* __NxPhysShapeDescFactory_H__ */
