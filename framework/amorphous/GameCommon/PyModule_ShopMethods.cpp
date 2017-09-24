#include "PyModule_ShopMethods.hpp"
#include "GameItemShop.hpp"
#include "amorphous/Item/ItemDatabaseManager.hpp"
#include "amorphous/Script/PythonScriptManager.hpp"


namespace amorphous
{

using std::shared_ptr;


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
		shared_ptr<GameItem> pItem = GetItemDatabaseManager().GetItem<GameItem>( item_name, 1 );

		if( pItem )
			pShop->AddItem( pItem );
	}

	Py_INCREF( Py_None );
	return Py_None;
}


static PyMethodDef sg_PyModuleShopMethod[] =
{
	{ "AddItem",				AddItem,				METH_VARARGS, "add an item to a shop" },
	{ NULL, NULL }
};


void RegisterPythonModule_Shop( PythonScriptManager& mgr )
{
	mgr.AddModule( "Shop", sg_PyModuleShopMethod );
}


} // namespace amorphous
