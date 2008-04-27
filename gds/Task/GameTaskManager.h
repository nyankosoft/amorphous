
#ifndef  __GAMETASKMANAGER_H__
#define  __GAMETASKMANAGER_H__


#include <vector>

#include "GameTask.h"
class CGameTaskFactoryBase;

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

	~CGameTaskManager();

	void Update( float dt );

	void Render();

	CGameTask *GetCurrentTask() { return m_pCurrentTask; }
};



#endif		/*  __GAMETASKMANAGER_H__  */
