#include "PyModule_shop.hpp"
#include "GameCommon/PyModule_ShopMethods.hpp"
#include "GameCommon/GameItemShop.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/python.hpp>


namespace amorphous
{

using std::string;
using boost::shared_ptr;


bool (GameItemShop::*AddItemByItemPointer)( shared_ptr<GameItem> pItem ) = &GameItemShop::AddItem;
int (GameItemShop::*AddItemByNameAndQuantity)( const string& item_name, int quantity ) = &GameItemShop::AddItem;


shared_ptr<GameItemShop> GetGameItemShop( const string& name )
{
	return GetGameItemShopList().GetShop( name );
}


void AddItemToShop( shared_ptr<GameItemShop> pShop, const string& item_name, int quantity )
{
	if( !pShop )
		return;

	pShop->AddItem( item_name, quantity );

//	shared_ptr<GameItem> pItem = ItemDatabaseManager().GetItem<GameItem>( item_name, 1 );

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

	class_< GameItemShop, shared_ptr<GameItemShop> >("GameItemShop")
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


} // namespace amorphous
