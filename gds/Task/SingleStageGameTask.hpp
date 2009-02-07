#ifndef  __SingleStageGameTask_H__
#define  __SingleStageGameTask_H__


#include "GUIGameTask.hpp"
#include "Graphics/fwd.hpp"
#include "GUI/fwd.hpp"
#include "Stage/fwd.hpp"


class CSingleStageGameTask : public CGUIGameTask
{
	boost::shared_ptr<CStage> m_pStage;

public:

	CSingleStageGameTask();

	virtual ~CSingleStageGameTask();

	void LoadStage();

	const std::string GetStageScriptFilepath() { return std::string(""); }

	virtual int FrameMove( float dt );

	virtual void Render();

};


#endif  /*  __SingleStageGameTask_H__  */
