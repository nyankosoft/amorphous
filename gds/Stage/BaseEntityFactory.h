#ifndef  __BaseEntityFactory_H__
#define  __BaseEntityFactory_H__


#include <string>

#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;

#include "fwd.h"


/**
 used by CBaseEntityManager to
 - serialize base entities
 - load base eitites from database
 - save base eitites to database
*/
class CBaseEntityFactory : public IArchiveObjectFactory
{

public:

	CBaseEntityFactory();

	virtual ~CBaseEntityFactory();

	void Init();

	IArchiveObjectBase *CreateObject( const unsigned int id );

	/// create a predefined base entity
	CBaseEntity *CreateBaseEntity( const unsigned int id );

	/// implemented by user to create instances of user defined base entity
	virtual CBaseEntity *CreateUserDefinedBaseEntity( const unsigned id ) { return NULL; }
};


// ================================== template ==================================

/*
// header
//------------------------------------------------------------------------
#ifndef  __UserBaseEntityFactory_H__
#define  __UserBaseEntityFactory_H__


#include <gsf/Stage/BaseEntityFactory.h>

class CUserBaseEntityFactory : public CBaseEntityFactory
{
public:

	CUserBaseEntityFactory();

	virtual ~CUserBaseEntityFactory();

	/// implemented by user to create instances of user defined base entity
	virtual CBaseEntity *CreateUserDefinedBaseEntity( const unsigned id );
}


#endif // __UserBaseEntityFactory_H__

//------------------------------------------------------------------------


// implementation
//------------------------------------------------------------------------
#include "UserBaseEntityFactory.h"
#include "UserDefinedBaseEntity00.h"
#include "UserDefinedBaseEntity01.h"
#include "UserDefinedBaseEntity02.h"


CBaseEntity *CUserBaseEntityFactory::CreateUserDefinedBaseEntity( const unsigned id )
{
	switch( id )
	{
	case USER_BASE_ENTITY_00:
		return new CUserDefinedBaseEntity00();

	case USER_BASE_ENTITY_01:
		return new CUserDefinedBaseEntity01();

	case USER_BASE_ENTITY_02:
		return new CUserDefinedBaseEntity02();

	default:
		return NULL;
	}

	return NULL;
}

//------------------------------------------------------------------------
*/

#endif		/*  __BaseEntityFactory_H__  */
