#include "XMLDocumentBase.hpp"

#include "BoostPTreeXMLNode.hpp"
#include "BoostPTreeXMLDocument.hpp"

#ifdef _BUILD_WITH_XERCESC_
#include "XMLDocumentBase.hpp"
#endif // _BUILD_WITH_XERCESC_


namespace amorphous
{


boost::shared_ptr<XMLDocumentBase> CreateXMLDocument( const std::string& filepath )
{
	boost::shared_ptr<XMLDocumentBase> pDocument;
#ifdef _BUILD_WITH_XERCESC_
	XMLDocumentBaseLoader doc_loader;
	return doc_loader.Load( filepath );
#else // _BUILD_WITH_XERCESC_
	pDocument.reset( new BoostPTreeXMLDocument( filepath ) );
//	BoostPTreeXMLDocument boost_ptree_xml_document;
#endif // _BUILD_WITH_XERCESC_

	return pDocument;
}


} // namespace amorphous
