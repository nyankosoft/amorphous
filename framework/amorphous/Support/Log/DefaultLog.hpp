#ifndef  __amorphous_DefaultLog_HPP__
#define  __amorphous_DefaultLog_HPP__


#ifdef DISABLE_DEFAULT_LOG

#define PrintLog(n)

#else  /*  DISABLE_DEFAULT_LOG  */


#include "LogInputBase.hpp"


namespace amorphous
{

inline LogInput_Default& GlobalLog()
{
	static LogInput_Default s_Log;
	return s_Log;
}

#define PrintLog(n)	g_Log.Print(n)

#define LOG_PRINT_VERBOSE( text ) GlobalLog().Print( WL_VERBOSE, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_CAUTION( text ) GlobalLog().Print( WL_CAUTION, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_WARNING( text ) GlobalLog().Print( WL_WARNING, std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT_ERROR( text )   GlobalLog().Print( WL_ERROR,   std::string(__FUNCTION__) + " " + std::string(text) )
#define LOG_PRINT( text )         GlobalLog().Print( WL_INFO,    std::string(__FUNCTION__) + " " + std::string(text) )

#define LOG_PRINTF_VERBOSE(x) LOG_PRINT_VERBOSE( std::string(" ") + fmt_string x )
#define LOG_PRINTF_CAUTION(x) LOG_PRINT_CAUTION( std::string(" ") + fmt_string x )
#define LOG_PRINTF_WARNING(x) LOG_PRINT_WARNING( std::string(" ") + fmt_string x )
#define LOG_PRINTF_ERROR(x)   LOG_PRINT_ERROR(   std::string(" ") + fmt_string x )
#define LOG_PRINTF(x)         LOG_PRINT(         std::string(" ") + fmt_string x )

} // namespace amorphous


#include "ScopeLog.hpp"


#endif  /*  DISABLE_DEFAULT_LOG  */



#endif  /*  __amorphous_DefaultLog_HPP__  */
