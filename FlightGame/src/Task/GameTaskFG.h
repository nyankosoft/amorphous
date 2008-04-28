#ifndef  __GameTaskFG_H__
#define  __GameTaskFG_H__


#include "Task/GameTask.h"
#include "UI/ui_fwd.h"


class CGameTaskFG : public CGameTask
{
protected:

	void LoadFonts( CGM_ControlRendererManagerSharedPtr pControlRenedererMgr );

	void SetSounds( CGM_DialogManagerSharedPtr pDialogManager );

	void DoCommonInit( CGM_DialogManagerSharedPtr pDialogManager );

public:

	enum eGameTaskFG
	{
		ID_TITLE_FG = CGameTask::USER_GAMETASK_ID_OFFSET,
		ID_AIRCRAFT_SELECT,
		ID_STAGE_FG,
		ID_INSTAGEMENU_FG,
		ID_ON_MISSIONFAILED_FG,
		ID_BRIEFING_FG,
		ID_DEBRIEFING_FG,
		ID_MAINMENU_FG,
		ID_SHOP_FG,
		ID_CONTROLCUSTOMIZER_FG,
		ID_LOAD_FG,
		ID_SAVE_FG,
		ID_ASYNCSTAGELOADER_FG,
		NUM_GAMETASK_IDS
	};

	enum eParams
	{
		UIID_FG_DERIVEDTASK_OFFSET = 5000
	};
};


#endif		/*  __GameTaskFG_H__  */
