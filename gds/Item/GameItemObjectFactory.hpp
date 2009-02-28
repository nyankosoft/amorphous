#ifndef __GameItemObjectFactory_H__
#define __GameItemObjectFactory_H__


#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"
using namespace GameLib1::Serialization;


class CGameItem;


//===========================================================================
// CGameItemObjectFactory
//===========================================================================

class CGameItemObjectFactory : public IArchiveObjectFactory
{
public:

	CGameItem *CreateGameItem( const unsigned int id );

	IArchiveObjectBase *CreateObject( const unsigned int id );
};


#endif  /* __GameItemObjectFactory_H__   */
