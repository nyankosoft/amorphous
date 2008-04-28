#ifndef  __GameTask_AsyncStageLoaderFG_H__
#define  __GameTask_AsyncStageLoaderFG_H__


#include "Task/GameTask_AsyncStageLoader.h"
#include "Stage/fwd.h"
#include "Stage/StageLoader.h"

class CFontBase;


class CGameTask_AsyncStageLoaderFG : public CGameTask_AsyncStageLoader
{

public:

	CGameTask_AsyncStageLoaderFG();
	virtual ~CGameTask_AsyncStageLoaderFG();

	virtual int FrameMove( float dt );
	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif  /*  __GameTask_AsyncStageLoaderFG_H__  */
