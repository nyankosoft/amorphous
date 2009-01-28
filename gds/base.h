#ifndef __BASETYPES_H__
#define __BASETYPES_H__


//==========================================================================
// platform-independent settings
//==========================================================================

//
// typedefs
//

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;


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


//
// function results
//

class Result
{
public:
	enum Name
	{
		SUCCESS = 0,        ///< succeeded
		OUT_OF_MEMORY,      ///< not enough memory
		INVALID_ARGS,       ///< invalid argument(s)
		RESOURCE_NOT_FOUND, ///< the requested resource was not found (to be used by async loader).
		RESOURCE_IN_USE,    ///< the resource is being used by another thread (to be used by async loader).
		UNKNOWN_ERROR,      ///< an unknown error
		NUM_RESULTS
	};
};



//==========================================================================
// platform-dependent settings
//==========================================================================

//#if WINDOWS

#define NOMINMAX // avoid error in nxmath.h of Ageia PhysX SDK
#include <windows.h>


//
// platform-dependent typedefs
//

typedef BYTE  U8;
typedef WORD  U16;
typedef DWORD U32;
//typedef ????? U64;


//#else

//#endif


#endif  /* __BASETYPES_H__ */
