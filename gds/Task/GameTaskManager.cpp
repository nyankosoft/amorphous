
#include "GameTaskManager.h"

#include "GameTask.h"
#include "GameTaskFactoryBase.h"

#include "Support/SafeDelete.h"
#include "Support/msgbox.h"

#include "3DCommon/RenderTask.h"
#include "3DCommon/RenderTaskProcessor.h"

#include <stdlib.h>


CGameTask *CGameTaskManager::CreateTask( int iTaskID )
{
	return m_pFactory->CreateTask( iTaskID );
}


CGameTaskManager::CGameTaskManager( CGameTaskFactoryBase* pFactory,
								    int iInitialTaskID )
:
m_pFactory(pFactory),
m_pCurrentTask(NULL),
m_CurrentTaskID(CGameTask::ID_INVALID),
m_NextTaskID(iInitialTaskID)
{
}


CGameTaskManager::~CGameTaskManager()
{
	SafeDelete( m_pCurrentTask );

	SafeDelete( m_pFactory );
}


void CGameTaskManager::Update( float dt )
{
	// get the prev task id if it has been requested
	int next_task_id;
	if( m_NextTaskID == CGameTask::ID_PREVTASK )
	{
		if( 0 < m_vecTaskIDStack.size() )
		{
			next_task_id = m_vecTaskIDStack.back();
			m_vecTaskIDStack.pop_back();
		}
		else
		{
			// prev task has been requested, but there is no task in the stack
			next_task_id = CGameTask::ID_INVALID;
		}
	}
	else
		next_task_id = m_NextTaskID;

	// switch tasks if new one has been requested
	if( next_task_id != CGameTask::ID_INVALID )
	{
		if( m_pCurrentTask )
		{
			if( m_NextTaskID != CGameTask::ID_PREVTASK )
                m_vecTaskIDStack.push_back( m_CurrentTaskID );
//			if( TASKID_STACK_SIZE < m_vecTaskIDStack.size() )
//				m_vecTaskIDStack.erase( m_vecTaskIDStack.begin() );
			delete m_pCurrentTask;
		}

		m_pCurrentTask = CreateTask( next_task_id );
		m_CurrentTaskID = next_task_id;
	}

	if( m_pCurrentTask )
	{
		m_NextTaskID = m_pCurrentTask->FrameMove( dt );
	}

}


void CGameTaskManager::Render()
{
	if( m_pCurrentTask )
	{
		// create list of non-hierarchical render operations
		m_pCurrentTask->CreateRenderTasks();

		RenderTaskProcessor.Render();

//		m_pCurrentTask->Render();
	}
}
