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
#include <xercesc/dom/DOMWriter.hpp>
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

class CXMLNodeReader
{
	/// borrowed reference
	xercesc_2_8::DOMNode *m_pNode;

public:

	CXMLNodeReader( xercesc_2_8::DOMNode* pNode ) : m_pNode(pNode) {}

	~CXMLNodeReader() {}

	xercesc_2_8::DOMNode *GetDOMNode() { return m_pNode; }

	bool IsValid() const { return ( m_pNode != NULL ); }

	inline xercesc_2_8::DOMNode *GetTargetNode( const std::string& name );

	/// get the text content of a child node
	inline bool GetTextContent( const std::string& child_element_path, std::string& dest );
	inline bool GetTextContent( const std::string& child_element_path, float& dest );
	inline bool GetTextContent( const std::string& child_element_path, int& dest );
	inline bool GetTextContentLTWH( const std::string& child_element_path, SRect& dest );
	inline bool GetTextContentLTRB( const std::string& child_element_path, SRect& dest );
	inline bool GetTextContentRGB( const std::string& child_element_path, SFloatRGBColor& dest );
	inline bool GetTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& dest );
	inline bool GetTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& dest );
	inline bool GetTextContent( const std::string& child_element_path, Vector3& dest );

	/// get the text content of the current node
	inline bool GetTextContent( std::string& dest )         { return GetTextContent( "", dest ); }
	inline bool GetTextContent( float& dest )               { return GetTextContent( "", dest ); }
	inline bool GetTextContent( int& dest )                 { return GetTextContent( "", dest ); }
	inline bool GetTextContentLTWH( SRect& dest )           { return GetTextContentLTWH( "", dest ); }
	inline bool GetTextContentLTRB( SRect& dest )           { return GetTextContentLTRB( "", dest ); }
	inline bool GetTextContentRGB( SFloatRGBColor& dest )   { return GetTextContentRGB( "", dest ); }
	inline bool GetTextContentRGB( SFloatRGBAColor& dest )  { return GetTextContentRGB( "", dest ); }
	inline bool GetTextContentRGBA( SFloatRGBAColor& dest ) { return GetTextContentRGBA( "", dest ); }
	inline bool GetTextContent( Vector3& dest )             { return GetTextContent( "", dest ); }

	std::string GetName()
	{
		if( m_pNode )
			return to_string(m_pNode->getNodeName());
		else
			return std::string();
	}

	CXMLNodeReader GetChild( const std::string& name )
	{
		return CXMLNodeReader( GetChildNode( m_pNode, name ) );
	}

	std::string GetAttributeText( const std::string& attrib_name )
	{
		if( m_pNode )
			return ::GetAttributeText( m_pNode, attrib_name );
		else
			return std::string();
	}

	std::vector<CXMLNodeReader> GetImmediateChildren( const std::string& name )
	{
		std::vector<xercesc_2_8::DOMNode *> vecpChild = GetImmediateChildNodes( m_pNode, name );
		std::vector<CXMLNodeReader> children;
		const size_t num = vecpChild.size();
		for( size_t i=0; i<num; i++ )
		{
			children.push_back( CXMLNodeReader( vecpChild[i] ) );
		}
		return children;
	}

	std::vector<CXMLNodeReader> GetImmediateChildren()
	{
		std::vector<CXMLNodeReader> children;

		if( !m_pNode )
			return children;

		xercesc_2_8::DOMNodeList *pNodeList = m_pNode->getChildNodes();

		if( !pNodeList )
			return children;

		const size_t num = pNodeList->getLength();
		for( size_t i=0; i<num; i++ )
		{
			children.push_back( CXMLNodeReader( pNodeList->item((XMLSize_t)i) ) );
		}

		return children;
	}
};

//===================================== inline implementations ======================================


inline xercesc_2_8::DOMNode *CXMLNodeReader::GetTargetNode( const std::string& child_element_path )
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
		return loader.GetTargetNode( child_element_path.substr( slash_pos + 1 ) );
	}
}

inline bool CXMLNodeReader::GetTextContentLTWH( const std::string& child_element_path, SRect& rect )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
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

inline bool CXMLNodeReader::GetTextContentLTRB( const std::string& child_element_path, SRect& rect )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
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

inline bool CXMLNodeReader::GetTextContentRGB( const std::string& child_element_path, SFloatRGBColor& color )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &color.fRed, &color.fGreen, &color.fBlue );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& color )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &color.fRed, &color.fGreen, &color.fBlue );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& color )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f %f", &color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha );
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetTextContent( const std::string& child_element_path, Vector3& v )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &v.x, &v.y, &v.z );
		return true;
	}
	else
		return false;

}

inline bool CXMLNodeReader::GetTextContent( const std::string& child_element_path, std::string& text )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
	if( pNode )
	{
		text = to_string(pNode->getTextContent());
		return true;
	}
	else
		return false;
}

inline bool CXMLNodeReader::GetTextContent( const std::string& child_element_path, float& val )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
	if( pNode )
	{
		sscanf( to_string(pNode->getTextContent()).c_str(), "%f", &val );
		return true;
	}
	else
		return false;
}


inline bool CXMLNodeReader::GetTextContent( const std::string& child_element_path, int& val )
{
	xercesc_2_8::DOMNode *pNode = GetTargetNode( child_element_path );
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

	xercesc_2_8::DOMNode *pNode = GetChildNode( m_pNode, name );
	if( !pNode )
		return false;

	f = to_float(pNode->getTextContent());

	return true;
}


inline bool CXMLNodeReader::get( const std::string& name, Vector3& v )
{
	if( !m_pNode )
		return false;

	xercesc_2_8::DOMNode *pNode = GetChildNode( m_pNode, name );
	if( !pNode )
		return false;

	sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &v.x, &v.y, &v.z );

	return true;
}


inline bool CXMLNodeReader::get( const std::string& name, SFloatRGBColor& color )
{
	if( !m_pNode )
		return false;

	xercesc_2_8::DOMNode *pNode = GetChildNode( m_pNode, name );
	if( !pNode )
		return false;

	sscanf( to_string(pNode->getTextContent()).c_str(), "%f %f %f", &color.fRed, &color.fGreen, &color.fBlue );

	return true;
}
*/



#endif /* __XMLNodeReader_H__ */
