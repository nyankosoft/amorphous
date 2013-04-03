#include "ResourceLoadingStateHolder.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/GraphicsResourceEntries.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/ShaderHandle.hpp"


namespace amorphous
{

using std::list;
using boost::shared_ptr;


ResourceLoadingState::ResourceLoadingState()
{
//	m_ID = ???
}


bool ResourceLoadingState::IsLoaded()
{
	shared_ptr<GraphicsResourceEntry> pResourceEntry = m_pResourceEntry.lock();
	if( !pResourceEntry )
		return false; // released

	shared_ptr<GraphicsResource> pResource
		= pResourceEntry->GetResource();

	if( !pResource
	 || pResource->GetState() != GraphicsResourceState::LOADED )
		return false;

	return true;
}


bool ResourceLoadingState::IsReleased()
{
	shared_ptr<GraphicsResourceEntry> pResourceEntry = m_pResourceEntry.lock();
	if( !pResourceEntry || pResourceEntry->GetState() == GraphicsResourceEntry::STATE_RELEASED )
		return true; // released
	else
		return false;
}


bool TextureLoadingStateHolder::IsLoaded()
{
	return ResourceLoadingState::IsLoaded();
}


bool MeshLoadingStateHolder::IsLoaded()
{
	shared_ptr<GraphicsResourceEntry> pMeshResourceEntry = m_pResourceEntry.lock();
	if( !pMeshResourceEntry )
		return false; // released

	shared_ptr<MeshResource> pMeshResource
		= pMeshResourceEntry->GetMeshResource();

	if( !pMeshResource
	 || pMeshResource->GetState() != GraphicsResourceState::LOADED )
		return false;

	return true;
/*
	// The mesh has been loaded.
	// - See if its textures have been loaded.

	shared_ptr<CD3DXMeshObjectBase> pMesh
		= pMeshResource->GetMesh();

	if( !pMesh )
		return false;

	const int num_materials = pMesh->GetNumMaterials();
	for( int i=0; i<num_materials; i++ )
	{
		const int num_textures = pMesh->GetNumTextures( i );
		for( int j=0; j<num_textures; j++ )
		{
			TextureHandle tex = pMesh->GetTexture( i, j );

			if( tex.GetEntryState() != GraphicsResourceState::LOADED )
				return false;
		}
	}

	return true;*/
}


bool ShaderLoadingStateHolder::IsLoaded()
{
	return ResourceLoadingState::IsLoaded();
}


//============================================================================
// ResourceLoadingStateHolder
//============================================================================

void ResourceLoadingStateHolder::Add( ResourceLoadingState *pLoadingState )
{
	shared_ptr<ResourceLoadingState> p
		= shared_ptr<ResourceLoadingState>( pLoadingState );

	m_lstpResourceLoadingState.push_back( p );
}


void ResourceLoadingStateHolder::Add( TextureHandle& texture_handle )
{
	TextureLoadingStateHolder *p = new TextureLoadingStateHolder( texture_handle.GetEntry() );
	Add( p );
}


void ResourceLoadingStateHolder::Add( MeshHandle& mesh_handle )
{
	MeshLoadingStateHolder *p = new MeshLoadingStateHolder( mesh_handle.GetEntry() );
	Add( p );
}


void ResourceLoadingStateHolder::Add( ShaderHandle& shader_handle )
{
	ShaderLoadingStateHolder *p = new ShaderLoadingStateHolder( shader_handle.GetEntry() );
	Add( p );
}


void ResourceLoadingStateHolder::AddFromResourceEntry( shared_ptr<GraphicsResourceEntry> pEntry )
{
	Add( new ResourceLoadingState( pEntry ) );
}

/*
/// Remove released resources from the list
bool ResourceLoadingStateHolder::Update()
{
	list< shared_ptr<ResourceLoadingState> >::iterator itr;
		itr = m_lstpResourceLoadingState.begin();

	for( ; itr != m_lstpResourceLoadingState.end(); )
	{
		if( 
	}
}
*/

bool ResourceLoadingStateHolder::AreAllResourceLoaded()
{
//	Update();

	list< shared_ptr<ResourceLoadingState> >::iterator itr;
		itr = m_lstpResourceLoadingState.begin();

	for( ; itr != m_lstpResourceLoadingState.end(); /* Don't increment itr since we do it through erase() call or return from the function. */ )
	{
		if( (*itr)->IsLoaded()
		 || (*itr)->IsReleased() )
		{
			itr = m_lstpResourceLoadingState.erase( itr );
		}
		else
		{
			return false;
		}
	}

	return true;
}


} // namespace amorphous
