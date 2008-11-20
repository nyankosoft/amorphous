#ifndef  __LOGINPUTBASE_H__
#define  __LOGINPUTBASE_H__


#include <vector>
#include <string>

#include "LogOutputBase.h"
#include "../TimeFormats.h"


class CLogOutputBase;
class CTimer;


class CLogInputBase
{
	char *m_pBuffer;
	int m_BufferSize;

	TimeFormat::Format m_TimeStampFormat;

	CTimer *m_pTimer;

	/// holds pointers to output device (borrowed reference)
	/// NOTE: user is responsible for releasing these instances
	std::vector<CLogOutputBase *> m_vecpLogOutput;

	void PrintInternal( int filter_val, const char *format,... );

	void GetTimeStampString( std::string& strTime );

	void SendLogMessageToOutput( int filter_val, const std::string strText );

public:

	CLogInputBase( int buffer_size = 256 );

	virtual ~CLogInputBase();

	void Print( const std::string& text );

	void Print( int filter_val, const std::string& text );

	void Print( const char *format,... );

	void Print( int filter_val, const char *format,... );

	/// add a log output instance to the list
	inline void AddLogOutput( CLogOutputBase *pLogOutput ) { m_vecpLogOutput.push_back( pLogOutput ); }

	/// removes the specified log output instance from the list
	/// returns fales if it was not found in the list
	/// NOTE: does not release the actual memory, just releases the pointer from the list
	bool RemoveLogOutput( CLogOutputBase *pLogOutput );

	virtual bool TestFilter( int filter_val ) { return true; }

	TimeFormat::Format GetTimeStampFormat() { return m_TimeStampFormat; }

	void SetTimeStampFormat( TimeFormat::Format time_format ) { m_TimeStampFormat = time_format; }

	unsigned long GetCurrentTimeMS() const;

//	inline std::vector<CLogBase *>& GetLogOutput() const { return m_vecpLogOutput; }

};


class CLogInput_Default : public CLogInputBase
{

	/// filter reference value
	/// the filter value handed as an argument in Print() is tested against this by default
	int m_FilterRefVal;

	int m_FilterType;

public:

	enum eFilterType
	{
		FILTER_EQUAL,
		FILTER_LESS_THAN_REFVAL,
		FILTER_LESSTHAN_EQUALTO_REFVAL,
		FILTER_GREATER_THAN_REFVAL,
		FILTER_GREATERTHAN_EQUALTO_REFVAL,
		NUM_FILTERTYPES
	};
	
	CLogInput_Default() : m_FilterRefVal(0), m_FilterType(FILTER_GREATERTHAN_EQUALTO_REFVAL) {}

	virtual ~CLogInput_Default() {}


	void SetRefVal( int ref_val ) { m_FilterRefVal = ref_val; }

	int GetRefVal() const { return m_FilterRefVal; }

	void SetFilterType( int filter_type ) { m_FilterType = filter_type; }

	int GetFilterType() const { return m_FilterType; }

	virtual bool TestFilter( int filter_val );
};


#endif		/*  __LOGINPUTBASE_H__  */
