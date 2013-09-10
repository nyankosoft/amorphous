#include "CoreBaseEntitiesLoader.hpp"
#include "BaseEntityFactory.hpp"
#include "BE_AreaSensor.hpp"
#include "BE_Blast.hpp"
#include "BE_Bullet.hpp"
#include "BE_Cloud.hpp"
#include "BE_Decal.hpp"
#include "BE_Door.hpp"
#include "BE_DoorController.hpp"
#include "BE_EventTrigger.hpp"
#include "BE_ExplosionSmoke.hpp"
#include "BE_Explosive.hpp"
#include "BE_GeneralEntity.hpp"
#include "BE_HomingMissile.hpp"
#include "BE_LaserDot.hpp"
#include "BE_MuzzleFlash.hpp"
#include "BE_ParticleSet.hpp"
#include "BE_PhysicsBaseEntity.hpp"
#include "BE_PointLight.hpp"
#include "BE_SmokeTrace.hpp"
#include "BE_SupplyItem.hpp"
#include "BE_TextureAnimation.hpp"
#include "BE_DirectionalLight.hpp"
#include "BE_StaticParticleSet.hpp"
#include "BE_NozzleExhaust.hpp"
#include "BE_StaticGeometry.hpp"
#include "BE_Skybox.hpp"
#include "BE_CameraController.hpp"
#include "BE_ScriptedCamera.hpp"
#include "BE_IndividualEntity.hpp"
#include "BE_StaticLiquid.hpp"


