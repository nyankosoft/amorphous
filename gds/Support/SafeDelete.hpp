#ifndef __SAFE_DELETE_H__
#define __SAFE_DELETE_H__


#include <stdio.h>
#include <stdlib.h>


namespace amorphous
{

template<typename PointerType>
void SafeDelete(PointerType*& p)
{
	if(!p)
	{
		//DeleteNULL();
	}
	else
	{
		delete p;
		p = NULL;
	}
}

template<typename ArrayPointer>
void SafeDeleteArray(ArrayPointer*& p)
{
	if(!p)
	{
		//DeleteNULL();
	}
	else
	{
		delete [] p;
		p = NULL;
	}
}


} // namespace amorphous



#endif  /*  __SAFE_DELETE_MEMORY_H__  */
