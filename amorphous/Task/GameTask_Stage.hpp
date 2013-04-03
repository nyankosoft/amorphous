#ifndef  __GAMETASK_STAGE_H__
#define  __GAMETASK_STAGE_H__


#include "GameTask.hpp"
#include "gds/Stage/fwd.hpp"


namespace amorphous
{


extern CStageSharedPtr g_pStage;

extern void SetGlobalStageScriptFilename( const std::string& filename );
extern const std::string& GetGlobalStageScriptFilename();
extern void LoadStage( const std::string& strStageScriptFilename );


/**
 - GameTask_Stage.cpp does some works for global stage



 */
class GameTask_Stage : public GameTask
{
protected:

	static unsigned int ms_FadeInTime;

public:

	GameTask_Stage();
	virtual ~GameTask_Stage();

	virtual int FrameMove( float dt );
	virtual void Render();

	static void SetStageFadeInTimeMS( unsigned int time ) { ms_FadeInTime = time; }

	enum State
	{
		STATE_PLAYER_IN_STAGE,
		STATE_PLAYER_LEFT_STAGE,
		NUM_STATES
	};

	enum StageSelectState
	{
		STAGE_LOADED = 0,
		STAGE_NOT_LOADED
	};
};

} // namespace amorphous



#endif  /*  __GAMETASK_STAGE_H__  */
