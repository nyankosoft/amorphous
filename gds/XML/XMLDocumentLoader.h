#ifndef __XMLDocumentLoader_H__
#define __XMLDocumentLoader_H__


#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
//#include <xercesc/dom/DOMWriter.hpp>
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

#ifdef _DEBUG
	#pragma comment( lib, "xerces-c_static_3D.lib" )
//	#pragma comment( lib, "xerces-depdom_3D.lib" )
#else // _DEBUG
	#pragma comment( lib, "xerces-c_static_3.lib" )
//	#pragma comment( lib, "xerces-depdom_3.lib" )
#endif // _DEBUG




//
// Global Functions
//
extern xercesc::DOMNode *GetRootNode( xercesc::DOMDocument *pXMLDocument );
extern xercesc::DOMNode *GetChildNode( xercesc::DOMNode *pParentNode, const std::string& node_name );
extern std::vector<xercesc::DOMNode *> GetImmediateChildNodes( xercesc::DOMNode *pParentNode,
										  const std::string& child_node_name );
extern std::string GetTextContentOfImmediateChildNode( xercesc::DOMNode *pParentNode,
												const std::string& child_node_name );
extern std::vector<std::string> GetTextContentsOfImmediateChildNodes( xercesc::DOMNode *pParentNode,
													 const std::string& child_node_name );
extern std::string GetAttributeText( xercesc::DOMNode *pNode, const std::string& attrib_name );

//
// Global Functions (inline)
//


/**
 - create an instance of this as a local variable in main() before using any XML parser classes
*/
class CXMLParserInitReleaseManager
{
public:

	CXMLParserInitReleaseManager() { xercesc::XMLPlatformUtils::Initialize(); }
	~CXMLParserInitReleaseManager() { xercesc::XMLPlatformUtils::Terminate(); }
};


class CXMLDocumentLoader
{
public:

	CXMLDocumentLoader() {}

	CXMLDocumentLoader( const std::string& src_fileapth, xercesc::DOMDocument** ppDoc );

	~CXMLDocumentLoader() {}

	/// Returns true on success
	bool Load( const std::string& filepath, xercesc::DOMDocument** ppDoc );

	bool Load( const XMLCh *src_fileapth, xercesc::DOMDocument** ppDoc );

//	xercesc::DOMNode *GetRootNode();
};


#endif /* __XMLDocumentLoader_H__ */
