
#include "GameItemShop.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "PyModule_Shop.hpp"

using namespace std;

CSingleton<CGameItemShopList> CGameItemShopList::m_obj;


bool CGameItemShopList::AddShop( CGameItemShop* pShop )
{
	if( !pShop )
		return false;

	vector<CGameItemShop *>::iterator shop;

	for( shop = m_vecpShopList.begin();
		 shop != m_vecpShopList.end();
		 shop++ )
	{
		if( (*shop)->Name == pShop->Name )
			return false;	// already registered
	}

	m_vecpShopList.push_back( pShop );
	return true;
}


bool CGameItemShopList::ReleaseShop( const string& name )
{
	vector<CGameItemShop *>::iterator shop;
	for( shop = m_vecpShopList.begin();
		 shop != m_vecpShopList.end();
		 shop++ )
	{
		if( (*shop)->Name == name )
		{
			m_vecpShopList.erase( shop );
			return true;
		}
	}

	return false;
}


CGameItemShop *CGameItemShopList::GetShop( const string& name )
{
	vector<CGameItemShop *>::iterator shop;
	for( shop = m_vecpShopList.begin();
		 shop != m_vecpShopList.end();
		 shop++ )
	{
		if( (*shop)->Name == name )
			return *shop;
	}

	return NULL;
}



PyObject* AddItem( PyObject* self, PyObject* args )
{
	char *shop_name = NULL, *item_name = NULL;
	int result = PyArg_ParseTuple( args, "ss", &shop_name, &item_name );

	if( !shop_name )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	CGameItemShop* pShop = GameItemShopList.GetShop( shop_name );

	if( pShop )
	{
		CGameItem* pItem = ItemDatabaseManager().GetItemRawPtr( item_name, 1 );

		if( pItem )
			pShop->AddItem( pItem );
	}

	Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef g_PyModuleShopMethod[] =
{
	{ "AddItem",				AddItem,				METH_VARARGS, "add an item to a shop" },
	{ NULL, NULL }
};



