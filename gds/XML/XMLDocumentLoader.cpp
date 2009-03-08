#include "XMLDocumentLoader.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "XercesString.hpp"
#include "xmlch2x.hpp"

using namespace std;
using namespace boost;


//=======================================================================
// CXMLParserInitReleaseManager
//=======================================================================

CXMLParserInitReleaseManager::CXMLParserInitReleaseManager()
{
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const xercesc::XMLException& toCatch)
	{
		char *pMessage = xercesc::XMLString::transcode(toCatch.getMessage());
		LOG_PRINT_ERROR( "Error during XMLPlatformUtils::Initialize(). Message: " + string(pMessage) );
		xercesc::XMLString::release(&pMessage);
	}

	//xercesc::XMLPlatformUtils::Initialize();
}



//=======================================================================
// CXMLDocument
//=======================================================================

CXMLDocument::CXMLDocument( xercesc::DOMDocument *pDocument,
						    xercesc::XercesDOMParser *pParser )
:
m_pParser(pParser),
m_pLSParser(NULL),
m_pDocument(pDocument)
{
}

CXMLDocument::CXMLDocument( xercesc::DOMDocument *pDocument, xercesc::DOMLSParser *pParser )
:
m_pParser(NULL),
m_pLSParser(pParser),
m_pDocument(pDocument)
{
}

CXMLDocument::CXMLDocument()
:
m_pParser(NULL),
m_pLSParser(NULL),
m_pDocument(NULL)
{
}

CXMLDocument::~CXMLDocument()
{
//	SafeDelete( m_pDocument );
	SafeDelete( m_pParser );

	if( m_pLSParser )
	{
		m_pLSParser->release();
		SafeDelete( m_pParser );
//		m_pDocument->release();
//		SafeDelete( m_pDocument );
	}
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
		parser->setDoXInclude(true);
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
	// test
	// - How to use XInclude with Xerces C++ ???
	return LoadWithLSParser( filepath );

	xercesc::DOMDocument *pDoc = NULL;
	xercesc::XercesDOMParser *pParser = NULL;
	bool loaded = Load( filepath, &pDoc, &pParser );

	if( !loaded )
		return shared_ptr<CXMLDocument>();

	shared_ptr<CXMLDocument> pDocument = shared_ptr<CXMLDocument>( new CXMLDocument( pDoc, pParser ) );

	return pDocument;
}

/*
// ---------------------------------------------------------------------------
//  Simple error handler deriviative to install on parser
// ---------------------------------------------------------------------------
class XIncludeErrorHandler : public DOMErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XIncludeErrorHandler()
		:
    fSawErrors(false) 
	{}

	~XIncludeErrorHandler() {}


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;


    // -----------------------------------------------------------------------
    //  Implementation of the DOM ErrorHandler interface
    // -----------------------------------------------------------------------
    bool handleError(const DOMError& domError);
    void resetErrors();


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    XIncludeErrorHandler(const XIncludeErrorHandler&);
    void operator=(const XIncludeErrorHandler&);


    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------
    bool    fSawErrors;
};


// ---------------------------------------------------------------------------
//  XIncludeHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool XIncludeErrorHandler::handleError(const DOMError& domError)
{
	bool continueParsing = true;
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning at file ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
    {
        XERCES_STD_QUALIFIER cerr << "\nError at file ";
        fSawErrors = true;
    }
	else {
        XERCES_STD_QUALIFIER cerr << "\nFatal Error at file ";
		continueParsing = false;
        fSawErrors = true;
	}

    XERCES_STD_QUALIFIER cerr << StrX(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << StrX(domError.getMessage()) << XERCES_STD_QUALIFIER endl;

    return continueParsing;
}

void XIncludeErrorHandler::resetErrors()
{
    fSawErrors = false;
}

bool XIncludeErrorHandler::getSawErrors() const
{
    return fSawErrors;
}
*/

shared_ptr<CXMLDocument> CXMLDocumentLoader::LoadWithLSParser( const std::string& filepath )
{

	//============================================================================
	// Instantiate the DOM parser to use for the source documents
	//============================================================================
	static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
	DOMLSParser       *parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
	DOMConfiguration  *config = parser->getDomConfig();

	config->setParameter(XMLUni::fgDOMNamespaces, true);
	config->setParameter(XMLUni::fgXercesSchema, true);
	config->setParameter(XMLUni::fgXercesSchemaFullChecking, true);

	if(config->canSetParameter(XMLUni::fgXercesDoXInclude, true)){
		config->setParameter(XMLUni::fgXercesDoXInclude, true);
	}

	// enable datatype normalization - default is off
	//config->setParameter(XMLUni::fgDOMDatatypeNormalization, true);

	// And create our error handler and install it
//	XIncludeErrorHandler errorHandler;
//	config->setParameter(XMLUni::fgDOMErrorHandler, &errorHandler);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

	try
	{
        // load up the test source document
//		XERCES_STD_QUALIFIER cerr << "Parse " << filepath.c_str() << " in progress ...";
        parser->resetDocumentPool();
		doc = parser->parseURI( filepath.c_str() );
//		XERCES_STD_QUALIFIER cerr << " finished." << XERCES_STD_QUALIFIER endl;
	}
	catch (const XMLException& toCatch)
	{
		LOG_PRINT_ERROR( " Error during parsing: " + to_string(toCatch.getMessage()) );
//		XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << testFileName << "'\n"
//				<< "Exception message is:  \n"
//				<< StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
	}
	catch (const DOMException& toCatch)
	{
/*		const unsigned int maxChars = 2047;
		XMLCh errText[maxChars + 1];

		XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << testFileName << "'\n"
				<< "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

		if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
				XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;
*/
	}
	catch (...)
	{
//		XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << testFileName << "'\n";
	}
/*
	if (!errorHandler.getSawErrors() && doc) {
		DOMLSSerializer	*writer = ((DOMImplementationLS*)impl)->createLSSerializer();
		DOMLSOutput     *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();

		try {
			// write out the results
//			XERCES_STD_QUALIFIER cerr << "Writing result to: " << outputFileName << XERCES_STD_QUALIFIER endl;

			XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(outputFileName);
			theOutputDesc->setByteStream(myFormTarget);
			writer->write(doc, theOutputDesc);
			delete myFormTarget;
		}
		catch (const XMLException& toCatch)
		{
//			XERCES_STD_QUALIFIER cerr << "\nXMLException during writing: '" << testFileName << "'\n"
//				<< "Exception message is:  \n"
//				<< StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
		}
		catch (const DOMException& toCatch)
		{
//			const unsigned int maxChars = 2047;
//			XMLCh errText[maxChars + 1];

//			XERCES_STD_QUALIFIER cerr << "\nDOM Error during writing: '" << testFileName << "'\n"
//				<< "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

//			if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
//				XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;
		}
		catch (...)
		{
//			XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during writing: '" << testFileName << "'\n";
		}
		writer->release();
		theOutputDesc->release();
	}
*/
	//
	//  Delete the parser itself.  Must be done prior to calling Terminate, below.
	//
//	parser->release();

	shared_ptr<CXMLDocument> pDoc = shared_ptr<CXMLDocument>( new CXMLDocument( doc, parser ) );

	return pDoc;
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

