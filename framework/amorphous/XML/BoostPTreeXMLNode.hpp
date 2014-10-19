#ifndef __amorphous_BoostPTreeXMLNode_HPP__
#define __amorphous_BoostPTreeXMLNode_HPP__


#include <boost/property_tree/xml_parser.hpp>
#include "XMLNode.hpp"


namespace amorphous
{


class BoostPTreeXMLNode : public XMLNodeImpl
{
	std::string m_NodeName;

	boost::property_tree::ptree m_PropertyTree;

public:

	BoostPTreeXMLNode() {}

	BoostPTreeXMLNode( boost::property_tree::ptree& pt  ) : m_PropertyTree(pt) {}

	~BoostPTreeXMLNode() {}

	bool IsValid() const { return true; }

	bool GetTargetElementNodeTextContent( const std::string& child_element_path, std::string& dest )
	{
		return false;
	}

	/// get the text content of a child node
//	template<typename T>
//	inline bool GetChildElementTextContent( const std::string& child_element_path, T& dest );
	inline bool GetChildElementTextContent( const std::string& child_element_path, std::string& dest );
	inline bool GetChildElementTextContentLTWH( const std::string& child_element_path, SRect& dest );
	inline bool GetChildElementTextContentLTRB( const std::string& child_element_path, SRect& dest );
	inline bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBColor& dest );
	inline bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& dest );
	inline bool GetChildElementTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& dest );
	inline bool GetChildElementTextContent( const std::string& child_element_path, Vector3& dest );

	/// get the text content of the current node
//	inline std::string GetTextContent() { std::string dest; GetTextContent(dest); return dest; }
//	template<typename T>
//	inline bool GetTextContent( T& dest )                   { return GetChildElementTextContent( "", dest ); }
//	inline bool GetTextContentLTWH( SRect& dest )           { return GetChildElementTextContentLTWH( "", dest ); }
//	inline bool GetTextContentLTRB( SRect& dest )           { return GetChildElementTextContentLTRB( "", dest ); }
//	inline bool GetTextContentRGB( SFloatRGBColor& dest )   { return GetChildElementTextContentRGB( "", dest ); }
//	inline bool GetTextContentRGB( SFloatRGBAColor& dest )  { return GetChildElementTextContentRGB( "", dest ); }
//	inline bool GetTextContentRGBA( SFloatRGBAColor& dest ) { return GetChildElementTextContentRGBA( "", dest ); }
//	inline bool GetTextContent( Vector3& dest )             { return GetChildElementTextContent( "", dest ); }

	inline std::string GetName();

	inline XMLNode GetChild( const std::string& name );

	inline std::string GetAttributeText( const std::string& attrib_name );

	inline std::vector<XMLNode> GetImmediateChildren( const std::string& name );

	inline std::vector<XMLNode> GetImmediateChildren();

	friend class BoostPTreeXMLDocument;
};

//===================================== inline implementations ======================================


inline std::string BoostPTreeXMLNode::GetName()
{
	return m_NodeName;
}


inline XMLNode BoostPTreeXMLNode::GetChild( const std::string& name )
{
	boost::property_tree::ptree empty_tree;
	boost::shared_ptr<XMLNodeImpl> pImpl( new BoostPTreeXMLNode( m_PropertyTree.get_child( name, empty_tree ) ) );
	XMLNode node( pImpl );
	return node;
}


inline std::string BoostPTreeXMLNode::GetAttributeText( const std::string& attrib_name )
{
	std::string empty_text;
	std::string attrib_text = m_PropertyTree.get( "<xmlattr>." + attrib_name, empty_text );
	return attrib_text;
}


inline std::vector<XMLNode> BoostPTreeXMLNode::GetImmediateChildren( const std::string& name )
{
	std::vector<XMLNode> children;

	for( auto itr = m_PropertyTree.begin(); itr != m_PropertyTree.end(); itr++ )
	{
		std::string node_name = itr->first;
		if( node_name != name )
			continue;

		boost::shared_ptr<BoostPTreeXMLNode> pChild( new BoostPTreeXMLNode() );
		pChild->m_NodeName     = itr->first;
		pChild->m_PropertyTree = itr->second;
		XMLNode node( pChild );

		children.push_back( node );
	}

	return children;
}


inline std::vector<XMLNode> BoostPTreeXMLNode::GetImmediateChildren()
{
	std::vector<XMLNode> children;

	for( auto itr = m_PropertyTree.begin(); itr != m_PropertyTree.end(); itr++ )
	{
		boost::shared_ptr<BoostPTreeXMLNode> pChild( new BoostPTreeXMLNode() );
		pChild->m_NodeName     = itr->first;
		pChild->m_PropertyTree = itr->second;
		XMLNode node( pChild );

		children.push_back( node );
	}

	return children;
}


inline bool BoostPTreeXMLNode::GetChildElementTextContent( const std::string& child_element_path, std::string& dest )
{
	if( m_PropertyTree.find( child_element_path ) == m_PropertyTree.not_found() )
		return false;

	dest = m_PropertyTree.get( child_element_path, std::string("") );

	return true;
}


inline bool BoostPTreeXMLNode::GetChildElementTextContentLTWH( const std::string& child_element_path, SRect& rect )
{
	std::string text_content;
	bool res = GetTargetElementNodeTextContent( child_element_path, text_content );
	if( res )
	{
		int l=0,t=0,w=0,h=0;
		sscanf( text_content.c_str(), "%d %d %d %d", &l, &t, &w, &h );
		rect = RectLTWH( l, t, w, h );
		return true;
	}
	else
		return false;
}

inline bool BoostPTreeXMLNode::GetChildElementTextContentLTRB( const std::string& child_element_path, SRect& rect )
{
	std::string text_content;
	bool res = GetTargetElementNodeTextContent( child_element_path, text_content );
	if( res )
	{
		int l=0,t=0,r=0,b=0;
		sscanf( text_content.c_str(), "%d %d %d %d", &l, &t, &r, &b );
		rect = RectLTRB( l, t, r, b );
		return true;
	}
	else
		return false;
}

inline bool BoostPTreeXMLNode::GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBColor& color )
{
	std::string text_content;
	bool res = GetTargetElementNodeTextContent( child_element_path, text_content );
	if( res )
	{
		sscanf( text_content.c_str(), "%f %f %f", &color.red, &color.green, &color.blue );
		return true;
	}
	else
		return false;
}

inline bool BoostPTreeXMLNode::GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& color )
{
	std::string text_content;
	bool res = GetTargetElementNodeTextContent( child_element_path, text_content );
	if( res )
	{
		sscanf( text_content.c_str(), "%f %f %f", &color.red, &color.green, &color.blue );
		return true;
	}
	else
		return false;
}

inline bool BoostPTreeXMLNode::GetChildElementTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& color )
{
	std::string text_content;
	bool res = GetTargetElementNodeTextContent( child_element_path, text_content );
	if( res )
	{
		sscanf( text_content.c_str(), "%f %f %f %f", &color.red, &color.green, &color.blue, &color.alpha );
		return true;
	}
	else
		return false;
}

inline bool BoostPTreeXMLNode::GetChildElementTextContent( const std::string& child_element_path, Vector3& v )
{
	std::string text_content;
	bool res = GetTargetElementNodeTextContent( child_element_path, text_content );
	if( res )
	{
		sscanf( text_content.c_str(), "%f %f %f", &v.x, &v.y, &v.z );
		return true;
	}
	else
		return false;

}


} // namespace amorphous


#endif /* __amorphous_BoostPTreeXMLNode_HPP__ */
