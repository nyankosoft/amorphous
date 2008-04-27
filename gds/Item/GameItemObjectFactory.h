
#ifndef __GAMEITEMOBJECTFACTORY_H__
#define __GAMEITEMOBJECTFACTORY_H__


#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


//===========================================================================
// CGameItemObjectFactory
//===========================================================================

class CGameItemObjectFactory : public IArchiveObjectFactory
{
public:
	IArchiveObjectBase *CreateObject( const unsigned int id );
};


#endif  /* __GAMEITEMOBJECTFACTORY_H__   */
