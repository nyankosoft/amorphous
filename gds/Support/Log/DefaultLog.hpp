#ifndef  __DEFAULTLOG_H__
#define  __DEFAULTLOG_H__


#ifdef DISABLE_DEFAULT_LOG

#define PrintLog(n)

#else  /*  DISABLE_DEFAULT_LOG  */

#include <string>
#include "Log.hpp"

extern CLogInput_Default g_Log;

#define PrintLog(n)	g_Log.Print(n)

#define LOG_PRINT_VERBOSE( text ) g_Log.Print( WL_VERBOSE, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_CAUTION( text ) g_Log.Print( WL_CAUTION, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_WARNING( text ) g_Log.Print( WL_WARNING, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_ERROR( text )   g_Log.Print( WL_ERROR,   std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT( text )         g_Log.Print( WL_INFO,    std::string(__FUNCTION__) + " " + std::string(text) )

#define LOG_PRINTF_VERBOSE(x) LOG_PRINT_VERBOSE( std::string(" ") + fmt_string x )
#define LOG_PRINTF_CAUTION(x) LOG_PRINT_CAUTION( std::string(" ") + fmt_string x )
#define LOG_PRINTF_WARNING(x) LOG_PRINT_WARNING( std::string(" ") + fmt_string x )
#define LOG_PRINTF_ERROR(x)   LOG_PRINT_ERROR(   std::string(" ") + fmt_string x )
#define LOG_PRINTF(x)         LOG_PRINT(         std::string(" ") + fmt_string x )

#include "ScopeLog.hpp"


#endif  /*  DISABLE_DEFAULT_LOG  */


#endif  /*  __DEFAULTLOG_H__  */
