#ifndef __XMLDocumentLoader_H__
#define __XMLDocumentLoader_H__


#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "XMLNodeReader.hpp"


namespace amorphous
{

#ifdef _DEBUG
	#pragma comment( lib, "xerces-c_3D.lib" )
#else // _DEBUG
	#pragma comment( lib, "xerces-c_3.lib" )
#endif // _DEBUG




/**
 - create an instance of this as a local variable in main() before using any XML parser classes
*/
class CXMLParserInitReleaseManager
{
public:

	CXMLParserInitReleaseManager();

	~CXMLParserInitReleaseManager() { xercesc::XMLPlatformUtils::Terminate(); }
};


class CXMLDocument
{
	xercesc::XercesDOMParser *m_pParser;

	xercesc::DOMLSParser *m_pLSParser;

	xercesc::DOMDocument *m_pDocument;

public:

	CXMLDocument( xercesc::DOMDocument *pDocument, xercesc::XercesDOMParser *pParser );

	CXMLDocument( xercesc::DOMDocument *pDocument, xercesc::DOMLSParser *pParser );

	CXMLDocument();

	~CXMLDocument();

	CXMLNodeReader GetRootNodeReader();
};


class CXMLDocumentLoader
{
	// test
	boost::shared_ptr<CXMLDocument> CXMLDocumentLoader::LoadWithLSParser( const std::string& filepath );

public:

	CXMLDocumentLoader();

	CXMLDocumentLoader( const std::string& src_fileapth,
		xercesc::DOMDocument** ppDoc,
		xercesc::XercesDOMParser **ppParser = NULL );

	~CXMLDocumentLoader();

	/// Returns true on success
	bool Load( const std::string& filepath, xercesc::DOMDocument** ppDoc, xercesc::XercesDOMParser **ppParser );

	bool Load( const XMLCh *src_fileapth, xercesc::DOMDocument** ppDoc, xercesc::XercesDOMParser **ppParser );

	boost::shared_ptr<CXMLDocument> Load( const std::string& filepath );

//	xercesc::DOMNode *GetRootNode();
};


inline boost::shared_ptr<CXMLDocument> CreateXMLDocument( const std::string& filepath )
{
	CXMLDocumentLoader doc_loader;
	return doc_loader.Load( filepath );
}


} // namespace amorphous



#endif /* __XMLDocumentLoader_H__ */
