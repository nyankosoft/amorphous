#ifndef __ITEMDATABASEMANAGER_SINGLETON_H__
#define __ITEMDATABASEMANAGER_SINGLETON_H__


#include "fwd.hpp"
#include "GameItem.hpp"
#include "gds/Support/Singleton.hpp"
#include "gds/Support/Serialization/ArchiveObjectFactory.hpp"
#include "gds/Support/Serialization/BinaryDatabase.hpp"
#include <boost/shared_ptr.hpp>


namespace amorphous
{

using namespace serialization;


/**
 * holds database of items
 */
class CItemDatabaseManager
{
	CBinaryDatabase<std::string> *m_pItemDatabase;

private:

	/// \return the owned reference of an item object
	/// \return NULL if the item with the specified name was not found
	CGameItem *GetItemRawPtr( const std::string& item_name, int quantity );

public:

	static CSingleton<CItemDatabaseManager> m_obj;

	static CItemDatabaseManager* Get() { return m_obj.get(); }

	CItemDatabaseManager() : m_pItemDatabase(NULL) {}

	~CItemDatabaseManager() { Release(); }

	void Release();

	bool Update( const std::string& filename, const std::string& default_output_filename );

	bool LoadFromFile( const std::string& filename );

	template<class T>
	inline boost::shared_ptr<T> GetItem( const std::string& item_name, int quantity );
};


//--------------------------------- inline implementations ---------------------------------

template<class T>
inline boost::shared_ptr<T> CItemDatabaseManager::GetItem( const std::string& item_name, int quantity )
{
	CGameItem *pRawOwnedPtr = GetItemRawPtr( item_name, quantity );
	if( !pRawOwnedPtr )
		return  boost::shared_ptr<T>();

	boost::shared_ptr<CGameItem> pBasePtr = boost::shared_ptr<CGameItem>( pRawOwnedPtr );

	pBasePtr->SetWeakPtr( pBasePtr );

	pBasePtr->OnLoadedFromDatabase();

	return boost::dynamic_pointer_cast<T,CGameItem>(pBasePtr);
}


/// Returns the reference to the singleton instance of CItemDatabaseManager
inline CItemDatabaseManager& ItemDatabaseManager()
{
	return (*(CItemDatabaseManager::Get()));
}


} // namespace amorphous



#endif  /*  __ITEMDATABASEMANAGER_SINGLETON_H__  */
