
#ifndef __CJL_PhysicsActorDescFACTORY_H__
#define __CJL_PhysicsActorDescFACTORY_H__


#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/ArchiveObjectFactory.hpp"


namespace amorphous
{
using namespace GameLib1::Serialization;


//===========================================================================
// CJL_PhysicsActorDescFactory
//===========================================================================

class CJL_PhysicsActorDescFactory : public IArchiveObjectFactory
{
public:
	IArchiveObjectBase *CreateObject( const unsigned int id );
};

} // amorphous



#endif  /* __CJL_PhysicsActorDescFACTORY_H__   */