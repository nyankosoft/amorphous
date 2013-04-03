#ifndef __SAFE_DELETE_VECTOR_H__
#define __SAFE_DELETE_VECTOR_H__


#include "SafeDelete.hpp"
#include <vector>


namespace amorphous
{

template<class PointerType>
void SafeDeleteVector( std::vector<PointerType>& vecp )
{
	size_t i, num = vecp.size();
	for( i=0; i<num; i++ )
		SafeDelete( vecp[i] );

	vecp.resize( 0 );
}

} // namespace amorphous



#endif  /*  __SAFE_DELETE_VECTOR_H__  */
