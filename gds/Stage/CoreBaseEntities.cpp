#include "CoreBaseEntitiesLoader.hpp"
#include "BaseEntityFactory.hpp"
#include "BaseEntity.hpp"
#include "BE_AreaSensor.hpp"
#include "BE_Blast.hpp"
#include "BE_Bullet.hpp"
#include "BE_Cloud.hpp"
#include "BE_Decal.hpp"
#include "BE_Door.hpp"
#include "BE_DoorController.hpp"
//#include "BE_Enemy.hpp"
#include "BE_EventTrigger.hpp"
#include "BE_ExplosionSmoke.hpp"
#include "BE_Explosive.hpp"
#include "BE_FixedTurretPod.hpp"
//#include "BE_Floater.hpp"
#include "BE_GeneralEntity.hpp"
#include "BE_HomingMissile.hpp"
#include "BE_LaserDot.hpp"
#include "BE_MuzzleFlash.hpp"
#include "BE_ParticleSet.hpp"
#include "BE_PhysicsBaseEntity.hpp"
#include "BE_Platform.hpp"
#include "BE_Player.hpp"


namespace amorphous
{
//#include "BE_PlayerPseudoAircraft.hpp"		// 20061119
//#include "BE_PlayerPseudoLeggedVehicle.hpp"
//#include "BE_PlayerShip.hpp"
#include "BE_PointLight.hpp"
#include "BE_SmokeTrace.hpp"
#include "BE_SupplyItem.hpp"
#include "BE_TextureAnimation.hpp"
#include "BE_Turret.hpp"
//#include "BE_EnemyAircraft.hpp"    // 20070106
#include "BE_DirectionalLight.hpp"   // 20070108
#include "BE_StaticParticleSet.hpp"  // 20070328
#include "BE_NozzleExhaust.hpp"      // 20070328
//#include "BE_EnemyShip.hpp"        // 20070520
#include "BE_StaticGeometry.hpp"     // 20070817
#include "BE_Skybox.hpp"             // 20070817
#include "BE_CameraController.hpp"   // 20070909
#include "BE_ScriptedCamera.hpp"     // 20070909
#include "BE_IndividualEntity.hpp"   // 20070909
#include "BE_StaticLiquid.hpp"       // 20110418




template<class CDerivedBaseEntity>
CDerivedBaseEntity *CreateBaseEntity( int id )
{
	CBaseEntityFactory factory;
	CBaseEntity *pBaseEntity = factory.CreateBaseEntity( id );
	return dynamic_cast<CDerivedBaseEntity *>( pBaseEntity );
}


void CCoreBaseEntitiesLoader::AddBlast( const char *name, float base_damage, float max_blast_radius, float blast_duration, float impulse, std::vector<CBaseEntity *>& pBaseEntities )
{
	CBE_Blast *pBlast = CreateBaseEntity<CBE_Blast>( CBaseEntity::BE_BLAST );
	pBlast->m_strName         = name;
	pBlast->m_fBaseDamage     = base_damage;
	pBlast->m_fMaxBlastRadius = max_blast_radius;
	pBlast->m_fBlastDuration  = blast_duration;
	pBlast->m_fImpulse        = impulse;
	pBaseEntities.push_back( pBlast );
}


void CCoreBaseEntitiesLoader::AddExplosion( const char *name, const char *blast_name, float anim_time_offset, std::vector<CBaseEntity *>& pBaseEntities )
{
	CBE_Explosive *pExplosion = CreateBaseEntity<CBE_Explosive>( CBaseEntity::BE_EXPLOSIVE );
	pExplosion->m_strName         = name;
	pExplosion->m_Blast.SetBaseEntityName( blast_name );
	pExplosion->m_fExplosionAnimTimeOffset = anim_time_offset;
	pBaseEntities.push_back( pExplosion );
}


void CCoreBaseEntitiesLoader::AddDefaultLight( CBE_Light *pLight, const char *name, std::vector<CBaseEntity *>& pBaseEntities )
{
	pLight->m_strName = name;
	pLight->m_DefaultDesc.aColor[0] = SFloatRGBAColor( 1.00f, 1.00f, 1.00f, 1.00f );
	pLight->m_DefaultDesc.aColor[1] = SFloatRGBAColor( 0.25f, 0.25f, 0.25f, 1.00f );
	pLight->m_aabb.vMin = -Vector3(1,1,1) * 0.05f;
	pLight->m_aabb.vMax =  Vector3(1,1,1) * 0.05f;
	pLight->m_BoundingVolumeType = BVTYPE_DOT;
	pBaseEntities.push_back( pLight );
}


void CCoreBaseEntitiesLoader::AddPhysicsBaseEntity( const char *name, std::vector<CBaseEntity *>& pBaseEntities )
{
	CBE_GeneralEntity *pBaseEntity = CreateBaseEntity<CBE_GeneralEntity>( CBaseEntity::BE_GENERALENTITY );
	pBaseEntity->m_strName         = name;
	// The AABB is overwritten by the dimension of the box mesh specified by the user
	pBaseEntity->m_aabb.vMin = Vector3(1,1,1) * -0.001f;
	pBaseEntity->m_aabb.vMax = Vector3(1,1,1) *  0.001f;
	pBaseEntity->RaiseEntityFlag( BETYPE_LIGHTING | BETYPE_SHADOW_CASTER | BETYPE_SHADOW_RECEIVER );

	// Shader techniques are not set up here.
	// By default, generic shader is used for the mesh, and the shader technique "Default" is set.
//	pBaseEntity->m_MeshProperty.m_ShaderTechnique

	pBaseEntities.push_back( pBaseEntity );
}


void CCoreBaseEntitiesLoader::LoadCoreBaseEntities( std::vector<CBaseEntity *>& pBaseEntities )
{
//	CBE_GeneralEntity *pBox  = new CBE_GeneralEntity;
//	CBE_Explosive *pImEx     = new CBE_Explosive;

	CBE_CameraController *pCutsceneCamController = CreateBaseEntity<CBE_CameraController>( CBaseEntity::BE_CAMERACONTROLLER );
	pCutsceneCamController->m_strName = "CutsceneCameraController";
	pCutsceneCamController->m_bUseCutsceneInputHandler = true;
	pBaseEntities.push_back( pCutsceneCamController );

	CBE_CameraController *pCamController = CreateBaseEntity<CBE_CameraController>( CBaseEntity::BE_CAMERACONTROLLER );
	pCamController->m_strName = "CameraController";
	pCamController->m_bUseCutsceneInputHandler = false;
	pBaseEntities.push_back( pCamController );

	CBE_ScriptedCamera *pCam = CreateBaseEntity<CBE_ScriptedCamera>( CBaseEntity::BE_SCRIPTEDCAMERA );
	pCam->m_strName = "ScriptedCamera";
	pBaseEntities.push_back( pCam );

	CBE_EventTrigger *pET = CreateBaseEntity<CBE_EventTrigger>( CBaseEntity::BE_EVENTTRIGGER );
	pET->m_strName = "EventBox";
	pET->m_BoundingVolumeType = BVTYPE_AABB;
	pBaseEntities.push_back( pET );

	CBE_StaticGeometry *pSG = CreateBaseEntity<CBE_StaticGeometry>( CBaseEntity::BE_STATICGEOMETRY );
	pSG->m_strName = "StaticGeometry";
	pBaseEntities.push_back( pSG );

	CBE_Skybox *pSkybox = CreateBaseEntity<CBE_Skybox>( CBaseEntity::BE_SKYBOX );
	pSkybox->m_strName = "skybox";
	pSkybox->m_BoundingVolumeType = BVTYPE_AABB;
	pBaseEntities.push_back( pSkybox );

	CBE_Bullet *pBullet = CreateBaseEntity<CBE_Bullet>( CBaseEntity::BE_BULLET );
	pBullet->m_strName = "__DefaultBullet__";
	pBullet->m_BoundingVolumeType = BVTYPE_DOT;
	pBaseEntities.push_back( pBullet );

	CBE_IndividualEntity *pAlphaEntityBase = CreateBaseEntity<CBE_IndividualEntity>( CBaseEntity::BE_INDIVIDUALENTITY );
	pAlphaEntityBase->m_strName = "AlphaEntityBase";
	pAlphaEntityBase->m_BoundingVolumeType = BVTYPE_AABB;
	pAlphaEntityBase->m_bNoClip = true;
	pAlphaEntityBase->RaiseEntityFlag( BETYPE_SUPPORT_TRANSPARENT_PARTS | BETYPE_SHADOW_CASTER | BETYPE_SHADOW_RECEIVER );
	pAlphaEntityBase->ClearEntityFlag( BETYPE_RIGIDBODY );
	pAlphaEntityBase->m_MeshProperty.m_ShaderTechnique.resize(1,1);
	pAlphaEntityBase->m_MeshProperty.m_ShaderTechnique(0,0).SetTechniqueName( "SingleHSPL_Specular_CTS" );
	pBaseEntities.push_back( pAlphaEntityBase );

	CBE_IndividualEntity *pForItemEntity = CreateBaseEntity<CBE_IndividualEntity>( CBaseEntity::BE_INDIVIDUALENTITY );
	pForItemEntity->m_strName = "__ForItemEntity__";
	pForItemEntity->m_BoundingVolumeType = BVTYPE_AABB;
	pBaseEntities.push_back( pForItemEntity );

	CBE_StaticLiquid *pStaticLiquid = CreateBaseEntity<CBE_StaticLiquid>( CBaseEntity::BE_STATICLIQUID );
	pStaticLiquid->m_strName = "__StaticLiquidWater__";
	pStaticLiquid->m_BoundingVolumeType = BVTYPE_AABB;
	pBaseEntities.push_back( pStaticLiquid );

	// Add preset base entities for games

	AddBlast( "t.blast",    10.0f,    1.0f, 0.05f, 10.0f, pBaseEntities );
	AddBlast( "s.blast",   320.0f,    6.0f, 0.05f, 10.0f, pBaseEntities );
	AddBlast( "m.blast",   450.0f,   12.0f, 0.16f, 10.0f, pBaseEntities );
	AddBlast( "l.blast", 10000.0f,   80.0f, 0.05f, 10.0f, pBaseEntities );
	AddBlast( "h.blast", 10000.0f,  300.0f, 0.05f, 10.0f, pBaseEntities );

	AddExplosion( "t.imex", "t.blast", -0.02f, pBaseEntities );
	AddExplosion( "s.imex", "s.blast", -0.02f, pBaseEntities );
	AddExplosion( "m.imex", "m.blast", -0.08f, pBaseEntities );
	AddExplosion( "l.imex", "l.blast", -0.08f, pBaseEntities );
	AddExplosion( "h.imex", "h.blast", -0.08f, pBaseEntities );

	// Add lights

	CBE_DirectionalLight *pDLight   = CreateBaseEntity<CBE_DirectionalLight>( CBaseEntity::BE_DIRECTIONALLIGHT );
	CBE_PointLight *pPLight         = CreateBaseEntity<CBE_PointLight>( CBaseEntity::BE_POINTLIGHT );
	CBE_DirectionalLight *pHSDLight = CreateBaseEntity<CBE_DirectionalLight>( CBaseEntity::BE_DIRECTIONALLIGHT );
	CBE_PointLight *pHSPLight       = CreateBaseEntity<CBE_PointLight>( CBaseEntity::BE_POINTLIGHT );
	AddDefaultLight( pDLight,   "__DirectionalLight__",            pBaseEntities );
	AddDefaultLight( pPLight,   "__PointLight__",                  pBaseEntities );
	AddDefaultLight( pHSDLight, "__HemisphericDirectionalLight__", pBaseEntities );
	AddDefaultLight( pHSPLight, "__HemisphericPointLight__",       pBaseEntities );

	AddPhysicsBaseEntity( "__BoxFromDimension__",     pBaseEntities );
	AddPhysicsBaseEntity( "__BoxFromMesh__",          pBaseEntities );
	AddPhysicsBaseEntity( "__TriangleMeshFromMesh__", pBaseEntities );

//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
//	BE_GeneralEntity *pBox = new BE_GeneralEntity;
}


} // namespace amorphous
