#ifndef __XMLNodeReader_H__
#define __XMLNodeReader_H__

#include <string>
#include <vector>
#include "3DMath/Vector3.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/FloatRGBAColor.h"
#include "Graphics/Rect.h"
using namespace Graphics;

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "XMLDocumentLoader.h"
#include "xmlch2x.h"

/*
	<AABB3>
		<Min></Min>
		<Max></Max>
	</AABB3>

	<PointLight>
		<Pos>1.0 3.0 1.0</Pos>
		<Color>1.0 0.2 0.2</Color>
		<Range>100.0f</Range>
	</PointLight>

	<HemisphericPointLight>
		<Pos>1.0 3.0 1.0</Pos>
		<Color>
			<Upper>1.0 0.2 0.2</Upper>
			<Lower>0.2 0.0 0.0</Lower>
		</Color>
	</HemisphericPointLight>

/*
	CXMLNodeReader loader( pNode );
	loader.get( "Pos", vPos );
	loader.get( "Color", color );
	CXMLNodeReader color_node = load.GetChildNode( "Color" );
	color_node.GetTextContentRGB( "Upper", hs_point_light.Attribute.UpperColor );
	color_node.GetTextContentRGB( "Lower", hs_point_light.Attribute.UpperColor );

	CXMLNodeReader light_loader = loader.get_child( "HemisphericPointLight" );
	light_loader.GetTextContent( "Pos", hs_point_light.vPosition );
	light_loader.GetTextContentRGB( "Color/Upper", hs_point_light.Attribute.UpperColor );
	light_loader.GetTextContentRGB( "Color/Lower", hs_point_light.Attribute.UpperColor );
*/


//
// Global Functions (defined in XMLDocumentLoader.cpp)
//
extern xercesc::DOMNode *GetRootNode( xercesc::DOMDocument *pXMLDocument );
extern xercesc::DOMNode *GetChildNode( xercesc::DOMNode *pParentNode, const std::string& node_name );
extern std::vector<xercesc::DOMNode *> GetImmediateChildNodes( xercesc::DOMNode *pParentNode,
										  const std::string& child_node_name );
extern std::string GetTextContentOfImmediateChildNode( xercesc::DOMNode *pParentNode,
												const std::string& child_node_name );
extern std::vector<std::string> GetTextContentsOfImmediateChildNodes( xercesc::DOMNode *pParentNode,
													 const std::string& child_node_name );
extern std::string GetAttributeText( xercesc::DOMNode *pNode, const std::string& attrib_name );


class CXMLNodeReader
{
	/// borrowed reference
	xercesc::DOMNode *m_pNode;

public:

	CXMLNodeReader( xercesc::DOMNode* pNode ) : m_pNode(pNode) {}

	~CXMLNodeReader() {}

	xercesc::DOMNode *GetDOMNode() { return m_pNode; }

	bool IsValid() const { return ( m_pNode != NULL ); }

	inline xercesc::DOMNode *GetTargetElementNode( const std::string& name );

	/// get the text content of a child node
	inline bool GetChildElementTextContent( const std::string& child_element_path, std::string& dest );
	inline bool GetChildElementTextContent( const std::string& child_element_path, float& dest );
	inline bool GetChildElementTextContent( const std::string& child_element_path, int& dest );
	inline bool GetChildElementTextContentLTWH( const std::string& child_element_path, SRect& dest );
	inline bool GetChildElementTextContentLTRB( const std::string& child_element_path, SRect& dest );
	inline bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBColor& dest );
	inline bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& dest );
	inline bool GetChildElementTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& dest );
	inline bool GetChildElementTextContent( const std::string& child_element_path, Vector3& dest );

	/// get the text content of the current node
	inline std::string GetTextContent() { std::string dest; GetTextContent(dest); return dest; }
	inline bool GetTextContent( std::string& dest )         { return GetChildElementTextContent( "", dest ); }
	inline bool GetTextContent( float& dest )               { return GetChildElementTextContent( "", dest ); }
	inline bool GetTextContent( int& dest )                 { return GetChildElementTextContent( "", dest ); }
	inline bool GetTextContentLTWH( SRect& dest )           { return GetChildElementTextContentLTWH( "", dest ); }
	inline bool GetTextContentLTRB( SRect& dest )           { return GetChildElementTextContentLTRB( "", dest ); }
	inline bool GetTextContentRGB( SFloatRGBColor& dest )   { return GetChildElementTextContentRGB( "", dest ); }
	inline bool GetTextContentRGB( SFloatRGBAColor& dest )  { return GetChildElementTextContentRGB( "", dest ); }
	inline bool GetTextContentRGBA( SFloatRGBAColor& dest ) { return GetChildElementTextContentRGBA( "", dest ); }
	inline bool GetTextContent( Vector3& dest )             { return GetChildElementTextContent( "", dest ); }

	inline std::string GetName();

	inline CXMLNodeReader GetChild( const std::string& name );

	inline std::string GetAttributeText( const std::string& attrib_name );

	inline std::vector<CXMLNodeReader> GetImmediateChildren( const std::string& name );

	inline std::vector<CXMLNodeReader> GetImmediateChildren();
};

//===================================== inline implementations ======================================


