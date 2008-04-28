#ifndef  __GameTask_OnMissionFailedFG_H__
#define  __GameTask_OnMissionFailedFG_H__


#include "GameTaskFG.h"
#include "UI/ui.h"

#include <vector>
#include <string>

class CInputHandler;
class CGameTask_OnMissionFailedFG;
class CBE_PlayerPseudoAircraft;


enum UI_OnMissionFailedID
{
	ID_OMF_DLG_ROOT = 1000,	// TODO: proper offset for UI control
	ID_OMF_RETRY_YES,
	ID_OMF_RETRY_NO,
	ID_OMF_QUIT_YES,
	ID_OMF_QUIT_NO,
//	ID_OMF_OPEN_SYSTEM_MENU,
	NUM_OMF_IDS
};


class CEventHandler_OnMissionFailedFG : public CGM_EventHandlerBase
{
	CGameTask_OnMissionFailedFG *m_pTask;

public:
	CEventHandler_OnMissionFailedFG( CGameTask_OnMissionFailedFG *pTask ) : m_pTask(pTask) {}
	virtual ~CEventHandler_OnMissionFailedFG() {}

	void HandleEvent( CGM_Event &event );
};


class CGameTask_OnMissionFailedFG : public CGameTaskFG
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

	int m_SubMenu;
	int m_NextSubMenu;

	CGM_DialogManagerSharedPtr m_pDialogManager;

	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	enum eMissionFailedType
	{
		MFAILED_INVALID,
		MFAILED_CRASHED,
		MFAILED_KIA,
		MFAILED_TIMEUP,
		NUM_MISSION_FAILED_TYPES
	};

	int m_MissionFailedType;

private:

	void InitMenu();

//	CGM_Dialog *CreateSubDialog0();
//	CGM_Dialog *CreateSubDialog1();

	CGM_Dialog *CreateRootMenu();

	void RenderStage();

    void CreateCamerasAndCameraController( CBE_PlayerPseudoAircraft *pAircraftBaseEntity );

public:

	CGameTask_OnMissionFailedFG();
	virtual ~CGameTask_OnMissionFailedFG();

	virtual int FrameMove( float dt );
	virtual void Render();
	virtual void CreateRenderTasks();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

//	void OnXXXChanged( int index );
};


#endif  /*  __GameTask_OnMissionFailedFG_H__  */
