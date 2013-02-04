
#include "QuickMenuManager.hpp"

#include "Graphics/Font/FontBase.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"

#include "GameCommon/3DActionCode.hpp"

#include "Item/WeaponSystem.hpp"


namespace amorphous
{

using boost::shared_ptr;


//--------------------------------------------------------------------
// the following 5 input codes are used for quick menu operation
// 	ACTION_QMENU_UP
//	ACTION_QMENU_DOWN
//	ACTION_QMENU_OPEN
//	ACTION_ATK_FIRE
//	ACTION_ATK_RAISEWEAPON (ACTION_ATK_UNLOCK_TRIGGER_SAFETY)	(key state is stored in m_aKeyState[KEY_QMENU_CANCEL] )
//--------------------------------------------------------------------


CQM_Menu::CQM_Menu()
{
	m_iCurrentItem = 0;
	m_MenuType = -1;
	m_iParentMenuID = -1;

	m_TypeFlag = 0;
}


void CQM_Menu::Render( FontBase *pFont )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// blend texture color and diffuse color
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// draw a background rect
	C2DRect bg_rect;
	bg_rect.SetColor( 0x60000000 );
	bg_rect.SetPosition( Vector2(350,380), Vector2(450,460) );
	bg_rect.Draw();

	// draw item titles
	int start_index = take_max( m_iCurrentItem - 2, 0 );
	int end_index = take_min( GetNumMaxItems() - 1, start_index + 4 );

	float offset_x = 0;
	Vector2 vPos = Vector2(350 + offset_x, 380);
	int text_height = pFont->GetFontHeight();
	DWORD dwColor;

    int current_item_index = m_iCurrentItem;

	char acItemName[36];
	int quantity, num_digits;
	char acQuantity[12];

	int i;
	for( i=start_index; i<=end_index; i++ )
	{
		if( i == current_item_index )
			dwColor = 0xFF40FF40;
		else
			dwColor = 0xFF00CC00;

		GetItemInfo( i, acItemName, quantity );

		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		// draw the name of the item
		pFont->DrawText( acItemName, vPos, dwColor );

		if( GetMenuType() != CQM_Menu::QMENU_SUBMENU &&
			GetMenuType() != CQM_Menu::QMENU_WEAPON )
		{
			// display how many items are left
			sprintf( acQuantity, "%d", quantity );
			num_digits = strlen(acQuantity);

			pFont->DrawText( acQuantity, vPos + Vector2( 100 - num_digits * 8, 0 ), dwColor );

		}

		vPos.y += text_height;
	}
}


CQuickMenuManager::CQuickMenuManager()
{
	m_TimeSinceMenuOpened = 0;

	m_iCurrentMenu = -1;

	for( int i=0; i<NUM_QMENU_KEYS; i++ )
		m_aKeyState[i] = 0;

//	m_aQuickMenu[CQM_Menu::QMENU_WEAPON]
	m_aQuickMenu[CQM_Menu::QMENU_SUPPLY].SetParentMenuID( CQM_Menu::QMENU_SUBMENU );
	m_aQuickMenu[CQM_Menu::QMENU_UTILITY].SetParentMenuID( CQM_Menu::QMENU_SUBMENU );
	m_aQuickMenu[CQM_Menu::QMENU_KEY].SetParentMenuID( CQM_Menu::QMENU_SUBMENU );

	// set menu id for each quick menu
	for( int i=0; i<CQM_Menu::NUM_QMENUS; i++ )
		m_aQuickMenu[i].SetMenuType( i );

	m_aQuickMenu[CQM_Menu::QMENU_AMMO_12GAUGE].SetTypeFlag( CQM_Menu::TYPE_AMMUNITION );
	m_aQuickMenu[CQM_Menu::QMENU_AMMO_762X51].SetTypeFlag( CQM_Menu::TYPE_AMMUNITION );
	m_aQuickMenu[CQM_Menu::QMENU_AMMO_40MMGRENADE].SetTypeFlag( CQM_Menu::TYPE_AMMUNITION );
	m_aQuickMenu[CQM_Menu::QMENU_AMMO_MISSILE].SetTypeFlag( CQM_Menu::TYPE_AMMUNITION );
	m_aQuickMenu[CQM_Menu::QMENU_AMMO_RIGIDBODY].SetTypeFlag( CQM_Menu::TYPE_AMMUNITION );
//	m_aQuickMenu[CQM_Menu::QMENU_AMMO_CAL50].SetTypeFlag( CQM_Menu::TYPE_AMMUNITION );
}


CQuickMenuManager::~CQuickMenuManager()
{
}


