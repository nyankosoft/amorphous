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


namespace amorphous
{


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
 Dislplay message boxes at the start & the end of a scope
 note: text data must be const and not be changed within the scope
 */
class CScopeMsgBox
{
	const char *m_pText;

public:

	inline CScopeMsgBox( const char *pText );
	inline ~CScopeMsgBox();
};


inline CScopeMsgBox::CScopeMsgBox( const char *pText )
:
m_pText(pText)
{
	MsgBoxFmt( "Entered: %s", m_pText );
}


inline CScopeMsgBox::~CScopeMsgBox()
{
	MsgBoxFmt( "Leaving: %s", m_pText );
}


#define MSGBOX_FUNCTION_SCOPE() CScopeMsgBox function_scope_msgbox(__FUNCTION__"()")


} // amorphous



#endif  /*  __MSGBOX_H__  */