namespace amorphous
{


template<class CDerivedBaseEntity>
CDerivedBaseEntity *CreateBaseEntity( int id )
{
	BaseEntityFactory factory;
	BaseEntity *pBaseEntity = factory.CreateBaseEntity( id );
	return dynamic_cast<CDerivedBaseEntity *>( pBaseEntity );
}


void CoreBaseEntitiesLoader::AddBlast( const char *name, float base_damage, float max_blast_radius, float blast_duration, float impulse, std::vector<BaseEntity *>& pBaseEntities )
{
	CBE_Blast *pBlast = CreateBaseEntity<CBE_Blast>( BaseEntity::BE_BLAST );
	pBlast->m_strName         = name;
	pBlast->m_fBaseDamage     = base_damage;
	pBlast->m_fMaxBlastRadius = max_blast_radius;
	pBlast->m_fBlastDuration  = blast_duration;
	pBlast->m_fImpulse        = impulse;
	pBaseEntities.push_back( pBlast );
}


void CoreBaseEntitiesLoader::AddExplosion( const char *name, const char *blast_name, float anim_time_offset, std::vector<BaseEntity *>& pBaseEntities )
{
	CBE_Explosive *pExplosion = CreateBaseEntity<CBE_Explosive>( BaseEntity::BE_EXPLOSIVE );
	pExplosion->m_strName         = name;
	pExplosion->m_Blast.SetBaseEntityName( blast_name );
	pExplosion->m_fExplosionAnimTimeOffset = anim_time_offset;
	pBaseEntities.push_back( pExplosion );
}


void CoreBaseEntitiesLoader::AddDefaultLight( CBE_Light *pLight, const char *name, std::vector<BaseEntity *>& pBaseEntities )
{
	pLight->m_strName = name;
	pLight->m_DefaultDesc.aColor[0] = SFloatRGBAColor( 1.00f, 1.00f, 1.00f, 1.00f );
	pLight->m_DefaultDesc.aColor[1] = SFloatRGBAColor( 0.25f, 0.25f, 0.25f, 1.00f );
	pLight->m_aabb.vMin = -Vector3(1,1,1) * 0.05f;
	pLight->m_aabb.vMax =  Vector3(1,1,1) * 0.05f;
	pLight->m_BoundingVolumeType = BVTYPE_DOT;
	pBaseEntities.push_back( pLight );
}


void CoreBaseEntitiesLoader::AddPhysicsBaseEntity( const char *name, std::vector<BaseEntity *>& pBaseEntities )
{
	CBE_GeneralEntity *pBaseEntity = CreateBaseEntity<CBE_GeneralEntity>( BaseEntity::BE_GENERALENTITY );
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


void CoreBaseEntitiesLoader::LoadCoreBaseEntities( std::vector<BaseEntity *>& pBaseEntities )
{
//	CBE_GeneralEntity *pBox  = new CBE_GeneralEntity;
//	CBE_Explosive *pImEx     = new CBE_Explosive;

	CBE_CameraController *pCutsceneCamController = CreateBaseEntity<CBE_CameraController>( BaseEntity::BE_CAMERACONTROLLER );
	pCutsceneCamController->m_strName = "CutsceneCameraController";
	pCutsceneCamController->m_bUseCutsceneInputHandler = true;
	pBaseEntities.push_back( pCutsceneCamController );

	CBE_CameraController *pCamController = CreateBaseEntity<CBE_CameraController>( BaseEntity::BE_CAMERACONTROLLER );
	pCamController->m_strName = "CameraController";
	pCamController->m_bUseCutsceneInputHandler = false;
	pBaseEntities.push_back( pCamController );

	CBE_ScriptedCamera *pCam = CreateBaseEntity<CBE_ScriptedCamera>( BaseEntity::BE_SCRIPTEDCAMERA );
	pCam->m_strName = "ScriptedCamera";
	pBaseEntities.push_back( pCam );

	CBE_EventTrigger *pET = CreateBaseEntity<CBE_EventTrigger>( BaseEntity::BE_EVENTTRIGGER );
	pET->m_strName = "EventBox";
	pET->m_BoundingVolumeType = BVTYPE_AABB;
	pBaseEntities.push_back( pET );

	CBE_StaticGeometry *pSG = CreateBaseEntity<CBE_StaticGeometry>( BaseEntity::BE_STATICGEOMETRY );
	pSG->m_strName = "StaticGeometry";
	pBaseEntities.push_back( pSG );

	CBE_Skybox *pSkybox = CreateBaseEntity<CBE_Skybox>( BaseEntity::BE_SKYBOX );
	pSkybox->m_strName = "skybox";
	pSkybox->m_BoundingVolumeType = BVTYPE_AABB;
	pBaseEntities.push_back( pSkybox );

	CBE_Bullet *pBullet = CreateBaseEntity<CBE_Bullet>( BaseEntity::BE_BULLET );
	pBullet->m_strName = "__DefaultBullet__";
	pBullet->m_BoundingVolumeType = BVTYPE_DOT;
	pBaseEntities.push_back( pBullet );

	CBE_IndividualEntity *pAlphaEntityBase = CreateBaseEntity<CBE_IndividualEntity>( BaseEntity::BE_INDIVIDUALENTITY );
	pAlphaEntityBase->m_strName = "AlphaEntityBase";
	pAlphaEntityBase->m_BoundingVolumeType = BVTYPE_AABB;
	pAlphaEntityBase->m_bNoClip = true;
	pAlphaEntityBase->RaiseEntityFlag( BETYPE_SUPPORT_TRANSPARENT_PARTS | BETYPE_SHADOW_CASTER | BETYPE_SHADOW_RECEIVER );
	pAlphaEntityBase->ClearEntityFlag( BETYPE_RIGIDBODY );
	pAlphaEntityBase->m_MeshProperty.m_ShaderTechnique.resize(1,1);
	pAlphaEntityBase->m_MeshProperty.m_ShaderTechnique(0,0).SetTechniqueName( "SingleHSPL_Specular_CTS" );
	pBaseEntities.push_back( pAlphaEntityBase );

	CBE_IndividualEntity *pForItemEntity = CreateBaseEntity<CBE_IndividualEntity>( BaseEntity::BE_INDIVIDUALENTITY );
	pForItemEntity->m_strName = "__ForItemEntity__";
	pForItemEntity->m_BoundingVolumeType = BVTYPE_AABB;
	pBaseEntities.push_back( pForItemEntity );

	CBE_StaticLiquid *pStaticLiquid = CreateBaseEntity<CBE_StaticLiquid>( BaseEntity::BE_STATICLIQUID );
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

	CBE_DirectionalLight *pDLight   = CreateBaseEntity<CBE_DirectionalLight>( BaseEntity::BE_DIRECTIONALLIGHT );
	CBE_PointLight *pPLight         = CreateBaseEntity<CBE_PointLight>( BaseEntity::BE_POINTLIGHT );
	CBE_DirectionalLight *pHSDLight = CreateBaseEntity<CBE_DirectionalLight>( BaseEntity::BE_DIRECTIONALLIGHT );
	CBE_PointLight *pHSPLight       = CreateBaseEntity<CBE_PointLight>( BaseEntity::BE_POINTLIGHT );
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
