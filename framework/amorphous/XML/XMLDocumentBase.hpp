#ifndef __amorphous_XMLDocumentBase_HPP__
#define __amorphous_XMLDocumentBase_HPP__


#include <vector>
#include <string>
#include <memory>
#include "XMLNode.hpp"


namespace amorphous
{

class XMLDocumentBase
{
public:

	XMLDocumentBase() {}

	virtual ~XMLDocumentBase() {};

	virtual XMLNode GetRootNode() { return XMLNode(); }
};

/*
class XMLDocumentBaseLoader
{
	// test
	std::shared_ptr<XMLDocumentBase> LoadWithLSParser( const std::string& filepath );

public:

	XMLDocumentBaseLoader();

	XMLDocumentBaseLoader( const std::string& src_fileapth,
		xercesc::DOMDocument** ppDoc,
		xercesc::XercesDOMParser **ppParser = NULL );

	~XMLDocumentBaseLoader();

	/// Returns true on success
	bool Load( const std::string& filepath, xercesc::DOMDocument** ppDoc, xercesc::XercesDOMParser **ppParser );

	bool Load( const XMLCh *src_fileapth, xercesc::DOMDocument** ppDoc, xercesc::XercesDOMParser **ppParser );

	std::shared_ptr<XMLDocumentBase> Load( const std::string& filepath );

//	xercesc::DOMNode *GetRootNode();
};
*/

std::shared_ptr<XMLDocumentBase> CreateXMLDocument( const std::string& filepath );



} // namespace amorphous



#endif /* __amorphous_XMLDocumentBase_HPP__ */
