#ifndef __amorphous_XercesXMLDocumentLoader_HPP__
#define __amorphous_XercesXMLDocumentLoader_HPP__


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
#include "amorphous/XML/XMLDocumentBase.hpp"
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


class XercesXMLDocument : public XMLDocumentBase
{
	xercesc::XercesDOMParser *m_pParser;

	xercesc::DOMLSParser *m_pLSParser;

	xercesc::DOMDocument *m_pDocument;

public:

	XercesXMLDocument( xercesc::DOMDocument *pDocument, xercesc::XercesDOMParser *pParser );

	XercesXMLDocument( xercesc::DOMDocument *pDocument, xercesc::DOMLSParser *pParser );

	XercesXMLDocument();

	~XercesXMLDocument();

	CXMLNodeReader GetRootNodeReader();
};


class XercesXMLDocumentLoader
{
	// test
	std::shared_ptr<XercesXMLDocument> LoadWithLSParser( const std::string& filepath );

public:

	XercesXMLDocumentLoader();

	XercesXMLDocumentLoader( const std::string& src_fileapth,
		xercesc::DOMDocument** ppDoc,
		xercesc::XercesDOMParser **ppParser = NULL );

	~XercesXMLDocumentLoader();

	/// Returns true on success
	bool Load( const std::string& filepath, xercesc::DOMDocument** ppDoc, xercesc::XercesDOMParser **ppParser );

	bool Load( const XMLCh *src_fileapth, xercesc::DOMDocument** ppDoc, xercesc::XercesDOMParser **ppParser );

	std::shared_ptr<XercesXMLDocument> Load( const std::string& filepath );

//	xercesc::DOMNode *GetRootNode();
};


inline std::shared_ptr<XercesXMLDocument> CreateXMLDocument( const std::string& filepath )
{
	XercesXMLDocumentLoader doc_loader;
	return doc_loader.Load( filepath );
}


} // namespace amorphous



#endif /* __amorphous_XercesXMLDocumentLoader_HPP__ */
