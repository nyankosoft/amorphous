#ifndef  __GAMETASK_INSTAGEMENU_H__
#define  __GAMETASK_INSTAGEMENU_H__


#include "GameTaskFG.h"

#include "Stage/Stage.h"
#include "UI/ui_fwd.h"
#include "UI/ui.h"

//#include "GameCommon/CriticalDamping.h"
//#include "3DMath/Matrix34.h"

#include <vector>
#include <string>

class CInputHandler;
class CInputHandler_Debug;
class CGameTask_InStageMenuFG;


enum UI_InStageMenuID
{
	ID_ISM_DLG_ROOT = CGameTaskFG::UIID_FG_DERIVEDTASK_OFFSET,
	ID_ISM_RETRY_YES,
	ID_ISM_RETRY_NO,
	ID_ISM_QUIT_YES,
	ID_ISM_QUIT_NO,
	ID_ISM_RETURN_TO_STAGE,
	ID_ISM_OPEN_SYSTEM_MENU,
	ID_ISM_CUSTOMIZE_CONTROLS,
	NUM_ISM_IDS
};


class CEventHandler_InStageMenuFG : public CGM_EventHandlerBase
{
	CGameTask_InStageMenuFG *m_pTask;

public:
	CEventHandler_InStageMenuFG( CGameTask_InStageMenuFG *pTask ) : m_pTask(pTask) {}
	virtual ~CEventHandler_InStageMenuFG() {}

	void HandleEvent( CGM_Event &event );
};


class CGameTask_InStageMenuFG : public CGameTaskFG
{
public:

	enum sub_menu
	{
		SM_INVALID = -1,
		SM_SELECT1 = 0,
		SM_SELECT2,
		SM_CONFIRM,
		NUM_SUB_MENUS
	};

private:

	CGM_DialogManagerSharedPtr m_pDialogManager;

    CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	/// stage that manages scene objects for the task (owned reference)
//	CStageSharedPtr m_pStage;

private:

	void InitMenu();

	CGM_Dialog *CreateRootMenu();

//	void UpdateCamera( float dt );

public:

	CGameTask_InStageMenuFG();
	virtual ~CGameTask_InStageMenuFG();

	virtual int FrameMove( float dt );
	virtual void Render();
	virtual void CreateRenderTasks();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif  /*  __GAMETASK_INSTAGEMENU_H__  */
