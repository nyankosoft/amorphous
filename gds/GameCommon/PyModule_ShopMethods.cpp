#include "GameItemShop.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "PyModule_ShopMethods.hpp"

using namespace std;
using namespace boost;


PyObject* AddItem( PyObject* self, PyObject* args )
{
	char *shop_name = NULL, *item_name = NULL;
	int result = PyArg_ParseTuple( args, "ss", &shop_name, &item_name );

	if( !shop_name )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	shared_ptr<CGameItemShop> pShop = GameItemShopList().GetShop( shop_name );

	if( pShop )
	{
		shared_ptr<CGameItem> pItem = ItemDatabaseManager().GetItem<CGameItem>( item_name, 1 );

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



