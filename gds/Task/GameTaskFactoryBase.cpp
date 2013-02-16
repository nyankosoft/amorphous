#include "GameTaskFactoryBase.hpp"

#include "GameTask.hpp"
#include "GameTask_Stage.hpp"
#include "GameTask_MainMenu.hpp"
#include "GameTask_GlobalStageLoader.hpp"
#include "GameTask_AsyncStageLoader.hpp"
#include "GUIGameTask.hpp"
#include "SingleStageGameTask.hpp"
#include "StageViewerGameTask.hpp"


namespace amorphous
{


GameTaskFactoryBase::~GameTaskFactoryBase()
{
}


GameTask *GameTaskFactoryBase::CreateTask( const std::string& task_name )
{
	return CreateTask( GameTask::GetTaskIDFromTaskName( task_name ) );
}


GameTask *GameTaskFactoryBase::CreateTask( int iTaskID )
{
	switch( iTaskID )
	{
	case GameTask::ID_STAGE:
		return new GameTask_Stage;

	case GameTask::ID_MAIN_MENU:
		return new GameTask_MainMenu;

	case GameTask::ID_GLOBALSTAGELOADER:
		return new GameTask_GlobalStageLoader;

	case GameTask::ID_ASYNCSTAGELOADER:
		return new GameTask_AsyncStageLoader;

	case GameTask::ID_GUI_TASK:
		return new GUIGameTask;

	case GameTask::ID_SINGLE_STAGE_TASK:
		return new SingleStageGameTask;

	case GameTask::ID_STAGE_VIEWER_TASK:
		return new StageViewerGameTask;

	default:
//		LOG_PRINT_ERROR( "An invalid task id:" + to_string(iTaskID) );
		return NULL;
	}
}


} // namespace amorphous