/// get element node of the specified name
inline xercesc::DOMNode *CXMLNodeReader::GetTargetElementNode( const std::string& child_element_path )
{
	if( child_element_path.length() == 0 )
		return m_pNode;

	size_t slash_pos = child_element_path.find( "/" );
	if( slash_pos == std::string::npos )
	{
		return GetChildNode( m_pNode, child_element_path );
	}
	else
	{
		if( slash_pos == child_element_path.length() - 1 )
			return NULL;

		CXMLNodeReader loader( GetChildNode( m_pNode, child_element_path.substr( 0, slash_pos ) ) );
		return loader.GetTargetElementNode( child_element_path.substr( slash_pos + 1 ) );
	}
}


inline std::string CXMLNodeReader::GetName()
{
	if( m_pNode )
		return to_string(m_pNode->getNodeName());
	else
		return std::string();
}


inline CXMLNodeReader CXMLNodeReader::GetChild( const std::string& name )
{
	return CXMLNodeReader( GetChildNode( m_pNode, name ) );
}


inline std::string CXMLNodeReader::GetAttributeText( const std::string& attrib_name )
{
	if( m_pNode )
		return ::GetAttributeText( m_pNode, attrib_name );
	else
		return std::string();
}


inline std::vector<CXMLNodeReader> CXMLNodeReader::GetImmediateChildren( const std::string& name )
{
	std::vector<xercesc::DOMNode *> vecpChild = GetImmediateChildNodes( m_pNode, name );
	std::vector<CXMLNodeReader> children;
	const size_t num = vecpChild.size();
	for( size_t i=0; i<num; i++ )
	{
		children.push_back( CXMLNodeReader( vecpChild[i] ) );
	}
	return children;
}


inline std::vector<CXMLNodeReader> CXMLNodeReader::GetImmediateChildren()
{
	std::vector<CXMLNodeReader> children;

	if( !m_pNode )
		return children;

	xercesc::DOMNodeList *pNodeList = m_pNode->getChildNodes();

	if( !pNodeList )
		return children;

	const size_t num = pNodeList->getLength();
	for( size_t i=0; i<num; i++ )
	{
		children.push_back( CXMLNodeReader( pNodeList->item((XMLSize_t)i) ) );
	}

	return children;
}


inline bool CXMLNodeReader::GetChildElementTextContentLTWH( const std::string& child_element_path, SRect& rect )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		int l=0,t=0,w=0,h=0;
		sscanf( to_string(pNode->getTextContent()).c_str(), "%d %d %d %d", &l, &t, &w, &h );
		rect = RectLTWH( l, t, w, h );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetChildElementTextContentLTRB( const std::string& child_element_path, SRect& rect )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		int l=0,t=0,r=0,b=0;
		sscanf( to_string(pNode->getTextContent()).c_str(), "%d %d %d %d", &l, &t, &r, &b );
		rect = RectLTRB( l, t, r, b );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBColor& color )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &color.fRed, &color.fGreen, &color.fBlue );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& color )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &color.fRed, &color.fGreen, &color.fBlue );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetChildElementTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& color )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f %f", &color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetChildElementTextContent( const std::string& child_element_path, Vector3& v )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &v.x, &v.y, &v.z );
		return true;
	}
	else
		return false;

}

inline bool CXMLNodeReader::GetChildElementTextContent( const std::string& child_element_path, std::string& text )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		text = to_string(pNode->getTextContent());
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetChildElementTextContent( const std::string& child_element_path, float& val )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f", &val );
		return true;
	}
	else
		return false;
}


inline bool CXMLNodeReader::GetChildElementTextContent( const std::string& child_element_path, int& val )
{
	xercesc::DOMNode *pNode = GetTargetElementNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%d", &val );
		return true;
	}
	else
		return false;
}



/*
template<class T>
inline bool CXMLNodeReader::Get( const std::string& name, T& obj )
{
	size_t slash_pos = name.find( "/" );
	if( slash_pos == std::string::npos )
	{
		return get( name, obj );
	}
	else
	{
		if( slash_pos == name.length() - 1 )
			return false;

		CXMLNodeReader loader( GetChildNode( m_pNode, name.substr( 0, slash_pos ) ) );
		return loader.Get( name.substr( slash_pos + 1 ), obj );
	}
}
*/
/*
inline bool CXMLNodeReader::get( const std::string& name, float& f )
{
	if( !m_pNode )
		return false;

	xercesc::DOMNode *pNode = GetChildNode( m_pNode, name );
	if( !pNode )
		return false;

	f = to_float(pNode->getTextContent());

	return true;
}


inline bool CXMLNodeReader::get( const std::string& name, Vector3& v )
{
	if( !m_pNode )
		return false;

	xercesc::DOMNode *pNode = GetChildNode( m_pNode, name );
	if( !pNode )
		return false;

	sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &v.x, &v.y, &v.z );

	return true;
}


inline bool CXMLNodeReader::get( const std::string& name, SFloatRGBColor& color )
{
	if( !m_pNode )
		return false;

	xercesc::DOMNode *pNode = GetChildNode( m_pNode, name );
	if( !pNode )
		return false;

	sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &color.fRed, &color.fGreen, &color.fBlue );

	return true;
}
*/



#endif /* __XMLNodeReader_H__ */
