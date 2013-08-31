#include "Item/GameItem.hpp"
#include "Item/GameItemObjectFactory.hpp"
#include "GameItemDatabase.hpp"
#include <assert.h>
#include <algorithm>


namespace amorphous
{

using namespace std;


class GameItemSerializer : public IArchiveObjectBase
{
public:
	GameItem *pItem;
//	string strFilename;

	GameItemSerializer() { pItem = NULL; }
//	GameItemSerializer( const char *pFilename ) { pItem = NULL; strFilename = pFilename; }
	void Serialize( IArchive& ar, const unsigned int version );
};


void GameItemSerializer::Serialize( IArchive& ar, const unsigned int version )
{
	GameItemObjectFactory factory;
	ar.Polymorphic( pItem, factory );
}


GameItem *GameItemInfo::GetItem( const int quantity )
{
//	if( !pItem )
//	{
		BinaryArchive_Input archive( strFilename.c_str() );
//		GameItemObjectFactory factory;
		GameItemSerializer temp_serializer;
		if( !(archive >> temp_serializer) )
			assert(0);	// failed to load an item file

		pItem = temp_serializer.pItem;

		pItem->m_iCurrentQuantity = quantity;
//	}

//	return pItem;

	GameItem *pTempItem = pItem;
	pItem = NULL;
	return pTempItem;
}


void GameItemInfo::Serialize( IArchive& ar, const unsigned int version )
{
	ar & strItemName;
	ar & strFilename;
}


GameItemDatabase::~GameItemDatabase()
{
	// release all the items
	int i, num_items = m_vecGameItemInfo.size();
	for( i=0; i<num_items; i++ )
	{
		m_vecGameItemInfo[i].ReleaseItem();
	}
}


void GameItemDatabase::AddItemInfo( const string& strFilename, const string& strItemName )
{
	GameItemInfo item_info;

	item_info.strFilename = strFilename;
	item_info.strItemName = strItemName;
	m_vecGameItemInfo.push_back( item_info );
}


inline bool ABC_Order( GameItemInfo& ItemInfo0, GameItemInfo& ItemInfo1 )
{
	return ItemInfo0.strItemName < ItemInfo1.strItemName;
}


void GameItemDatabase::SortItemsInAlphabeticalOrder()
{
	std::sort( m_vecGameItemInfo.begin(), m_vecGameItemInfo.end(), ABC_Order );
}


bool GameItemDatabase::LoadFromFile( const char *pcFilename )
{
	BinaryArchive_Input archive( pcFilename );
	
	return archive >> (*this);
}


bool GameItemDatabase::SaveToFile( const char *pcFilename )
{
	// sort items in the alphabetical order before saving them
	SortItemsInAlphabeticalOrder();

	BinaryArchive_Output archive( pcFilename );

	return archive << (*this);
}


void GameItemDatabase::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecGameItemInfo;
}



GameItem *GameItemDatabase::GetItem( const char *pItemName, int quantity )
{
	int size = m_vecGameItemInfo.size();
	if( size == 0 )
		return NULL;

//	int i0 = 0, i1 = size - 1, mid = size / 2; 
	int i0 = 0, i1 = size, mid = size / 2; 

	while(1)
	{
		if( i1 - i0 <= 1 )
		{
			if( m_vecGameItemInfo[i0].strItemName == pItemName )
			{
				return m_vecGameItemInfo[i0].GetItem( quantity );
					
			}
			else
				return NULL;
		}

		if( m_vecGameItemInfo[(i0 + i1) / 2].strItemName > pItemName )
//		if( m_vecGameItemInfo[(i0 + i1) / 2].strItemName < pItemName )
		{
			i1 = (i0 + i1) / 2;
		}
		else
		{
			i0 = (i0 + i1) / 2;
		}
	}
}


void GameItemDatabase::SaveItem( const char *pFilename, GameItem *pItem )
{
	GameItemSerializer temp_serializer;
	BinaryArchive_Output archive( pFilename );
	temp_serializer.pItem = pItem;
	archive << temp_serializer;
}


} // namespace amorphous
