#ifndef  __FG_EntityFactory_H__
#define  __FG_EntityFactory_H__


#include "Stage/fwd.h"
#include "Stage/EntityFactory.h"

#include "FG_AIAircraftEntity.h"


class CFG_EntityFactory : public CEntityFactory
{
	prealloc_pool<CFG_AIAircraftEntity> m_AIAircraftPool;

public:

	enum eFG_EntityFactoryParams
	{
		DEFAULT_MAX_NUM_AI_AIRCRAFT_ENTITIES = 256,
	};

public:

	CFG_EntityFactory();

	virtual ~CFG_EntityFactory() {}

	virtual CCopyEntity *CreateDerivedEntity( unsigned int entity_type_id );

	virtual void ReleaseDerivedEntity( CCopyEntity *pEntity );

	virtual void ReleaseAllDerivedEntities();
};


#endif		/*  __FG_EntityFactory_H__  */
