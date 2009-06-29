#include "GameTaskFactoryBase.hpp"

#include "GameTask.hpp"
#include "GameTask_Stage.hpp"
#include "GameTask_MainMenu.hpp"
#include "GameTask_GlobalStageLoader.hpp"
#include "GameTask_AsyncStageLoader.hpp"
#include "GUIGameTask.hpp"
#include "SingleStageGameTask.hpp"
#include "StageViewerGameTask.hpp"


CGameTaskFactoryBase::~CGameTaskFactoryBase()
{
}


CGameTask *CGameTaskFactoryBase::CreateTask( const std::string& task_name )
{
	return CreateTask( CGameTask::GetTaskIDFromTaskName( task_name ) );
}


CGameTask *CGameTaskFactoryBase::CreateTask( int iTaskID )
{
	switch( iTaskID )
	{
	case CGameTask::ID_STAGE:
		return new CGameTask_Stage;

	case CGameTask::ID_MAIN_MENU:
		return new CGameTask_MainMenu;

	case CGameTask::ID_GLOBALSTAGELOADER:
		return new CGameTask_GlobalStageLoader;

	case CGameTask::ID_ASYNCSTAGELOADER:
		return new CGameTask_AsyncStageLoader;

	case CGameTask::ID_GUI_TASK:
		return new CGUIGameTask;

	case CGameTask::ID_SINGLE_STAGE_TASK:
		return new CSingleStageGameTask;

	case CGameTask::ID_STAGE_VIEWER_TASK:
		return new CStageViewerGameTask;

	default:
//		LOG_PRINT_ERROR( "An invalid task id:" + to_string(iTaskID) );
		return NULL;
	}
}
