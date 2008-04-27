/*#ifdef DISABLE_MSGBOX

#ifndef __MSGBOX_NULL_H__
#define __MSGBOX_NULL_H__

inline void MsgBox( const char *message ) {}
inline void MsgBoxAlert( const char *message ) {}

#endif /* __MSGBOX_NULL_H__ *


#else /* DISABLE_MSGBOX *
*/

#ifndef __MSGBOX_H__
#define __MSGBOX_H__


#include <cstdio>
#include <cstdarg>

#include <stdio.h>
#include <stdarg.h>

#include <string.h>
#include <windows.h>

#include <string>


inline void MsgBox( const char *message )
{
	MessageBox( NULL, message, "message", MB_OK );
}


inline void MsgBox( const std::string& message )
{
	MsgBox( message.c_str() );
}


inline void MsgBoxAlert( const char *message )
{
	MessageBox( NULL, message, "message", MB_OK|MB_ICONWARNING );
}

/**
 * print formatted string
 */
inline void MsgBoxFmt( const char *format,... )
{
//	g_LogMutex.lock();

	const static int s_BufferSize = 512;
	static char s_buffer[s_BufferSize];
	int size = s_BufferSize;
	va_list argptr;

	va_start(argptr,format);
	_vsnprintf(s_buffer,size,format,argptr);
	s_buffer[size-1] = '\0';
	va_end(argptr);

//	g_LogMutex.unlock();

	MsgBox( s_buffer );
}


/**
 * dislplay message boxes at the start & the end of a scope
 * note: text data must be const and not be changed within the scope
 */
class CheckScope
{
	enum param { MAX_TEXT_LENGTH = 32 };
//	const char text[MAX_TEXT_LENGTH];

	const char *m_title;

public:

	CheckScope( const char *title ) : m_title(title)
	{
		char msg[MAX_TEXT_LENGTH] = { "start: " };
		strncat( msg, m_title, MAX_TEXT_LENGTH - strlen(msg) - 1 );

//		std::string msg( std::string("start: ") + std::string(text) );
		MsgBox( msg );
	}

	~CheckScope()
	{
		char msg[MAX_TEXT_LENGTH] = { "end: " };
		strncat( msg, m_title, MAX_TEXT_LENGTH - strlen(msg) - 1 );

//		std::string msg( std::string("end: ") + std::string(text) );
		MsgBox( msg );
	}
};


#endif  /*  __MSGBOX_H__  */
