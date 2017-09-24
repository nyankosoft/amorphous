#ifndef  __GAMETASK_GLOBALSTAGELOADER_H__
#define  __GAMETASK_GLOBALSTAGELOADER_H__


#include "GameTask.hpp"


namespace amorphous
{


class GameTask_GlobalStageLoader : public GameTask
{
	std::shared_ptr<FontBase> m_pFont;

	bool m_bRendered;

	bool m_bStageLoaded;

	static std::string ms_strStageTask;

public:

	GameTask_GlobalStageLoader();
	virtual ~GameTask_GlobalStageLoader();

	virtual int FrameMove( float dt );
	virtual void Render();

	static void SetTaskForGlobalStage( const std::string& next_task_name ) { ms_strStageTask = next_task_name; }
};

} // namespace amorphous



#endif  /*  __GAMETASK_GLOBALSTAGELOADER_H__  */
