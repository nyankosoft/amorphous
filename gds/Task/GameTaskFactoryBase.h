#ifndef  __GameTaskFactoryBase_H__
#define  __GameTaskFactoryBase_H__


//#include <vector>

#include "GameTask.h"
//class CGameTask;

class CGameTaskFactoryBase
{

public:

	CGameTaskFactoryBase();

	virtual ~CGameTaskFactoryBase();

	virtual CGameTask *CreateTask( int iTaskID );
};



#endif		/*  __GameTaskFactoryBase_H__  */
