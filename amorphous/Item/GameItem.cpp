#include "GameItem.hpp"
#include "XML/XMLNodeReader.hpp"
#include "Support/memory_helpers.hpp"
#include "Stage/MeshBonesUpdateCallback.hpp"


namespace amorphous
{

using namespace boost;


void ItemDesc::LoadFromXMLNode( CXMLNodeReader& reader )
{
	int lang_index = 0;
	CXMLNodeReader text_node_reader = reader.GetChild( "Text" );
	text_node_reader.GetChildElementTextContent( "Japanese", text[Lang::Japanese] );
	text_node_reader.GetChildElementTextContent( "English",  text[Lang::English] );
}


//=======================================================================
// GameItem
//=======================================================================

GameItem::GameItem()
:
m_iCurrentQuantity(0),
m_iMaxQuantity(1),
m_Price(1),
m_TypeFlag(0)
{
	// create a mesh container in the root node
	// - used as a default mesh container
	shared_ptr<MeshObjectContainer> pMeshContainer( new MeshObjectContainer() );

	m_MeshContainerRootNode.AddMeshContainer( pMeshContainer, Matrix34Identity() );
}


GameItem::~GameItem()
{
}


void GameItem::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_strName;
	ar & m_MeshContainerRootNode;
	ar & m_iMaxQuantity;
	ar & m_iCurrentQuantity;
	ar & m_Price;
	ar & m_TypeFlag;

	ar & m_Desc;
}


bool GameItem::LoadMeshObject()
{
//	return m_MeshObjectContainer.m_MeshObjectHandle.Load( m_MeshObjectContainer.m_MeshDesc );

	bool meshes_loaded = m_MeshContainerRootNode.LoadMeshesFromDesc();
	bool shaders_loaded = m_MeshContainerRootNode.LoadShadersFromDesc();

	return ( meshes_loaded && shaders_loaded );
}


void GameItem::Render()
{
	boost::shared_ptr<ItemEntity> pItemEntity = GetItemEntity().Get();

	m_MeshContainerRootNode.UpdateWorldTransforms( pItemEntity ? pItemEntity->GetWorldPose() : Matrix34Identity() );
	m_MeshContainerRootNode.Render();
}


void GameItem::GetStatusForDebugging( std::string& dest_buffer ) const
{
	dest_buffer += fmt_string( "name: %s\n", GetName().c_str() );
}


/**
- model node (simple version)
<Model path="box.msh"/>
- model node (detailed version)
(See Graphics/MeshObjectContainer.cpp)
*/
void GameItem::LoadFromXMLNode( CXMLNodeReader& reader )
{
	reader.GetChildElementTextContent( "Name",        m_strName );
	reader.GetChildElementTextContent( "Price",       m_Price );
	reader.GetChildElementTextContent( "MaxQuantity", m_iMaxQuantity );

//	m_MeshContainerRootNode.LoadFromXMLNode( reader.GetChild( "Model/MeshNode" ) );
	CXMLNodeReader model_node = reader.GetChild( "Model" );
	if( model_node.IsValid() )
	{
		CXMLNodeReader meshnode_node = model_node.GetChild( "MeshNode" );
		if( meshnode_node.IsValid() )
			m_MeshContainerRootNode.LoadFromXMLNode( meshnode_node ); // detailed version
		else
		{
			// simplified version: a single mesh file without local offset
			m_MeshContainerRootNode.ClearMeshContainers();
			boost::shared_ptr<MeshObjectContainer> pContainer( new MeshObjectContainer );
			m_MeshContainerRootNode.AddMeshContainer(pContainer);
			m_MeshContainerRootNode.MeshContainer(0)->LoadFromXMLNode( model_node );
		}
	}

	m_Desc.LoadFromXMLNode( reader.GetChild( "Desc" ) );
}


void GameItem::SetGraphicsUpdateCallbackForSkeletalMesh()
{
	if( MeshContainerRootNode().GetNumMeshContainers() == 0 )
		return; // has no mesh

	shared_ptr<MeshObjectContainer> pMeshContainer = MeshContainerRootNode().MeshContainer( 0 );
	if( !pMeshContainer )
		return;

	shared_ptr<BasicMesh> pBasicMesh = pMeshContainer->m_MeshObjectHandle.GetMesh();
	if( !pBasicMesh )
		return;

	if( pBasicMesh->GetMeshType() != MeshType::SKELETAL )
		return;

	boost::shared_ptr<ItemEntity> pItemEntity = GetItemEntity().Get();
	if( !pItemEntity )
		return;

	pItemEntity->m_pGraphicsUpdate.reset( new CMeshBonesUpdateCallback );
}


} // namespace amorphous
