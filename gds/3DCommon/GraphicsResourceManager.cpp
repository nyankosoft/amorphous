#include "3DCommon/GraphicsResourceManager.h"
#include "3DCommon/GraphicsResourceHandle.h"
#include "3DCommon/AsyncResourceLoader.h"
#include "3DCommon/Direct3D9.h"

#include "Support/Log/DefaultLog.h"
#include "Support/SafeDelete.h"
#include "Support/SafeDeleteVector.h"

#include "Support/Serialization/BinaryDatabase.h"

using namespace std;
using namespace boost;


//==================================================================================================
// CGraphicsResourceManager
//==================================================================================================

/// define the singleton instance
CSingleton<CGraphicsResourceManager> CGraphicsResourceManager::m_obj;


CGraphicsResourceManager::CGraphicsResourceManager()
{
	m_AsyncLoadingAllowed = false;
}


CGraphicsResourceManager::~CGraphicsResourceManager()
{
	Release();

	AsyncResourceLoader().Release();
}


//void CGraphicsResourceManager::Init()
//{}


void CGraphicsResourceManager::Release()
{
	ReleaseGraphicsResources();

//	SafeDeleteVector( m_vecpResourceEntry );
}


/// Register the resource entry. Notify the entry of its own index in the array
template<class T>
size_t CGraphicsResourceManager::AddEntryToVacantSlot( T ptr, vector<T>& vecPtr )
{
	size_t index = 0;
	for( size_t i=0; i<vecPtr.size(); i++ )
	{
		if( !vecPtr[i] )
		{
			index = i;
			ptr->SetIndex( (int)index );
			vecPtr[i] = ptr;
			return index;
		}
	}

	// create a new element
	index = vecPtr.size();
	ptr->SetIndex( (int)index );
	vecPtr.push_back( ptr );
	return index;
}


template<class T>
size_t add_weak_ptr_to_vacant_slot( weak_ptr<T> ptr, vector<weak_ptr<T>>& vecPtr )
{
	for( size_t i=0; i<vecPtr.size(); i++ )
	{
		if( !vecPtr[i].lock() )
		{
			vecPtr[i] = ptr;
			return i;
		}
	}

	// create a new element
	vecPtr.push_back( ptr );
	return (int)vecPtr.size() - 1;
}


/// Created resource entry has its own index
shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::CreateGraphicsResourceEntry( const CGraphicsResourceDesc& desc )
{
	shared_ptr<CGraphicsResourceEntry> pEntry;
	switch( desc.GetResourceType() )
	{
	case GraphicsResourceType::Texture:
		{
			shared_ptr<CTextureEntry> ptr
				= shared_ptr<CTextureEntry>( new CTextureEntry(dynamic_cast<const CTextureResourceDesc *>(&desc)) );

			AddEntryToVacantSlot( ptr, m_vecpTextureEntry );
			pEntry = ptr;
		}
		break;

	case GraphicsResourceType::Mesh:
		{
			shared_ptr<CMeshObjectEntry> ptr
				= shared_ptr<CMeshObjectEntry>( new CMeshObjectEntry(dynamic_cast<const CMeshResourceDesc *>(&desc)) );

			AddEntryToVacantSlot( ptr, m_vecpMeshEntry );
			pEntry = ptr;
		}
		break;

	case GraphicsResourceType::Shader:
		{
			shared_ptr<CShaderManagerEntry> ptr
				= shared_ptr<CShaderManagerEntry>( new CShaderManagerEntry(dynamic_cast<const CShaderResourceDesc *>(&desc)) );

			AddEntryToVacantSlot( ptr, m_vecpShaderEntry );
			pEntry = ptr;
		}
		break;

	default:
		LOG_PRINT_WARNING( " - invalid resource type" );
		return shared_ptr<CGraphicsResourceEntry>();
	}

	// register the created entry to the ptr array that holds all the resource entries
	weak_ptr<CGraphicsResourceEntry> pWeakPtr = pEntry;
	add_weak_ptr_to_vacant_slot( pWeakPtr, m_vecpResourceEntry );

	return pEntry;
}


template<class T>
inline int share_as_same_resource( const CGraphicsResourceDesc& desc, const std::vector<boost::shared_ptr<T>>& vecPtr )
{
	const  size_t num_resources = vecPtr.size();
	for( size_t i=0; i<num_resources; i++ )
	{
		// check if the requested can be shared with one already registered to the graphics resource manager
		if( vecPtr[i]
		 && vecPtr[i]->CanBeSharedAsSameResource( desc ) )
		{
			// requested resource was found in the list
			// - no need to add a new resource.
			// - resource gets loaded if reference count is incremented from 0 to 1
			vecPtr[i]->IncRefCount();
			return (int)i;
		}
	}

	return -1;
}


