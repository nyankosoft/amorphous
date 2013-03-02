#ifndef __GameItemShop_HPP__
#define __GameItemShop_HPP__


#include <vector>
#include <boost/shared_ptr.hpp>
#include "gds/Support/singleton.hpp"
#include "gds/Item/fwd.hpp"


namespace amorphous
{


class CCustomer
{
//	int m_Money;

public:

	/// \return actual added quantity
	virtual int AddItem( boost::shared_ptr<GameItem> pItem ) = 0;

	virtual bool Pay( int amount ) = 0;

	virtual int GetMoneyLeft() const = 0;
};


class GameItemShop
{
	std::vector< boost::shared_ptr<GameItem> > m_vecpItem;

public:

	std::string m_Name;

public:

	GameItemShop();

	GameItemShop( const std::string& name );

	virtual ~GameItemShop();

	void Release()
	{
		m_vecpItem.resize( 0 );
		//SafeDeleteVector( m_vecpItem );
	}

//	int LoadItems( const std::vector<std::string>& m_vecItemName );

	bool AddItem( boost::shared_ptr<GameItem> pItem );

	int AddItem( const std::string& item_name, int quantity );

	bool PurchaseItem( CCustomer& customer, const std::string& item_name );

	bool PurchaseItem( CCustomer& customer, int index, int quantity = 1 );

	const boost::shared_ptr<GameItem> GetItem( int index );

	size_t GetNumItems() const { return m_vecpItem.size(); }

	/*
	bool LoadFromFile( std::string& script_filename )
	{
//		stream_buffer buffer;
//		if( !buffer.LoadFromFile( script_filename ) )
//			return false;

		SetShopForScript( this );

		ScriptManager event_mgr;
		event_mgr.AddModule( "Shop",  );
		event_mgr.LoadScriptArchiveFile( script_filename );

		SetShopForScript( NULL );
	}*/
};


/**
 * singleton class
 */
class GameItemShopList
{
	static singleton<GameItemShopList> m_obj;

	std::vector< boost::shared_ptr<GameItemShop> > m_vecpShopList;

public:

	static GameItemShopList* Get() { return m_obj.get(); }

	GameItemShopList() {}

	~GameItemShopList() { Release(); }

	void Release() {}

	/// registers a shop to the list.
	/// - called by CreateGameItemShop()
	bool AddShop( boost::shared_ptr<GameItemShop> pShop );

	/// - called by dtor of GameItemShop
	bool RemoveShop( const std::string& name );

	boost::shared_ptr<GameItemShop> GetShop( const std::string& name );
};


inline GameItemShopList& GetGameItemShopList()
{
	return *(GameItemShopList::Get());
}


extern boost::shared_ptr<GameItemShop> CreateGameItemShop( const std::string& shop_name );


} // namespace amorphous



#endif /* __GameItemShop_HPP__ */
