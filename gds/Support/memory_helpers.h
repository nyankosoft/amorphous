#ifndef __MEMHELPERS_H__
#define __MEMHELPERS_H__


#include "SafeDelete.h"
#include "SafeDeleteVector.h"


template< class T1, class T2 >
inline void CopyVector( std::vector<T1>& dest, const std::vector<T2>& src )
{
    size_t i, size = src.size();
	dest.resize( size );
	for( i=0; i<size; i++ )
		dest[i] = src[i];
}


#endif  /*  __MEMHELPERS_H__  */
