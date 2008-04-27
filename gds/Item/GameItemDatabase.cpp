
#include "Item/GameItem.h"
#include "Item/GameItemObjectFactory.h"
#include "GameItemDatabase.h"

#include <assert.h>

#include <algorithm>


class CGameItemSerializer : public IArchiveObjectBase
{
public:
	CGameItem *pItem;
//	string strFilename;

	CGameItemSerializer() { pItem = NULL; }
//	CGameItemSerializer( const char *pFilename ) { pItem = NULL; strFilename = pFilename; }
	void Serialize( IArchive& ar, const unsigned int version );
};


void CGameItemSerializer::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItemObjectFactory factory;
	ar.Polymorphic( pItem, factory );
}


CGameItem *CGameItemInfo::GetItem( const int quantity )
{
//	if( !pItem )
//	{
		CBinaryArchive_Input archive( strFilename.c_str() );
//		CGameItemObjectFactory factory;
		CGameItemSerializer temp_serializer;
		if( !(archive >> temp_serializer) )
			assert(0);	// failed to load an item file

		pItem = temp_serializer.pItem;

		pItem->m_iCurrentQuantity = quantity;
//	}

//	return pItem;

	CGameItem *pTempItem = pItem;
	pItem = NULL;
	return pTempItem;
}


void CGameItemInfo::Serialize( IArchive& ar, const unsigned int version )
{
	ar & strItemName;
	ar & strFilename;
}


CGameItemDatabase::~CGameItemDatabase()
{
	// release all the items
	int i, num_items = m_vecGameItemInfo.size();
	for( i=0; i<num_items; i++ )
	{
		m_vecGameItemInfo[i].ReleaseItem();
	}
}


void CGameItemDatabase::AddItemInfo( const string& strFilename, const string& strItemName )
{
	CGameItemInfo item_info;

	item_info.strFilename = strFilename;
	item_info.strItemName = strItemName;
	m_vecGameItemInfo.push_back( item_info );
}


inline bool ABC_Order( CGameItemInfo& ItemInfo0, CGameItemInfo& ItemInfo1 )
{
	return ItemInfo0.strItemName < ItemInfo1.strItemName;
}


void CGameItemDatabase::SortItemsInAlphabeticalOrder()
{
	std::sort( m_vecGameItemInfo.begin(), m_vecGameItemInfo.end(), ABC_Order );
}


bool CGameItemDatabase::LoadFromFile( const char *pcFilename )
{
	CBinaryArchive_Input archive( pcFilename );
	
	return archive >> (*this);
}


bool CGameItemDatabase::SaveToFile( const char *pcFilename )
{
	// sort items in the alphabetical order before saving them
	SortItemsInAlphabeticalOrder();

	CBinaryArchive_Output archive( pcFilename );

	return archive << (*this);
}


void CGameItemDatabase::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecGameItemInfo;
}



CGameItem *CGameItemDatabase::GetItem( const char *pItemName, int quantity )
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


void CGameItemDatabase::SaveItem( const char *pFilename, CGameItem *pItem )
{
	CGameItemSerializer temp_serializer;
	CBinaryArchive_Output archive( pFilename );
	temp_serializer.pItem = pItem;
	archive << temp_serializer;
}
