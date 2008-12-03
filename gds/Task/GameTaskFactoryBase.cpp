#include "GameTaskFactoryBase.h"

#include "GameTask.h"
#include "GameTask_Stage.h"
#include "GameTask_StageSelect.h"
#include "GameTask_MainMenu.h"
#include "GameTask_GlobalStageLoader.h"
#include "GameTask_AsyncStageLoader.h"


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

	case CGameTask::ID_STAGE_SELECT:
		return new CGameTask_StageSelect;

	case CGameTask::ID_GLOBALSTAGELOADER:
		return new CGameTask_GlobalStageLoader;

	case CGameTask::ID_ASYNCSTAGELOADER:
		return new CGameTask_AsyncStageLoader;

	case CGameTask::ID_TITLE:
		return NULL;

	case CGameTask::ID_SETTINGS:
		return NULL;

	case CGameTask::ID_CREDIT:
		return NULL;

	default:
//		LOG_PRINT_ERROR( "An invalid task id:" + to_string(iTaskID) );
		return NULL;
	}
}
