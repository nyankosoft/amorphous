
#ifndef  __GAMETASK_GLOBALSTAGELOADER_H__
#define  __GAMETASK_GLOBALSTAGELOADER_H__


#include "GameTask.hpp"


namespace amorphous
{

class InputHandler;
class CGM_DialogManager;
class CGM_StdControlRendererManager;
class TitleEventHandler;
class FontBase;


class GameTask_GlobalStageLoader : public GameTask
{
	// handles the input to the player ship
//	InputHandler *m_pInputHandler;
//	CGM_DialogManagerSharedPtr m_pDialogManager;
//	CGM_ControlRendererManagerSharedPtr m_pUIRenderManager;
//	TitleEventHandler* m_pEventHandler;

	FontBase *m_pFont;

	bool m_bRendered;

	bool m_bStageLoaded;

	static std::string ms_strStageTask;

public:

	GameTask_GlobalStageLoader();
	virtual ~GameTask_GlobalStageLoader();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );

	static void SetTaskForGlobalStage( const std::string& next_task_name ) { ms_strStageTask = next_task_name; }
};

} // namespace amorphous



#endif  /*  __GAMETASK_GLOBALSTAGELOADER_H__  */
