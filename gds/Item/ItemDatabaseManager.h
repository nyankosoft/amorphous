#ifndef __ITEMDATABASEMANAGER_SINGLETON_H__
#define __ITEMDATABASEMANAGER_SINGLETON_H__


#include "Support/Singleton.h"
using namespace NS_KGL;

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;


#include <boost/shared_ptr.hpp>


class CGameItem;


/**
 * holds database of items
 */
class CItemDatabaseManager
{
	CBinaryDatabase<std::string> *m_pItemDatabase;

public:

	static CSingleton<CItemDatabaseManager> m_obj;

	static CItemDatabaseManager* Get() { return m_obj.get(); }

	CItemDatabaseManager() : m_pItemDatabase(NULL) {}

	~CItemDatabaseManager() { Release(); }

	void Release();

	bool Update( const std::string& filename );

	bool LoadFromFile( const std::string& filename );

	/// \return the owned reference of an item object
	/// \return NULL if the item with the specified name was not found
	CGameItem *GetItemRawPtr( const std::string& item_name, int quantity );

	template<class T>
	inline boost::shared_ptr<T> GetItem( const std::string& item_name, int quantity );
};


template<class T>
inline boost::shared_ptr<T> CItemDatabaseManager::GetItem( const std::string& item_name, int quantity )
{
	CGameItem *pRawOwnedPtr = GetItemRawPtr( item_name, quantity );
	if( !pRawOwnedPtr )
		return  boost::shared_ptr<T>();

	boost::shared_ptr<CGameItem> pBasePtr = boost::shared_ptr<CGameItem>( pRawOwnedPtr );

	return boost::dynamic_pointer_cast<T,CGameItem>(pBasePtr);
}


/// Returns the reference to the singleton instance of CItemDatabaseManager
inline CItemDatabaseManager& ItemDatabaseManager()
{
	return (*(CItemDatabaseManager::Get()));
}



#endif  /*  __ITEMDATABASEMANAGER_SINGLETON_H__  */
