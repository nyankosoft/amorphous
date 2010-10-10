#include "MeshObjectContainer.hpp"
#include "Support/Serialization/Serialization_BoostSmartPtr.hpp"
#include "XML.hpp"
#include "XML/LoadFromXMLNode_3DMath.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/GenericShaderGenerator.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"

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

	if( version < 1 )
		ar & m_ShaderDesc.ResourcePath;
	else
		ar & m_ShaderDesc;
}


void CMeshObjectContainer::LoadFromXMLNode( CXMLNodeReader& reader )
{
	// A simplified version - load the path from the attribute "path"
	string mesh_path;
	reader.GetAttributeValue( "path", mesh_path );
	if( 0 < mesh_path.length() )
		m_MeshDesc.ResourcePath = mesh_path;

	// A detailed version - load mesh properties from a "MeshDesc" node
	m_MeshDesc.LoadFromXMLNode( reader.GetChild( "MeshDesc" ) );

	reader.GetChildElementTextContent( "ShaderFilepath", m_ShaderDesc.ResourcePath );

	string shader_technique;
	if( reader.GetChildElementTextContent( "SingleShaderTechnique", shader_technique ) )
	{
		m_ShaderTechnique.resize( 1, 1 );
		m_ShaderTechnique(0,0).SetTechniqueName( shader_technique.c_str() );
	}

	// Load path and technique as attributes from a single node

	CXMLNodeReader shader_node = reader.GetChild("Shader");
	if( shader_node.IsValid() )
	{
		string shader_path( shader_node.GetAttributeText("path") );
		if( 0 < shader_path.length() )
			m_ShaderDesc.ResourcePath = shader_path;
		else
		{
			// try to load generic shader attributes
			CGenericShaderDesc desc;
//			LoadFromXMLNode( shader_node, desc );
			m_ShaderDesc.pShaderGenerator.reset( new CGenericShaderGenerator(desc) );
		}
		shader_node.GetAttributeValue("path",     m_ShaderDesc.ResourcePath );

		shader_technique = "";
		shader_node.GetAttributeValue("technique",shader_technique );
		if( 0 < shader_technique.length() )
		{
			m_ShaderTechnique.resize( 1, 1 );
			m_ShaderTechnique(0,0).SetTechniqueName( shader_technique.c_str() );
		}
	}
}



CMeshContainerNode::CMeshContainerNode()
:
m_LocalPose(Matrix34Identity()),
m_LocalTransform(Matrix34Identity()),
m_WorldTransform(Matrix34Identity())
{
}


Matrix34 CMeshContainerNode::GetMeshContainerWorldTransform( int mesh_container_index )
{
/*	Matrix34 transform
		= m_WorldTransform
		* m_vecMeshLocalPose[mesh_container_index]
		* m_LocalTransform
		* m_vecpMeshContainer[mesh_container_index]->m_MeshTransform;
//		* m_vecMeshLocalPose[i].GetInverseROT();*/

	Matrix34 transform
		= m_WorldTransform
		* m_vecMeshLocalPose[mesh_container_index]
//		* m_vecpMeshContainer[mesh_container_index]->m_LocalTransform;
		* m_vecpMeshContainer[mesh_container_index]->m_MeshTransform;

	return transform;
}



/// Render the meshes with the shaders stored in the mesh containers
void CMeshContainerNode::Render( /*const Matrix34& parent_transform*/ )
{
	Matrix34 world_transform = m_WorldTransform;

	for( size_t i=0; i<m_vecpMeshContainer.size(); i++ )
	{
		shared_ptr<CBasicMesh> pMesh
			= m_vecpMeshContainer[i]->m_MeshObjectHandle.GetMesh();

		if( !pMesh )
			continue;

		Matrix34 mesh_world_transform = GetMeshContainerWorldTransform( (int)i );

		CShaderManager *pShaderMgr = NULL;

		// Use the shaders stored in the mesh container
		pShaderMgr = m_vecpMeshContainer[i]->m_ShaderHandle.GetShaderManager();
//		if( pShaderMgr )
//			pMesh->Render( *pShaderMgr );

		pShaderMgr->SetWorldTransform( mesh_world_transform );

		m_vecShaderTechniqueBuffer.resize(0);
		int res = 0;
		if( 0 < m_vecpMeshContainer[i]->m_ShaderTechnique.size_y() )
		{
			for( int j=0; j<m_vecpMeshContainer[i]->m_ShaderTechnique.size_x(); j++ )
				m_vecShaderTechniqueBuffer.push_back( m_vecpMeshContainer[i]->m_ShaderTechnique(j,res) );
		}
		pMesh->Render( *pShaderMgr, m_vecShaderTechniqueBuffer );
	}

	for( size_t i=0; i<m_vecpChild.size(); i++ )
	{
		m_vecpChild[i]->Render( /*world_transform*/ );
	}
}

/*
void CMeshContainerNode::Render( const Matrix34& parent_transform, CMeshContainerNodeRenderMethod& render_method );
{
	Render_r( parent_transform, render_method, false );
}


void CMeshContainerNode::Render( const Matrix34& parent_transform )
{
	CMeshContainerNodeRenderMethod null_render_method;
	Render_r( parent_transform, null_render_method, true );
}
*/

void CMeshContainerNode::UpdateWorldTransforms( const Matrix34& parent_transform  )
{
	Matrix34 world_transform
		= parent_transform
		* m_LocalTransform
		* m_LocalPose;

	m_WorldTransform = world_transform;

	for( size_t i=0; i<m_vecpChild.size(); i++ )
		m_vecpChild[i]->UpdateWorldTransforms( world_transform );
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


bool CMeshContainerNode::LoadShadersFromDesc()
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
		loaded = m_vecpMeshContainer[i]->m_ShaderHandle.Load( m_vecpMeshContainer[i]->m_ShaderDesc );
		if( !loaded )
			loaded_so_far = false;
	}

	// recursively load the meshses of the child nodes
	for( size_t i=0; i<m_vecpChild.size(); i++ )
	{
		loaded = m_vecpChild[i]->LoadShadersFromDesc();
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
		::LoadFromXMLNode( vecReader[i].GetChild( "MeshLocalPose" ), m_vecMeshLocalPose[i] );
	}

	vector<CXMLNodeReader> vecChild = reader.GetImmediateChildren( "MeshNode" );
	m_vecpChild.resize( vecChild.size() );
	for( size_t i=0; i<vecChild.size(); i++ )
	{
		m_vecpChild[i] = shared_ptr<CMeshContainerNode>( new CMeshContainerNode() ); 
		m_vecpChild[i]->LoadFromXMLNode( vecChild[i] );
	}
}
