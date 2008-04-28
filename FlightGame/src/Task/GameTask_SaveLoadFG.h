#ifndef  __GAMETASK_SAVELOADFG_H__
#define  __GAMETASK_SAVELOADFG_H__


#include "GameTaskFG.h"

#include "GameCommon/CriticalDamping.h"
#include "3DMath/Matrix34.h"
#include "Stage/fwd.h"
#include "UI/ui_fwd.h"
#include "UI/GM_StdControlRendererManager.h"

#include <vector>
#include <string>


enum UIID_SAVELOAD_FG
{
	UIID_SL_DLG_ROOT = 2000,
};


class CUIRender_SaveLoadFG : public CGM_StdControlRendererManager
{
public:

	CUIRender_SaveLoadFG() {}
	virtual ~CUIRender_SaveLoadFG() {}
};


class SaveDataPreviewEntryFG
{
public:

	std::string m_Title;
	std::string m_Text;

	std::string m_TotalPlayTimeHHMM;	/// total played time (HH:MM)
	unsigned long m_OwnedMoney;

	SaveDataPreviewEntryFG() : m_OwnedMoney(0) {}
};



class CGameTask_SaveLoadFG : public CGameTaskFG
{
public:

protected:

	enum eParams
	{
		MAX_NUM_SUB_MENUS = 4
	};

	int m_SubMenu;
	int m_NextSubMenu;

	CGM_DialogManagerSharedPtr m_apDialogManager[MAX_NUM_SUB_MENUS];

	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	/// access to ui controls (borrowed references)
	CGM_ListBox *m_pSaveSlotListBox;
//	CGM_SubDialogButton *m_apItemButton[3];

	std::vector<SaveDataPreviewEntryFG> m_vecPreviewEntryTable;

	int m_CurrentFocusedSlotIndex;

	/// stage that manages scene objects for the task (owned reference)
//	CStageSharedPtr m_pStage;

protected:

	void InitFonts();

	virtual CGM_Dialog *CreateSaveDataSlotListBox();

	void CreateSaveDataPreviewInfoTable();

	bool UpdatePreviewEntry( int index );

	virtual void RenderTaskMenu() = 0;

	virtual void InitMenu() {};

//	void InitStage();

public:

	CGameTask_SaveLoadFG();
	virtual ~CGameTask_SaveLoadFG();

	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void SetNextSubMenu( int next_submenu_id );

	int GetCurrentSubMenu() const { return m_SubMenu; }

	void SetSaveDataSlotFocus( int index );

};


#endif  /*  __GAMETASK_SAVELOADFG_H__  */
