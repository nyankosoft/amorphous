#ifndef __XMLDocumentLoader_H__
#define __XMLDocumentLoader_H__


#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMWriter.hpp>
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
	#pragma comment( lib, "xerces-c_2D.lib" )
	#pragma comment( lib, "xerces-depdom_2D.lib" )
#else // _DEBUG
	#pragma comment( lib, "xerces-c_2.lib" )
	#pragma comment( lib, "xerces-depdom_2.lib" )
#endif // _DEBUG




//
// Global Functions
//
extern xercesc_2_8::DOMNode *GetRootNode( xercesc_2_8::DOMDocument *pXMLDocument );
extern xercesc_2_8::DOMNode *GetChildNode( xercesc_2_8::DOMNode *pParentNode, const std::string& node_name );
extern std::vector<xercesc_2_8::DOMNode *> GetImmediateChildNodes( xercesc_2_8::DOMNode *pParentNode,
										  const std::string& child_node_name );
extern std::string GetTextContentOfImmediateChildNode( xercesc_2_8::DOMNode *pParentNode,
												const std::string& child_node_name );
extern std::vector<std::string> GetTextContentsOfImmediateChildNodes( xercesc_2_8::DOMNode *pParentNode,
													 const std::string& child_node_name );
extern std::string GetAttributeText( xercesc_2_8::DOMNode *pNode, const std::string& attrib_name );

//
// Global Functions (inline)
//


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

//	xercesc_2_8::DOMNode *GetRootNode();
};


#endif /* __XMLDocumentLoader_H__ */
