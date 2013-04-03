#ifndef  __GAMEITEMDATABASE_H__
#define  __GAMEITEMDATABASE_H__


#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;

class GameItem;


class GameItemInfo : public IArchiveObjectBase
{
public:

	std::string strItemName;
	std::string strFilename;

	GameItem *pItem;

	/// indicates if the item has been loaded on the memory
//	bool bIsLoaded;

	GameItemInfo() { pItem = NULL; /*bIsLoaded = false;*/ }

//	~GameItemInfo() { ReleaseItem(); }

	GameItem *GetItem( const int quantity );

	/// release the item if it is currently loaded
	/// must be called explicitly by the owner before destroying the object
	void ReleaseItem() { SafeDelete( pItem ); }

	void Serialize( IArchive& ar, const unsigned int version );
};


class GameItemDatabase : public IArchiveObjectBase
{
	std::vector<GameItemInfo> m_vecGameItemInfo;

public:

	GameItemDatabase() {}

	~GameItemDatabase();

	void AddItemInfo( const std::string& strFilename, const std::string& strItemName );

	bool LoadFromFile( const char *pcFilename );

    bool SaveToFile( const char *pcFilename );

	void Serialize( IArchive& ar, const unsigned int version );

	/// returns item(s) with the specified name
	/// returns NULL if the specified item is not found
	GameItem *GetItem( const char *pItemName, int quantity );

	/// save an item object as a binary file.
	/// not used during runtime
	void SaveItem( const char *pFilename, GameItem *pItem );

	void SortItemsInAlphabeticalOrder();
};


} // namespace amorphous



#endif  /*  __GAMEITEMDATABASE_H__  */
