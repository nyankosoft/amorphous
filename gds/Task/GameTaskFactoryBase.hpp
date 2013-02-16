#ifndef  __GameTaskFactoryBase_H__
#define  __GameTaskFactoryBase_H__


#include <string>
#include "fwd.hpp"


namespace amorphous
{


class GameTaskFactoryBase
{	

public:

	GameTaskFactoryBase() {}

	virtual ~GameTaskFactoryBase();

	virtual GameTask *CreateTask( int iTaskID );

	GameTask *CreateTask( const std::string& task_name );
};

} // namespace amorphous



#endif		/*  __GameTaskFactoryBase_H__  */
