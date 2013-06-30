#ifndef  __amorphous_LogOutputBase_HPP__
#define  __amorphous_LogOutputBase_HPP__


#include <string>


namespace amorphous
{


enum LogWarningLevel
{
	WL_VERBOSE,
	WL_CAUTION,
	WL_WARNING,
	WL_ERROR,
	WL_INFO,	///< Use this for information that should always be logged
	NUM_LOGWARNINGLEVELS
};


/**
 * holds log info such as time & text
 * 
 */
class LogMessage
{
public:

	const std::string m_Time;

	const int m_FilterVal;

	const std::string m_Text;

	LogMessage( const std::string time, int filter_val, const std::string strText )
		:
	m_Time(time),
	m_FilterVal(filter_val),
	m_Text(strText) {}
};


/**
 * base class for log output device
 * each derived class output logs in a particular format
 */
class LogOutputBase
{
public:

	LogOutputBase() {}

	virtual ~LogOutputBase() {}

	virtual void Print( const LogMessage& msg ) = 0;
//	virtual void Print( const LogMessage& msg ) { printf( "pure virtual function call\n" ); }

};


} // namespace amorphous



#endif		/*  __amorphous_LogOutputBase_HPP__  */
