#ifndef __time_string_HPP__
#define __time_string_HPP__


#include "StringAux.hpp"
#include "TimeFormats.hpp"


namespace amorphous
{


inline std::string get_time_stamp_string( unsigned long time_ms, TimeFormat::Format time_format )
{
	char strbuf[16];

	if( time_format == TimeFormat::NONE )
		return std::string("");

	// create a string that represents the given time in the specified format

	int hrs, mins, hrs_ms, mins_ms;
	hrs =  time_ms / ( 1000*60*60 );
	hrs_ms = hrs*1000*60*60;
	mins = ( time_ms - hrs_ms ) / ( 1000 * 60 );
	mins_ms = mins*1000*60;
	if( time_format == TimeFormat::HHMMSS || time_format == TimeFormat::HHMMSSFF )
	{
		int sec;
		sec = ( time_ms - hrs_ms - mins_ms ) / 1000;
		if( time_format == TimeFormat::HHMMSS )
		{
			STRAUX_SNPRINTF( strbuf, numof(strbuf)-1, "%02d:%02d:%02d", hrs, mins, sec );
		}
		else if( time_format == TimeFormat::HHMMSSFF )
		{
			int frm = (time_ms - hrs_ms - mins_ms - sec*1000) * 60 / 1000;
			STRAUX_SNPRINTF( strbuf, numof(strbuf)-1, "%02d:%02d:%02d:%02d", hrs, mins, sec, frm );
		}
		
	}
	else if( time_format == TimeFormat::HHMMSSMS )
	{
		int msec = (time_ms - hrs_ms - mins_ms);
		STRAUX_SNPRINTF( strbuf, numof(strbuf)-1, "%02d:%02d:%02d.%03d", hrs, mins, msec/1000, msec%1000 );
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


} // amorphous



#endif /* __time_string_HPP__ */
