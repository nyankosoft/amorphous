#ifndef  __ScopeLog_H__
#define  __ScopeLog_H__


#include "../../base.hpp"
#include "../time_string.hpp"
#include "DefaultLog.hpp"


namespace amorphous
{


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
m_StartTimeMS(GlobalLog().GetCurrentTimeMS())
{
	GlobalLog().Print( std::string("Entered: ") + m_pText );
}


inline CScopeLog::~CScopeLog()
{
	GlobalLog().Print( std::string("Leaving: ") + m_pText );

	U32 time_spent_ms = GlobalLog().GetCurrentTimeMS() - m_StartTimeMS;

	GlobalLog().Print( m_pText + std::string(" - Total Time: ") + ms_to_hhmmssms(time_spent_ms) );
}


#define LOG_SCOPE( title )   CScopeLog scope_log(title)
#define LOG_FUNCTION_SCOPE() CScopeLog function_scope_log(__FUNCTION__)

} // amorphous



#endif  /*  __ScopeLog_H__  */
