#include "GraphicsResourceDescs.hpp"
#include "XML.hpp"


void CGraphicsResourceDesc::LoadFromXMLNode( CXMLNodeReader& reader )
{
	reader.GetChildElementTextContent( "Path", ResourcePath );
	reader.GetChildElementTextContent( "File", ResourcePath );
}


void CTextureResourceDesc::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGraphicsResourceDesc::LoadFromXMLNode( reader );
}


void CMeshResourceDesc::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGraphicsResourceDesc::LoadFromXMLNode( reader );

	string mesh_type = reader.GetAttributeText( "type" );
	if( 0 < mesh_type.length() )
	{
		if( mesh_type == "basic" )            this->MeshType = CMeshType::BASIC;
		else if( mesh_type == "progressive" ) this->MeshType = CMeshType::PROGRESSIVE;
		else if( mesh_type == "skeletal" )    this->MeshType = CMeshType::SKELETAL;
	}
}
