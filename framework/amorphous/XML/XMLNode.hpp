#ifndef __XMLNode_HPP__
#define __XMLNode_HPP__


#include <string>
#include <vector>
#include <memory>
#include "amorphous/Support/StringAux.hpp"
#include "amorphous/3DMath/Vector3.hpp"
#include "amorphous/Graphics/FloatRGBColor.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Graphics/Rect.hpp"
#include "fwd.hpp"


namespace amorphous
{


//
// Inline Global Functions (used by CXMLNodeReader::GetAttributeValue())
//
inline void conv_to_x( const std::string& src, int& dest )
{
	dest = atoi( src.c_str() );
}

inline void conv_to_x( const std::string& src, uint& dest )
{
	dest = (uint)atoi( src.c_str() );
}

inline void conv_to_x( const std::string& src, short& dest )
{
	dest = (short)atoi( src.c_str() );
}

inline void conv_to_x( const std::string& src, float& dest )
{
	dest = (float)atof( src.c_str() );
}

inline void conv_to_x( const std::string& src, double& dest )
{
	dest = atof( src.c_str() );
}

inline void conv_to_x( const std::string& src, std::string& dest )
{
	dest = src;
}

inline void conv_to_x( const std::string& src, bool& dest )
{
	if( src == "true" )
		dest = true;
	else if( src == "false" )
		dest = false;
}



class XMLNodeImpl
{
public:

	XMLNodeImpl(){}

	virtual ~XMLNodeImpl(){}

	virtual bool IsValid() const { return false; }

	virtual bool GetChildElementTextContent( const std::string& child_element_path, std::string& dest )         const { return false; }
	virtual bool GetChildElementTextContentLTWH( const std::string& child_element_path, SRect& dest )           const { return false; }
	virtual bool GetChildElementTextContentLTRB( const std::string& child_element_path, SRect& dest )           const { return false; }
	virtual bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBColor& dest )   const { return false; }
	virtual bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& dest )  const { return false; }
	virtual bool GetChildElementTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& dest ) const { return false; }
	virtual bool GetChildElementTextContent( const std::string& child_element_path, Vector3& dest )             const { return false; }

	virtual std::string GetName() { return ""; }

	virtual XMLNode GetChild( const std::string& name ) = 0;//{ return XMLNode(); }

	/**
	\return true if the attribute with the name 'attrib_name' was found, false otherwise
	*/
	virtual bool GetAttributeText( const std::string& attrib_name, std::string& dest ) { return false; }

	virtual std::string GetAttributeText( const std::string& attrib_name ) { return ""; }

//	template<typename T>
//	inline void GetAttributeValue( const std::string& attrib_name, T& dest );

	virtual std::vector<XMLNode> GetImmediateChildren( const std::string& name ) = 0;//{ return std::vector<XMLNode>(); }

	virtual std::vector<XMLNode> GetImmediateChildren() = 0;//{ return std::vector<XMLNode>(); }
};


class XMLNode
{
	std::shared_ptr<XMLNodeImpl> m_pImpl;

public:

	XMLNode(){}

	XMLNode( std::shared_ptr<XMLNodeImpl> pImpl ) : m_pImpl(pImpl) {}

	~XMLNode(){}

	bool IsValid() const { return m_pImpl->IsValid(); }

	/// get the text content of a child node
	template<typename T>
	inline bool GetChildElementTextContent( const std::string& child_element_path, T& dest ) const;

	bool GetChildElementTextContent( const std::string& child_element_path, std::string& dest )         const { return m_pImpl->GetChildElementTextContent(child_element_path,dest); }
	bool GetChildElementTextContentLTWH( const std::string& child_element_path, SRect& dest )           const { return m_pImpl->GetChildElementTextContentLTWH(child_element_path,dest); }
	bool GetChildElementTextContentLTRB( const std::string& child_element_path, SRect& dest )           const { return m_pImpl->GetChildElementTextContentLTRB(child_element_path,dest); }
	bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBColor& dest )   const { return m_pImpl->GetChildElementTextContentRGB(child_element_path,dest); }
	bool GetChildElementTextContentRGB( const std::string& child_element_path, SFloatRGBAColor& dest )  const { return m_pImpl->GetChildElementTextContentRGB(child_element_path,dest); }
	bool GetChildElementTextContentRGBA( const std::string& child_element_path, SFloatRGBAColor& dest ) const { return m_pImpl->GetChildElementTextContentRGBA(child_element_path,dest); }
	bool GetChildElementTextContent( const std::string& child_element_path, Vector3& dest )             const { return m_pImpl->GetChildElementTextContent(child_element_path,dest); }

	/// get the text content of the current node
	inline std::string GetTextContent() const { std::string dest; GetTextContent(dest); return dest; }

	template<typename T>
	inline bool GetTextContent( T& dest )                   const { return GetChildElementTextContent( "", dest ); }
	inline bool GetTextContentLTWH( SRect& dest )           const { return GetChildElementTextContentLTWH( "", dest ); }
	inline bool GetTextContentLTRB( SRect& dest )           const { return GetChildElementTextContentLTRB( "", dest ); }
	inline bool GetTextContentRGB( SFloatRGBColor& dest )   const { return GetChildElementTextContentRGB( "", dest ); }
	inline bool GetTextContentRGB( SFloatRGBAColor& dest )  const { return GetChildElementTextContentRGB( "", dest ); }
	inline bool GetTextContentRGBA( SFloatRGBAColor& dest ) const { return GetChildElementTextContentRGBA( "", dest ); }
	inline bool GetTextContent( Vector3& dest )             const { return GetChildElementTextContent( "", dest ); }

	std::string GetName() const { return m_pImpl->GetName(); }

	XMLNode GetChild( const std::string& name ) const { return m_pImpl->GetChild(name); }

	std::string GetAttributeText( const std::string& attrib_name ) const { return m_pImpl->GetAttributeText(attrib_name); }

	template<typename T>
	inline void GetAttributeValue( const std::string& attrib_name, T& dest ) const;

	std::vector<XMLNode> GetImmediateChildren( const std::string& name ) const { return m_pImpl->GetImmediateChildren(name); }

	std::vector<XMLNode> GetImmediateChildren() const { return m_pImpl->GetImmediateChildren(); }
};


template<typename T>
inline bool XMLNode::GetChildElementTextContent( const std::string& child_element_path, T& dest ) const
{
	std::string text;
	bool res = GetChildElementTextContent( child_element_path, text );
	if( res )
	{
		conv_to_x( text, dest );
		return true;
	}
	else
		return false;
}


/// Udpates the values of dest if the attribute with the specified name is found in the node.
/// If the attribute is not found, the value of dest is not changed.
template<typename T>
inline void XMLNode::GetAttributeValue( const std::string& attrib_name, T& dest ) const
{
	std::string text;
	bool attribute_found = m_pImpl->GetAttributeText( attrib_name, text );
	if( !attribute_found )
		return;

	if( 0 < text.length() )
		conv_to_x( text, dest );
}


} // namespace amorphous


#endif /* __XMLNode_HPP__ */
