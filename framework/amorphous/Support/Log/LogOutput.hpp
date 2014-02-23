#ifndef  __amorphous_LogOutput_HPP__
#define  __amorphous_LogOutput_HPP__


#include "LogOutputBase.hpp"
#include <iostream>
#include <fstream>


namespace amorphous
{


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



#endif		/*  __amorphous_LogOutput_HPP__  */
