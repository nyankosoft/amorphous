#ifndef  __GAMETASK_MAINMENU_H__
#define  __GAMETASK_MAINMENU_H__


#include "GameTask.h"
#include "MainMenuEventHandler.h"
#include "UI/ui_fwd.h"


class CInputHandler;
class CFont;


enum eUIControlID
{
	ID_UI_DLG_ROOT = 10000,
	ID_UI_CONFIG_CONTROL,
	ID_UI_EXIT,
	ID_UI_RESUME_GAME,

	ID_UI_SAVE,
	ID_UI_LOAD,
	ID_UI_CONTROL,
	ID_UI_SETTINGS,
	ID_UI_DEBUG,

    ID_UI_VIDEO_OPTIONS,
	ID_UI_VOP_RESOLUTION,
	ID_UI_VOP_EFFECT,

	ID_UI_SOUND_OPTIONS,

	ID_UI_RESOLUTION_640X480,
	ID_UI_RESOLUTION_800X600,
	ID_UI_RESOLUTION_1024X768,
	ID_UI_RESOLUTION_1280X960,
	ID_UI_RESOLUTION_1600X1200,
	ID_UI_RESOLUTION_FULLSCREEN,
	ID_UI_RESOLUTION_APPLY,

	ID_UI_DEBUG_RENDERING,
	ID_UI_DEBUG_ENTITY,
	ID_UI_DEBUG_JIGLIB,
	ID_UI_DEBUG_OTHERS,
	ID_UI_DEBUG_PHYSSIM,

	ID_UI_PHYSSIM_CONTACT_POINTS = 2000,
	ID_UI_PHYSSIM_CONTACT_NORMALS,
	ID_UI_PHYSSIM_AABB,
	ID_UI_PHYSSIM_VELOCITY,
	ID_UI_PHYSSIM_ANGULAR_VELOCITY

};

#define ID_UI_RESOLUTION_LOWEST		ID_UI_RESOLUTION_640X480
#define ID_UI_RESOLUTION_HIGHEST	ID_UI_RESOLUTION_1280X960



class CGameTask_MainMenu : public CGameTask
{
private:

	CInputHandler *m_pInputHandler;

	CGM_DialogManagerSharedPtr m_pDialogManager;

	CGM_ControlRendererManagerSharedPtr m_pUIRenderManager;

//	CMainMenuEventHandler m_RootMenuEventHandler;

	CResolutionChangeEventHandler m_ResChangerEventHandler;
	CPhysSimVisEventHandler m_PhysSimVisEventHandler;

//	CGM_FlowTextCaption* m_pFlowCaptionRenderRoutine;

	CFont *m_pFont;

	static CGameTask::eGameTask ms_NextTaskID;

private:

	void InitFont();

	void InitFontForBackgroundText();

	void CreateMenu();
	CGM_Dialog *CreateControlMenu();
	CGM_Dialog *CreateOptionMenu();
	CGM_Dialog *CreateVideoOptionMenu();
	CGM_Dialog *CreateScreenResolutionSelectMenu();
	CGM_Dialog *CreateSoundOptionMenu();
	CGM_Dialog *CreateDebugMenu();
	CGM_Dialog *CreatePhysicsDebugSubDialog();

public:

	CGameTask_MainMenu();
	virtual ~CGameTask_MainMenu();

	void InitMenu();

	virtual int FrameMove( float dt );

	virtual void Render();

	void RenderQuickGuide();

	static void SetNextTaskID( CGameTask::eGameTask task_id ) { ms_NextTaskID = task_id; }

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif  /*  __GAMETASK_MAINMENU_H__  */
