#include "EntityFactory.hpp"
#include "CopyEntity.hpp"
#include "CopyEntityDesc.hpp"
#include "LightEntity.hpp"
#include "AlphaEntity.hpp"
#include "SoundEntity.hpp"
#include "BE_ScriptedCamera.hpp"
#include "amorphous/Support/memory_helpers.hpp"


namespace amorphous
{

using namespace boost;


EntityFactory::EntityFactory()
{
	Init();
}


EntityFactory::~EntityFactory()
{
}


void EntityFactory::Init()
{
	m_CopyEntityPool.init( DEFAULT_MAX_NUM_ENTITIES );
	m_AlphaEntityPool.init( DEFAULT_MAX_NUM_ALPHA_ENTITIES );
	m_LightEntityPool.init( DEFAULT_MAX_NUM_LIGHT_ENTITIES );
	m_SoundEntityPool.init( DEFAULT_MAX_NUM_SOUND_ENTITIES );
	m_ScriptedCameraEntityPool.init( DEFAULT_MAX_NUM_SCRIPTED_CAMERA_ENTITIES );
}


shared_ptr<CCopyEntity> EntityFactory::CreateEntity( unsigned int entity_type_id )
{
	switch( entity_type_id )
	{
	case CCopyEntityTypeID::DEFAULT:
		return m_CopyEntityPool.get_new_object();
	case CCopyEntityTypeID::ALPHA_ENTITY:
		return m_AlphaEntityPool.get_new_object();
	case CCopyEntityTypeID::LIGHT_ENTITY:
	{
		shared_ptr<LightEntity> pLightEntity = m_LightEntityPool.get_new_object();
		pLightEntity->m_pLightEntitySelf = pLightEntity;
		return pLightEntity;
	}
	case CCopyEntityTypeID::SOUND_ENTITY:
		return m_SoundEntityPool.get_new_object();
	case CCopyEntityTypeID::SCRIPTED_CAMERA_ENTITY:
		return m_ScriptedCameraEntityPool.get_new_object();

	// if your derived entity does not require memory pool, you can
	// simply allocate them on heap with default 'new' and return
	// the shared pointer of it
	// e.g., because they are created before the stage starts and not likely
	// to impact performance from memory allocation during gameplay
	// EntityManager needs to set stock id to -2 after this to mark this entity as non-pooled object
//	case CCopyEntityTypeID::ANOTHER_ENTITY:
//		return shared_ptr<CCopyEntity>( new CAnotherEntity );
	default:
		return CreateDerivedEntity( entity_type_id );
	}

	return shared_ptr<CCopyEntity>();
}


void EntityFactory::ReleaseEntity( shared_ptr<CCopyEntity> pEntity )
{
	switch( pEntity->GetEntityTypeID() )
	{
	case CCopyEntityTypeID::DEFAULT:
		m_CopyEntityPool.release( pEntity );
		break;
	case CCopyEntityTypeID::ALPHA_ENTITY:
		m_AlphaEntityPool.release<CCopyEntity>( pEntity );
//		m_AlphaEntityPool.release( dynamic_pointer_cast<AlphaEntity,CCopyEntity>(pEntity) );
		break;
//	case CCopyEntityTypeID::LIGHT_ENTITY:
//		???
//		break;
	case CCopyEntityTypeID::SOUND_ENTITY:
		m_SoundEntityPool.release( pEntity );
		break;
	case CCopyEntityTypeID::SCRIPTED_CAMERA_ENTITY:
		m_ScriptedCameraEntityPool.release<CCopyEntity>( pEntity );
		break;
//	case CCopyEntityTypeID::ANOTHER_ENTITY:
//		pEntity.reset();
		break;
	default:
		ReleaseDerivedEntity( pEntity );
		break;
	}
}


void EntityFactory::ReleaseAllEntities()
{
	m_CopyEntityPool.release_all();

	m_AlphaEntityPool.release_all();

	m_SoundEntityPool.release_all();

	m_ScriptedCameraEntityPool.release_all();

	ReleaseAllDerivedEntities();
}


} // namespace amorphous
