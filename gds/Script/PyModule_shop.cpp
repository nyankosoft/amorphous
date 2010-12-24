#include "PyModule_shop.hpp"
#include "GameCommon/PyModule_ShopMethods.hpp"
#include "GameCommon/GameItemShop.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/python.hpp>

using std::string;
using boost::shared_ptr;


bool (CGameItemShop::*AddItemByItemPointer)( shared_ptr<CGameItem> pItem ) = &CGameItemShop::AddItem;
int (CGameItemShop::*AddItemByNameAndQuantity)( const string& item_name, int quantity ) = &CGameItemShop::AddItem;


shared_ptr<CGameItemShop> GetGameItemShop( const string& name )
{
	return GameItemShopList().GetShop( name );
}


void AddItemToShop( shared_ptr<CGameItemShop> pShop, const string& item_name, int quantity )
{
	if( !pShop )
		return;

	pShop->AddItem( item_name, quantity );

//	shared_ptr<CGameItem> pItem = ItemDatabaseManager().GetItem<CGameItem>( item_name, 1 );

//	if( pItem )
//		pShop->AddItem( pItem );
}


BOOST_PYTHON_MODULE(shop)
{
	using namespace boost::python;
	namespace py = boost::python;

	def( "CreateGameItemShop", CreateGameItemShop );
	def( "GetGameItemShop",    GetGameItemShop );
	def( "AddItemToShop",      AddItemToShop, (py::arg("shop"), py::arg("item_name"), py::arg("quantity")) );

	class_< CGameItemShop, shared_ptr<CGameItemShop> >("GameItemShop")
//		.def( "AddItem", AddItemByItemPointer,     (py::arg("item")) )
		.def( "AddItem", AddItemByNameAndQuantity, (py::arg("item_name"), py::arg("quantity")) )
	;
}


void RegisterPythonModule_shop()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("shop", initshop) == -1)
	{
		const char *msg = "Failed to add 'shop' to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}
