#include "XMLDocumentLoader.h"
namespace xer = xercesc_2_8;

#include "Support/StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "XercesString.h"
#include "xmlch2x.h"

using namespace std;


CXMLDocumentLoader::CXMLDocumentLoader( const std::string& src_fileapth, xercesc_2_8::DOMDocument** ppDoc )
{
	Load( src_fileapth, ppDoc );
}


bool CXMLDocumentLoader::Load( const std::string& src_fileapth, xercesc_2_8::DOMDocument** ppDoc )
{
	return Load( XercesString(src_fileapth.c_str()), ppDoc );
}


bool CXMLDocumentLoader::Load( const XMLCh *src_fileapth, xercesc_2_8::DOMDocument** ppDoc )
{
	bool bSuccess = false;
	//DOMString strMessage;

	xercesc_2_8::XercesDOMParser *parser = new xercesc_2_8::XercesDOMParser;
	if (parser)
	{
		char *strSrc = xer::XMLString::transcode(src_fileapth);
//		const char *strSrc = src_filepath.c_str();
		parser->setValidationScheme(xer::XercesDOMParser::Val_Auto);
		parser->setDoNamespaces(false);
		parser->setDoSchema(false);
		parser->setCreateEntityReferenceNodes(false);
		//parser->setToCreateXMLDeclTypeNode(true);
		try
		{
			xer::LocalFileInputSource source(src_fileapth);
			bSuccess = false;

			parser->parse(source);

			bSuccess = parser->getErrorCount() == 0;
			if (!bSuccess)
			{
				LOG_PRINT_ERROR( fmt_string("Parsing %s - error count: %d", strSrc, parser->getErrorCount() ) );
			}
		}
		catch (const xer::DOMException& e)
		{
			LOG_PRINT_ERROR( "DOM Exception parsing " + std::string(strSrc) );

			std::cerr << " reports: ";
			// was message provided?
			if (e.msg)
			{
				// yes: display it as ascii.
				char *strMsg = xer::XMLString::transcode(e.msg);
				std::cerr << strMsg << std::endl;
				xer::XMLString::release(&strMsg);
			}
			else
			// no: just display the error code.
			std::cerr << e.code << std::endl;
		}
		catch (const xer::XMLException& e)
		{
			std::cerr << "XML Exception parsing ";
			std::cerr << strSrc;
			std::cerr << " reports: ";
			std::cerr << e.getMessage() << std::endl;
		}
		catch (const xer::SAXException& e)
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
		  *ppDoc = parser->getDocument();

		xer::XMLString::release(&strSrc);
	}
	return bSuccess;
}

/*

Need to make pXMLDocument a member variable of CXMLDocumentLoader to do this

xercesc_2_8::DOMNode *CXMLDocumentLoader::GetRootNode()
{
	xercesc_2_8::DOMDocument *pXMLDocument = NULL;

	CXMLDocumentLoader xml_doc_loader;
	xml_doc_loader.Load( source_script_filename, &pXMLDocument );

	xer::DOMElement *pElem = pXMLDocument->getDocumentElement();

	xercesc_2_8::DOMNodeIterator *iterator
		= pXMLDocument->createNodeIterator( pXMLDocument->getFirstChild(), xer::DOMNodeFilter::SHOW_TEXT, NULL, false );

	return iterator->getRoot();
}
*/


DOMNode *GetRootNode( xercesc_2_8::DOMDocument *pXMLDocument )
{	
	xercesc_2_8::DOMElement *pElem = pXMLDocument->getDocumentElement();

	xercesc_2_8::DOMNodeIterator *iterator
		= pXMLDocument->createNodeIterator( pXMLDocument->getFirstChild(), xercesc_2_8::DOMNodeFilter::SHOW_TEXT, NULL, false );

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



std::string GetAttributeText( xercesc_2_8::DOMNode *pNode, const std::string& attrib_name )
{

	const xercesc_2_8::DOMNamedNodeMap *pAttrib = pNode->getAttributes();

	if( !pAttrib )
	{
//		LOG_PRINT_WARNING( " - No attribute named " );
		return std::string();
	}

	xercesc_2_8::DOMNode *pNameNode = pAttrib->getNamedItem( XercesString(attrib_name.c_str()) );

	return to_string(pNameNode->getNodeValue());
}


xercesc_2_8::DOMNode *GetChildNode( xercesc_2_8::DOMNode *pParentNode, const std::string& node_name )
{
	xercesc_2_8::DOMNodeList *pNodeList = pParentNode->getChildNodes();
	const int num_nodes = (int)pNodeList->getLength();
	for( int i=0; i<num_nodes; i++ )
	{
		if( to_string(pNodeList->item(i)->getNodeName()) == node_name )
			return pNodeList->item(i);
	}

	return NULL; // not found
}


vector<xercesc_2_8::DOMNode *> GetImmediateChildNodes( xercesc_2_8::DOMNode *pParentNode,
										  const std::string& child_node_name )
{
	vector<xercesc_2_8::DOMNode *> child_nodes;
	xercesc_2_8::DOMNodeList *pNodeList = pParentNode->getChildNodes();
	const int num_nodes = (int)pNodeList->getLength();
	for( int i=0; i<num_nodes; i++ )
	{
		if( to_string(pNodeList->item(i)->getNodeName()) == child_node_name )
			child_nodes.push_back( pNodeList->item(i) );
	}

	return child_nodes;
}


std::string GetTextContentOfImmediateChildNode( xercesc_2_8::DOMNode *pParentNode,
												const std::string& child_node_name )
{
	xercesc_2_8::DOMNode *pNode = GetChildNode( pParentNode, child_node_name );

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
vector<string> GetTextContentsOfImmediateChildNodes( xercesc_2_8::DOMNode *pParentNode,
													 const std::string& child_node_name )
{
	vector<xercesc_2_8::DOMNode *> nodes = GetImmediateChildNodes( pParentNode, child_node_name );

	vector<string> text_contents;
	const size_t num_nodes = nodes.size();
	for( size_t i=0; i<num_nodes; i++ )
	{
		text_contents.push_back( to_string(nodes[i]->getTextContent()) );
	}

	return text_contents;
}

