#ifndef  __DEFAULTLOG_H__
#define  __DEFAULTLOG_H__


#ifdef DISABLE_DEFAULT_LOG

#define PrintLog(n)

#else  /*  DISABLE_DEFAULT_LOG  */

#include <string>
#include "Log.h"

extern CLogInput_Default g_Log;

#define PrintLog(n)	g_Log.Print(n)

#define LOG_PRINT( text )         g_Log.Print( WL_NORMAL,  std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_CAUTION( text ) g_Log.Print( WL_CAUTION, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_WARNING( text ) g_Log.Print( WL_WARNING, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_ERROR( text )   g_Log.Print( WL_ERROR,   std::string(__FUNCTION__) + " " + std::string(text) )

#include "ScopeLog.h"


#endif  /*  DISABLE_DEFAULT_LOG  */


#endif  /*  __DEFAULTLOG_H__  */
