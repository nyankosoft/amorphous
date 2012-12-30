#include "BaseEntityFactory.hpp"
#include "Support/Log/DefaultLog.hpp"

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


CBaseEntityFactory::CBaseEntityFactory()
{
}


CBaseEntityFactory::~CBaseEntityFactory()
{
}


void CBaseEntityFactory::Init()
{
}


CBaseEntity *CBaseEntityFactory::CreateBaseEntity( const unsigned int id )
{
	CBaseEntity *pBaseEntity = CreateUserDefinedBaseEntity( id );

	if( pBaseEntity )
		return pBaseEntity;

	switch(id)
	{
//		case :									return new CBaseEntity;
		case CBaseEntity::BE_AREASENSOR:		return new CBE_AreaSensor;
		case CBaseEntity::BE_BLAST:				return new CBE_Blast;
		case CBaseEntity::BE_BULLET:			return new CBE_Bullet;
		case CBaseEntity::BE_CLOUD:				return new CBE_Cloud;
		case CBaseEntity::BE_DECAL:				return new CBE_Decal;
		case CBaseEntity::BE_DOOR:				return new CBE_Door;
		case CBaseEntity::BE_DOORCONTROLLER:	return new CBE_DoorController;
		case CBaseEntity::BE_ENEMY:				return new CBE_Enemy;
		case CBaseEntity::BE_EVENTTRIGGER:		return new CBE_EventTrigger;
		case CBaseEntity::BE_EXPLOSIONSMOKE:	return new CBE_ExplosionSmoke;
		case CBaseEntity::BE_EXPLOSIVE:			return new CBE_Explosive;
		case CBaseEntity::BE_FIXEDTURRETPOD:	return new CBE_FixedTurretPod;
		case CBaseEntity::BE_FLOATER:			return new CBE_Floater;
		case CBaseEntity::BE_GENERALENTITY:		return new CBE_GeneralEntity;
		case CBaseEntity::BE_HOMINGMISSILE:		return new CBE_HomingMissile;
		case CBaseEntity::BE_LASERDOT:			return new CBE_LaserDot;
		case CBaseEntity::BE_MUZZLEFLASH:		return new CBE_MuzzleFlash;
		case CBaseEntity::BE_PARTICLESET:		return new CBE_ParticleSet;
		case CBaseEntity::BE_PHYSICSBASEENTITY:	return new CBE_PhysicsBaseEntity;
		case CBaseEntity::BE_PLATFORM:			return new CBE_Platform;
		case CBaseEntity::BE_PLAYER:			return new CBE_Player;
		case CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT:		return new CBE_PlayerPseudoAircraft;
		case CBaseEntity::BE_PLAYERPSEUDOLEGGEDVEHICLE:	return new CBE_PlayerPseudoLeggedVehicle;
		case CBaseEntity::BE_PLAYERSHIP:		return new CBE_PlayerShip;
		case CBaseEntity::BE_POINTLIGHT:		return new CBE_PointLight;
		case CBaseEntity::BE_SMOKETRACE:		return new CBE_SmokeTrace;
		case CBaseEntity::BE_SUPPLYITEM:		return new CBE_SupplyItem;
		case CBaseEntity::BE_TEXTUREANIMATION:	return new CBE_TextureAnimation;
		case CBaseEntity::BE_TURRET:			return new CBE_Turret;
		case CBaseEntity::BE_ENEMYAIRCRAFT:		return new CBE_EnemyAircraft;
		case CBaseEntity::BE_DIRECTIONALLIGHT:	return new CBE_DirectionalLight;
		case CBaseEntity::BE_STATICPARTICLESET:	return new CBE_StaticParticleSet;
		case CBaseEntity::BE_NOZZLEEXHAUST:		return new CBE_NozzleExhaust;
		case CBaseEntity::BE_ENEMYSHIP:			return new CBE_EnemyShip;
		case CBaseEntity::BE_STATICGEOMETRY:	return new CBE_StaticGeometry;
		case CBaseEntity::BE_SKYBOX:			return new CBE_Skybox;
		case CBaseEntity::BE_CAMERACONTROLLER:	return new CBE_CameraController;
		case CBaseEntity::BE_SCRIPTEDCAMERA:	return new CBE_ScriptedCamera;
		case CBaseEntity::BE_INDIVIDUALENTITY:	return new CBE_IndividualEntity;
		case CBaseEntity::BE_STATICLIQUID:		return new CBE_StaticLiquid;

		default:
			LOG_PRINT_ERROR( "invalid base entity id: " + to_string(id) );
			return NULL;
	}
}


IArchiveObjectBase *CBaseEntityFactory::CreateObject( const unsigned int id )
{
	return CreateBaseEntity( id );
}




//=========================== draft ===========================
#if 0

CBaseEntityManager::RegisterUserDefinedBaseEntityFactory( CBaseEntityFactorySharedPtr pFactory )
{
	m_pBaseEntityFactory = pFactory;
}


void SomeAppInitFunctionInUserApp()
{
	BaseEntityManager().RegisterBaseEntityFactory( CBaseEntityFactorySharedPtr( new CUserBaseEntityFactory() );
}


IArchiveObjectBase *CUserBaseEntityFactory::CreateUserDefinedBaseEnitity( const unsigned int id )
{
	switch(id)
	{
		// create an instance of a user defined base entity
		case CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT:		return new CBE_PlayerPseudoAircraft;
		case CBaseEntity::BE_PLAYERPSEUDOLEGGEDVEHICLE:	return new CBE_PlayerPseudoLeggedVehicle;
		case CBaseEntity::BE_ENEMYAIRCRAFT:				return new CBE_EnemyAircraft;
		case CBaseEntity::BE_ENEMYSHIP:					return new CBE_EnemyShip;

		// return NULL if the id did not match with any of the user defined base entity
		default:
			return NULL;
	}
}


#endif


} // namespace amorphous
