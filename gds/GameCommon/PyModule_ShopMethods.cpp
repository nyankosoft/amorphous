#include "GameItemShop.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "PyModule_ShopMethods.hpp"


namespace amorphous
{

using boost::shared_ptr;


PyObject* AddItem( PyObject* self, PyObject* args )
{
	char *shop_name = NULL, *item_name = NULL;
	int result = PyArg_ParseTuple( args, "ss", &shop_name, &item_name );

	if( !shop_name )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	shared_ptr<GameItemShop> pShop = GetGameItemShopList().GetShop( shop_name );

	if( pShop )
	{
		shared_ptr<GameItem> pItem = ItemDatabaseManager().GetItem<GameItem>( item_name, 1 );

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





} // namespace amorphous
