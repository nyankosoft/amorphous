#ifndef __BASETYPES_H__
#define __BASETYPES_H__


//#if WINDOWS

#define NOMINMAX // avoid error in nxmath.h of Ageia PhysX SDK
#include <windows.h>



//
// typedefs
//

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

typedef BYTE  U8;
typedef WORD  U16;
typedef DWORD U32;
//typedef ????? U64;


//
// macros
//

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif


//
// inline functions
//

template<class T>
inline T get_clamped( const T& val, const T& min_val, const T& max_val )
{
	if( val < min_val )
		return min_val;
	else if( max_val < val )
		return max_val;
	else
		return val;
}


template<class T>
inline void clamp( T& val, const T& min_val, const T& max_val )
{
	val = get_clamped( val, min_val, max_val );
}


//#else

//#endif


#endif  /* __BASETYPES_H__ */
