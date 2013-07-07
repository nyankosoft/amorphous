#ifndef __NxPhysShapeDescFactory_H__
#define __NxPhysShapeDescFactory_H__


// ageia header
#include "NxPhysics.h"

// wrapper headers
#include "amorphous/Physics/fwd.hpp"
#include "amorphous/Physics/ShapeDesc.hpp"
#include "fwd.hpp"


namespace amorphous
{


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

} // namespace amorphous



#endif /* __NxPhysShapeDescFactory_H__ */
