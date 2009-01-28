#ifndef _XERCESSTRING_H
#define _XERCESSTRING_H

#include <xercesc/util/XMLString.hpp>

// TODO:
// - Avoid conflicts with DOMDocument in Microsoft Visual Studio 8/VC/PlatformSDK/Include/msxml.h 

#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif

class XercesString
{
	XMLCh *_wstr;
public:
	XercesString() : _wstr(0L) { };
	XercesString(const char *str);
	XercesString(XMLCh *wstr);
	XercesString(const XMLCh *wstr);
	XercesString(const XercesString &copy);
	~XercesString();
	bool append(const XMLCh *tail);
	bool erase(const XMLCh *head, const XMLCh *tail);
	const XMLCh* begin() const;
	const XMLCh* end() const;
	int size() const;
	XMLCh & operator [] (const int i);
	const XMLCh operator [] (const int i) const;
	operator const XMLCh * () const { return _wstr; };

	inline bool operator==( const XercesString& rhs ) const
	{
		if( this->size() != rhs.size() )
			return false; // string lengthes did not match

		int num_chars = this->size();
		for( int i=0; i<num_chars; i++ )
		{
			if( operator[](i) != rhs[i] )
				return false;
		}

		return true;
	}

	inline bool operator!=( const XercesString& rhs ) const
	{
		return !( this->operator ==(rhs) );
	}
};

#endif
