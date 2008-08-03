
#include "ItemDatabaseManager.h"
#include "ItemDatabaseBuilder.h"

#include "GameItem.h"
#include "GameItemObjectFactory.h"

#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"

using namespace std;


CSingleton<CItemDatabaseManager> CItemDatabaseManager::m_obj;


void CItemDatabaseManager::Release()
{
	SafeDelete( m_pItemDatabase );
}


bool CItemDatabaseManager::Update( const std::string& filename )
{
	CItemDatabaseBuilder item_db_builder;

	return item_db_builder.CreateItemDatabaseFile( filename );
}


bool CItemDatabaseManager::LoadFromFile( const std::string& filename )
{
	Release();
	m_pItemDatabase = new CBinaryDatabase<string>;

	bool db_loaded = m_pItemDatabase->Open( filename );

	if( !db_loaded )
	{
//		assert( !"unable to load the item database" );
		g_Log.Print( WL_WARNING, "unable to load the item database" );
		SafeDelete( m_pItemDatabase );
		return false;
	}
	else
		return true;
}


CGameItem *CItemDatabaseManager::GetItemRawPtr( const std::string& item_name, int quantity )
{
	if( !m_pItemDatabase )
		return NULL;

	CGameItemObjectFactory factory;

	// copy from the database
	CGameItem *pItem = NULL;
	if( !m_pItemDatabase->GetPolymorphicData( item_name, pItem, factory ) )
		return NULL;

	// set the quantity
	pItem->m_iCurrentQuantity = quantity;

	return pItem;
}
