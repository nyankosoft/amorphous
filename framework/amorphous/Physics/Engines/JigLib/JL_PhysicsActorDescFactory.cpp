
#include "CJL_PhysicsActorDescFactory.hpp"


#include "CJL_PhysicsActorDesc.hpp"


namespace amorphous
{



IArchiveObjectBase *CJL_PhysicsActorDescFactory::CreateObject( const unsigned int id )
{
	switch( id )
	{
	case CJL_PHYSICSACTORDESC:	return new CJL_PhysicsActorDesc;

	default:	return NULL;
	}
}

} // amorphous
