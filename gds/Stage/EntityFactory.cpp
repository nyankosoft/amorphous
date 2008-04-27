
#include "EntityFactory.h"
#include "CopyEntity.h"
#include "CopyEntityDesc.h"

#include "Support/memory_helpers.h"

using namespace std;


CEntityFactory::CEntityFactory()
{
	Init();
}


CEntityFactory::~CEntityFactory()
{
}


void CEntityFactory::Init()
{
	m_CopyEntityPool.init( DEFAULT_MAX_NUM_ENTITIES );
}


CCopyEntity *CEntityFactory::CreateEntity( unsigned int entity_type_id )
{
	if( entity_type_id == CCopyEntityTypeID::DEFAULT )
		return m_CopyEntityPool.get();
	else
		return CreateDerivedEntity( entity_type_id );

	return NULL;
}


void CEntityFactory::ReleaseEntity( CCopyEntity *pEntity )
{
	if( pEntity->GetEntityTypeID() == CCopyEntityTypeID::DEFAULT )
		m_CopyEntityPool.release( pEntity );
	else
		ReleaseDerivedEntity( pEntity );
}
	

void CEntityFactory::ReleaseAllEntities()
{
	m_CopyEntityPool.release_all();

	ReleaseAllDerivedEntities();
}
