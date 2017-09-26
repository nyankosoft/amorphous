#include "GameItemShop.hpp"
#include "amorphous/Script/PyModule_shop.hpp"
#include "amorphous/Item/ItemDatabaseManager.hpp"


namespace amorphous
{

using namespace std;


//=============================================================================
// GameItemShop
//=============================================================================

GameItemShop::GameItemShop()
{
//	GameItemShopList().AddShop( this );
}


GameItemShop::GameItemShop( const std::string& name )
:
m_Name(name)
{
//	GameItemShopList().AddShop( this );
}


GameItemShop::~GameItemShop()
{
	Release();
	GetGameItemShopList().RemoveShop( m_Name );
}

/*
int GameItemShop::LoadItems( const std::vector<std::string>& m_vecItemName );
{
	int num_loaded_items = 0;
	size_t i, num_items = m_vecItemName.size();
	for( i=0; i<num_items; i++ )
	{
		GameItem *pItem = ItemDatabase.Get( m_vecItemName[i] );

		if( pItem )
		{
			m_vecpItem.push_back( pItem );
			num_loaded_items++;
		}
	}

	return num_loaded_items;
}
*/

bool GameItemShop::AddItem( std::shared_ptr<GameItem> pItem )
{
	if( !pItem )
		return false;

	m_vecpItem.push_back( pItem );

	return true;
}


int GameItemShop::AddItem( const std::string& item_name, int quantity )
{
	shared_ptr<GameItem> pItem = GetItemDatabaseManager().GetItem<GameItem>( item_name, quantity );
	if( !pItem )
		return 0;

	m_vecpItem.push_back( pItem );

	return quantity;
}


bool GameItemShop::PurchaseItem( CCustomer& customer, const std::string& item_name )
{
	size_t i, num = m_vecpItem.size();
	for( i=0; i<num; i++ )
	{
		if( item_name == m_vecpItem[i]->GetName() )
			return PurchaseItem( customer, (int)i );
	}
	return false;
}


bool GameItemShop::PurchaseItem( CCustomer& customer, int index, int quantity )
{
	if( customer.GetMoneyLeft() < m_vecpItem[index]->GetPrice() )
		return false;

//	bool paid = customer.Pay( m_vecpItem[index]->GetPrice() );

	if( true/*paid*/ )
	{
		std::shared_ptr<GameItem> pItemCopy
			= GetItemDatabaseManager().GetItem<GameItem>( m_vecpItem[index]->GetName(), quantity );

		int num_items_received = customer.AddItem( pItemCopy );

		bool paid = false;
		if( 0 < num_items_received )
			paid = customer.Pay( m_vecpItem[index]->GetPrice() * num_items_received );

//		if( !paid )
//			customer.ReturnItem( name, quantity );

		return 0 < num_items_received ? true : false;
	}
	else
	{
		return false;
	}
}


const shared_ptr<GameItem> GameItemShop::GetItem( int index )
{
	if( index < 0 || (int)m_vecpItem.size() <= index )
		return std::shared_ptr<GameItem>();

	return m_vecpItem[index];
}



//=============================================================================
// GameItemShopList
//=============================================================================

singleton<GameItemShopList> GameItemShopList::m_obj;


bool GameItemShopList::AddShop( shared_ptr<GameItemShop> pShop )
{
	if( !pShop )
		return false;

	vector< shared_ptr<GameItemShop> >::iterator shop;

	for( shop = m_vecpShopList.begin();
		 shop != m_vecpShopList.end();
		 shop++ )
	{
		if( (*shop)->m_Name == pShop->m_Name )
		{
			LOG_PRINT_ERROR( "The shop named '" + pShop->m_Name + "' already exists in the list" );
			return false;	// already registered
		}
	}

	m_vecpShopList.push_back( pShop );
	return true;
}


bool GameItemShopList::RemoveShop( const string& name )
{
	vector< shared_ptr<GameItemShop> >::iterator shop;
	for( shop = m_vecpShopList.begin();
		 shop != m_vecpShopList.end();
		 shop++ )
	{
		if( (*shop)->m_Name == name )
		{
			m_vecpShopList.erase( shop );
			return true;
		}
	}

	return false;
}


shared_ptr<GameItemShop> GameItemShopList::GetShop( const string& name )
{
	vector< shared_ptr<GameItemShop> >::iterator shop;
	for( shop = m_vecpShopList.begin();
		 shop != m_vecpShopList.end();
		 shop++ )
	{
		if( (*shop)->m_Name == name )
			return *shop;
	}

	return shared_ptr<GameItemShop>();
}


shared_ptr<GameItemShop> CreateGameItemShop( const std::string& shop_name )
{
	shared_ptr<GameItemShop> pShop = shared_ptr<GameItemShop>( new GameItemShop(shop_name) );

	GetGameItemShopList().AddShop( pShop );

	return pShop;
}


} // namespace amorphous
