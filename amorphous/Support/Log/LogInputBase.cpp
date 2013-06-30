#include <cstdio>
#include <cstdarg>
#include <stdio.h>
#include <stdarg.h>

#include "LogInputBase.hpp"
#include "LogOutputBase.hpp"

#include "../../Platform.hpp"
//#include "../../Support/Timer.hpp"
#include "../../Support/SafeDelete.hpp"
#include "../../Support/time_string.hpp"


namespace amorphous
{

using namespace std;


//================================================================================
// LogInputBase
//================================================================================

LogInputBase::LogInputBase( int buffer_size )
:
m_TimeStampFormat(TimeFormat::HHMMSSMS)
{
	if( buffer_size == 0 )
	{
		m_pBuffer = NULL;
		return;
	}

	m_pBuffer	 = new char [buffer_size];
	m_BufferSize = buffer_size;

//	m_pTimer = new Timer;
//	m_pTimer->Start();
}


LogInputBase::~LogInputBase()
{
	SafeDelete( m_pBuffer );
//	SafeDelete( m_pTimer );
}


void LogInputBase::Print( const string& text )
{
	// print request without any filter value - output the text unconditionally
	SendLogMessageToOutput( 0, text );
}


void LogInputBase::Print( int filter_val, const string& text )
{
	if( !TestFilter( filter_val ) )
		return;

	SendLogMessageToOutput( filter_val, text );
}


void LogInputBase::Print( const char *format,... )
{
	// print request without any filter value - output the text unconditionally

//	g_LogMutex.lock();

//	PrintInternal( 0, format );

	char *buffer = m_pBuffer;
	int size = m_BufferSize;
	va_list argptr;

	va_start(argptr,format);
	vsnprintf(buffer,size,format,argptr);
	buffer[size-1] = '\0';
	va_end(argptr);

	string strText = buffer;
	SendLogMessageToOutput( 0, strText );

//	g_LogMutex.unlock();
}


void LogInputBase::Print( int filter_val, const char *format,... )
{
	if( !TestFilter( filter_val ) )
		return;

//	g_LogMutex.lock();

//	PrintInternal( filter_val, format );

	char *buffer = m_pBuffer;
	int size = m_BufferSize;
	va_list argptr;

	va_start(argptr,format);
	vsnprintf(buffer,size,format,argptr);
	buffer[size-1] = '\0';
	va_end(argptr);

	string strText = buffer;
	SendLogMessageToOutput( filter_val, strText );

//	g_LogMutex.unlock();
}


void LogInputBase::PrintInternal( int filter_val, const char *format,... )
{
//	g_LogMutex.lock();

	char *buffer = m_pBuffer;
	int size = m_BufferSize;
	va_list argptr;

	va_start(argptr,format);
	vsnprintf(buffer,size,format,argptr);
	buffer[size-1] = '\0';
	va_end(argptr);

	string strText = buffer;
	SendLogMessageToOutput( filter_val, strText );

//	g_LogMutex.unlock();
}


void LogInputBase::GetTimeStampString( string& strTime )
{
	unsigned long time_ms = GetCurrentTimeMS();

	strTime = get_time_stamp_string( time_ms, m_TimeStampFormat );
}


void LogInputBase::SendLogMessageToOutput( int filter_val, const string strText )
{
	string strTime;
	GetTimeStampString( strTime );

	LogMessage msg( strTime, filter_val, strText );

	size_t i, num_output_devices = m_vecpLogOutput.size();
	for( i=0; i<num_output_devices; i++ )
	{
		m_vecpLogOutput[i]->Print( msg );
	}
}


bool LogInputBase::RemoveLogOutputDevice( LogOutputBase *pLogOutput )
{
	vector<LogOutputBase *>::iterator itr;
	for( itr = m_vecpLogOutput.begin(); itr != m_vecpLogOutput.end(); itr++ )
	{
		if( *itr == pLogOutput )
		{
			m_vecpLogOutput.erase( itr );
			return true;
		}
	}
	return false;
}


unsigned long LogInputBase::GetCurrentTimeMS() const
{
//	return (unsigned long)(m_pTimer->GetTime() * 1000.0f);
	boost::timer::cpu_times elapsed = m_Timer.elapsed();
	return (unsigned long)(elapsed.system / 1000);
}



//================================================================================
// LogInput_Default
//================================================================================

bool LogInput_Default::TestFilter( int filter_val )
{
	switch(m_FilterType)
	{
	case FILTER_EQUAL:						return (m_FilterRefVal == filter_val ? true : false);
	case FILTER_LESS_THAN_REFVAL:			return (m_FilterRefVal > filter_val ? true : false);
	case FILTER_LESSTHAN_EQUALTO_REFVAL:	return (m_FilterRefVal >= filter_val ? true : false);
	case FILTER_GREATER_THAN_REFVAL:		return (m_FilterRefVal < filter_val ? true : false);
	case FILTER_GREATERTHAN_EQUALTO_REFVAL:	return (m_FilterRefVal <= filter_val ? true : false);
	default: return true;
	}
}


} // namespace amorphous
