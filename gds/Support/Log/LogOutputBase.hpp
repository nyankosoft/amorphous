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
class CLogMessage
{
public:

	const std::string m_Time;

	const int m_FilterVal;

	const std::string m_Text;

	CLogMessage( const std::string time, int filter_val, const std::string strText )
		:
	m_Time(time),
	m_FilterVal(filter_val),
	m_Text(strText) {}
};


/**
 * base class for log output device
 * each derived class output logs in a particular format
 */
class CLogOutputBase
{
public:

	CLogOutputBase();

	virtual ~CLogOutputBase();

	virtual void Print( const CLogMessage& msg ) = 0;
//	virtual void Print( const CLogMessage& msg ) { printf( "pure virtual function call\n" ); }

};


class CLogOutput_TextFile : public CLogOutputBase
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

	CLogOutput_TextFile( const std::string& filename );

	~CLogOutput_TextFile();

	virtual void Print( const CLogMessage& msg );

	void SetNewLineCharacterOption( int option );
};


class CLogOutput_Console : public CLogOutputBase
{
public:

	CLogOutput_Console() {}

	~CLogOutput_Console() {}

	virtual void Print( const CLogMessage& msg );

};


/**
 * outputs log as a simple html source
 */
class CLogOutput_HTML : public CLogOutput_TextFile
{
public:

	CLogOutput_HTML( const std::string& filename );

	virtual ~CLogOutput_HTML();

	virtual void Print( const CLogMessage& msg );

};


class CLogOutput_Overlay : public CLogOutputBase
{
public:

	CLogOutput_Overlay();

	virtual ~CLogOutput_Overlay();

	virtual void Print( const CLogMessage& msg );

};


} // namespace amorphous



#endif		/*  __LOGOUTPUTBASE_H__  */
