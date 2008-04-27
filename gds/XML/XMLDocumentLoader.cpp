#include "XMLDocumentLoader.h"
namespace xer = xercesc_2_8;

#include "Support/StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "xmlch2x.h"


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
