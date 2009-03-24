#include "MeshObjectContainer.hpp"
#include "Support/Serialization/Serialization_BoostSmartPtr.hpp"
#include "XML.hpp"

using namespace std;
using namespace boost;



/*
XML sample for MeshContainerNode CGameItem::m_MeshContainerRootNode

<Model>
	<MeshNode>
		<LocalPose>
			<Position>0 0 0</Position>
		</LocalPose>
		<!-- Mesh node contains one or more -->
		<MeshUnit>
			<MeshContainer>
				
			</MeshContainer>
			<LocalPose>
				<Heading>60</Heading>
				<Pitch>10</Pitch>
				<Position>0 0 0</Position>
			</LocalPose>
		</MeshUnit>
		<MeshUnit>
			<MeshContainer>
				
			</MeshContainer>
			<LocalPose>
				<Heading>60</Heading>
				<Pitch>60</Pitch>
				<Position>0 0 0</Position>
			</LocalPose>
		</MeshUnit>

		<!-- Child mesh noes -->
		<MeshNode>

		</MeshNode>
	</MeshNode>
</Model>


*/



/*
--------------------------------------------------
 XML format for shader technique
--------------------------------------------------

no LOD, single technique

<SingleShaderTechnique>HSLighting</SingleShaderTechnique>


no LOD, multiple materials (Not implemented yet)

<ShaderTechniques>
	<Material index="0">EnvMap</Material>
	<Material index="1">HSLighting</Material>
	<Material index="2">HSLighting</Material>
</ShaderTechniques>


LOD, multiple materials (Not implemented yet)

<ShaderTechniques>
	<Material index="0">
		<High>EnvMap</High>
		<Mid>EnvMap</Mid>
		<Low>Default</Low>
	</Material>
	<Material index="1">
		<High>HSLighting</High>
		<Mid>HSLighting</Mid>
		<Low>Default</Low>
	</Material>
	<Material index="2">
		<High>HSLighting</High>
		<Mid>HSLighting</Mid>
		<Low>Default</Low>
	</Material>
</ShaderTechniques>

*/




//  old text desc file (deprecated. Use the XML format instead)
/*

How to load from text file?

text file
MeshContainer.MeshObjectFilename	plane.msh

begin: MeshContainer
	filename	models/plane.msh
	extra_tex	0 textures/specular.msh
	shader_tech 0 VertexBlend_1HSDL_Specular_CTS  SingleHSDL_Specular_CTS     VertLights
	shader_tech 1 SpecularMembrane_1HSDL_CTS      SpecularMembrane_1HSDL_CTS  VertLights
end: MeshContainer
*/



void CMeshObjectContainer::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_MeshDesc;
	ar & m_ShaderTechnique;
	ar & m_vecExtraTextureFilepath;
	ar & m_ShaderFilepath;
}


void CMeshObjectContainer::LoadFromXMLNode( CXMLNodeReader& reader )
{
	m_MeshDesc.LoadFromXMLNode( reader.GetChild( "MeshDesc" ) );

	reader.GetChildElementTextContent( "ShaderFilepath", m_ShaderFilepath );

	string shader_technique;
	if( reader.GetChildElementTextContent( "SingleShaderTechnique", shader_technique ) )
	{
		m_ShaderTechnique.resize( 1, 1 );
		m_ShaderTechnique(0,0).SetTechniqueName( shader_technique.c_str() );
	}
}


inline void LoadFromXMLNode( CXMLNodeReader& reader, Vector3& dest )
{
	std::string pos_str;
	reader.GetChildElementTextContent( "Position", pos_str );
	sscanf( pos_str.c_str(), "%f %f %f", &dest.x, &dest.y, &dest.z );
}


inline void LoadFromXMLNode( CXMLNodeReader& reader, Matrix34& dest )
{
	LoadFromXMLNode( reader, dest.vPosition );
//	LoadFromXMLNode( reader, dest.matOrient );
}


bool CMeshContainerNode::LoadMeshesFromDesc()
{
	if( m_vecpMeshContainer.size() == 0
	 && m_vecpChild.size() == 0 )
	{
		// nothing to load
		return false;
	}

	bool loaded = false;
	bool loaded_so_far = true;
	for( size_t i=0; i<m_vecpMeshContainer.size(); i++ )
	{
		loaded = m_vecpMeshContainer[i]->m_MeshObjectHandle.Load( m_vecpMeshContainer[i]->m_MeshDesc );
		if( !loaded )
			loaded_so_far = false;
	}

	// recursively load the meshses of the child nodes
	for( size_t i=0; i<m_vecpChild.size(); i++ )
	{
		loaded = m_vecpChild[i]->LoadMeshesFromDesc();
		if( !loaded )
			loaded_so_far = false;
	}

	return loaded_so_far;
}


void CMeshContainerNode::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecpMeshContainer;
	ar & m_vecpChild;
}



/// Use "MeshNode" as the element name of the argument 'reader'
void CMeshContainerNode::LoadFromXMLNode( CXMLNodeReader& reader )
{
	// local pose of the node
	::LoadFromXMLNode( reader.GetChild( "LocalPose" ), m_LocalPose );

	vector<CXMLNodeReader> vecReader = reader.GetImmediateChildren( "MeshUnit" );
	m_vecpMeshContainer.resize( vecReader.size() );
	m_vecMeshLocalPose.resize( vecReader.size(), Matrix34Identity() );
	for( size_t i=0; i<vecReader.size(); i++ )
	{
		m_vecpMeshContainer[i] = shared_ptr<CMeshObjectContainer>( new CMeshObjectContainer() );
		m_vecpMeshContainer[i]->LoadFromXMLNode( vecReader[i].GetChild( "MeshContainer" ) );
		::LoadFromXMLNode( vecReader[i].GetChild( "LocalPose" ), m_vecMeshLocalPose[i] );
	}

	vector<CXMLNodeReader> vecChild = reader.GetImmediateChildren( "MeshNode" );
	m_vecpChild.resize( vecChild.size() );
	for( size_t i=0; i<vecChild.size(); i++ )
	{
		m_vecpChild[i] = shared_ptr<CMeshContainerNode>( new CMeshContainerNode() ); 
		m_vecpChild[i]->LoadFromXMLNode( vecChild[i] );
	}
}