bool CQuickMenuManager::SelectCurrentItem()
{
	// select the highlighted item in the current menu
	if( m_iCurrentMenu == CQM_Menu::QMENU_SUBMENU )
	{
		// open a child menu
		const int submenu_index[3] = { CQM_Menu::QMENU_SUPPLY, CQM_Menu::QMENU_UTILITY, CQM_Menu::QMENU_KEY };
		int index = m_aQuickMenu[m_iCurrentMenu].GetCurrentItemIndex();
		m_iCurrentMenu = submenu_index[index];
	}
	else if( m_iCurrentMenu == CQM_Menu::QMENU_WEAPON )
	{
		// select the highlighted weapon slot
		SinglePlayerInfo().GetWeaponSystem()->SelectPrimaryWeapon( m_aQuickMenu[m_iCurrentMenu].GetCurrentItemIndex() );
		// close weapon select menu
		m_iCurrentMenu = -1;
	}
	else if( m_aQuickMenu[m_iCurrentMenu].GetTypeFlag() & CQM_Menu::TYPE_AMMUNITION )
	{
		// select the highlighted weapon slot
		SinglePlayerInfo().GetWeaponSystem()->GetPrimaryWeaponSlot().Load( (CGI_Ammunition *)m_aQuickMenu[m_iCurrentMenu].GetCurrentItem().get() );
		// close ammo select menu
		m_iCurrentMenu = -1;
	}
	else
	{
		// select the current item
		shared_ptr<GameItem> pItem = m_aQuickMenu[m_iCurrentMenu].GetCurrentItem();
		if( pItem )
		{
			pItem->OnSelected();
			return true;
		}
		else
			return false;
	}
	return true;
}


// select a list of ammo which has the same caliber as the current weapon
int OpenAmmoSelectMenu()
{
	const std::string& ammo_type = SinglePlayerInfo().GetWeaponSystem()->GetPrimaryWeaponSlot().pWeapon->GetAmmoType();
	if( ammo_type == "7.62x51")		return CQM_Menu::QMENU_AMMO_762X51;
	else if( ammo_type == "12GS")	return CQM_Menu::QMENU_AMMO_12GAUGE;
	else if( ammo_type == "40MMG")	return CQM_Menu::QMENU_AMMO_40MMGRENADE;
	else if( ammo_type == "MSSL")	return CQM_Menu::QMENU_AMMO_MISSILE;
	else if( ammo_type == "RBODY")	return CQM_Menu::QMENU_AMMO_RIGIDBODY;
//	else if( ammo_type == "CAL50") )
//		current_menu_id = CQM_Menu::QMENU_AMMO_;
	else
		return -1;	// an unregistered type of ammo
}


//bool CQuickMenuManager::HandleMouseInput( SInputData& input )
bool CQuickMenuManager::HandleMouseInput( int iActionCode, int input_type, float fVal )
{

//	switch( iGICode )
	switch( iActionCode )
	{
	case ACTION_QMENU_UP:
	case ACTION_QMENU_DOWN:
		if( m_iCurrentMenu == -1 )
		{
			// open a quick menu
			if( m_aKeyState[KEY_QMENU_CANCEL] )
			{
				// open ammo select menu
				m_iCurrentMenu = OpenAmmoSelectMenu();
				if( m_iCurrentMenu == -1 )
					return false;
			}
			else
			{
				// open weapon select menu
				m_iCurrentMenu = CQM_Menu::QMENU_WEAPON;
			}
		}
		if( iActionCode == ACTION_QMENU_UP )
			m_aQuickMenu[m_iCurrentMenu].CursorUp();
		else
			m_aQuickMenu[m_iCurrentMenu].CursorDown();
		return true;

	case ACTION_QMENU_OPEN:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			if( m_iCurrentMenu == -1 )
			{
				// open a quick menu
				if( m_aKeyState[KEY_QMENU_CANCEL] )
				{
					m_iCurrentMenu = OpenAmmoSelectMenu();
					if( m_iCurrentMenu == -1 )
						return false;
				}
				else
				{
					m_iCurrentMenu = CQM_Menu::QMENU_SUBMENU;
					return true;
				}
			}
			else
			{
				SelectCurrentItem();
				return true;
			}
		}
		break;

	case ACTION_ATK_FIRE:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			if( m_iCurrentMenu == -1 )
				return false;	// no menu is open

			SelectCurrentItem();
			return true;
		}
		break;

//	case ACTION_QMENU_CANCEL:
	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aKeyState[KEY_QMENU_CANCEL] = 1;
			if( 0 <= m_iCurrentMenu )
			{
				// close the current menu
				int parent_menu_id = m_aQuickMenu[m_iCurrentMenu].GetParentMenuID();
				if( 0 <= parent_menu_id )
				{
					m_iCurrentMenu = parent_menu_id;
				}
				else
				{
					m_iCurrentMenu = -1;
				}

				return true;
			}
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aKeyState[KEY_QMENU_CANCEL] = 0;
		}
		break;
	}

	return false;
}


void CQuickMenuManager::Update( float dt )
{
	m_TimeSinceMenuOpened += dt;
}


} // namespace amorphous
