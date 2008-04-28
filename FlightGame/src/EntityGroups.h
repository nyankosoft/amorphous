#ifndef __FG_EntityGroupIDs_H__
#define __FG_EntityGroupIDs_H__


#include <Stage/EntitySet.h>
#include <Stage/Stage.h>



enum eCopyEntityGroup
{
	CE_GROUP_GENERAL = ENTITY_GROUP_MIN_ID,

	CE_GROUP_PLAYER,
	CE_GROUP_PLAYER_ALLY,
	CE_GROUP_ENEMY,

	CE_GROUP_PLAYER_PROJECTILE,
	CE_GROUP_PLAYER_ALLY_PROJECTILE,
	CE_GROUP_ENEMY_PROJECTILE,

	CE_GROUP_PLAYER_TARGET_FILTER,
	CE_GROUP_ENEMY_TARGET_FILTER,
	CE_GROUP_PLAYER_ALLY_TARGET_FILTER,

	CE_GROUP_NEUTRAL,
	CE_GROUP_STATICGEOMETRY,
	CE_GROUP_NON_COLLIDABLE,
//	CE_GROUP_NOCLIP,
//	CE_GROUP_DEBRIS,
	CE_NUM_GROUPS
};


inline void InitEntityGroup( CStageSharedPtr pStage )
{
	CEntitySet *pEntitySet = pStage->GetEntitySet();

	//
	// set up id -> string mapping
	//

	// group id that corresponds to 'ENTITY_GROUP_MIN_ID' is used
	// as a default id of CBaseEntity::m_EntityGroup
	pEntitySet->SetEntityGroupName( CE_GROUP_GENERAL,                   "EG_General" );

	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER,                    "EG_Player" );
	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER_ALLY,               "EG_PlayerAlly" );
	pEntitySet->SetEntityGroupName( CE_GROUP_ENEMY,                     "EG_Enemy" );

	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER_PROJECTILE,         "EG_PlayerProjectile" );
	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER_ALLY_PROJECTILE,    "EG_PlayerAllyProjectile" );
	pEntitySet->SetEntityGroupName( CE_GROUP_ENEMY_PROJECTILE,          "EG_EnemyProjectile" );

	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER_PROJECTILE,         "EG_PlayerProjectile" );
	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER_ALLY_PROJECTILE,    "EG_PlayerAllyProjectile" );
	pEntitySet->SetEntityGroupName( CE_GROUP_ENEMY_PROJECTILE,          "EG_EnemyProjectile" );

	// target filters
	// - not used as groups: used as a filter for entity to decide which entity groups are selected as its target
	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER_TARGET_FILTER,      "EG_PlayerTargetFilter" );
	pEntitySet->SetEntityGroupName( CE_GROUP_PLAYER_ALLY_TARGET_FILTER, "EG_PlayerAllyTargetFilter" );
	pEntitySet->SetEntityGroupName( CE_GROUP_ENEMY_TARGET_FILTER,       "EG_EnemyTargetFilter" );

	//
	// set collision groups
	//

	// - collisions are enabled between all groups
	// - disable collisions between groups that should not hit each other
	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER,                    CE_GROUP_PLAYER_PROJECTILE,      false );
	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER_ALLY,               CE_GROUP_PLAYER_ALLY_PROJECTILE, false );
	pEntitySet->SetCollisionGroup( CE_GROUP_ENEMY,                     CE_GROUP_ENEMY_PROJECTILE,       false );

	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER_PROJECTILE,	       CE_GROUP_PLAYER_PROJECTILE,      false );
	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER_ALLY_PROJECTILE,	   CE_GROUP_PLAYER_ALLY_PROJECTILE, false );
	pEntitySet->SetCollisionGroup( CE_GROUP_ENEMY_PROJECTILE,	       CE_GROUP_ENEMY_PROJECTILE,       false );

	pEntitySet->SetCollisionGroup( CE_GROUP_NON_COLLIDABLE, false );

	// set filters
	// - used to determine targets for entities on each side

	// player side
	// player targets enemy forces
	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER_TARGET_FILTER,      false );
	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER_TARGET_FILTER,      CE_GROUP_ENEMY,                 true );
	// allied forces target forces
	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER_ALLY_TARGET_FILTER, false );
	pEntitySet->SetCollisionGroup( CE_GROUP_PLAYER_ALLY_TARGET_FILTER, CE_GROUP_ENEMY,                 true );

	// enemy side
	// enemy forces target player and its allied forces
	pEntitySet->SetCollisionGroup( CE_GROUP_ENEMY_TARGET_FILTER,       false );
	pEntitySet->SetCollisionGroup( CE_GROUP_ENEMY_TARGET_FILTER,       CE_GROUP_PLAYER,                true );
	pEntitySet->SetCollisionGroup( CE_GROUP_ENEMY_TARGET_FILTER,       CE_GROUP_PLAYER_ALLY,           true );
}





#endif  /*  __FG_EntityGroupIDs_H__  */
