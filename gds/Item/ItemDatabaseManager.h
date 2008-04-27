#ifndef __ITEMDATABASEMANAGER_SINGLETON_H__
#define __ITEMDATABASEMANAGER_SINGLETON_H__


#include "Support/Singleton.h"
using namespace NS_KGL;

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

#include <stdlib.h>

class CGameItem;


#define ItemDatabaseManager	(*(CItemDatabaseManager::Get()))

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

	bool Update( const char *pcFilename );

	bool LoadFromFile( const std::string& filename );

	/// \return the owned reference of an item object
	/// \return NULL if the item with the specified name was not found
	CGameItem *GetItem( const std::string& item_name, int quantity );
};



#endif  /*  __ITEMDATABASEMANAGER_SINGLETON_H__  */
