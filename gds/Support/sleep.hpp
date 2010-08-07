#ifndef __sleep_HPP__
#define __sleep_HPP__


#include "time.hpp"


#ifdef MSC_VER

void sleep( const stime& t )
{
	Sleep( t.get_millisec() );
}

#else /* MSC_VER */

void sleep( const stime& t )
{
	usleep( t.get_microsec() );
}

#endif /* MSC_VER */



#endif /* __sleep_HPP__ */
