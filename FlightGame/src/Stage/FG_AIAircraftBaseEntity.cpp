
#include "FG_AIAircraftBaseEntity.h"
#include "FG_AIAircraftEntity.h"

#include "Stage/GameMessage.h"
#include "Stage/CopyEntityDesc.h"
#include "Stage/CopyEntity.h"
#include "Stage/trace.h"
#include "Stage/Stage.h"
#include "Item/ItemDatabaseManager.h"
#include "JigLib/JL_PhysicsActor.h"
#include "Support/Log/DefaultLog.h"


CFG_AIAircraftBaseEntity::CFG_AIAircraftBaseEntity()
{
	m_bLighting = true;
	m_bNoClip = false;
}


CFG_AIAircraftBaseEntity::~CFG_AIAircraftBaseEntity()
{
	ReleaseGraphicsResources();
}


void CFG_AIAircraftBaseEntity::Init()
{
	Init3DModel();

	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;
}


void CFG_AIAircraftBaseEntity::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	CFG_AIAircraftEntity *pAircraftEntity = dynamic_cast<CFG_AIAircraftEntity *> (pCopyEnt);

	if( pAircraftEntity )
	{
		// retrieve aircraft item
		CGameItem *pAircraft = ItemDatabaseManager.GetItem( m_strAircraftItemName, 1 );
		pAircraftEntity->m_pAircraft = dynamic_cast<CGI_Aircraft *> (pAircraft);
		if( !pAircraftEntity->m_pAircraft )
			LOG_PRINT_ERROR( "" );

		// retrieve weapon item
		CGameItem *pItem = NULL;
		pItem = ItemDatabaseManager.GetItem( "", 1 );
		CGI_Weapon *pWeapon = dynamic_cast<CGI_Weapon *> (pItem);
		if( pWeapon )
		{
			pAircraftEntity->m_vecpWeapon.resize( 1 );
			pAircraftEntity->m_vecpWeapon[0] = pWeapon;
		}
		else
			LOG_PRINT_ERROR( "" );


		// retrieve ammo item
		pItem = ItemDatabaseManager.GetItem( "", 1 );
		CGI_Ammunition *pAmmo = dynamic_cast<CGI_Ammunition *> (pItem);
		if( pAmmo )
		{
			pAircraftEntity->m_vecpAmmo.resize( 1 );
			pAircraftEntity->m_vecpAmmo[0] = pAmmo;
		}
		else
			LOG_PRINT_ERROR( "" );

//		pAircraftEntity->InitItems();

//		pAircraftEntity->ClearEntityFlag( BETYPE_ACTIVE );
	}

}


void CFG_AIAircraftBaseEntity::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
/*	switch( rGameMessage.iEffect )
	{
	}
*/
	// this will send the message to pCopyEnt_Self->HandleMessage()
	CBE_IndividualEntity::MessageProcedure( rGameMessage, pCopyEnt_Self );

	if( pCopyEnt_Self->fLife <= 0 )
	{
		m_Destruction.CreateExplosion( *pCopyEnt_Self, m_pStage );
		m_Destruction.CreateFragments( *pCopyEnt_Self, m_pStage );
//		m_Destruction.OnEntityDestroyed( *pCopyEnt_Self, m_pStage );

		m_pStage->TerminateEntity( pCopyEnt_Self );
	}
}


void CFG_AIAircraftBaseEntity::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_IndividualEntity::Serialize( ar, version );

	ar & m_ProjectileEntityGroup;
	ar & m_TargetFilter;

	ar & m_strAircraftItemName;
	ar & m_vecWeaponItemName;
	ar & m_vecAmmoItemName;

	ar & m_Destruction;
}


bool CFG_AIAircraftBaseEntity::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_IndividualEntity::LoadSpecificPropertiesFromFile( scanner ) )
		return true;

	if( m_Destruction.LoadFromFile( scanner ) )
		return true;

	string group_name;
	string tgt_filter;
	string item_name;

	if( scanner.TryScanLine( "PROJECTILE_GROUP", group_name ) )
	{
		m_ProjectileEntityGroup.SetGroupName( group_name );
		return true;
	}

	if( scanner.TryScanLine( "TARGET_FILTER", tgt_filter ) )
	{
		m_TargetFilter.SetGroupName( tgt_filter );
		return true;
	}

	if( scanner.TryScanLine( "AIRCRAFT_NAME",	m_strAircraftItemName ) ) return true;

	if( scanner.TryScanLine( "WEAPON_NAME",		item_name ) )
	{
		m_vecWeaponItemName.push_back(item_name);
		return true;
	}

	if( scanner.TryScanLine( "AMMO_NAME",		item_name ) )
	{
		m_vecAmmoItemName.push_back(item_name);
		return true;
	}

	return false;
}



/*
void CFG_AIAircraftBaseEntity::Draw(CCopyEntity* pCopyEnt)
{
	pCopyEnt->sState |= CESTATE_LIGHT_INFORMATION_INVALID;

	if( m_MeshProperty.m_MeshObjectHandle.IsLoaded() )
	{
		Draw3DModel(pCopyEnt);
	}
}
*/

/*
void CFG_AIAircraftBaseEntity::ReleaseGraphicsResources()
{
}

void CFG_AIAircraftBaseEntity::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}
*/
