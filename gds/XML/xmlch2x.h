#ifndef  __XMLCH2X_H__
#define  __XMLCH2X_H__


#include "XercesString.h"
#include <string>


inline int to_int( const XMLCh *pChar )
{
	return atoi( xercesc_2_8::XMLString::transcode( pChar ) );
}


inline float to_float( const XMLCh *pChar )
{
	std::string buffer = xercesc_2_8::XMLString::transcode(pChar);
	float val;

	sscanf( buffer.c_str(), "%f", &val );
	return val;
}


/// "true" -> true
/// anything else -> false
inline bool to_bool( const XMLCh *pChar, const std::string& true_str = "true", const std::string& false_str = "false" )
{
	std::string input_string = std::string(xercesc_2_8::XMLString::transcode(pChar));

	if( input_string == true_str )
		return true;
	else if( input_string == false_str )
		return false;
	else
		return false;
}


inline std::string to_string( const XMLCh *pChar )
{
	if( pChar )
		return xercesc_2_8::XMLString::transcode( XercesString(pChar) );
	else
		return std::string("");
}

/*
inline std::string to_string( const XercesString& xerces_string )
{
#ifdef _NATIVE_WCHAR_T_DEFINED 

	// typedef wchar_t  XMLCh;

	return string();

#else // _NATIVE_WCHAR_T_DEFINED

	// typedef unsigned short  XMLCh;

	return string();

#endif // _NATIVE_WCHAR_T_DEFINED
}
*/

/*
inline std::string from_wchar_to_string( const wchar_t *pOrig )
{
	const size_t newsize = 256;

	size_t origsize = wcslen(pOrig) + 1;
	size_t convertedChars = 0;
	char nstring[newsize];
	wcstombs_s(&convertedChars, nstring, origsize, pOrig, _TRUNCATE);

	return string(nstring);
}
*/


#endif /* __XMLCH2X_H__ */
