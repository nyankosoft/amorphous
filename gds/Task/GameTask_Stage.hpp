#ifndef  __GAMETASK_STAGE_H__
#define  __GAMETASK_STAGE_H__


#include "GameTask.h"
#include "Stage/fwd.h"

class CStage;
class CInputHandler;
class CFont;
class CDebugOutput;

extern CStageSharedPtr g_pStage;

extern void SetGlobalStageScriptFilename( const std::string& filename );
extern const std::string& GetGlobalStageScriptFilename();


/**
 - CGameTask_Stage.cpp does some works for global stage



 */
class CGameTask_Stage : public CGameTask
{
protected:

	static unsigned int ms_FadeInTime;

public:

	CGameTask_Stage();
	virtual ~CGameTask_Stage();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	static void SetStageFadeInTimeMS( unsigned int time ) { ms_FadeInTime = time; }

	enum State
	{
		STATE_PLAYER_IN_STAGE,
		STATE_PLAYER_LEFT_STAGE,
		NUM_STATES
	};
};


#endif  /*  __GAMETASK_STAGE_H__  */
