
#include "CJL_PhysicsActorDescFactory.h"


#include "CJL_PhysicsActorDesc.h"



IArchiveObjectBase *CJL_PhysicsActorDescFactory::CreateObject( const unsigned int id )
{
	switch( id )
	{
	case CJL_PHYSICSACTORDESC:	return new CJL_PhysicsActorDesc;

	default:	return NULL;
	}
}