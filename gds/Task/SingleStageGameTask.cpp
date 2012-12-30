#include "SingleStageGameTask.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Stage.hpp"
#include "GUI.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using namespace std;


CSingleStageGameTask::CSingleStageGameTask()
{
}


CSingleStageGameTask::~CSingleStageGameTask()
{
}



void CSingleStageGameTask::LoadStage( const std::string& script_name )
{
	using namespace boost::filesystem;

	string name_of_script_to_load;

	if( exists( script_name ) )
		name_of_script_to_load = script_name;
	else
	{
		std::string stage_script_filepath = GetStageScriptFilepath();

		if( exists( stage_script_filepath ) )
			name_of_script_to_load = stage_script_filepath;
		else
		{
			LOG_PRINT_WARNING( string(" Script'") + stage_script_filepath + "'does not exist." );
			return;
		}
	}

	CStageLoader stage_loader;
	m_pStage = stage_loader.LoadStage( name_of_script_to_load );
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


} // namespace amorphous
