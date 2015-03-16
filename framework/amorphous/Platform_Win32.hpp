#ifndef __Platform_Win32_HPP__
#define __Platform_Win32_HPP__


#include <stdio.h>
#include <stdlib.h>


//#define strcpy      strcpy_s
#define strncpy     strncpy_s
#define strcat      strcat_s
#define sscanf      sscanf_s
#define fscanf      fscanf_s
//#define sprintf     sprintf_s
#define snprintf   _snprintf_s
//#define _snprintf   _snprintf_s
#define vsnprintf _vsnprintf_for_win32 // error
//#define getcwd      _getcwd
//#define chdir       _chdir
#define fopen fopen_for_win32



inline FILE *fopen_for_win32( const char *filename, const char *mode )
{
	FILE *fp = nullptr;
	fopen_s( &fp, filename, mode );
	return fp;
}


inline int _vsnprintf_for_win32(
	char *buffer,
	size_t n,
	const char *format,
	va_list argptr )
{
	return _vsnprintf_s( buffer, n, _TRUNCATE, format, argptr );
}



#endif /* __Platform_Win32_HPP__ */
