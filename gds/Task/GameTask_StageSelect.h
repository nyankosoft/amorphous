#ifndef  __GAMETASK_STAGESELECT_H__
#define  __GAMETASK_STAGESELECT_H__


#include "GameTask.h"
#include "UI/fwd.h"

#include <vector>
#include <string>

class CStage;
class CInputHandler;

#define STG_BUTTON_ID_OFFSET	100

#define NUM_MAX_STAGES	64


extern void LoadStage( const std::string& strStageScriptFilename );


class CGameTask_StageSelect : public CGameTask
{
	int m_State;

	std::string m_strStageFile[NUM_MAX_STAGES];
	int m_iNumStages;

	CInputHandler *m_pInputHandler;

	CGM_DialogManagerSharedPtr m_pDialogManager;

	CGM_ControlRendererManagerSharedPtr m_pUIRenderManager;

public:

	CGameTask_StageSelect();
	virtual ~CGameTask_StageSelect();

	void InitMenu();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	enum eStageSelectState
	{
		STAGE_LOADED = 0,
		STAGE_NOT_LOADED
	};

};


#endif  /*  __GAMETASK_STAGESELECT_H__  */
