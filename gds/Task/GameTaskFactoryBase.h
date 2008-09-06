#ifndef  __GameTaskFactoryBase_H__
#define  __GameTaskFactoryBase_H__


#include <string>


class CGameTask;


class CGameTaskFactoryBase
{

public:

	CGameTaskFactoryBase();

	virtual ~CGameTaskFactoryBase();

	virtual CGameTask *CreateTask( int iTaskID );

	CGameTask *CreateTask( const std::string& task_name );
};



#endif		/*  __GameTaskFactoryBase_H__  */
