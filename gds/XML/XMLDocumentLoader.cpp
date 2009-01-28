#include "XMLDocumentLoader.h"
#include "Support/SafeDelete.h"
#include "Support/StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "XercesString.h"
#include "xmlch2x.h"

using namespace std;
using namespace boost;


//=======================================================================
// CXMLDocument
//=======================================================================

CXMLDocument::CXMLDocument( xercesc::DOMDocument *pDocument,
						    xercesc::XercesDOMParser *pParser )
:
m_pParser(pParser),
m_pDocument(pDocument)
{
}

CXMLDocument::CXMLDocument()
:
m_pParser(NULL),
m_pDocument(NULL)
{
}

CXMLDocument::~CXMLDocument()
{
//	SafeDelete( m_pDocument );
	SafeDelete( m_pParser );
}


CXMLNodeReader CXMLDocument::GetRootNodeReader()
{
	DOMNode *pRootNode = GetRootNode( m_pDocument );

	return CXMLNodeReader( pRootNode );
}



//=======================================================================
// CXMLDocumentLoader
//=======================================================================

CXMLDocumentLoader::CXMLDocumentLoader()
{
}


/// Deprecated. Use shared_ptr<CXMLDocument> CXMLDocumentLoader::Load( const std::string& filepath )
CXMLDocumentLoader::CXMLDocumentLoader( const std::string& src_fileapth,
									   xercesc::DOMDocument** ppDoc,
									   xercesc::XercesDOMParser **ppParser )
{
	Load( src_fileapth, ppDoc, ppParser );
}


CXMLDocumentLoader::~CXMLDocumentLoader()
{
//	SafeDelete( m_pParser );
}


bool CXMLDocumentLoader::Load( const std::string& src_fileapth,
							  xercesc::DOMDocument** ppDoc,
							  xercesc::XercesDOMParser **ppParser )
{
	return Load( XercesString(src_fileapth.c_str()), ppDoc, ppParser );
}


bool CXMLDocumentLoader::Load( const XMLCh *src_fileapth,
							   xercesc::DOMDocument** ppDoc,
							   xercesc::XercesDOMParser **pParser )
{
	bool bSuccess = false;
	//DOMString strMessage;

	xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser;
	if (parser)
	{
		char *strSrc = xercesc::XMLString::transcode(src_fileapth);
//		const char *strSrc = src_filepath.c_str();
		parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
		parser->setDoNamespaces(false);
		parser->setDoSchema(false);
		parser->setCreateEntityReferenceNodes(false);
		//parser->setToCreateXMLDeclTypeNode(true);
		try
		{
			xercesc::LocalFileInputSource source(src_fileapth);
			bSuccess = false;

			parser->parse(source);

			bSuccess = parser->getErrorCount() == 0;
			if (!bSuccess)
			{
				LOG_PRINT_ERROR( fmt_string("Parsing %s - error count: %d", strSrc, parser->getErrorCount() ) );
			}
		}
		catch (const xercesc::DOMException& e)
		{
			LOG_PRINT_ERROR( "DOM Exception parsing " + std::string(strSrc) );

			std::cerr << " reports: ";
			// was message provided?
			if (e.msg)
			{
				// yes: display it as ascii.
				char *strMsg = xercesc::XMLString::transcode(e.msg);
				std::cerr << strMsg << std::endl;
				xercesc::XMLString::release(&strMsg);
			}
			else
			// no: just display the error code.
			std::cerr << e.code << std::endl;
		}
		catch (const xercesc::XMLException& e)
		{
			std::cerr << "XML Exception parsing ";
			std::cerr << strSrc;
			std::cerr << " reports: ";
			std::cerr << e.getMessage() << std::endl;
		}
		catch (const xercesc::SAXException& e)
		{
			std::cerr << "SAX Exception parsing ";
			std::cerr << strSrc;
			std::cerr << " reports: ";
			std::cerr << e.getMessage() << std::endl;
		}
		catch (...)
		{
			std::cerr << "An exception parsing ";
			std::cerr << strSrc << std::endl;
		}
		// did the input document parse okay?
		if (bSuccess)
		{
			*ppDoc = parser->getDocument();
			if( pParser )
				*pParser = parser;
		}

		xercesc::XMLString::release(&strSrc);

	}
	return bSuccess;
}


