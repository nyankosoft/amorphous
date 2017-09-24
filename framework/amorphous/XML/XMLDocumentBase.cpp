#include "XMLDocumentBase.hpp"
#include <boost/filesystem.hpp>
#include "amorphous/Support/Log/DefaultLog.hpp"

#include "BoostPTreeXMLNode.hpp"
#include "BoostPTreeXMLDocument.hpp"

#ifdef _BUILD_WITH_XERCESC_
#include "XMLDocumentBase.hpp"
#endif // _BUILD_WITH_XERCESC_


namespace amorphous
{


std::shared_ptr<XMLDocumentBase> CreateXMLDocument( const std::string& filepath )
{
	if( !boost::filesystem::exists( filepath ) )
	{
		LOG_PRINT_ERROR( "Couldn't find the file: " + filepath );
		return std::shared_ptr<XMLDocumentBase>();
	}

	std::shared_ptr<XMLDocumentBase> pDocument;

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
