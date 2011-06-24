#ifndef  __SingleStageGameTask_H__
#define  __SingleStageGameTask_H__


#include "GUIGameTask.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/Stage/fwd.hpp"


class CSingleStageGameTask : public CGUIGameTask
{
protected:

	boost::shared_ptr<CStage> m_pStage;

public:

	CSingleStageGameTask();

	virtual ~CSingleStageGameTask();

	void LoadStage();

	const std::string GetStageScriptFilepath() { return std::string("default_stage.bin"); }

	virtual int FrameMove( float dt );

	virtual void Render();

};


#endif  /*  __SingleStageGameTask_H__  */
