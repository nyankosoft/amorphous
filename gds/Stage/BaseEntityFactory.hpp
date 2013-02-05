#ifndef  __BaseEntityFactory_H__
#define  __BaseEntityFactory_H__


#include "gds/Support/Serialization/ArchiveObjectFactory.hpp"
#include "fwd.hpp"


namespace amorphous
{

using namespace serialization;


/**
 used by BaseEntityManager to
 - serialize base entities
 - load base eitites from database
 - save base eitites to database
*/
class BaseEntityFactory : public IArchiveObjectFactory
{

public:

	BaseEntityFactory();

	virtual ~BaseEntityFactory();

	void Init();

	IArchiveObjectBase *CreateObject( const unsigned int id );

	/// create a predefined base entity
	BaseEntity *CreateBaseEntity( const unsigned int id );

	/// implemented by user to create instances of user defined base entity
	virtual BaseEntity *CreateUserDefinedBaseEntity( const unsigned id ) { return NULL; }
};


// ================================== template ==================================

/*
// header
//------------------------------------------------------------------------
#ifndef  __UserBaseEntityFactory_H__
#define  __UserBaseEntityFactory_H__


#include <gsf/Stage/BaseEntityFactory.hpp>

class CUserBaseEntityFactory : public BaseEntityFactory
{
public:

	CUserBaseEntityFactory();

	virtual ~CUserBaseEntityFactory();

	/// implemented by user to create instances of user defined base entity
	virtual BaseEntity *CreateUserDefinedBaseEntity( const unsigned id );
}


#endif // __UserBaseEntityFactory_H__

//------------------------------------------------------------------------


// implementation
//------------------------------------------------------------------------
#include "UserBaseEntityFactory.hpp"
#include "UserDefinedBaseEntity00.h"
#include "UserDefinedBaseEntity01.h"
#include "UserDefinedBaseEntity02.h"


BaseEntity *CUserBaseEntityFactory::CreateUserDefinedBaseEntity( const unsigned id )
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
} // namespace amorphous



#endif		/*  __BaseEntityFactory_H__  */
