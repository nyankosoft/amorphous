#include "GraphicsResourceDescs.hpp"
#include "TextureGenerators/TextureFillingAlgorithm.hpp"
#include "TextureGenerators/TextureGeneratorFactory.hpp"
#include "Shader/ShaderGenerator.hpp"
#include "amorphous/XML.hpp"


namespace amorphous
{

using namespace std;


//================================================================================
// GraphicsResourceDesc
//================================================================================

void GraphicsResourceDesc::LoadFromXMLNode( XMLNode& reader )
{
	reader.GetChildElementTextContent( "Path", ResourcePath );
	reader.GetChildElementTextContent( "File", ResourcePath );
}



//================================================================================
// TextureResourceDesc
//================================================================================

bool TextureResourceDesc::IsValid() const
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


bool TextureResourceDesc::CanBeSharedAsSameTextureResource( const TextureResourceDesc& desc ) const
{
	if( 0 < this->ResourcePath.length()
	 && 0 < desc.ResourcePath.length()
	 && this->ResourcePath == desc.ResourcePath )
		return true;
	else
		return false;
}


void TextureResourceDesc::Serialize( IArchive& ar, const unsigned int version )
{
	GraphicsResourceDesc::Serialize( ar, version );

	ar & Width & Height & MipLevels;
	ar & (int&)Format;
	ar & UsageFlags;
	ar & TypeFlags;

	TextureGeneratorFactory factory;
	ar.Polymorphic( pLoader, factory );
}


void TextureResourceDesc::LoadFromXMLNode( XMLNode& reader )
{
	GraphicsResourceDesc::LoadFromXMLNode( reader );
}



//================================================================================
// MeshResourceDesc
//================================================================================

bool MeshResourceDesc::IsValid() const
{
	return ( 0 < ResourcePath.length() ) || pMeshGenerator;
}


int MeshResourceDesc::CanBeUsedAsMeshCache( const MeshResourceDesc& desc ) const
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

	if( desc.VertexFormatFlags != this->VertexFormatFlags )
	{
		// The vertex elements are not identical.
		return 0;
	}

/*	if( desc.vecVertElement.size() != this->vecVertElement.size() )
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
	}*/

	int score
		= (desc.NumVertices - this->NumVertices + 1)
		* (desc.NumIndices  - this->NumIndices  + 1);

	return score;
}


void MeshResourceDesc::LoadFromXMLNode( XMLNode& reader )
{
	GraphicsResourceDesc::LoadFromXMLNode( reader );

	string mesh_type = reader.GetAttributeText( "type" );
	if( 0 < mesh_type.length() )
	{
		if( mesh_type == "basic" )            this->MeshType = MeshType::BASIC;
		else if( mesh_type == "progressive" ) this->MeshType = MeshType::PROGRESSIVE;
		else if( mesh_type == "skeletal" )    this->MeshType = MeshType::SKELETAL;
	}
}


bool ShaderResourceDesc::IsValid() const
{
	if( ShaderType == ShaderType::NON_PROGRAMMABLE )
		return true;
	else
	{
		// Programmable shader need to have a valid filepath.
		if( 0 < ResourcePath.length() )
		{
			return true;
		}
		else if( pShaderGenerator )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


bool ShaderResourceDesc::CanBeSharedAsSameShaderResource( const ShaderResourceDesc& desc ) const
{
	if( 0 < this->ResourcePath.length()
	 && 0 < desc.ResourcePath.length()
	 && this->ResourcePath == desc.ResourcePath )
	{
		return true;
	}

	if( this->pShaderGenerator
	 && desc.pShaderGenerator
	 && this->pShaderGenerator->IsSharableWith( *desc.pShaderGenerator ) )
	{
		return true;
	}

	return false;
}


} // namespace amorphous
