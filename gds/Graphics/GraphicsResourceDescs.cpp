#include "GraphicsResourceDescs.hpp"
#include "XML.hpp"

using namespace std;


//================================================================================
// CGraphicsResourceDesc
//================================================================================

void CGraphicsResourceDesc::LoadFromXMLNode( CXMLNodeReader& reader )
{
	reader.GetChildElementTextContent( "Path", ResourcePath );
	reader.GetChildElementTextContent( "File", ResourcePath );
}



//================================================================================
// CTextureResourceDesc
//================================================================================

bool CTextureResourceDesc::IsValid() const
{
	if( 0 < this->ResourcePath.length() )
	{
		return true;
	}
	else
	{
		if( 0 < this->Width
		 && 0 < this->Height
		 && this->Format != TextureFormat::Invalid )
		{
			return true;
		}
		else
			return false;
	}
}


bool CTextureResourceDesc::CanBeSharedAsSameTextureResource( const CTextureResourceDesc& desc ) const
{
	if( 0 < this->ResourcePath.length()
	 && 0 < desc.ResourcePath.length()
	 && this->ResourcePath == desc.ResourcePath )
		return true;
	else
		return false;
}


void CTextureResourceDesc::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGraphicsResourceDesc::LoadFromXMLNode( reader );
}



//================================================================================
// CMeshResourceDesc
//================================================================================

bool CMeshResourceDesc::IsValid() const
{
	return ( 0 < ResourcePath.length() ) || pMeshGenerator;
}


int CMeshResourceDesc::CanBeUsedAsMeshCache( const CMeshResourceDesc& desc ) const
{
	if( MeshType        != desc.MeshType
	 || LoadOptionFlags != desc.LoadOptionFlags )
		return 0;

/*	if( this->NumVertices <= desc.NumVertices
	 && this->NumIndices  <= desc.NumIndices )
		return 1;
	else
		return 0;
*/

	if( desc.NumVertices < this->NumVertices
	 || desc.NumIndices  < this->NumIndices )
	{
		// not enough vertices / indices
		return 0;
	}

	// use as cache only if all the vertex elements exactly match

	if( desc.vecVertElement.size() != this->vecVertElement.size() )
		return 0; // different vertex elements

	for( size_t i=0; i<this->vecVertElement.size(); i++ )
	{
		if( desc.vecVertElement[i].Stream     != this->vecVertElement[i].Stream    
		 || desc.vecVertElement[i].Offset     != this->vecVertElement[i].Offset    
		 || desc.vecVertElement[i].Type       != this->vecVertElement[i].Type      
		 || desc.vecVertElement[i].Method     != this->vecVertElement[i].Method    
		 || desc.vecVertElement[i].Usage      != this->vecVertElement[i].Usage     
		 || desc.vecVertElement[i].UsageIndex != this->vecVertElement[i].UsageIndex
		 )
			return 0;
	}

	int score
		= (desc.NumVertices - this->NumVertices + 1)
		* (desc.NumIndices  - this->NumIndices  + 1);

	return score;
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


bool CShaderResourceDesc::IsValid() const
{
	if( ShaderType == CShaderType::NON_PROGRAMMABLE )
		return true;
	else
	{
		// Programmable shader need to have a valid filepath.
		if( 0 < ResourcePath.length() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
