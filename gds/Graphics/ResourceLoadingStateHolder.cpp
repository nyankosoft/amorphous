#include "ResourceLoadingStateHolder.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/GraphicsResourceEntries.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/ShaderHandle.hpp"

using std::list;
using boost::shared_ptr;


CResourceLoadingState::CResourceLoadingState()
{
//	m_ID = ???
}


bool CResourceLoadingState::IsLoaded()
{
	shared_ptr<CGraphicsResourceEntry> pResourceEntry = m_pResourceEntry.lock();
	if( !pResourceEntry )
		return false; // released

	shared_ptr<CGraphicsResource> pResource
		= pResourceEntry->GetResource();

	if( !pResource
	 || pResource->GetState() != GraphicsResourceState::LOADED )
		return false;

	return true;
}


bool CResourceLoadingState::IsReleased()
{
	shared_ptr<CGraphicsResourceEntry> pResourceEntry = m_pResourceEntry.lock();
	if( !pResourceEntry || pResourceEntry->GetState() == CGraphicsResourceEntry::STATE_RELEASED )
		return true; // released
	else
		return false;
}


bool CTextureLoadingStateHolder::IsLoaded()
{
	return CResourceLoadingState::IsLoaded();
}


bool CMeshLoadingStateHolder::IsLoaded()
{
	shared_ptr<CGraphicsResourceEntry> pMeshResourceEntry = m_pResourceEntry.lock();
	if( !pMeshResourceEntry )
		return false; // released

	shared_ptr<CMeshResource> pMeshResource
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
			CTextureHandle tex = pMesh->GetTexture( i, j );

			if( tex.GetEntryState() != GraphicsResourceState::LOADED )
				return false;
		}
	}

	return true;*/
}


bool CShaderLoadingStateHolder::IsLoaded()
{
	return CResourceLoadingState::IsLoaded();
}


//============================================================================
// CResourceLoadingStateHolder
//============================================================================

void CResourceLoadingStateHolder::Add( CResourceLoadingState *pLoadingState )
{
	shared_ptr<CResourceLoadingState> p
		= shared_ptr<CResourceLoadingState>( pLoadingState );

	m_lstpResourceLoadingState.push_back( p );
}


void CResourceLoadingStateHolder::Add( CTextureHandle& texture_handle )
{
	CTextureLoadingStateHolder *p = new CTextureLoadingStateHolder( texture_handle.GetEntry() );
	Add( p );
}


void CResourceLoadingStateHolder::Add( CMeshObjectHandle& mesh_handle )
{
	CMeshLoadingStateHolder *p = new CMeshLoadingStateHolder( mesh_handle.GetEntry() );
	Add( p );
}


void CResourceLoadingStateHolder::Add( CShaderHandle& shader_handle )
{
	CShaderLoadingStateHolder *p = new CShaderLoadingStateHolder( shader_handle.GetEntry() );
	Add( p );
}


void CResourceLoadingStateHolder::AddFromResourceEntry( shared_ptr<CGraphicsResourceEntry> pEntry )
{
	Add( new CResourceLoadingState( pEntry ) );
}

/*
/// Remove released resources from the list
bool CResourceLoadingStateHolder::Update()
{
	list< shared_ptr<CResourceLoadingState> >::iterator itr;
		itr = m_lstpResourceLoadingState.begin();

	for( ; itr != m_lstpResourceLoadingState.end(); )
	{
		if( 
	}
}
*/

bool CResourceLoadingStateHolder::AreAllResourceLoaded()
{
//	Update();

	list< shared_ptr<CResourceLoadingState> >::iterator itr;
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
