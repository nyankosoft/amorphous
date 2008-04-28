#ifndef  __GAMETASK_STAGE_FG_H__
#define  __GAMETASK_STAGE_FG_H__


#include "Task/GameTask_Stage.h"


class CGameTask_StageFG : public CGameTask_Stage
{
	void UpdateStage( float dt );

public:

	CGameTask_StageFG();
	virtual ~CGameTask_StageFG();

	virtual int FrameMove( float dt );
	virtual void Render();
	virtual void CreateRenderTasks();

//	void DrawDebugInfo();
//	void DrawBasicInfo();
//	void RenderProfileResult();
//	void RenderLog();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	static void SetStageFadeInTimeMS( unsigned int time ) { ms_FadeInTime = time; }

};


#endif  /*  __GAMETASK_STAGE_FG_H__  */
