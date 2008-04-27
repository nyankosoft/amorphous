
#include "PlayerInfo.h"

#include "GameInput/InputHub.h"
#include "Input/InputHandler_PlayerShip.h"
#include "Input/InputHandler_PlayerPAC.h"
#include "Item/GameItem.h"
#include "Item/GI_Ammunition.h"
#include "Item/GI_Aircraft.h"
#include "Item/ItemDatabaseManager.h"
#include "Item/GameItemObjectFactory.h"
#include "Item/WeaponSystem.h"
#include "HUD_PlayerShip.h"
#include "HUD_PlayerAircraft.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"

#include "BE_Player.h"

using namespace std;


// definition of the singleton instance
CPlayerInfo CPlayerInfo::ms_SingletonInstance_;


CPlayerInfo::CPlayerInfo()
: 
m_pHUD(NULL)
{
	m_pCurrentPlayerBaseEntity = NULL;

	m_TaskID = CGameTask::ID_INVALID;

	m_pInputHandler = NULL;

	m_pWeaponSystem = new CWeaponSystem;
	m_pWeaponSystem->SetProjectileGroup( ENTITY_GROUP_MIN_ID );// CE_GROUP_PLAYER_PROJECTILE );

	m_vecpActiveItem.reserve( 8 );
	m_pFocusedItem = NULL;

	m_pCurrentAircraft = NULL;

	m_Money = 1000000;
}


CPlayerInfo::~CPlayerInfo()
{
	Release();
}


void CPlayerInfo::Release()
{
	SafeDelete( m_pInputHandler );
	SafeDelete( m_pHUD );
	SafeDelete( m_pWeaponSystem );

	SafeDeleteVector( m_vecpItem );
}


CCopyEntity *CPlayerInfo::GetCurrentPlayerEntity()
{
	if( GetCurrentPlayerBaseEntity() )
        return GetCurrentPlayerBaseEntity()->GetPlayerCopyEntity();
	else
		return NULL;
}


void CPlayerInfo::SetInputHandlerForPlayerShip()
{

	if( !GetCurrentPlayerBaseEntity() )
	{
		assert( !" a player base entity has not been set to PlayerInfo" );
		return;
	}

	SafeDelete( m_pInputHandler );
	SafeDelete( m_pHUD );


	switch( GetCurrentPlayerBaseEntity()->GetArchiveObjectID() )
	{
	case CBaseEntity::BE_PLAYERSHIP:
	case CBaseEntity::BE_PLAYERPSEUDOLEGGEDVEHICLE:
		m_pInputHandler = new CInputHandler_PlayerShip;
		m_pHUD = new HUD_PlayerShip;
		break;
	case CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT:
		m_pInputHandler = new CInputHandler_PlayerPAC;
		m_pHUD = new HUD_PlayerAircraft;
		break;
	}

	m_pHUD->Init();

	INPUTHUB.SetInputHandler( m_pInputHandler );
}

void CPlayerInfo::AddItemToCategoryList( CGameItem *pItem )
{
	int category;
	switch( pItem->GetTypeFlag() )
	{
	case CGameItem::TYPE_WEAPON:
		category = CItemCategory::WEAPON;
		break;
	case CGameItem::TYPE_AMMO:
		{
			CGI_Ammunition *pAmmo = (CGI_Ammunition *)pItem;
			if( !strcmp(pAmmo->GetAmmoType(), "7.62x51") )
				category = CItemCategory::AMMO_762X51;
			else if( !strcmp(pAmmo->GetAmmoType(), "12GS") )
				category = CItemCategory::AMMO_12GAUGE;
			else if( !strcmp(pAmmo->GetAmmoType(), "40MMG") )
				category = CItemCategory::AMMO_40MMGRENADE;
			else if( !strcmp(pAmmo->GetAmmoType(), "MSSL") )
				category = CItemCategory::AMMO_MISSILE;
			else if( !strcmp(pAmmo->GetAmmoType(), "RBODY") )
				category = CItemCategory::AMMO_RIGIDBODY;
//			else if( !strcmp(pAmmo->GetAmmoType(), "50CAL") )
//				category = CItemCategory::AMMO_50CAL;
			else
				category = -1;
			break;
		}

	case CGameItem::TYPE_SUPPLY:
		category = CItemCategory::SUPPLY;
		break;
	case CGameItem::TYPE_UTILITY:
		category = CItemCategory::UTILITY;
		break;
	case CGameItem::TYPE_KEY:
		category = CItemCategory::KEY;
		break;
	default:
		category = -1;
		break;
	}

	if( 0 <= category )
		m_vecpCategoryItem[category].push_back( pItem );
}


