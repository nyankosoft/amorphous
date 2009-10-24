#ifndef __Platform_Win32_HPP__
#define __Platform_Win32_HPP__


#include <stdio.h>
#include <stdlib.h>


//#ifdef _MSC_VER
#define fopen fopen_for_win32
//#endif /* _MSC_VER */


inline FILE *fopen_for_win32( const char *filename, const char *mode )
{
	FILE *fp = NULL;
	fopen_s( &fp, filename, mode );
	return fp;
}


//#define strcpy      strcpy_s
#define strncpy     strncpy_s
#define strcat      strcat_s
#define sscanf      sscanf_s
#define fscanf      fscanf_s
#define sprintf     sprintf_s
#define _snprintf   _snprintf_s
#define _vsnprintfs _vsnprintf_s
//#define getcwd      _getcwd
//#define chdir       _chdir



#endif /* __Platform_Win32_HPP__ */
