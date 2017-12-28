#ifndef __AMORPHOUS_STRING_AUX_HPP__
#define __AMORPHOUS_STRING_AUX_HPP__

#include <vector>
#include <string>

#include <cstdio>
#include <cstdarg>
#include <stdio.h>
#include <string.h> // for memset()
#include <stdarg.h>

#include "../base.hpp"
#include "../Platform.hpp"
#include "Macro.h"


namespace amorphous
{


inline std::string to_string( int num )
{
	char buffer[32];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, "%d", num );
	return std::string(buffer);
}


inline std::string to_string( unsigned int num )
{
	char buffer[32];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, "%u", num );
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
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, "%ld", num );
	return std::string(buffer);
}


inline std::string to_string( unsigned long num )
{
	char buffer[32];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, "%lu", num );
	return std::string(buffer);
}


inline std::string to_string( double num )
{
	char buffer[32];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, "%.3f", num );
	return std::string(buffer);
}


inline std::string to_string( float num, int precision = 3, int num_zfills = 0 )
{
//	char buffer[32];
//	sprintf( buffer, "%.3f", num );
//	return std::string(buffer);

	std::string fmt_buffer = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";

	char buffer[32];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, fmt_buffer.c_str(), num );
	return std::string(buffer);
}


template<typename T>
inline std::string to_string( const std::vector<T>& src, const char *delimiter = ", " )
{
	std::string dest;
	const size_t num_elements = src.size();
	for( size_t i=0; i<num_elements; i++ )
	{
		dest += to_string(src[i]);
		if( i < num_elements - 1 )
			dest += delimiter;
	}

	return dest;
}


/// Converts a text that represents hexadecimal integer into int type value.
/// - example: hex_int = "0x000000FF" -> return value = 255
inline int from_hex_to_int( const std::string& hex_int )
{
	if( hex_int.length() == 0 )
		return 0;

	int res = 0;
	sscanf( hex_int.c_str(), "%x", &res );
	return res;
}


inline std::string fmt_string( const char *format,... )
{
//	g_LogMutex.lock();

	const int buffer_size = 2048;

	char buffer[buffer_size];
//	char *buffer = m_pBuffer;
	int size = buffer_size;//m_BufferSize;
	va_list argptr;

	va_start(argptr,format);
	vsnprintf(buffer,size,format,argptr);
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


/**
 * separate a given string by separators
 * \param pSrcStr [in] src string
 * \param rDestStr [out] dest buffer
 *
 * example:
 *  SeparateStrings( buf, "pos	1.0 2.0 -3.0", " \t" );
 *  then, you will get buf = [ "pos", "1.0", "2.0", "3.0" ]
 */
inline void SeparateStrings( std::vector<std::string>& rDestStr, const char *pSrcStr, const char *pSeparators )
{
	if( !pSrcStr || !pSeparators )
		return;

	size_t i, str_len = strlen(pSrcStr);
	size_t j, next_pos = 0, num_separaters = strlen(pSeparators);
	char str[256];

	for( i=0; i<str_len; i++ )
	{
		for( j=0; j<num_separaters; j++ )
		{
			if( pSrcStr[i] == pSeparators[j] )
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


inline size_t replace_all(std::string &str, const std::string &from, const std::string &to) {
	size_t count = 0;

	size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
		++count;
	}

	return count;
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


/**
Returns true if the target string has a valid length, its last char is a newline char, and it is successfully removed.
Returns false otherwise.
*/
inline bool remove_newline_char_at_the_end( std::string& target )
{
	if( 0 < target.length() && target[target.length() - 1] == '\n' )
	{
		target = target.substr( 0, target.length()-1 );
		return true;
	}
	else
		return false;
}

} // namespace amorphous



#endif  /*  __AMORPHOUS_STRING_AUX_HPP__  */
