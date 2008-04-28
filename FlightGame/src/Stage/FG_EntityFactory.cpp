

#include "FG_EntityFactory.h"
#include "FG_AIAircraftEntityDesc.h"


CFG_EntityFactory::CFG_EntityFactory()
{
	m_AIAircraftPool.init( DEFAULT_MAX_NUM_AI_AIRCRAFT_ENTITIES );
}



CCopyEntity *CFG_EntityFactory::CreateDerivedEntity( unsigned int entity_type_id )
{
	switch( entity_type_id )
	{
	case CFG_EntityTypeID::AI_AIRCRAFT:
		return m_AIAircraftPool.get();

	default:
		return NULL;
	}

	return NULL;
}


void CFG_EntityFactory::ReleaseDerivedEntity( CCopyEntity *pEntity )
{
	switch( pEntity->GetEntityTypeID() )
	{
	case CFG_EntityTypeID::AI_AIRCRAFT:
		return m_AIAircraftPool.release( (CFG_AIAircraftEntity *)pEntity ); // TODO: safer downcasting

	default:
		return;
	}
}


void CFG_EntityFactory::ReleaseAllDerivedEntities()
{
	m_AIAircraftPool.release_all();
}
