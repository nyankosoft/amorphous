#ifndef __BASETYPES_H__
#define __BASETYPES_H__


//#if WINDOWS

#define NOMINMAX // avoid error in nxmath.h of Ageia PhysX SDK
#include <windows.h>

typedef BYTE  U8;
typedef WORD  U16;
typedef DWORD U32;
//typedef ????? U64;

//#else

//#endif


#endif  /* __BASETYPES_H__ */