#ifndef  __GAMETASK_TITLE_FG_H__
#define  __GAMETASK_TITLE_FG_H__


#include "GameTaskFG.h"
#include "3DCommon/fwd.h"
#include "UI/ui_fwd.h"

class CInputHandler;
class TitleEventHandler;


// ui ids for Task of Control Customization
enum UIID_TITLE_FG
{
	UIID_TTL_DLG_ROOT = CGameTaskFG::UIID_FG_DERIVEDTASK_OFFSET,
	NUM_TTL_IDS
};


class CGameTask_TitleFG : public CGameTaskFG
{
	CGM_DialogManagerSharedPtr m_pDialogManager;

	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

public:

	CGameTask_TitleFG();
	virtual ~CGameTask_TitleFG();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

};


#endif  /*  __GAMETASK_TITLE_FG_H__  */
