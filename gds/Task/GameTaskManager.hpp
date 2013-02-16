#ifndef  __GAMETASKMANAGER_H__
#define  __GAMETASKMANAGER_H__


#include <vector>
#include "fwd.hpp"


namespace amorphous
{


// m_p = new GameTaskManager( new GameTaskFactoryCustomized, init_task_id );


class GameTaskManager
{
	/// owned reference?
	GameTaskFactoryBase* m_pFactory;

	GameTask *m_pCurrentTask;

	int m_CurrentTaskID;

	int m_NextTaskID;

	std::vector<int> m_vecTaskIDStack;

private:

	GameTask *CreateTask( int iTaskID );

public:

	GameTaskManager( GameTaskFactoryBase* pFactory, int iInitialTaskID );

	/// - NOTE: Task names and task ids must be mapped before creating task manager
	GameTaskManager( GameTaskFactoryBase* pFactory, const std::string& initial_task_name );

	~GameTaskManager();

	void Update( float dt );

	void Render();

	GameTask *GetCurrentTask() { return m_pCurrentTask; }
};


} // namespace amorphous



#endif		/*  __GAMETASKMANAGER_H__  */
