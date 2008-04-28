#ifndef  __GameTask_ControlCustomizerFG_H__
#define  __GameTask_ControlCustomizerFG_H__


#include "GameTaskFG.h"

#include "GameInput/InputHandler.h"

#include "GameCommon/CriticalDamping.h"
#include "3DMath/Matrix34.h"
#include "Stage/fwd.h"
#include "Stage/PlayerInfo.h"
#include "UI/ui_fwd.h"

#include <vector>
#include <string>

class CInputHandler;
class CInputHandler_Dialog;


// ui ids for Task of Control Customization
enum TCC_UI_ID
{
	ID_TCC_DLG_ROOT_INPUTDEVICE_SELECT = CGameTaskFG::UIID_FG_DERIVEDTASK_OFFSET,
	ID_TCC_CONFIRM_OK,
	ID_TCC_CONFIRM_CANCEL,
	ID_TCC_ITEMSELECT_OK,
	ID_TCC_ITEMSELECT_CANCEL,
	ID_TCC_ITEMSELECT_DIALOG,
	ID_TCC_ITEMSELECT_DIALOG_BUTTON,	// not visible to the user. used to hold items select sub-dialog
	ID_TCC_GAMEPAD,
	ID_TCC_KEYBOARD,
	ID_TCC_MOUSE,
	ID_TCC_LISTBOX_GPD,
	ID_TCC_LISTBOX_KBD,
	ID_TCC_LISTBOX_MSE,
	NUM_CC_IDS
};



class CGameTask_ControlCustomizerFG : public CGameTaskFG
{
public:

	enum states
	{
		STATE_ASSIGNING_KEY,
		STATE_WAITING_INPUT,
		NUM_STATES
	};

	enum sub_menu
	{
		GPD,
		KBD,
		MSE,
		ID_TCC_INPUTDEVICE_SELECT_DIALOG,
		NUM_SUB_MENUS
	};


	enum params
	{
		NUM_INPUT_DEVICE_TYPES = NUM_SUB_MENUS
	};

private:

	CInputHandler_Dialog *m_pDialogInputHandler;
	CInputHandler *m_pKeyBindInputHandler;

	int m_State;

	/// general input code > action code
//	PlayerKeyBindFG m_PlayerKeyBind;

	CGM_DialogManagerSharedPtr m_apDialogManager[NUM_SUB_MENUS];

	CGM_ListBox *m_apItemListBox[NUM_INPUT_DEVICE_TYPES];

	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;
//	CAircraftCapsDisplay *m_pUIRendererManager;

	int m_CurrentActionIndex;

private:

	CGM_Dialog *CreateKeyBindDialog();

	void InitMenu();
//	void InitStage();
//	void UpdateCamera( float dt );

	void SetState( int state ) { m_State = state; }
	int GetState() const { return m_State; }

	void ClearPreviousGICode( int gi_code, int input_device_index );

public:

	CGameTask_ControlCustomizerFG();
	virtual ~CGameTask_ControlCustomizerFG();

	virtual void ProcessTaskTransitionRequest();

	virtual int FrameMove( float dt );
	virtual void Render();
	virtual void CreateRenderTasks();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	/// assign input code to currently selected action
	void AssignKey( int gi_code, int input_device_index );

	/// \param input_device_index gamepad: 0, keyboard: 1, mouse: 2
	void OnActionSelected( CGM_ListBoxItem& item );

	void SetNextSubMenu( int next_submenu_id );
};


#endif  /*  __GameTask_ControlCustomizerFG_H__  */
