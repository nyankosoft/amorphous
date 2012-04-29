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

	/// Synchronously loads a stage
	/// \param script_name [in] a script archive which contains the stage information
	/// If the argument string is omitted, the string returned by GetStageScriptFilepath() is used.
	/// Only supports archived script files for now.
	void LoadStage( const std::string& script_name = "" );

	virtual std::string GetStageScriptFilepath() const { return std::string("default_stage.bin"); }

	virtual int FrameMove( float dt );

	virtual void Render();

};


#endif  /*  __SingleStageGameTask_H__  */
