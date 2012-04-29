#include "GameTaskManager.hpp"
#include "GameTask.hpp"

#include "Graphics/RenderTask.hpp"
#include "Graphics/RenderTaskProcessor.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "GameTask.hpp"
#include "GameTaskFactoryBase.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/Profile.hpp"


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


CGameTaskManager::CGameTaskManager( CGameTaskFactoryBase* pFactory, const std::string& initial_task_name )
:
m_pFactory(pFactory),
m_pCurrentTask(NULL),
m_CurrentTaskID(CGameTask::ID_INVALID),
m_NextTaskID(CGameTask::GetTaskIDFromTaskName(initial_task_name))
{
}


CGameTaskManager::~CGameTaskManager()
{
	SafeDelete( m_pCurrentTask );

	SafeDelete( m_pFactory );
}


void CGameTaskManager::Update( float dt )
{
	PROFILE_FUNCTION();

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

			m_pCurrentTask->OnLeaveTask();

			delete m_pCurrentTask;

			m_pCurrentTask = NULL;
		}

		// create a new task
		m_pCurrentTask = CreateTask( next_task_id );

		// init
		if( m_pCurrentTask )
		{
			int prev_task_id = 0 < m_vecTaskIDStack.size() ? m_vecTaskIDStack.back() : CGameTask::ID_INVALID;
			m_pCurrentTask->SetPrevTaskID( prev_task_id );

			m_pCurrentTask->OnEnterTask();
		}

		// save the current task id
		m_CurrentTaskID = next_task_id;
	}

	if( m_pCurrentTask )
	{
		m_NextTaskID = m_pCurrentTask->FrameMove( dt );

		// When app exit request is made, should there be a callback which is called after FrameMove()?
//		if( m_pCurrentTask->IsAppExitRequested() )
//		{
//			m_pCurrentTask->OnAppExitRequested();
//		}
	}

}


void CGameTaskManager::Render()
{
	PROFILE_FUNCTION();

	if( m_pCurrentTask )
	{
		const bool use_render_task = false;
		if( use_render_task )
		{
/*			// create list of non-hierarchical render operations
			m_pCurrentTask->CreateRenderTasks();

			RenderTaskProcessor.Render();
*/
		}
		else
		{
			// BeginScene() here if render task system is NOT used
			HRESULT hr = DIRECT3D9.GetDevice()->BeginScene();

			m_pCurrentTask->RenderBase();

			// EndScene() and Present() here if render task system is NOT used
			hr = DIRECT3D9.GetDevice()->EndScene();

			hr = DIRECT3D9.GetDevice()->Present( NULL, NULL, NULL, NULL );
		}
	}
}
