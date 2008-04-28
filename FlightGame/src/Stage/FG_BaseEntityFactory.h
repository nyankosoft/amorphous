#ifndef  __FG_BaseEntityFactory_H__
#define  __FG_BaseEntityFactory_H__


#include "Stage/BaseEntityFactory.h"


class CFG_BaseEntityFactory : public CBaseEntityFactory
{
public:

	CFG_BaseEntityFactory();

	virtual ~CFG_BaseEntityFactory();

	/// implemented by user to create instances of user defined base entity
	virtual CBaseEntity *CreateUserDefinedBaseEntity( const unsigned id );
};


#endif /* __FG_BaseEntityFactory_H__ */
