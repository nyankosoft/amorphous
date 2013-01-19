#ifndef  __QUICKMENU_H__
#define  __QUICKMENU_H__

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Input/InputHandler.hpp"

#include "Stage/PlayerInfo.hpp"

#include "Item/GameItem.hpp"
#include "Item/WeaponSystem.hpp"

#include <windows.h>

#include <assert.h>


namespace amorphous
{


class FontBase;
class CGameItem;

//===============================================================================
// CQM_Menu
//===============================================================================

class CQM_Menu
{
	int m_iCurrentItem;

	int m_MenuType;

	int m_iParentMenuID;

	unsigned int m_TypeFlag;

//	vector<string> m_vecstrText;	// used by sub menus

	void SetParentMenuID( int id ) { m_iParentMenuID = id; }

	void SetMenuType( int id ) { m_MenuType = id; }

	void SetTypeFlag( unsigned int flag ) { m_TypeFlag = flag; }

public:

	enum eQuickMenu
	{
		QMENU_WEAPON = 0,
		QMENU_AMMO_762X51,
		QMENU_AMMO_12GAUGE,
		QMENU_AMMO_40MMGRENADE,
		QMENU_AMMO_MISSILE,
		QMENU_AMMO_RIGIDBODY,
//		QMENU_AMMO_CAL50,
//		QMENU_GENERAL,
		QMENU_SUPPLY,
		QMENU_UTILITY,
		QMENU_KEY,
		QMENU_SUBMENU,
//		QMENU_,
		NUM_QMENUS
	};

	enum eTypeFlag
	{
		TYPE_AMMUNITION	= (1 << 0),
//		TYPE_SUBMENU	= (1 << 1),
	};

	CQM_Menu();

	inline int GetMenuType() const { return m_MenuType; }

	inline unsigned int GetTypeFlag() const { return m_TypeFlag; }

	inline int GetNumMaxItems();

	inline void GetItemInfo( int item_index, char* pItemName, int& quantity );

	inline int GetCurrentItemIndex() const { return m_iCurrentItem; }

	inline boost::shared_ptr<CGameItem> GetCurrentItem();

	inline void CursorUp();

	inline void CursorDown();

	inline int GetParentMenuID() { return m_iParentMenuID; }

	void Render( FontBase *pFont );

	friend class CQuickMenuManager;
};



//===============================================================================
// CQuickMenuManager
//===============================================================================

class CQuickMenuManager
{
	DWORD m_TimeSinceMenuOpened;

	int m_iCurrentMenu;

	CQM_Menu m_aQuickMenu[CQM_Menu::NUM_QMENUS];

	enum eQMenuKey { KEY_QMENU_CANCEL, KEY_QMENU_OK, NUM_QMENU_KEYS };

	/// toggle states of quick menu keys
	/// 1: pressed / 0: released;
	int m_aKeyState[NUM_QMENU_KEYS];

	/// select the highlighted item in the current menu
	bool SelectCurrentItem();

public:
	CQuickMenuManager();
	~CQuickMenuManager();

	void Update( float dt );

//	bool HandleMouseInput( SInputData& input );
	bool HandleMouseInput( int iActionCode, int input_type, float fVal );

	void Render( FontBase *pFont );
};



// =============================== inline implementations ===============================


inline void CQM_Menu::CursorUp()
{
	if( m_iCurrentItem == 0 )
		return;

	m_iCurrentItem--;

/*	if( GetMenuType() == CQM_Menu::QMENU_WEAPON )
	{
		// if it is the weapon select menu, update the weapon slot
		WEAPONSYSTEM.SelectWeaponSlot( m_iCurrentItem );
	}*/
}


inline void CQM_Menu::CursorDown()
{
	if( GetNumMaxItems() - 1 <= m_iCurrentItem )
		return;

	m_iCurrentItem++;

/*	if( GetMenuType() == CQM_Menu::QMENU_WEAPON )
	{
		// if it is the weapon select menu, update the weapon slot
		WEAPONSYSTEM.SetWeaponToSlot( m_iCurrentItem );
	}*/
}


static const int g_NumSubMenuTexts = 3;
static const char g_acSubMenuText[g_NumSubMenuTexts][12] =
{
	"Supply",
	"Utility",
	"Key"
};


static const int g_QMenuToItemCategoryMap[CQM_Menu::NUM_QMENUS] =
{
	CItemCategory::WEAPON,
	CItemCategory::AMMO_762X51,
	CItemCategory::AMMO_12GAUGE,
	CItemCategory::AMMO_40MMGRENADE,
	CItemCategory::AMMO_MISSILE,
	CItemCategory::AMMO_RIGIDBODY,
//	CItemCategory::AMMO_CAL50,
	CItemCategory::SUPPLY,
	CItemCategory::UTILITY,
	CItemCategory::KEY
//	CItemCategory::,

};


inline void CQM_Menu::GetItemInfo( int item_index, char* pItemName, int& quantity )
{
	if( GetMenuType() == QMENU_SUBMENU )
	{
		strcpy( pItemName, g_acSubMenuText[item_index] );
		return;
	}
	else if( GetMenuType() == QMENU_WEAPON )
	{
		SWeaponSlot& rWeaponSlot = SinglePlayerInfo().GetWeaponSystem()->GetWeaponSlot( item_index );
		if( rWeaponSlot.pWeapon )
            sprintf( pItemName, "%d %s", item_index, rWeaponSlot.pWeapon->GetName().c_str() );
		else
			strcpy( pItemName, " " );

	}
	else
	{
		boost::shared_ptr<CGameItem> pItem = SinglePlayerInfo().GetCategoryItemList( g_QMenuToItemCategoryMap[m_MenuType] )[item_index];

		assert( strlen(pItem->GetName().c_str()) <= 32 );

		strcpy( pItemName, pItem->GetName().c_str() );
		quantity = pItem->GetCurrentQuantity();
	}
}


inline boost::shared_ptr<CGameItem> CQM_Menu::GetCurrentItem()
{
	if( m_MenuType == QMENU_SUBMENU )
	{
		return boost::shared_ptr<CGameItem>();
	}
	else if( GetMenuType() == QMENU_WEAPON )
	{
		return boost::shared_ptr<CGameItem>();
//		SinglePlayerInfo().GetWeaponSystem()->GetWeaponSlot( m_iCurrentItem ).pWeapon;
	}
	else
	{
		std::vector< boost::shared_ptr<CGameItem> >& rvecpItemList = SinglePlayerInfo().GetCategoryItemList( g_QMenuToItemCategoryMap[m_MenuType] );
		if( m_iCurrentItem < (int)rvecpItemList.size() )	// check if the item index is valid
			return rvecpItemList[m_iCurrentItem];
		else
			return boost::shared_ptr<CGameItem>();
	}

	return boost::shared_ptr<CGameItem>();
}


inline int CQM_Menu::GetNumMaxItems()
{
	if( m_MenuType == QMENU_SUBMENU )
	{
		return g_NumSubMenuTexts;
	}
	else
	{
		return (int)SinglePlayerInfo().GetCategoryItemList( g_QMenuToItemCategoryMap[m_MenuType] ).size();
	}
}


inline void CQuickMenuManager::Render( FontBase *pFont )
{
	if( 0 <= m_iCurrentMenu )
		m_aQuickMenu[m_iCurrentMenu].Render( pFont );
}

} // namespace amorphous



#endif		/*  __QUICKMENU_H__  */