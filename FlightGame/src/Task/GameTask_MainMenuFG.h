#ifndef  __GAMETASK_MAINMENU_FG_H__
#define  __GAMETASK_MAINMENU_FG_H__


#include "GameTaskFG.h"

#include "GameCommon/CriticalDamping.h"
#include "GameCommon/AircraftCapsDisplay.h"
#include "3DMath/Matrix34.h"
#include "Stage/fwd.h"
#include "UI/ui_fwd.h"

#include <vector>
#include <string>

class CInputHandler_DebugMainMenuFG;
class CGI_Aircraft;
class CGI_Ammunition;
class CInputHandler_Debug;

class StageNode;


enum TMM_UI_ID
{
	ID_TMM_DLG_ROOT = CGameTaskFG::UIID_FG_DERIVEDTASK_OFFSET,
	ID_TMM_NEXT_CONFIRM_OK,
	ID_TMM_NEXT_CONFIRM_CANCEL,
	ID_TMM_NEXT,
	ID_TMM_STAGESELECT,
	ID_TMM_SHOP,
	ID_TMM_TITLE,
	ID_TMM_SAVE,
	NUM_TMM_IDS
};

/*
class CUIRender_MainMenuFG : public CGM_StdControlRendererManager
{
public:

	CUIRender_MainMenuFG() {}
	virtual ~CUIRender_MainMenuFG() {}
};
*/

class StageInfo
{
public:
	std::string Name;
	std::string ScriptArchiveName;
	std::string Desc;
//	Vector2 world_map_pos;
//	bool cleared;

	StageInfo( const std::string& name, const std::string& script_name )
		:
	Name(name),
	ScriptArchiveName(script_name)
	{}
};


class CGameTask_MainMenuFG : public CGameTaskFG
{
public:

	enum prams
	{
		NUM_LISTBOXES = 1,
	};

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

//	CInputHandler_DebugMainMenuFG *m_pInputHandler;

//	int m_SubMenu;
//	int m_NextSubMenu;

	CGM_DialogManagerSharedPtr m_apDialogManager[1];

	/// event handler for list box (owned reference)
	/// access to ui controls (borrowed references)
	CGM_ListBox *m_apListBox[NUM_LISTBOXES];
	CGM_SubDialogButton *m_apItemButton[3];

	CGM_ControlRendererManagerSharedPtr  m_pUIRendererManager;

	std::vector<StageInfo> m_vecStage;

	std::string m_SelectedStageName;

//	int m_CurrentStageIndex;

	std::vector<StageNode *> m_vecpStageNode;

	CCopyEntity* m_pWorldMapDisplay;	/// holds 3d model currently on display (borrowed reference)

	CCopyEntity* m_pCameraEntity;

	boost::shared_ptr<CCamera> m_pCamera;

	/// stage that manages scene objects for the task (owned reference)
	CStageSharedPtr m_pStage;

//	cdv<Matrix34> m_CameraPose;
	cdv<Vector3> m_CamPosition;
	cdv<Matrix33> m_CamOrient;

private:

	void InitMenu();

	void InitStage();

	CGM_Dialog *CreateRootMenu();

	CGM_Dialog *CreateStageSelectMenu();

	void UpdateCamera( float dt );

	void RenderStageGraphSkeleton();
	void RenderStageGraphSkeleton_r( StageNode& node, int depth );

	virtual void ProcessTaskTransitionRequest();

public:

	CGameTask_MainMenuFG();
	virtual ~CGameTask_MainMenuFG();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void OnStageSelected( const std::string& stage_name );

//	void SetNextSubMenu( int next_submenu_id );

	friend class CInputHandler_DebugMainMenuFG;
	friend class CEventHandler_MainMenuFG;
};


#endif  /*  __GAMETASK_MAINMENU_FG_H__  */
