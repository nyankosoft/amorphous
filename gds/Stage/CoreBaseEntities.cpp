#include "BaseEntityFactory.hpp"
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
#include "BE_PlayerPseudoAircraft.hpp"		// 20061119
#include "BE_PlayerPseudoLeggedVehicle.hpp"
#include "BE_PlayerShip.hpp"
#include "BE_PointLight.hpp"
#include "BE_SmokeTrace.hpp"
#include "BE_SupplyItem.hpp"
#include "BE_TextureAnimation.hpp"
#include "BE_Turret.hpp"
#include "BE_EnemyAircraft.hpp"				// 20070106
#include "BE_DirectionalLight.hpp"			// 20070108
#include "BE_StaticParticleSet.hpp"			// 20070328
#include "BE_NozzleExhaust.hpp"				// 20070328
#include "BE_EnemyShip.hpp"					// 20070520
#include "BE_StaticGeometry.hpp"				// 20070817
#include "BE_Skybox.hpp"						// 20070817
#include "BE_CameraController.hpp"			// 20070909
#include "BE_ScriptedCamera.hpp"				// 20070909
#include "BE_IndividualEntity.hpp"				// 20070909



template<class CDerivedBaseEntity>
CDerivedBaseEntity *CreateBaseEntity( int id )
{
	CBaseEntityFactory factory;
	CBaseEntity *pBaseEntity = factory.CreateBaseEntity( id );
	return dynamic_cast<CDerivedBaseEntity *>( pBaseEntity );
}


void LoadCoreBaseEntities()
{
//	CBE_GeneralEntity *pBox  = new CBE_GeneralEntity;
//	CBE_Blast *pBlast        = new CBE_Blast;
//	CBE_Explosive *pImEx     = new CBE_Explosive;
//	CBE_ScriptedCamera *pCam = new CBE_ScriptedCamera;
//	CBE_CameraController *pCamController = new CBE_CameraController;
//	CBE_StaticGeometry *pBox = new CBE_StaticGeometry;

	CBE_StaticGeometry *pSG = CreateBaseEntity<CBE_StaticGeometry>( CBaseEntity::BE_STATICGEOMETRY );
//	pSG->m_strName = "StaticGeometry";

	CBE_Skybox *pSkybox = CreateBaseEntity<CBE_Skybox>( CBaseEntity::BE_SKYBOX );
//	pSkybox->m_strName = "skybox";
//	pSkybox->m_BoundingVolumeType = BVTYPE_AABB;


//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
}
