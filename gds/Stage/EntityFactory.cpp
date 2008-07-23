
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
	m_AlphaEntityPool.init( DEFAULT_MAX_NUM_ALPHA_ENTITIES );
}


CCopyEntity *CEntityFactory::CreateEntity( unsigned int entity_type_id )
{
	switch( entity_type_id )
	{
	case CCopyEntityTypeID::DEFAULT:
		return m_CopyEntityPool.get_new_object();
	case CCopyEntityTypeID::ALPHA_ENTITY:
		return m_AlphaEntityPool.get_new_object();
	default:
		return CreateDerivedEntity( entity_type_id );
	}

	return NULL;
}


void CEntityFactory::ReleaseEntity( CCopyEntity *pEntity )
{
	switch( pEntity->GetEntityTypeID() )
	{
	case CCopyEntityTypeID::DEFAULT:
		m_CopyEntityPool.release( pEntity );
		break;
	case CCopyEntityTypeID::ALPHA_ENTITY:
		m_AlphaEntityPool.release( dynamic_cast<CAlphaEntity *>(pEntity) );
		break;
	default:
		ReleaseDerivedEntity( pEntity );
		break;
	}
}


void CEntityFactory::ReleaseAllEntities()
{
	m_CopyEntityPool.release_all();

	ReleaseAllDerivedEntities();
}
