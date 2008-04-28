#ifndef  __GAMETASK_LOADFG_H__
#define  __GAMETASK_LOADFG_H__


#include "GameTask_SaveLoadFG.h"

#include "GameCommon/CriticalDamping.h"
#include "3DMath/Matrix34.h"
#include "UI/ui_fwd.h"

#include <vector>
#include <string>

class CInputHandler_Dialog;


enum TASK_LOAD_UI_ID
{
	ID_UI_LOAD_BACK,
	ID_UI_LOAD_NEXT,
	ID_UI_LOAD_PREVPAGE,
	ID_UI_LOAD_NEXTPAGE,
	ID_UI_LOAD_CONFIRM_YES,
	ID_UI_LOAD_CONFIRM_NO,
	NUM_LOAD_UI_IDS
};


class CGameTask_LoadFG : public CGameTask_SaveLoadFG
{
public:

	enum sub_menu
	{
		SM_INVALID = -1,
		SM_SELECT_SAVESLOT = 0,
		SM_CONFIRM_LOAD,
		NUM_SUB_MENUS
	};


protected:

/*

	CUIRender_SaveFG *m_pUIRendererManager;
*/

protected:

	virtual void InitMenu();

//	CGM_Dialog *CreateRootMenu();

	CGM_Dialog *CreateLoadConfirmationDialog();

	virtual void RenderTaskMenu();

public:

	CGameTask_LoadFG();
	virtual ~CGameTask_LoadFG();

	virtual int FrameMove( float dt );
//	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	// \param -1 > load from the currently selected slot
	bool LoadSavedState( int index = -1 );
};


#endif  /*  __GAMETASK_LOADFG_H__  */
