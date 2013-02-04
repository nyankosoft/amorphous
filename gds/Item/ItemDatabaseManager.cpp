#include "ItemDatabaseManager.hpp"
#include "ItemDatabaseBuilder.hpp"
#include "GameItem.hpp"
#include "GameItemObjectFactory.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


CSingleton<CItemDatabaseManager> CItemDatabaseManager::m_obj;


void CItemDatabaseManager::Release()
{
	SafeDelete( m_pItemDatabase );
}


bool CItemDatabaseManager::Update( const std::string& filename, const std::string& default_output_filename )
{
	CItemDatabaseBuilder item_db_builder;

	if( filename.rfind( "xml" ) == filename.length() - 3 )
		return item_db_builder.CreateItemDatabaseFileFromXMLFile( filename, default_output_filename );
	else
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


GameItem *CItemDatabaseManager::GetItemRawPtr( const std::string& item_name, int quantity )
{
	if( !m_pItemDatabase )
		return NULL;

	GameItemObjectFactory factory;

	// copy from the database
	GameItem *pItem = NULL;
	if( !m_pItemDatabase->GetPolymorphicData( item_name, pItem, factory ) )
		return NULL;

	// set the quantity
	pItem->m_iCurrentQuantity = quantity;

	return pItem;
}


} // namespace amorphous
