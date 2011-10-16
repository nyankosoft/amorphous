#ifndef  __ScopeLog_H__
#define  __ScopeLog_H__


#include <string>
#include "../../base.hpp"
#include "../time_string.hpp"
#include "DefaultLog.hpp"


class CScopeLog
{
	const char *m_pText;

	U32 m_StartTimeMS;

public:

	inline CScopeLog( const char *pText );
	inline ~CScopeLog();
};


inline CScopeLog::CScopeLog( const char *pText )
:
m_pText(pText),
m_StartTimeMS(g_Log.GetCurrentTimeMS())
{
	g_Log.Print( std::string("Entered: ") + m_pText );
}


inline CScopeLog::~CScopeLog()
{
	g_Log.Print( std::string("Leaving: ") + m_pText );

	U32 time_spent_ms = g_Log.GetCurrentTimeMS() - m_StartTimeMS;

	g_Log.Print( m_pText + std::string(" - Total Time: ") + ms_to_hhmmssms(time_spent_ms) );
}


#define LOG_SCOPE( title )   CScopeLog scope_log(title)
#define LOG_FUNCTION_SCOPE() CScopeLog function_scope_log(__FUNCTION__"()")


#endif  /*  __ScopeLog_H__  */
