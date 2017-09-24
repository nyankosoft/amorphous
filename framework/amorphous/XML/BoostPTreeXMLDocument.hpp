#ifndef __amorphous_BoostPTreeXMLDocument_HPP__
#define __amorphous_BoostPTreeXMLDocument_HPP__


#include "XMLDocumentBase.hpp"
#include "BoostPTreeXMLNode.hpp"
#include <boost/property_tree/xml_parser.hpp>


namespace amorphous
{


class BoostPTreeXMLDocument : public XMLDocumentBase
{
	boost::property_tree::ptree m_PropertyTree;

public:

	BoostPTreeXMLDocument() {}

	BoostPTreeXMLDocument( const std::string& xml_file_pathname )
	{
		boost::property_tree::read_xml( xml_file_pathname, m_PropertyTree );
	}

	~BoostPTreeXMLDocument() {};

	XMLNode GetRootNode()
	{
		boost::property_tree::ptree::iterator itr = m_PropertyTree.begin();
		for( ; itr != m_PropertyTree.end(); itr++ )
		{
			std::shared_ptr<BoostPTreeXMLNode> pImpl( new BoostPTreeXMLNode );
//			boost::property_tree::ptree child = m_PropertyTree.get_child( itr->first );
			pImpl->m_NodeName = itr->first;
			pImpl->m_PropertyTree = m_PropertyTree.get_child( itr->first );
			XMLNode node( pImpl );
			return node;
		}

		return XMLNode();
	}
};


} // namespace amorphous



#endif /* __amorphous_BoostPTreeXMLDocument_HPP__ */