int CPlayerInfo::SupplyItem( CGameItem* pItem )
{
	if( !pItem )
		return false;

	int supplied_quantity = SupplyItem( pItem->GetName().c_str(), pItem->GetCurrentQuantity() );

	SafeDelete( pItem );

	return supplied_quantity;
}


int CPlayerInfo::SupplyItem( const char *pcItemName, const int iSupplyQuantity )
//int CPlayerInfo::SupplyItem( CGameItem*& rpItem )/// \param owned reference
{
	size_t i, num_items = m_vecpItem.size();
	for(i=0; i<num_items; i++)
	{
		// search the array of items to find one with the same name as 'pcItemName'
		if( strcmp(pcItemName, m_vecpItem[i]->GetName().c_str() ) == 0 )
//		 && (*pSuppliedItem) == m_vecpItem[i] )	// check the items are the identical instances
		{
			break;
		}
	}

	if( i == num_items )
	{
		// a new item - needs to be copied from the item database
		CGameItem *pItem = ItemDatabaseManager.GetItem( pcItemName, iSupplyQuantity );
		if( !pItem /*|| !(pItem->GetTypeFlag() & CGameItem::TYPE_AMMO)*/ )
			return 0;	// the item was not found in the database

		// initialize quantity
		pItem->SetZeroQuantity();

		// add a new item entry to the list
		m_vecpItem.push_back( pItem );

		// put to a one of the category sorted lists
		AddItemToCategoryList( pItem );

	}
/*
	if( i == num_items )
	{
		// add a new item entry to the list
		m_vecpItem.push_back( pSuppliedItem );

		// put to a one of the category sorted lists
		// the reference is now owned by PlayerInfo
		AddItemToCategoryList( pItem );
	}
	else
	{
		// The supplied item is already in the list
		// just increase the quantity

		// delete the supplied item object
	}
*/
	// supply item(s)

	CGameItem& rItem = *m_vecpItem[i];
	if( rItem.GetCurrentQuantity() == rItem.GetMaxQuantity() )
		return 0;	// already been full - can't have any more
	else
	{
		int num = rItem.GetCurrentQuantity();
		int added_quantity;
		if( num + iSupplyQuantity <= rItem.GetMaxQuantity() )
			added_quantity = iSupplyQuantity;
		else
			added_quantity = rItem.GetMaxQuantity() - rItem.GetCurrentQuantity();  // prevent overload

		rItem.AddQuantity( added_quantity );

		return added_quantity;
	}

	return 0;
}


CGameItem *CPlayerInfo::GetItemByName( const char *pcItemName )
{
	size_t i=0, num_items = m_vecpItem.size();
	for( i=0; i<num_items; i++ )
	{
		if( m_vecpItem[i]->GetName() == pcItemName )
			return m_vecpItem[i];
	}

	MsgBoxFmt( "CPlayerInfo::GetItemByName() - cannot find the item '%s'", pcItemName );
	g_Log.Print( "CPlayerInfo::GetItemByName() - cannot find the item '%s'", pcItemName );
	return NULL;
}


void CPlayerInfo::Update( float dt )
{
	if( m_pHUD )
		m_pHUD->Update( dt );

	size_t i=0, num = m_vecpActiveItem.size();
	for( i=0; i<num; i++ )
	{
		m_vecpActiveItem[i]->Update( dt );
	}
}


void CPlayerInfo::RenderHUD()
{
	if( m_pHUD )
        m_pHUD->Render();

	// draw the status of the items currently active
	size_t i, num = m_vecpActiveItem.size();
	for( i=0; i<num; i++ )
	{
		m_vecpActiveItem[i]->RenderStatus( (int)i, m_pHUD->GetFont() );
	}
}


void CPlayerInfo::Serialize( IArchive& ar, const unsigned int version )
{
	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		m_pCurrentPlayerBaseEntity = NULL;
		SafeDelete( m_pInputHandler );
		SafeDelete( m_pHUD );
//		m_pWeaponSystem;

		m_TaskID = -1;
		SafeDeleteVector( m_vecpItem );

		size_t i, num_item_categories = CItemCategory::NUM_CATEGORIES;
		for( i=0; i<num_item_categories; i++ )
            m_vecpCategoryItem[i].resize(0);

		m_vecpActiveItem.resize(0);

		m_pCurrentAircraft = NULL;
	}

	CGameItemObjectFactory factory;

	ar.Polymorphic( m_vecpItem, factory );

	ar & m_Money;

	ar & m_KeyBind;

//	ar & m_PlayTime;

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// register items to m_vecpCategoryItem
	}

}