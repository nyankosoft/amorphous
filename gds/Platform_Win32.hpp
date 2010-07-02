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
//#define _snprintf   _snprintf_s
//#define _vsnprintf _vsnprintf_for_win32 // error
//#define getcwd      _getcwd
//#define chdir       _chdir
#define fopen fopen_for_win32



inline FILE *fopen_for_win32( const char *filename, const char *mode )
{
	FILE *fp = NULL;
	fopen_s( &fp, filename, mode );
	return fp;
}

/*
inline int _vsnprintf_for_win32(
	char *buffer,
	size_t count,
	const char *format,
	va_list argptr )
{
	size_t size_of_buffer = 0xFFFF; // FIXME: find a way to avoid buffer overrun
	return _vsnprintf_s( buffer, size_of_buffer, count, format, argptr );
}
*/


#endif /* __Platform_Win32_HPP__ */