shared_ptr<CXMLDocument> CXMLDocumentLoader::Load( const std::string& filepath )
{
	xercesc::DOMDocument *pDoc = NULL;
	xercesc::XercesDOMParser *pParser = NULL;
	bool loaded = Load( filepath, &pDoc, &pParser );

	if( !loaded )
		return shared_ptr<CXMLDocument>();

	shared_ptr<CXMLDocument> pDocument = shared_ptr<CXMLDocument>( new CXMLDocument( pDoc, pParser ) );

	return pDocument;
}


/*

Need to make pXMLDocument a member variable of CXMLDocumentLoader to do this

xercesc::DOMNode *CXMLDocumentLoader::GetRootNode()
{
	xercesc::DOMDocument *pXMLDocument = NULL;

	CXMLDocumentLoader xml_doc_loader;
	xml_doc_loader.Load( source_script_filename, &pXMLDocument );

	xercesc::DOMElement *pElem = pXMLDocument->getDocumentElement();

	xercesc::DOMNodeIterator *iterator
		= pXMLDocument->createNodeIterator( pXMLDocument->getFirstChild(), xercesc::DOMNodeFilter::SHOW_TEXT, NULL, false );

	return iterator->getRoot();
}
*/


DOMNode *GetRootNode( xercesc::DOMDocument *pXMLDocument )
{	
	xercesc::DOMElement *pElem = pXMLDocument->getDocumentElement();

	xercesc::DOMNodeIterator *iterator
		= pXMLDocument->createNodeIterator( pXMLDocument->getFirstChild(), xercesc::DOMNodeFilter::SHOW_TEXT, NULL, false );

	DOMNode *pRootNode = iterator->getRoot();

	if( pRootNode )
		return pRootNode;
	else
	{
		LOG_PRINT_WARNING( "- Cannot find a root node" );
		return NULL;
	}

	return NULL;
}



std::string GetAttributeText( xercesc::DOMNode *pNode, const std::string& attrib_name )
{

	const xercesc::DOMNamedNodeMap *pAttrib = pNode->getAttributes();

	if( !pAttrib )
	{
//		LOG_PRINT_WARNING( " - No attribute named " );
		return std::string();
	}

	xercesc::DOMNode *pNameNode = pAttrib->getNamedItem( XercesString(attrib_name.c_str()) );

	if( pNameNode )
		return to_string(pNameNode->getNodeValue());
	else
		return std::string();
}


xercesc::DOMNode *GetChildNode( xercesc::DOMNode *pParentNode, const std::string& node_name )
{
	if( !pParentNode )
		return NULL;

	xercesc::DOMNodeList *pNodeList = pParentNode->getChildNodes();
	const int num_nodes = (int)pNodeList->getLength();
	for( int i=0; i<num_nodes; i++ )
	{
		if( to_string(pNodeList->item(i)->getNodeName()) == node_name )
			return pNodeList->item(i);
	}

	return NULL; // not found
}


vector<xercesc::DOMNode *> GetImmediateChildNodes( xercesc::DOMNode *pParentNode,
										  const std::string& child_node_name )
{
	vector<xercesc::DOMNode *> child_nodes;
	xercesc::DOMNodeList *pNodeList = pParentNode->getChildNodes();
	const int num_nodes = (int)pNodeList->getLength();
	for( int i=0; i<num_nodes; i++ )
	{
		if( to_string(pNodeList->item(i)->getNodeName()) == child_node_name )
			child_nodes.push_back( pNodeList->item(i) );
	}

	return child_nodes;
}


std::string GetTextContentOfImmediateChildNode( xercesc::DOMNode *pParentNode,
												const std::string& child_node_name )
{
	xercesc::DOMNode *pNode = GetChildNode( pParentNode, child_node_name );

	if( !pNode )
		return string();

	return string( to_string( pNode->getTextContent() ) );
}


/**
 Returns an array of text contents of immediate child nodes named child_node_name
 - e.g.
 <Directory>
   <File>a.txt</File>
   <File>b.txt</File>
   <File>c.txt</File>
</Directory>

 - If the 1st arg, pParentNode, points to <Directory> and the 2nd arg,
   child_node_name, is "File", the function returns ["a.txt", "b.txt", "c.txt"]

*/
vector<string> GetTextContentsOfImmediateChildNodes( xercesc::DOMNode *pParentNode,
													 const std::string& child_node_name )
{
	vector<xercesc::DOMNode *> nodes = GetImmediateChildNodes( pParentNode, child_node_name );

	vector<string> text_contents;
	const size_t num_nodes = nodes.size();
	for( size_t i=0; i<num_nodes; i++ )
	{
		text_contents.push_back( to_string(nodes[i]->getTextContent()) );
	}

	return text_contents;
}

