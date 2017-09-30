#include "XMLDocumentBase.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"

#ifdef _BUILD_WITH_BOOST_PROPERTY_TREE_
#include "BoostPTreeXMLNode.hpp"
#include "BoostPTreeXMLDocument.hpp"
#endif // _BUILD_WITH_BOOST_PROPERTY_TREE_

#ifdef _BUILD_WITH_XERCESC_
#include "XMLDocumentBase.hpp"
#endif // _BUILD_WITH_XERCESC_


namespace amorphous
{


std::shared_ptr<XMLDocumentBase> CreateXMLDocument( const std::string& filepath )
{
	if( !lfs::path_exists( filepath ) )
	{
		LOG_PRINT_ERROR( "Couldn't find the file: " + filepath );
		return std::shared_ptr<XMLDocumentBase>();
	}

	std::shared_ptr<XMLDocumentBase> pDocument;

#ifdef _BUILD_WITH_XERCESC_
	XMLDocumentBaseLoader doc_loader;
	return doc_loader.Load( filepath );
#endif // _BUILD_WITH_XERCESC_

#ifdef _BUILD_WITH_BOOST_PROPERTY_TREE_
	pDocument.reset( new BoostPTreeXMLDocument( filepath ) );
//	BoostPTreeXMLDocument boost_ptree_xml_document;
#endif // _BUILD_WITH_BOOST_PROPERTY_TREE_

	return pDocument;
}


} // namespace amorphous
