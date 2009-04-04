#ifndef __GAMEITEMSHOP_H__
#define __GAMEITEMSHOP_H__


#include <vector>

class CGameItem;
class CCustomer;


#include "Item/GameItem.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "Support/SafeDeleteVector.hpp"


class CCustomer
{
//	int m_Money;

public:

	/// \return actual added quantity
	virtual int AddItem( boost::shared_ptr<CGameItem> pItem ) = 0;

	virtual bool Pay( int amount ) = 0;

	virtual int GetMoneyLeft() const = 0;
};


class CGameItemShop
{
	std::vector< boost::shared_ptr<CGameItem> > m_vecpItem;

public:

	CGameItemShop();

	CGameItemShop( const std::string& name );

	virtual ~CGameItemShop();

	std::string Name;

	void Release()
	{
		m_vecpItem.resize( 0 );
		//SafeDeleteVector( m_vecpItem );
	}

/*	int LoadItems( const std::vector<std::string>& m_vecItemName );
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
	}*/

	bool AddItem( boost::shared_ptr<CGameItem> pItem )
	{
		if( !pItem )
			return false;

		m_vecpItem.push_back( pItem );

		return true;
	}

	bool PurchaseItem( CCustomer& customer, const std::string& item_name )
	{
		size_t i, num = m_vecpItem.size();
		for( i=0; i<num; i++ )
		{
			if( item_name == m_vecpItem[i]->GetName() )
				return PurchaseItem( customer, (int)i );
		}
		return false;
	}

	bool PurchaseItem( CCustomer& customer, int index, int quantity = 1 )
	{
		if( customer.GetMoneyLeft() < m_vecpItem[index]->GetPrice() )
			return false;

//		bool paid = customer.Pay( m_vecpItem[index]->GetPrice() );

		if( true/*paid*/ )
		{
			boost::shared_ptr<CGameItem> pItemCopy
				= ItemDatabaseManager().GetItem<CGameItem>( m_vecpItem[index]->GetName(), quantity );

			int num_items_received = customer.AddItem( pItemCopy );

			bool paid = false;
			if( 0 < num_items_received )
				paid = customer.Pay( m_vecpItem[index]->GetPrice() * num_items_received );

//			if( !paid )
//				customer.ReturnItem( name, quantity );

			return 0 < num_items_received ? true : false;
		}
		else
		{
			return false;
		}
	}

	const boost::shared_ptr<CGameItem> GetItem( int index )
	{
		if( index < 0 || (int)m_vecpItem.size() <= index )
			return boost::shared_ptr<CGameItem>();

		return m_vecpItem[index];
	}

	size_t GetNumItems() const { return m_vecpItem.size(); }

	/*
	bool LoadFromFile( std::string& script_filename )
	{
//		stream_buffer buffer;
//		if( !buffer.LoadFromFile( script_filename ) )
//			return false;

		SetShopForScript( this );

		CScriptManager event_mgr;
		event_mgr.AddModule( "Shop",  );
		event_mgr.LoadScriptArchiveFile( script_filename );

		SetShopForScript( NULL );
	}*/
};


#endif /* __GAMEITEMSHOP_H__ */
