#ifndef __XMLNodeReader_H__
#define __XMLNodeReader_H__

#include <string>
#include "3DMath/Vector3.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/FloatRGBAColor.h"

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
	color_node.get( "Upper", hs_point_light.Attribute.UpperColor );
	color_node.get( "Lower", hs_point_light.Attribute.UpperColor );

	CXMLNodeReader light_loader = loader.get_child( "HemisphericPointLight" );
	light_loader.get( "Pos", hs_point_light.vPosition );
	light_loader.Get( "Color/Upper", hs_point_light.Attribute.UpperColor );
	light_loader.Get( "Color/Lower", hs_point_light.Attribute.UpperColor );
*/

class CXMLNodeReader
{
	/// borrowed reference
	xercesc_2_8::DOMNode* m_pNode;

public:

	CXMLNodeReader( xercesc_2_8::DOMNode* pNode ) : m_pNode(pNode) {}

	~CXMLNodeReader() {}

	bool valid() const { return ( m_pNode != NULL ); }

	template<class T>
	inline bool Get( const std::string& name, T& obj );

	void SetIgnoreAlphaComponent( bool ignore );

//	bool get( const std::string& name, int& val );
//	bool get( const std::string& name, short& val );
//	bool get( const std::string& name, long& val );
	bool get( const std::string& name, float& val );
	bool get( const std::string& name, double& val );
	bool get( const std::string& name, std::string& text );
	bool get( const std::string& name, Vector3& v );
	bool get( const std::string& name, SFloatRGBColor& color );
	bool get( const std::string& name, SFloatRGBAColor& color );
//	bool get( const std::string& name, AABB3& aabb );

	CXMLNodeReader get_child( const std::string& name )
	{
		return CXMLNodeReader( GetChildNode( m_pNode, name ) );
	}
};


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



/*


//
//pNode & v;
//
//	if( to_string(pNode->GetName()) == "Pos" )
//		pNode & v;


inline void DOMNode*& operator & ( DOMNode*& pNode, Vector3& v )
{
	sscanf( to_string(pNode->getContext()), "%f %f %f", &v.x, &v.y, &v.z );

	return pNode;
}


inline DOMNode*& operator & ( DOMNode*& pNode, SFloatRGBColor& color )
{
	sscanf( to_string(pNode->getContext()), "%f %f %f", &color.fRed, &color.fGreen, &color.fBlue );

	return pNode;
}

inline DOMNode*& operator & ( DOMNode*& pNode, SFloatRGBAColor& color )
{
	sscanf( to_string(pNode->getContext()), "%f %f %f %f", &color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha );

	return pNode;
}


inline IArchive& operator & ( IArchive& ar, Vector3& v )
{
	ar & v.x & v.y & v.z;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, Quaternion& q )
{
	ar & q.x & q.y & q.z & q.w;
	return ar;
}


inline IArchive& operator & ( IArchive& ar, AABB3& aabb )
{
	ar & aabb.vMin;
	ar & aabb.vMax;

	return ar;
}

inline IArchive& operator & ( IArchive& ar, SFloatRGBColor& color )
{
	ar & color.fRed;
	ar & color.fGreen;
	ar & color.fBlue;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, SFloatRGBAColor& color )
{
	ar & color.fRed;
	ar & color.fGreen;
	ar & color.fBlue;
	ar & color.fAlpha;

	return ar;
}


*/


#endif /* __XMLNodeReader_H__ */
