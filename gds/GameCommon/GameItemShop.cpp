#include "GameItemShop.h"
#include "PyModule_Shop.h"


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
