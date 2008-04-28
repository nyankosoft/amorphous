#ifndef  __GAMETASK_BRIEFINGFG_H__
#define  __GAMETASK_BRIEFINGFG_H__


#include "GameTaskFG.h"

#include "GameCommon/CriticalDamping.h"
#include "GameCommon/AircraftCapsDisplay.h"
#include "3DMath/Matrix34.h"
#include "Stage/fwd.h"
#include "UI/ui_fwd.h"

#include <vector>
#include <string>

class CGI_Aircraft;


extern void SetBriefingStageScriptFilename( const std::string& filename );


enum TB_UI_ID
{
	ID_BR_BACK,
	ID_BR_NEXT,
	NUM_TB_IDS
};

/*
class CUIRender_BriefingFG : public CGM_StdControlRendererManager
{
public:

	CUIRender_BriefingFG() {}
	virtual ~CUIRender_BriefingFG() {}
};
*/

class CGameTask_BriefingFG : public CGameTaskFG
{
public:

/*	enum sub_menu
	{
		SM_INVALID = -1,
		SM_AIRCRAFT_SELECT = 0,
		SM_ITEMS_SELECT,
		SM_CONFIRM,
		NUM_SUB_MENUS
	};
*/

private:

	int m_SubMenu;
	int m_NextSubMenu;

	CGM_DialogManagerSharedPtr m_apDialogManager[1];

//	CUIRender_BriefingFG *m_pUIRendererManager;
	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	/// access to ui controls (borrowed references)
	CGM_ListBox *m_apItemListBox[3];
	CGM_SubDialogButton *m_apItemButton[3];

	int m_CurrentFocusedUnit;

	CCopyEntity* m_pWorldMapDisplay;	/// holds 3d model currently on display (borrowed reference)

	std::vector<CCopyEntity *> m_vecpIconEntity;

	/// stage that manages scene objects for the task (owned reference)
	CStageSharedPtr m_pStage;

	CCopyEntity* m_pCamera;

//	cdv<Matrix34> m_CameraPose;
	cdv<Vector3> m_CamPosition;
	cdv<Matrix33> m_CamOrient;

private:

	void InitMenu();

	void InitStage();

	CGM_Dialog *CreateRootMenu();

	CGM_Dialog *CreateAircraftSelectDialog();

	void UpdateCamera( float dt );

public:

	CGameTask_BriefingFG();
	virtual ~CGameTask_BriefingFG();

	virtual void RequestTransitionToNextTask();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void OnUnitFocusChanged( int index );

	void OnEnterPressed();
	void OnAircraftSelected();
	void OnAmmunitionSelected( int ammo_type, const std::string& ammo_name );

	void SetNextSubMenu( int next_submenu_id );

	CGM_SubDialogButton *GetWeaponItemButton( int ammo_type ) { return m_apItemButton[ammo_type]; }

	friend class CEventHandler_Briefing;
};


#endif  /*  __GAMETASK_BRIEFINGFG_H__  */
