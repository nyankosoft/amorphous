#ifndef  __GAMETASKMANAGER_H__
#define  __GAMETASKMANAGER_H__


#include <vector>
#include "fwd.hpp"


// m_p = new CGameTaskManager( new CGameTaskFactoryCustomized, init_task_id );


class CGameTaskManager
{
	/// owned reference?
	CGameTaskFactoryBase* m_pFactory;

	CGameTask *m_pCurrentTask;

	int m_CurrentTaskID;

	int m_NextTaskID;

	std::vector<int> m_vecTaskIDStack;

private:

	CGameTask *CreateTask( int iTaskID );

public:

	CGameTaskManager( CGameTaskFactoryBase* pFactory, int iInitialTaskID );

	/// - NOTE: Task names and task ids must be mapped before creating task manager
	CGameTaskManager( CGameTaskFactoryBase* pFactory, const std::string& initial_task_name );

	~CGameTaskManager();

	void Update( float dt );

	void Render();

	CGameTask *GetCurrentTask() { return m_pCurrentTask; }
};



#endif		/*  __GAMETASKMANAGER_H__  */
