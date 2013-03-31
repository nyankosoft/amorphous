#ifndef  __LOGOUTPUTBASE_H__
#define  __LOGOUTPUTBASE_H__


#include <vector>
#include <string>
#include <iostream>
#include <fstream>


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

	LogOutputBase();

	virtual ~LogOutputBase();

	virtual void Print( const LogMessage& msg ) = 0;
//	virtual void Print( const LogMessage& msg ) { printf( "pure virtual function call\n" ); }

};


class LogOutput_TextFile : public LogOutputBase
{
protected:

//	FILE *m_pFile;
	std::ofstream m_OutputFileStream;

    int m_NewLineCharacterOption;

	enum NewLineCharOption
	{
		NLCO_DONT_ADD,
		NLCO_ADD_ALWAYS,
		NLCO_ADD_IF_ONT_FOUND,
		NUM_NEWLINE_OPTIONS
	};

protected:

//	std::ofstream& GetOutputFileStream() [ return m_OutputFileStream; }

public:

	LogOutput_TextFile( const std::string& filename );

	~LogOutput_TextFile();

	virtual void Print( const LogMessage& msg );

	void SetNewLineCharacterOption( int option );
};


class LogOutput_Console : public LogOutputBase
{
public:

	LogOutput_Console() {}

	~LogOutput_Console() {}

	virtual void Print( const LogMessage& msg );

};


/**
 * outputs log as a simple html source
 */
class LogOutput_HTML : public LogOutput_TextFile
{
public:

	LogOutput_HTML( const std::string& filename );

	virtual ~LogOutput_HTML();

	virtual void Print( const LogMessage& msg );

};


class LogOutput_Overlay : public LogOutputBase
{
public:

	LogOutput_Overlay();

	virtual ~LogOutput_Overlay();

	virtual void Print( const LogMessage& msg );

};


} // namespace amorphous



#endif		/*  __LOGOUTPUTBASE_H__  */
