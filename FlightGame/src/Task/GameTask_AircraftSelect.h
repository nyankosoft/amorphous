#ifndef  __GAMETASK_AIRCRAFTSELECT_H__
#define  __GAMETASK_AIRCRAFTSELECT_H__


#include "GameTaskFG.h"

#include "GameCommon/CriticalDamping.h"
#include "GameCommon/AircraftCapsDisplay.h"
#include "3DMath/Matrix34.h"
#include "Stage/fwd.h"
#include "UI/ui_fwd.h"

#include "3DCommon/RenderTask.h"

#include <vector>
#include <string>

class CGI_Aircraft;
class CGI_Ammunition;
class CInputHandler_Debug;
class CAircraftCapsDisplay;


// called from stage select task when skipping aircraft select task for debugging
extern void SupplyAircraftItemsToPlayer();


enum TAS_UI_ID
{
	ID_TAS_CONFIRM_OK = CGameTaskFG::UIID_FG_DERIVEDTASK_OFFSET,
	ID_TAS_DLG_ROOT_AIRCRAFTSELECT,
	ID_TAS_DLG_ROOT_AMMOSELECT,
	ID_TAS_DLG_ROOT_CONFIRM,
	ID_TAS_DLG_AIRCRAFTSELECT,
	ID_TAS_LBX_AIRCRAFTSELECT,
	ID_TAS_DLG_AMMOSELECT,
	ID_TAS_SDB_AIRCRAFTSELECT,
	ID_TAS_SDB_AMMOSELECT,
	ID_TAS_CONFIRM_CANCEL,
	ID_TAS_ITEMSELECT_OK,
	ID_TAS_ITEMSELECT_CANCEL,
	ID_TAS_ITEMSELECT_DIALOG_BUTTON,	// not visible to the user. used to hold items select sub-dialog
	ID_TAS_GUN,
	ID_TAS_MISSILE,
	ID_TAS_SPW,
	ID_TAS_LISTBOX_GUN,
	ID_TAS_LISTBOX_MISSILE,
	ID_TAS_LISTBOX_SPW,
	NUM_TAS_IDS

/*	ID_TAS_CAPS_NAME,
	ID_TAS_CAPS_DESC,

	ID_TAS_CAPS_SPEED,
	ID_TAS_CAPS_MANEUVERABILITY,
	ID_TAS_CAPS_AIR_TO_AIR,
	ID_TAS_CAPS_AIR_TO_G,*/
};


class CGameTask_AircraftSelect : public CGameTaskFG
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

	enum params
	{
		AIRCRAFT_BUTTON_ID_OFFSET = 100
	};

	enum ammo_types
	{
		AMMO_BULLET,
		AMMO_MISSILE,
		AMMO_SPW,
		NUM_AMMO_TYPES
	};

private:

//	CInputHandler_Debug *m_pInputHandler;

	int m_SubMenu;
	int m_NextSubMenu;

	CGM_DialogManagerSharedPtr m_pDialogManager;

//	CAircraftCapsDisplay *m_pUIRendererManager;
	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	/// list of aircrafts currently owned by the player (borrowed reference)
	std::vector<CGI_Aircraft *> m_vecpPlayerAircraft;
	int m_CurrentAircraftIndex;

	CGM_Dialog *m_apRootDialog[NUM_SUB_MENUS];

	/// access to ui controls (borrowed references)
	CGM_ListBox *m_apItemListBox[NUM_AMMO_TYPES];
	CGM_SubDialogButton *m_apItemButton[NUM_AMMO_TYPES];

	CAircraftCaps m_Caps;

	std::vector<CGI_Ammunition *> m_vecpBullet;
	std::vector<CGI_Ammunition *> m_vecpMissile;
	std::vector<CGI_Ammunition *> m_vecpSpAmmo;

	int m_CurrentAmmoType;

//	CItemSpecDisplay m_ItemDisplay;

//	CBE_PlayerPseudoAircraft* m_pAircraft;	// holds 3d model currently on display (borrowed reference)

//	CBE_GeneralEntity* m_pAircraft;	// holds 3d model currently on display (borrowed reference)
//	CBE_GeneralEntity* m_pWeapon;	// holds 3d model currently on display (borrowed reference)
//	CBE_GeneralEntity* m_pCam;
	CCopyEntity* m_pAircraftDisplay;	/// holds 3d model currently on display (borrowed reference)
//	CCopyEntity* m_pWeaponDisplay;
	CCopyEntity* m_apAmmoDisplay[NUM_AMMO_TYPES];	/// holds 3d model currently on display (borrowed reference)
	CCopyEntity* m_pCameraEntity;

	/// stage that manages scene objects for the task (owned reference)
	CStageSharedPtr m_pStage;

//	cdv<Matrix34> m_CameraPose;
	cdv<Vector3> m_CamPosition;
	cdv<Matrix33> m_CamOrient;

	CCamera *m_pCamera;

private:

	void InitMenu();

	void InitStage();

	void UpdateAmmoDescDisplay( CGI_Ammunition& ammo );

	void UpdateAmmunitionDisplay( int ammo_type );

	CGM_Dialog *CreateAircraftSelectDialog();

	CGM_Dialog *CreateAircraftSelectRootMenu();

	CGM_Dialog *CreateAmmoSelectDialog();

	CGM_Dialog *CreateAmmoSelectRootMenu();

	void UpdateCamera( float dt );

	void SelectDefaultAircraft();

	void ChangeSubMenu();

	virtual void CreateRenderTasks();

public:

	CGameTask_AircraftSelect();
	virtual ~CGameTask_AircraftSelect();

	virtual int FrameMove( float dt );
	virtual void Render();

//	virtual void RequestTransitionToNextTask() { RequestTaskTransition( ID_GLOBALSTAGELOADER ); }
	virtual void RequestTransitionToNextTask() { RequestTaskTransition( ID_ASYNCSTAGELOADER_FG ); }

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void OnAircraftChanged( const std::string& aircraft_name );

	void OnAircraftChanged( int index );

	void OnAircraftSelected();

	void OnAmmoTypeFocusChanged( int ammo_type );

	void OnAmmoFocusChanged( int ammo_type );

	void OnAmmunitionSelected( int ammo_type, const std::string& ammo_name );

	void SetNextSubMenu( int next_submenu_id );

	int GetNextSubMenu() const { return m_NextSubMenu; }

	CGM_SubDialogButton *GetWeaponItemButton( int ammo_type ) { return m_apItemButton[ammo_type]; }

	friend class CInputHandler_Debug;
};


#endif  /*  __GAMETASK_AIRCRAFTSELECT_H__  */
