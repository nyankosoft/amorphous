#include "SingleStageGameTask.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Timer.hpp"
#include "Graphics/GraphicsElementManager.hpp"
#include "Input/InputHub.hpp"
#include "Script/ScriptArchive.hpp"
#include "Stage.hpp"
#include "GUI.hpp"


CSingleStageGameTask::CSingleStageGameTask()
{
	LoadStage();
}


CSingleStageGameTask::~CSingleStageGameTask()
{
}



void CSingleStageGameTask::LoadStage()
{
	// Only support archived script file for now
	std::string stage_script_filepath = GetStageScriptFilepath();
	
	CStageLoader stage_loader;
	m_pStage = stage_loader.LoadStage( stage_script_filepath );
}


int CSingleStageGameTask::FrameMove( float dt )
{
	int ret = CGUIGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( m_pStage )
		m_pStage->Update( dt );

	return CGameTask::ID_INVALID;
}


void CSingleStageGameTask::Render()
{
	// render stage
	if( m_pStage )
		m_pStage->Render();

	// render GUI components over the stage
	CGUIGameTask::Render();
}
