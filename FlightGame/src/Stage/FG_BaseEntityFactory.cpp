#include "FG_BaseEntityFactory.h"
#include "Stage/BE_PlayerPseudoAircraft.h"
#include "FG_AIAircraftBaseEntity.h"
#include "FG_AIAircraftEntity.h"
//#include "UserDefinedBaseEntity02.h"


CFG_BaseEntityFactory::CFG_BaseEntityFactory()
{
}


CFG_BaseEntityFactory::~CFG_BaseEntityFactory()
{
}


CBaseEntity *CFG_BaseEntityFactory::CreateUserDefinedBaseEntity( const unsigned id )
{
	switch( id )
	{
	case CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT:
		return new CBE_PlayerPseudoAircraft();

	case CFG_BaseEntityID::BE_AIAIRCRAFTBASEENTITY:
		return new CFG_AIAircraftBaseEntity();

//	case USER_BASE_ENTITY_02:
//		return new CUserDefinedBaseEntity02();

	default:
		return NULL;
	}

	return NULL;
}
