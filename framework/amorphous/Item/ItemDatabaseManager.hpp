#ifndef __ITEMDATABASEMANAGER_SINGLETON_H__
#define __ITEMDATABASEMANAGER_SINGLETON_H__


#include "fwd.hpp"
#include "GameItem.hpp"
#include "amorphous/Support/singleton.hpp"
#include "amorphous/Support/Serialization/BinaryDatabase.hpp"
#include <boost/shared_ptr.hpp>


namespace amorphous
{

using namespace serialization;


/**
 * holds database of items
 */
class ItemDatabaseManager
{
	CBinaryDatabase<std::string> *m_pItemDatabase;

private:

	/// \return the owned reference of an item object
	/// \return NULL if the item with the specified name was not found
	GameItem *GetItemRawPtr( const std::string& item_name, int quantity );

public:

	static singleton<ItemDatabaseManager> m_obj;

	static ItemDatabaseManager* Get() { return m_obj.get(); }

	ItemDatabaseManager() : m_pItemDatabase(NULL) {}

	~ItemDatabaseManager() { Release(); }

	void Release();

	bool Update( const std::string& filename, const std::string& default_output_filename );

	bool LoadFromFile( const std::string& filename );

	template<class T>
	inline boost::shared_ptr<T> GetItem( const std::string& item_name, int quantity );
};


//--------------------------------- inline implementations ---------------------------------

template<class T>
inline boost::shared_ptr<T> ItemDatabaseManager::GetItem( const std::string& item_name, int quantity )
{
	GameItem *pRawOwnedPtr = GetItemRawPtr( item_name, quantity );
	if( !pRawOwnedPtr )
		return  boost::shared_ptr<T>();

	boost::shared_ptr<GameItem> pBasePtr = boost::shared_ptr<GameItem>( pRawOwnedPtr );

	pBasePtr->SetWeakPtr( pBasePtr );

	pBasePtr->OnLoadedFromDatabase();

	return boost::dynamic_pointer_cast<T,GameItem>(pBasePtr);
}


/// Returns the reference to the singleton instance of ItemDatabaseManager
inline ItemDatabaseManager& GetItemDatabaseManager()
{
	return (*(ItemDatabaseManager::Get()));
}


} // namespace amorphous



#endif  /*  __ITEMDATABASEMANAGER_SINGLETON_H__  */
