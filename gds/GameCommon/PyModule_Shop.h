#ifndef __PYMODULE_GAMEITEMSHOP_H__
#define __PYMODULE_GAMEITEMSHOP_H__


#include <vector>
#include <string>
#include <Python.h>

#include "Support/Singleton.h"
using namespace NS_KGL;


extern PyMethodDef g_PyModuleShopMethod[];


class CGameItemShop;
//extern bool AddShop( CGameItemShop* pShop )
//extern bool ReleaseShop( const std::string& name )


#define GameItemShopList	(*(CGameItemShopList::Get()))

/**
 * singleton class
 */
class CGameItemShopList
{
	static CSingleton<CGameItemShopList> m_obj;

	std::vector<CGameItemShop *> m_vecpShopList;

public:

	static CGameItemShopList* Get() { return m_obj.get(); }

	CGameItemShopList() {}

	~CGameItemShopList() { Release(); }

	void Release() {}

	bool AddShop( CGameItemShop* pShop );

	bool ReleaseShop( const std::string& name );

	CGameItemShop *GetShop( const std::string& name );
};


#endif  /*  __PYMODULE_GAMEITEMSHOP_H__  */