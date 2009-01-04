#include "BaseEntityFactory.h"
#include "Support/Log/DefaultLog.h"

#include "BaseEntity.h"
#include "BE_AreaSensor.h"
#include "BE_Blast.h"
#include "BE_Bullet.h"
#include "BE_Cloud.h"
#include "BE_Decal.h"
#include "BE_Door.h"
#include "BE_DoorController.h"
#include "BE_Enemy.h"
#include "BE_EventTrigger.h"
#include "BE_ExplosionSmoke.h"
#include "BE_Explosive.h"
#include "BE_FixedTurretPod.h"
#include "BE_Floater.h"
#include "BE_GeneralEntity.h"
#include "BE_HomingMissile.h"
#include "BE_LaserDot.h"
#include "BE_MuzzleFlash.h"
#include "BE_ParticleSet.h"
#include "BE_PhysicsBaseEntity.h"
#include "BE_Platform.h"
#include "BE_Player.h"
#include "BE_PlayerPseudoAircraft.h"		// 20061119
#include "BE_PlayerPseudoLeggedVehicle.h"
#include "BE_PlayerShip.h"
#include "BE_PointLight.h"
#include "BE_SmokeTrace.h"
#include "BE_SupplyItem.h"
#include "BE_TextureAnimation.h"
#include "BE_Turret.h"
#include "BE_EnemyAircraft.h"				// 20070106
#include "BE_DirectionalLight.h"			// 20070108
#include "BE_StaticParticleSet.h"			// 20070328
#include "BE_NozzleExhaust.h"				// 20070328
#include "BE_EnemyShip.h"					// 20070520
#include "BE_StaticGeometry.h"				// 20070817
#include "BE_Skybox.h"						// 20070817
#include "BE_CameraController.h"			// 20070909
#include "BE_ScriptedCamera.h"				// 20070909
#include "BE_IndividualEntity.h"				// 20070909

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
