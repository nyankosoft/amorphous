#ifndef __STRINGAUX_H__
#define __STRINGAUX_H__

#include <vector>
#include <string>


#include <cstdio>
#include <cstdarg>
#include <stdio.h>
#include <stdarg.h>

#include "../base.h"
#include "TimeFormats.h"


inline std::string to_string( int num )
{
	char buffer[32];
	sprintf( buffer, "%d", num );
	return std::string(buffer);
}


inline std::string to_string( unsigned int num )
{
	char buffer[32];
	sprintf( buffer, "%u", num );
	return std::string(buffer);
}

/*
// error - re-definition of to_string( unsigned int num )
inline std::string to_string( size_t num )
{
	return to_string((unsigned int)num);
}
*/

inline std::string to_string( long num )
{
	char buffer[32];
	sprintf( buffer, "%d", num );
	return std::string(buffer);
}


inline std::string to_string( double num )
{
	char buffer[32];
	sprintf( buffer, "%.3f", num );
	return std::string(buffer);
}


inline std::string to_string( float num, int precision = 3, int num_zfills = 0 )
{
//	char buffer[32];
//	sprintf( buffer, "%.3f", num );
//	return std::string(buffer);

	std::string fmt_buffer = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";

	char buffer[32];
	sprintf( buffer, fmt_buffer.c_str(), num );
	return std::string(buffer);
}


inline std::string fmt_string( const char *format,... )
{
//	g_LogMutex.lock();

	char buffer[1024];
//	char *buffer = m_pBuffer;
	int size = 1024;//m_BufferSize;
	va_list argptr;

	va_start(argptr,format);
	_vsnprintf(buffer,size,format,argptr);
	buffer[size-1] = '\0';
	va_end(argptr);

//	g_LogMutex.unlock();

	return std::string(buffer);
}


inline float to_float( const std::string& str )
{
	float val = 0.0f;
	sscanf( str.c_str(), "%f", &val );
	return val;
}


inline int to_int( const std::string& str )
{
	int val = 0;
	sscanf( str.c_str(), "%d", &val );
	return val;
}


inline std::string get_time_stamp_string( unsigned long time_ms, TimeFormat::Format time_format )
{
	char strbuf[16];

	if( time_format == TimeFormat::NONE )
		return std::string("");

	// create a string that represents the given time in the specified format

	int hrs, mins;
	hrs =  time_ms / ( 1000 * 60 * 60 );
	mins = (time_ms - hrs) / ( 1000 * 60 );
	if( time_format == TimeFormat::HHMMSS || time_format == TimeFormat::HHMMSSFF )
	{
		int sec;
		sec = (time_ms - hrs - mins) / 1000;
		if( time_format == TimeFormat::HHMMSS )
		{
			sprintf( strbuf, "%02d:%02d:%02d", hrs, mins, sec );
		}
		else if( time_format == TimeFormat::HHMMSSFF )
		{
			int frm = (time_ms - hrs - mins - sec) * 60 / 1000;
			sprintf( strbuf, "%02d:%02d:%02d:%02d", hrs, mins, sec, frm );
		}
		
	}
	else if( time_format == TimeFormat::HHMMSSMS )
	{
		int msec = (time_ms - hrs - mins);
		sprintf( strbuf, "%02d:%02d:%02d.%03d", hrs, mins, msec/1000, msec%1000 );
	}

	return std::string(strbuf);
}


/// returns HH:MM:SS
inline std::string ms_to_hhmmss( U32 time_ms )
{
	return get_time_stamp_string( time_ms, TimeFormat::HHMMSS );
}


/// returns HH:MM:SS.MS
inline std::string ms_to_hhmmssms( U32 time_ms )
{
	return get_time_stamp_string( time_ms, TimeFormat::HHMMSSMS );
}


/// returns HH:MM:SS.FF
inline std::string ms_to_hhmmssff( U32 time_ms )
{
	return get_time_stamp_string( time_ms, TimeFormat::HHMMSSFF );
}


/**
 * separate a given string by separators
 * \param pSrcStr [in] src string
 * \param rDestStr [out] dest buffer
 *
 * example:
 *  SeparateStrings( buf, "pos	1.0 2.0 -3.0", " \t" );
 *  then, you will get buf = [ "pos", "1.0", "2.0", "3.0" ]
 */
inline void SeparateStrings( std::vector<std::string>& rDestStr, const char *pSrcStr, const char *pSeparaters )
{
	size_t i, str_len = strlen(pSrcStr);
	size_t j, next_pos = 0, num_separaters = strlen(pSeparaters);
	char str[256];

	for( i=0; i<str_len; i++ )
	{
		for( j=0; j<num_separaters; j++ )
		{
			if( pSrcStr[i] == pSeparaters[j] )
				break;
		}

		if( j < num_separaters )
		{
			if( next_pos < i )
			{
				strncpy( str, pSrcStr + next_pos, i - next_pos );
				str[i - next_pos] = '\0';
				rDestStr.push_back( std::string( str ) );
			}
			next_pos = i + 1;
		}
	}

	// add the last separated string
	if( next_pos < i )
		rDestStr.push_back( std::string( pSrcStr + next_pos ) );
}


inline int get_str_index( const std::string& input_str, const std::vector<std::string>& reference_str )
{
	const size_t num_elements = reference_str.size();
	for( size_t i=0; i<num_elements; i++ )
	{
		if( input_str == reference_str[i] )
			return (int)i;
	}

	return -1;
}


#endif  /*  __STRINGAUX_H__  */