bool CGraphicsResourceManager::ReleaseResourceEntry( boost::shared_ptr<CGraphicsResourceEntry> ptr )
{
	const int index = ptr->GetIndex();
	switch(ptr->GetResourceType())
	{
	case GraphicsResourceType::Texture: m_vecpTextureEntry[index].reset(); return true;
	case GraphicsResourceType::Mesh:    m_vecpMeshEntry[index].reset();    return true;
	case GraphicsResourceType::Shader:  m_vecpShaderEntry[index].reset();  return true;
	default:
		return false;
	}

	return false;
}


shared_ptr<CGraphicsResourceLoader> CGraphicsResourceManager::CreateResourceLoader( shared_ptr<CGraphicsResourceEntry> pEntry )
{
	boost::shared_ptr<CGraphicsResourceLoader> pLoader;
	switch(pEntry->GetResourceType())
	{
	case GraphicsResourceType::Texture:
		pLoader = shared_ptr<CGraphicsResourceLoader>( new CDiskTextureLoader(m_vecpTextureEntry[pEntry->GetIndex()]) );
		break;
	case GraphicsResourceType::Mesh:
		pLoader = shared_ptr<CGraphicsResourceLoader>( new CMeshLoader(m_vecpMeshEntry[pEntry->GetIndex()]) );
		break;
//	case GraphicsResourceType::Shader:
//		pLoader = shared_ptr<CGraphicsResourceLoader>( new CDiskTextureLoader(m_vecpShaderEntry[pEntry->GetIndex()]) );
	default:
		return pLoader;
	}

	return pLoader;
}


/// Not implemented yet.
/// - Just returns -1 to indicate that there are no saharable resoureces
int CGraphicsResourceManager::FindSameLoadedResource( const CGraphicsResourceDesc& desc )
{
	return -1;
}


// async loading steps
// 1. see if the requested resource has already been loaded
//    true -> see if it is sharable
//      true -> share the resource.
//    false (not found) -> send load request
int CGraphicsResourceManager::LoadAsync( const CGraphicsResourceDesc& desc )
{
	if( desc.IsDiskResource() )
	{
		shared_ptr<CGraphicsResourceEntry> ptr;
		const size_t num_entries = m_vecpResourceEntry.size();
		for( size_t i=0; i<num_entries; i++ )
		{
			int shared_resource_index = FindSameLoadedResource( desc );
			if( 0 <= shared_resource_index )
			{
				return shared_resource_index;
			}
		}
	}
	else
	{
		// non-disc resources are not sharable
		return -1;
	}

	// create a new empty resource entry to determine the resource index now.
	// - reserves an entry index before loading the resource
	// - graphics resource handle needs resource index

	boost::shared_ptr<CGraphicsResourceEntry> pEntry = CreateGraphicsResourceEntry( desc );
	if( pEntry )
	{
		CResourceLoadRequest req( CResourceLoadRequest::LoadFromDisk, pEntry );
		req.m_pLoader = CreateResourceLoader(pEntry);
		AsyncResourceLoader().AddResourceLoadRequest( req );

		pEntry->IncRefCount();

		return pEntry->GetIndex();
	}
	else
		return -1;
}


//void CGraphicsResourceManager::CreateAt( const CGraphicsResourceDesc& desc, int dest_index );
boost::shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::CreateAt( const CGraphicsResourceDesc& desc, int dest_index )
{
	// 
	boost::shared_ptr<CGraphicsResourceEntry> pEntry = shared_ptr<CGraphicsResourceEntry>();
//		= GetCachedResource( desc );

	if( !pEntry )
	{
		// ceate a new resource
		pEntry = CreateGraphicsResourceEntry( desc );
	}

//	SetToSlot( dest_index, pEntry );
	switch(desc.GetResourceType())
	{
	case GraphicsResourceType::Texture: m_vecpTextureEntry[dest_index] = dynamic_pointer_cast<CTextureEntry,CGraphicsResourceEntry>(pEntry); break;
	case GraphicsResourceType::Mesh:    m_vecpMeshEntry[dest_index]    = dynamic_pointer_cast<CMeshObjectEntry,CGraphicsResourceEntry>(pEntry); break;
	case GraphicsResourceType::Shader:  m_vecpShaderEntry[dest_index]  = dynamic_pointer_cast<CShaderManagerEntry,CGraphicsResourceEntry>(pEntry); break;
	default:
		break;
	}

	if( !pEntry )
	{
		// cannot create a resource entry
	}

	// register the created entry to the ptr array that holds all the resource entries
	weak_ptr<CGraphicsResourceEntry> pWeakPtr = pEntry;
	add_weak_ptr_to_vacant_slot( pWeakPtr, m_vecpResourceEntry );

	// lock the resource
//	pEntry->Lock();

	return pEntry;
}


