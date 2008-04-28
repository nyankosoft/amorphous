#ifndef  __GAMETASK_DEBRIEFINGFG_H__
#define  __GAMETASK_DEBRIEFINGFG_H__


#include "GameTaskFG.h"

#include "GameCommon/CriticalDamping.h"
#include "GameCommon/AircraftCapsDisplay.h"
#include "3DMath/Matrix34.h"
#include "Stage/fwd.h"
#include "UI/ui_fwd.h"

#include <vector>
#include <string>

class CGI_Aircraft;
class CTextureFont;


enum DB_UI_ID
{
	ID_DB_BACK,
	ID_DB_NEXT,
//	ID_DB_RETRY,
	NUM_DB_IDS
};

/*
class CUIRender_DebriefingFG : public CGM_StdControlRendererManager
{
public:

	CUIRender_DebriefingFG() {}
	virtual ~CUIRender_DebriefingFG() {}
};
*/

class CGameTask_DebriefingFG : public CGameTaskFG
{
public:

	enum sub_menu
	{
/*		SM_INVALID = -1,
		SM_AIRCRAFT_SELECT = 0,
		SM_ITEMS_SELECT,
		SM_CONFIRM,
*/		NUM_SUB_MENUS = 1
	};


private:

	CTextureFont* m_pFont;

	int m_SubMenu;
	int m_NextSubMenu;

	CGM_DialogManagerSharedPtr m_apDialogManager[NUM_SUB_MENUS];

//	CUIRender_DebriefingFG *m_pUIRendererManager;
	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	/// access to ui controls (borrowed references)
	CGM_ListBox *m_apItemListBox[3];
	CGM_SubDialogButton *m_apItemButton[3];

	int m_CurrentAmmoType;

	CCopyEntity* m_pWorldMapDisplay;	/// holds 3d model currently on display (borrowed reference)

	CCopyEntity* m_pCamera;

	/// stage that manages scene objects for the task (owned reference)
	CStageSharedPtr m_pStage;

//	cdv<Matrix34> m_CameraPose;
	cdv<Vector3> m_CamPosition;
	cdv<Matrix33> m_CamOrient;

private:

	void InitMenu();

	void InitStage();

	CGM_Dialog *CreateRootMenu();

	CGM_Dialog *CreateAircraftSelectDialog();
	CGM_Dialog *CreateAmmoSelectDialog();
	CGM_Dialog *CreateAmmoSelectRootMenu();

	void UpdateCamera( float dt );

    void GetCombatResults();

public:

	CGameTask_DebriefingFG();
	virtual ~CGameTask_DebriefingFG();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void SetNextSubMenu( int next_submenu_id );

	virtual void RequestTransitionToNextTask();

	friend class CInputHandler_DebugMainMenuFG;
};


#endif  /*  __GAMETASK_DEBRIEFINGFG_H__  */
