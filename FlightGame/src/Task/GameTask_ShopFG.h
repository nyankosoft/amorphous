#ifndef  __GAMETASK_SHOPFG_H__
#define  __GAMETASK_SHOPFG_H__


#include "GameTaskFG.h"

#include "GameCommon/CriticalDamping.h"
#include "GameCommon/AircraftCapsDisplay.h"
#include "GameCommon/GameItemShop.h"
#include "3DMath/Matrix34.h"
#include "Stage/fwd.h"
#include "Stage/PlayerInfo.h"
#include "UI/ui_fwd.h"

#include <vector>
#include <string>

class CInputHandler_Dialog;
class CGameItem;
class CGameItemShop;
//class CBE_GeneralEntity;
class CInputHandler_Debug;


// ui ids for Task of Item Shops
enum TIS_UI_ID
{
	ID_TIS_CONFIRM_OK = CGameTaskFG::UIID_FG_DERIVEDTASK_OFFSET,
	ID_TIS_CONFIRM_CANCEL,
	ID_TIS_ITEMSELECT_OK,
	ID_TIS_ITEMSELECT_CANCEL,
	ID_TIS_ITEMSELECT_DIALOG,
	ID_TIS_ITEMSELECT_DIALOG_BUTTON,	// not visible to the user. used to hold items select sub-dialog
	ID_TIS_SPW,
	ID_TIS_GUN,
	ID_TIS_MISSILE,
	ID_TIS_LISTBOX_SPW,
	ID_TIS_LISTBOX_GUN,
	ID_TIS_LISTBOX_MISSILE,
	ID_TIS_ITEMTYPE_SELECT_DIALOG,
	NUM_TIS_IDS
};


class CPlayerCustomer : public CCustomer
{

public:

	virtual int AddItem( CGameItem* pItem )
	{
		if( !pItem )
			return 0;

//		CGameItem* pTgtItem = PLAYERINFO.GetItemByName( pItem->GetName() );
//		int orig_quantity = 0;
//		if( pTgtItem ) orig_quantity = pTgtItem->GetCurrentQuantity();

		return PLAYERINFO.SupplyItem(pItem);
	}

	virtual bool Pay( int amount )
	{
		if( PLAYERINFO.m_Money < amount )
			return false;

		PLAYERINFO.m_Money -= amount;
		return true;
	}

	virtual int GetMoneyLeft() const { return PLAYERINFO.m_Money; }

	virtual const CGameItem *GetItem( const std::string& name ) { return PLAYERINFO.GetItemByName( name.c_str() ); }
};



class CGameTask_ShopFG : public CGameTaskFG
{
public:

	enum sub_menu
	{
		SM_INVALID = -1,
		SM_AIRCRAFT_SELECT = 0,
		SM_ITEMS_SELECT,
		SM_CONFIRM,
		NUM_SUB_MENUS
	};

	enum item_type
	{
		AIRCRAFT,
		SP_AMMO,
		GUN,
		ITEM,
		NUM_ITEM_TYPES
	};

//	enum params
//	{
//	};

private:

	int m_SubMenu;
	int m_NextSubMenu;

	CGM_DialogManagerSharedPtr m_apDialogManager[NUM_SUB_MENUS];

	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;
//	CAircraftCapsDisplay *m_pUIRendererManager;

	std::vector<CGameItemShop *> m_vecpShop;

	/// items sold at this shop(??? reference)
	int m_CurrentAircraftIndex;

	/// access to ui controls (borrowed references)
	CGM_ListBox *m_apItemListBox[NUM_ITEM_TYPES];
	CGM_SubDialogButton *m_apItemButton[NUM_ITEM_TYPES];

	CAircraftCaps m_Caps;

//	CItemSpecDisplay m_ItemDisplay;

	CCopyEntity* m_pItemDisplay;	/// holds 3d model currently on display (borrowed reference)
//	CCopyEntity* m_pWeaponDisplay;
	CCopyEntity* m_apAmmoDisplay[NUM_ITEM_TYPES];	/// holds 3d model currently on display (borrowed reference)
	CCopyEntity* m_pCameraEntity;

	CCamera *m_pCamera;

	/// stage that manages scene objects for the task (owned reference)
	CStageSharedPtr m_pStage;

//	cdv<Matrix34> m_CameraPose;
	cdv<Vector3> m_CamPosition;
	cdv<Matrix33> m_CamOrient;

	CGM_Static *m_pMoneyLeft;


	void InitMenu();

	void InitStage();

	void UpdateItemDescDisplay( CGameItem& item );

	void UpdateAmmunitionDisplay( int ammo_type );

	CGM_Dialog *CreateAircraftSelectDialog();

	CGM_Dialog *CreateAircraftSelectRootMenu();

	CGM_Dialog *CreateItemSelectDialog();

	void UpdateItemListBox();

	void UpdateCamera( float dt );

	void SelectDefaultAircraft();

	void UpdatePlayerMoneyLeftDisplay();

public:

	CGameTask_ShopFG();
	virtual ~CGameTask_ShopFG();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void OnAircraftChanged( int index );

	void OnAircraftSelected();

	void OnAmmoTypeFocusChanged( int ammo_type );

	void OnItemFocusChanged( int ammo_type );

	void OnItemSelected( int item_type, const std::string& item_name );

	void SetNextSubMenu( int next_submenu_id );

	CGM_SubDialogButton *GetWeaponItemButton( int ammo_type ) { return m_apItemButton[ammo_type]; }

	friend class CInputHandler_Debug;
};


#endif  /*  __GAMETASK_SHOPFG_H__  */

