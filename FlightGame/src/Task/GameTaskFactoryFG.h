#ifndef  __GameTaskFactoryFG_H__
#define  __GameTaskFactoryFG_H__


#include "Task/GameTaskFactoryBase.h"


class CGameTaskFactoryFG : public CGameTaskFactoryBase
{
public:

	CGameTaskFactoryFG();

	virtual ~CGameTaskFactoryFG();

	virtual CGameTask *CreateTask( int iTaskID );
};



#endif		/*  __GameTaskFactoryFG_H__  */
