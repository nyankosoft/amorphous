#ifndef __single_instance_HPP__
#define __single_instance_HPP__


#ifdef _MSC_VER

#include "single_instance_win32.hpp"


namespace amorphous
{

#else /* _MSC_VER */

//inline bool is_another_instance_running( const char *application_name )
inline bool is_another_instance_running()
{
	printf( "%s() Not implemented.\n", __FUNCTION__ );
	return false;
}

#endif /* _MSC_VER */

} // amorphous



#endif /* __single_instance_HPP__ */
