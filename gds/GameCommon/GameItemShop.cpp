#include "GameItemShop.hpp"
#include "Script/PyModule_shop.hpp"
#include "Item/ItemDatabaseManager.hpp"

using namespace std;
using namespace boost;


//=============================================================================
// CGameItemShop
//=============================================================================

CGameItemShop::CGameItemShop()
{
//	GameItemShopList().AddShop( this );
}


CGameItemShop::CGameItemShop( const std::string& name )
:
m_Name(name)
{
//	GameItemShopList().AddShop( this );
}


CGameItemShop::~CGameItemShop()
{
	Release();
	GameItemShopList().RemoveShop( m_Name );
}

/*
int CGameItemShop::LoadItems( const std::vector<std::string>& m_vecItemName );
{
	int num_loaded_items = 0;
	size_t i, num_items = m_vecItemName.size();
	for( i=0; i<num_items; i++ )
	{
		CGameItem *pItem = ItemDatabase.Get( m_vecItemName[i] );

		if( pItem )
		{
			m_vecpItem.push_back( pItem );
			num_loaded_items++;
		}
	}

	return num_loaded_items;
}
*/

bool CGameItemShop::AddItem( boost::shared_ptr<CGameItem> pItem )
{
	if( !pItem )
		return false;

	m_vecpItem.push_back( pItem );

	return true;
}


int CGameItemShop::AddItem( const std::string& item_name, int quantity )
{
	shared_ptr<CGameItem> pItem = ItemDatabaseManager().GetItem<CGameItem>( item_name, quantity );
	if( !pItem )
		return 0;

	m_vecpItem.push_back( pItem );

	return quantity;
}


bool CGameItemShop::PurchaseItem( CCustomer& customer, const std::string& item_name )
{
	size_t i, num = m_vecpItem.size();
	for( i=0; i<num; i++ )
	{
		if( item_name == m_vecpItem[i]->GetName() )
			return PurchaseItem( customer, (int)i );
	}
	return false;
}


bool CGameItemShop::PurchaseItem( CCustomer& customer, int index, int quantity )
{
	if( customer.GetMoneyLeft() < m_vecpItem[index]->GetPrice() )
		return false;

//	bool paid = customer.Pay( m_vecpItem[index]->GetPrice() );

	if( true/*paid*/ )
	{
		boost::shared_ptr<CGameItem> pItemCopy
			= ItemDatabaseManager().GetItem<CGameItem>( m_vecpItem[index]->GetName(), quantity );

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


const shared_ptr<CGameItem> CGameItemShop::GetItem( int index )
{
	if( index < 0 || (int)m_vecpItem.size() <= index )
		return boost::shared_ptr<CGameItem>();

	return m_vecpItem[index];
}



//=============================================================================
// CGameItemShopList
//=============================================================================

CSingleton<CGameItemShopList> CGameItemShopList::m_obj;


bool CGameItemShopList::AddShop( shared_ptr<CGameItemShop> pShop )
{
	if( !pShop )
		return false;

	vector< shared_ptr<CGameItemShop> >::iterator shop;

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


bool CGameItemShopList::RemoveShop( const string& name )
{
	vector< shared_ptr<CGameItemShop> >::iterator shop;
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


shared_ptr<CGameItemShop> CGameItemShopList::GetShop( const string& name )
{
	vector< shared_ptr<CGameItemShop> >::iterator shop;
	for( shop = m_vecpShopList.begin();
		 shop != m_vecpShopList.end();
		 shop++ )
	{
		if( (*shop)->m_Name == name )
			return *shop;
	}

	return shared_ptr<CGameItemShop>();
}


shared_ptr<CGameItemShop> CreateGameItemShop( const std::string& shop_name )
{
	shared_ptr<CGameItemShop> pShop = shared_ptr<CGameItemShop>( new CGameItemShop(shop_name) );

	GameItemShopList().AddShop( pShop );

	return pShop;
}
