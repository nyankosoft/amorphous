#ifndef  __EntityFactory_H__
#define  __EntityFactory_H__


#include <vector>
#include <string>

#include "Support/shared_prealloc_pool.h"
#include "Stage/fwd.h"
#include "Stage/AlphaEntity.h"
#include "Stage/LightEntity.h"


class CEntityFactory
{
	shared_prealloc_pool<CCopyEntity> m_CopyEntityPool;

	shared_prealloc_pool<CAlphaEntity> m_AlphaEntityPool;

//	shared_prealloc_pool<CLightEntity> m_LightEntityPool;

public:

	enum eEntityFactoryParams
	{
		DEFAULT_MAX_NUM_ENTITIES = 1024,
		DEFAULT_MAX_NUM_ALPHA_ENTITIES = 64,
		DEFAULT_MAX_NUM_LIGHT_ENTITIES = 64,
	};

public:

	CEntityFactory();

	virtual ~CEntityFactory();

	/// allocate memory for entities
	/// must be called from a derived class if overridden
	virtual void Init();

	/// \param entity_type_id determines the type of entity to be created.
	/// By default, always returns CCopyEntity()
	virtual boost::shared_ptr<CCopyEntity> CreateEntity( unsigned int entity_type_id );

	/// must be implemented in a derived class to create user defined entity
	virtual boost::shared_ptr<CCopyEntity> CreateDerivedEntity( unsigned int entity_type_id ) { return boost::shared_ptr<CCopyEntity>(); }

	void ReleaseEntity( boost::shared_ptr<CCopyEntity> pEntity );

	/// must be implemented in a derived class if user defined entity is used
	/// - user needs to downcast 'pEntity' to release a derived entity
	virtual void ReleaseDerivedEntity( boost::shared_ptr<CCopyEntity> pEntity ) {}

	void ReleaseAllEntities();

	/// must be implemented in a derived class if user defined entity is used
	virtual void ReleaseAllDerivedEntities() {}

};


//
// template of a derived entity factory class
//

/*
#ifndef  __DerivedEntityFactory_H__
#define  __DerivedEntityFactory_H__

#include "EntityFactory.h"

class CDerivedEntityFactory : public CEntityFactory
{
	fixed_prealloc_pool<CDerivedEntity00> m_DerivedEntityPool00;
	fixed_prealloc_pool<CDerivedEntity01> m_DerivedEntityPool01;
	fixed_prealloc_pool<CDerivedEntity02> m_DerivedEntityPool02;

public:

	enum eDerivedEntityFactoryParams
	{
		DEFAULT_MAX_NUM_USER_DEFINED_ENTITIES = 256,
	};

public:

	CDerivedEntityFactory() {}

	virtual ~CDerivedEntityFactory() {}

	virtual CCopyEntity *CreateDerivedEntity( unsigned int entity_type_id );

	virtual void ReleaseDerivedEntity( CCopyEntity *pEntity );

	virtual void ReleaseAllDerivedEntities();
};

class CUserDerivedBaseEntityBase : public CBE_PhysicsBaseEntity
{
public:

	...

}

class CDerivedEntity : public CCopyEntity
{
public:

	enum eUserDerivedEntityID
	{
		DERIVED_ID0 = CBaseEntity::USER_DEFINED_ENTITY_ID_OFFSET,
		DERIVED_ID1,
		DERIVED_ID2,
		NUM_DERIVED
	};
};


class CUserDerivedBaseEntity0 : public CBE_PhysicsBaseEntity
{
public:

	...

	virtual unsigned int GetArchiveObjectID() { return DERIVED_ID0; }
}

class CUserDerivedBaseEntity1 : public CBE_PhysicsBaseEntity
{
public:

	...

	virtual unsigned int GetArchiveObjectID() { return DERIVED_ID1; }
}
#endif		/  __DerivedEntityFactory_H__  /

/// cpp

#include "DerivedEntityFactory.h"

CCopyEntity *CDerivedEntityFactory::CreateDerivedEntity( unsigned int entity_type_id )
{
	switch( entity_type_id )
	{
	case USER_DEFINED_ENTITY_00:
	case USER_DEFINED_ENTITY_01:
	case USER_DEFINED_ENTITY_02:
		return m_DerivedEntityPool.get();

	default:
		return NULL;
	}

	return NULL;
}

void CDerivedEntityFactory::ReleaseDerivedEntity( CCopyEntity *pEntity )
{
	switch( base_entity_type_id )
	{
	case USER_DEFINED_ENTITY_00:
	case USER_DEFINED_ENTITY_01:
	case USER_DEFINED_ENTITY_02:
		return m_DerivedEntityPool.release( (CDerivedEntity *)pEntity );

	default:
		return;
	}
}

void CDerivedEntityFactory::ReleaseAllDerivedEntities()
{
	CEntityFactory::ReleaseAllEntities();

	m_DerivedEntityPool.release_all();
}

*/


#endif		/*  __EntityFactory_H__  */
