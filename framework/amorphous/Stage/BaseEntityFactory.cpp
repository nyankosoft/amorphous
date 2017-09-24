#include "BaseEntityFactory.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"

#include "BaseEntity.hpp"
#include "BE_AreaSensor.hpp"
#include "BE_Blast.hpp"
#include "BE_Bullet.hpp"
#include "BE_Cloud.hpp"
#include "BE_Decal.hpp"
#include "BE_Door.hpp"
#include "BE_DoorController.hpp"
#include "BE_Enemy.hpp"
#include "BE_EventTrigger.hpp"
#include "BE_ExplosionSmoke.hpp"
#include "BE_Explosive.hpp"
#include "BE_FixedTurretPod.hpp"
#include "BE_Floater.hpp"
#include "BE_GeneralEntity.hpp"
#include "BE_HomingMissile.hpp"
#include "BE_LaserDot.hpp"
#include "BE_MuzzleFlash.hpp"
#include "BE_ParticleSet.hpp"
#include "BE_PhysicsBaseEntity.hpp"
#include "BE_Platform.hpp"
#include "BE_Player.hpp"
#include "BE_PlayerPseudoAircraft.hpp"      // 20061119
#include "BE_PlayerPseudoLeggedVehicle.hpp"
#include "BE_PlayerShip.hpp"
#include "BE_PointLight.hpp"
#include "BE_SmokeTrace.hpp"
#include "BE_SupplyItem.hpp"
#include "BE_TextureAnimation.hpp"
#include "BE_Turret.hpp"
#include "BE_EnemyAircraft.hpp"             // 20070106
#include "BE_DirectionalLight.hpp"          // 20070108
#include "BE_StaticParticleSet.hpp"         // 20070328
#include "BE_NozzleExhaust.hpp"             // 20070328
#include "BE_EnemyShip.hpp"                 // 20070520
#include "BE_StaticGeometry.hpp"            // 20070817
#include "BE_Skybox.hpp"                    // 20070817
#include "BE_CameraController.hpp"          // 20070909
#include "BE_ScriptedCamera.hpp"            // 20070909
#include "BE_IndividualEntity.hpp"          // 20070909
#include "BE_StaticLiquid.hpp"              // 20110418


