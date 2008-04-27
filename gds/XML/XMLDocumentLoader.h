#ifndef __XMLDocumentLoader_H__
#define __XMLDocumentLoader_H__


#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <string>


/**
 - create an instance of this as a local variable in main() before using any XML parser classes
*/
class CXMLParserInitReleaseManager
{
public:

	CXMLParserInitReleaseManager() { xercesc_2_8::XMLPlatformUtils::Initialize(); }
	~CXMLParserInitReleaseManager() { xercesc_2_8::XMLPlatformUtils::Terminate(); }
};


class CXMLDocumentLoader
{
public:

	CXMLDocumentLoader() {}

	CXMLDocumentLoader( const std::string& src_fileapth, xercesc_2_8::DOMDocument** ppDoc );

	~CXMLDocumentLoader() {}

	/// Returns true on success
	bool Load( const std::string& filepath, xercesc_2_8::DOMDocument** ppDoc );

	bool Load( const XMLCh *src_fileapth, xercesc_2_8::DOMDocument** ppDoc );

	xercesc_2_8::DOMNode *GetRootNode();
};


#endif /* __XMLDocumentLoader_H__ */
