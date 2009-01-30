#include "GameItemShop.hpp"
#include "PyModule_Shop.hpp"


CGameItemShop::CGameItemShop()
{
	GameItemShopList.AddShop( this );
}

CGameItemShop::CGameItemShop( const std::string& name )
:
Name(name)
{
	GameItemShopList.AddShop( this );
}

CGameItemShop::~CGameItemShop()
{
	Release();
	GameItemShopList.ReleaseShop( Name );
}