namespace amorphous
{

using namespace std;


BaseEntityFactory::BaseEntityFactory()
{
}


BaseEntityFactory::~BaseEntityFactory()
{
}


void BaseEntityFactory::Init()
{
}


BaseEntity *BaseEntityFactory::CreateBaseEntity( const unsigned int id )
{
	BaseEntity *pBaseEntity = CreateUserDefinedBaseEntity( id );

	if( pBaseEntity )
		return pBaseEntity;

	switch(id)
	{
//		case :									return new BaseEntity;
		case BaseEntity::BE_AREASENSOR:		return new CBE_AreaSensor;
		case BaseEntity::BE_BLAST:				return new CBE_Blast;
		case BaseEntity::BE_BULLET:			return new CBE_Bullet;
		case BaseEntity::BE_CLOUD:				return new CBE_Cloud;
		case BaseEntity::BE_DECAL:				return new CBE_Decal;
		case BaseEntity::BE_DOOR:				return new CBE_Door;
		case BaseEntity::BE_DOORCONTROLLER:	return new CBE_DoorController;
		case BaseEntity::BE_ENEMY:				return new CBE_Enemy;
		case BaseEntity::BE_EVENTTRIGGER:		return new CBE_EventTrigger;
		case BaseEntity::BE_EXPLOSIONSMOKE:	return new CBE_ExplosionSmoke;
		case BaseEntity::BE_EXPLOSIVE:			return new CBE_Explosive;
		case BaseEntity::BE_FIXEDTURRETPOD:	return new CBE_FixedTurretPod;
		case BaseEntity::BE_FLOATER:			return new CBE_Floater;
		case BaseEntity::BE_GENERALENTITY:		return new CBE_GeneralEntity;
		case BaseEntity::BE_HOMINGMISSILE:		return new CBE_HomingMissile;
		case BaseEntity::BE_LASERDOT:			return new CBE_LaserDot;
		case BaseEntity::BE_MUZZLEFLASH:		return new CBE_MuzzleFlash;
		case BaseEntity::BE_PARTICLESET:		return new CBE_ParticleSet;
		case BaseEntity::BE_PHYSICSBASEENTITY:	return new CBE_PhysicsBaseEntity;
		case BaseEntity::BE_PLATFORM:			return new CBE_Platform;
		case BaseEntity::BE_PLAYER:			return new CBE_Player;
		case BaseEntity::BE_PLAYERPSEUDOAIRCRAFT:		return new CBE_PlayerPseudoAircraft;
		case BaseEntity::BE_PLAYERPSEUDOLEGGEDVEHICLE:	return new CBE_PlayerPseudoLeggedVehicle;
		case BaseEntity::BE_PLAYERSHIP:		return new CBE_PlayerShip;
		case BaseEntity::BE_POINTLIGHT:		return new CBE_PointLight;
		case BaseEntity::BE_SMOKETRACE:		return new CBE_SmokeTrace;
		case BaseEntity::BE_SUPPLYITEM:		return new CBE_SupplyItem;
		case BaseEntity::BE_TEXTUREANIMATION:	return new CBE_TextureAnimation;
		case BaseEntity::BE_TURRET:			return new CBE_Turret;
		case BaseEntity::BE_ENEMYAIRCRAFT:		return new CBE_EnemyAircraft;
		case BaseEntity::BE_DIRECTIONALLIGHT:	return new CBE_DirectionalLight;
		case BaseEntity::BE_STATICPARTICLESET:	return new CBE_StaticParticleSet;
		case BaseEntity::BE_NOZZLEEXHAUST:		return new CBE_NozzleExhaust;
		case BaseEntity::BE_ENEMYSHIP:			return new CBE_EnemyShip;
		case BaseEntity::BE_STATICGEOMETRY:	return new CBE_StaticGeometry;
		case BaseEntity::BE_SKYBOX:			return new CBE_Skybox;
		case BaseEntity::BE_CAMERACONTROLLER:	return new CBE_CameraController;
		case BaseEntity::BE_SCRIPTEDCAMERA:	return new CBE_ScriptedCamera;
		case BaseEntity::BE_INDIVIDUALENTITY:	return new CBE_IndividualEntity;
		case BaseEntity::BE_STATICLIQUID:		return new CBE_StaticLiquid;

		default:
			LOG_PRINT_ERROR( "invalid base entity id: " + to_string(id) );
			return NULL;
	}
}


IArchiveObjectBase *BaseEntityFactory::CreateObject( const unsigned int id )
{
	return CreateBaseEntity( id );
}




//=========================== draft ===========================
#if 0

BaseEntityManager::RegisterUserDefinedBaseEntityFactory( BaseEntityFactorySharedPtr pFactory )
{
	m_pBaseEntityFactory = pFactory;
}


void SomeAppInitFunctionInUserApp()
{
	BaseEntityManager().RegisterBaseEntityFactory( BaseEntityFactorySharedPtr( new CUserBaseEntityFactory() );
}


IArchiveObjectBase *CUserBaseEntityFactory::CreateUserDefinedBaseEnitity( const unsigned int id )
{
	switch(id)
	{
		// create an instance of a user defined base entity
		case BaseEntity::BE_PLAYERPSEUDOAIRCRAFT:		return new CBE_PlayerPseudoAircraft;
		case BaseEntity::BE_PLAYERPSEUDOLEGGEDVEHICLE:	return new CBE_PlayerPseudoLeggedVehicle;
		case BaseEntity::BE_ENEMYAIRCRAFT:				return new CBE_EnemyAircraft;
		case BaseEntity::BE_ENEMYSHIP:					return new CBE_EnemyShip;

		// return NULL if the id did not match with any of the user defined base entity
		default:
			return NULL;
	}
}


#endif


} // namespace amorphous