int CGraphicsResourceManager::LoadGraphicsResource( const CGraphicsResourceDesc& desc )
{
//    boost::mutex::scoped_lock scoped_lock(m_IOMutex);

	LOG_FUNCTION_SCOPE();

	if( desc.ResourcePath.length() == 0 )
		return -1;	// invalid filename

	// search if a same resource has been already loaded or is being loaded
	int shared_index = -1;
	switch(desc.GetResourceType())
	{
	case GraphicsResourceType::Texture:
		shared_index = share_as_same_resource( desc, m_vecpTextureEntry );
		break;
	case GraphicsResourceType::Mesh:
		shared_index = share_as_same_resource( desc, m_vecpMeshEntry );
		break;
	case GraphicsResourceType::Shader:
		shared_index = share_as_same_resource( desc, m_vecpShaderEntry );
		break;
	default:
		break;
	}

	// found a resource to share
	// just return the index to the entry
	if( 0 <= shared_index )
		return shared_index;

	// not found in the list - need to load as a new resource
	shared_ptr<CGraphicsResourceEntry> pResourceEntry = CreateGraphicsResourceEntry(desc);

	if( !pResourceEntry )
		return -1;

	pResourceEntry->SetFilename( desc.ResourcePath );

	// load 

	// increment the reference count
	// - load the resource because the entry is new (increment the resource id from 0 to 1).
	pResourceEntry->IncRefCount();

	if( pResourceEntry->GetRefCount() == 1 )
	{
		LOG_PRINT( " - Created a graphics resource: " + desc.ResourcePath );

		// A resource has been successfully loaded
		return pResourceEntry->GetIndex();
		//return (int)i;
	}
	else
	{
		LOG_PRINT_WARNING( "Failed to create a graphics resource: " + desc.ResourcePath );

		ReleaseResourceEntry( pResourceEntry );
		return -1;	// failed to create a resource
	}
}


/// called from handle
int CGraphicsResourceManager::LoadTexture( const CTextureResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


int CGraphicsResourceManager::CreateTexture( const CTextureResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


/// called from handle
int CGraphicsResourceManager::LoadMesh( const CMeshResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


/// called from handle
int CGraphicsResourceManager::LoadShaderManager( std::string filename )
{
	CShaderResourceDesc desc;
	desc.ResourcePath = filename;
	return LoadGraphicsResource( desc );
}

/*
void CGraphicsResourceManager::IncResourceRefCount( const CGraphicsResourceHandle& handle )
{
	const int index = handle.m_EntryID;
	if( index < 0 )
		return;

	switch( handle.GetResourceType() )
	{
	case GraphicsResourceType::Texture: if( index < (int)m_vecpTextureEntry.size() ) m_vecpTextureEntry[index]->IncRefCount(); return;
	case GraphicsResourceType::Mesh:    if( index < (int)m_vecpMeshEntry.size() )    m_vecpMeshEntry[index]->IncRefCount();    return;
	case GraphicsResourceType::Shader:  if( index < (int)m_vecpShaderEntry.size() )  m_vecpShaderEntry[index]->IncRefCount();  return;
	}
}


void CGraphicsResourceManager::DecResourceRefCount( const CGraphicsResourceHandle& handle )
{
	const int index = handle.m_EntryID;
	if( index < 0 )
		return;

	switch( handle.GetResourceType() )
	{
	case GraphicsResourceType::Texture: if( index < (int)m_vecpTextureEntry.size() ) m_vecpTextureEntry[index]->DecRefCount(); return;
	case GraphicsResourceType::Mesh:    if( index < (int)m_vecpMeshEntry.size() )    m_vecpMeshEntry[index]->DecRefCount();    return;
	case GraphicsResourceType::Shader:  if( index < (int)m_vecpShaderEntry.size() )  m_vecpShaderEntry[index]->DecRefCount();  return;
	}
}
*/

void CGraphicsResourceManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = m_vecpResourceEntry[i].lock();
		if( pEntry )
			pEntry->Load();
	}
}


void CGraphicsResourceManager::ReleaseGraphicsResources()
{
//	g_Log.Print( "CGraphicsResourceManager::ReleaseGraphicsResources()" );

	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = m_vecpResourceEntry[i].lock();
		if( pEntry )
			pEntry->Release();
	}
}


void CGraphicsResourceManager::Refresh()
{
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = m_vecpResourceEntry[i].lock();
		if( pEntry )
			pEntry->Refresh();
	}
}

/*
int CGraphicsResourceManager::LoadAsync( const CGraphicsResourceDesc& desc )
{
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
//		m_vecpResourceCache[i]->;
	}
}
*/
